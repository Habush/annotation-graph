/**
Created by Abdulrahman Semrie<hsamireh@gmail.com> on 10/2/20
Copyright © 2020 Abdulrahman Semrie
This file is part of MOZI-AI Annotation Scheme
Annotation Graph is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.
This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.
You should have received a copy of the GNU General Public License
along with this software.  If not, see
<http://www.gnu.org/licenses/>.
**/

//
#include <iostream>
#include <cstdio>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <rapidjson/writer.h>
#include "GraphProcessor.h"
#include "Utils.h"

namespace annotation_graph {

    void GraphProcessor::processGraph() {
        //Parse Json
        if(!parseJSON()){
            throw std::runtime_error("Couldn't parse the json file. Make sure the file exists " + _path.string());
        }

        //Separate the graph into GO and nonGO subgraphs
        Graph goGraph;
        GraphAttributes goGraphAttr(goGraph, GraphAttributes::nodeGraphics | GraphAttributes::nodeId |
                                             GraphAttributes::edgeGraphics | GraphAttributes::nodeLabel |
                                             GraphAttributes::edgeLabel | GraphAttributes::edgeType |
                                             GraphAttributes::edgeStyle | GraphAttributes::nodeStyle);
        NodeInfos goNodeInfos;
        EdgeInfos goEdgesInfos;

        Graph nonGOGraph;
        GraphAttributes nonGOGraphAttr(nonGOGraph, GraphAttributes::nodeGraphics | GraphAttributes::nodeId |
                                                   GraphAttributes::edgeGraphics | GraphAttributes::nodeLabel |
                                                   GraphAttributes::edgeLabel | GraphAttributes::edgeType |
                                                   GraphAttributes::edgeStyle | GraphAttributes::nodeStyle);

        NodeInfos nonGoNodeInfos;
        EdgeInfos nonGoEdgesInfos;

        NodeFilter goFilter = [](const Value& data){
            return isGO(data) || isMain(data) ;
        };

        NodeFilter nonGOFilter = [](const Value& data){
            return !isGO(data) || isMain(data);
        };

        startTimer("Building Graph...");
        //Build GO graph
        readNodes(goGraph, goGraphAttr, goNodeInfos, goFilter);
        readEdges(goGraph,  goEdgesInfos, goGraphAttr);
        //Build non-GO graph
        readNodes(nonGOGraph, nonGOGraphAttr, nonGoNodeInfos, nonGOFilter);
        readEdges(nonGOGraph, nonGoEdgesInfos, nonGOGraphAttr);
        stopTimer("Finished building graph.");

        if(goGraph.numberOfNodes() > 1){
            _threads.push_back(thread([&goGraph, &goGraphAttr, &goNodeInfos, &goEdgesInfos, this](){
                startTimer("Running GO Layout..");
                hierarchyLayout(goGraphAttr);
                writeJSON(goGraph, goGraphAttr, goNodeInfos, goEdgesInfos, _path.parent_path() / path("go.json"));
                stopTimer("Finished Running GO Layout.");
            }));
        }
        if(nonGOGraph.numberOfNodes() > 1){
            _threads.push_back(thread([&nonGOGraph, &nonGOGraphAttr, &nonGoNodeInfos, &nonGoEdgesInfos, this](){
                startTimer("Running MLL Layout..");
                multiLevelLayout(nonGOGraphAttr);
                writeJSON(nonGOGraph, nonGOGraphAttr, nonGoNodeInfos, nonGoEdgesInfos, _path.parent_path() / path
                ("nongo.json"));
                stopTimer("Finished Running MLL Layout.");
            }));
        }

        for(thread &t : _threads){
            t.join();
        }

    }

    void GraphProcessor::readNodes(Graph &G, GraphAttributes &GA, NodeInfos &nodeInfos, const NodeFilter
    &filterNodes) {
        const Value &nodes = _document["nodes"];
        assert(nodes.IsArray());
        for (SizeType i = 0; i < nodes.Size(); i++) {
            const Value &data = nodes[i].GetObject()["data"];
            if (filterNodes != nullptr && filterNodes(data)) {
                node elm = G.newNode();
                GA.idNode(elm) = i;
                vector<string> groups;
                for(auto& group : data["group"].GetArray()){
                    groups.emplace_back(group.GetString());
                }
                nodeInfos.emplace_back(data["id"].GetString(), data["type"].GetString(),
                                       data["name"].GetString(), data["definition"].GetString(),
                                  data["location"].GetString(), groups);
                GA.label(elm) = data.GetObject()["id"].GetString();
                GA.width(elm) = 300;
                GA.height(elm) = 40;
            }
        }
    }

    void GraphProcessor::readEdges(Graph &G, EdgeInfos& edgeInfos, GraphAttributes &GA) {
        const Value &edges = _document["edges"];
        assert(edges.IsArray());
        string sourceId, targetId, edgeId;
        node source, target;
        for (SizeType i = 0; i < edges.Size(); i++) {
            const Value &data = edges[i].GetObject()["data"];
            sourceId = data.GetObject()["source"].GetString();
            targetId = data.GetObject()["target"].GetString();
            source = G.chooseNode([&GA, &sourceId](node n) { return GA.label(n) == sourceId; }, true);
            target = G.chooseNode([&GA, &targetId](node n) { return GA.label(n) == targetId; }, true);
            if (source && target) {
                G.newEdge(source, target);
                vector<string> groups;
                for(auto& group: data["group"].GetArray()){
                    groups.emplace_back(group.GetString());
                }
                boost::uuids::uuid uuid = boost::uuids::random_generator()();
                edgeInfos.emplace_back(boost::uuids::to_string(uuid), data["name"].GetString(),
                                       sourceId, targetId, data["subgroup"].GetString(),data["pubmedId"].GetString(),
                                       groups);
            }
        }
    }

    bool GraphProcessor::parseJSON() {
        if (!exists(_path)) {
            return false;
        }
        FILE *fp = fopen(_path.c_str(), "r");
        char readBuf[65536];
        FileReadStream readStream(fp, readBuf, sizeof(readBuf));

        _document.ParseStream(readStream);
        fclose(fp);
        return true;
    }

    void GraphProcessor::writeJSON(const Graph& graph, const GraphAttributes &GA,
                                   NodeInfos &nodeInfos, EdgeInfos &edgeInfos, const path& p) {

        Document writeDoc;
        writeDoc.SetObject();
        Value elms(kObjectType);
        Value nodes(kArrayType);
        Value edges(kArrayType);
        int i = 0;
        for(const node& n: graph.nodes){
            NodeInfo& info = nodeInfos[i];
            Value obj(kObjectType);
            info.serialize(writeDoc, obj, GA.x(n), GA.y(n));
            nodes.PushBack(obj, writeDoc.GetAllocator());
            i++;
        }
        elms.AddMember("nodes", nodes, writeDoc.GetAllocator());

        i = 0;
        for(const edge& e : graph.edges){
            EdgeInfo& info = edgeInfos[i];
            Value obj(kObjectType);
            info.serialize(writeDoc, obj);
            edges.PushBack(obj, writeDoc.GetAllocator());
            i++;
        }
        elms.AddMember("edges", edges, writeDoc.GetAllocator());
        writeDoc.AddMember("elements", elms, writeDoc.GetAllocator());

        FILE *fp = fopen(p.c_str(), "w");
        char writeBuf[65536];
        FileWriteStream ws(fp, writeBuf, sizeof(writeBuf));
        Writer<FileWriteStream> writer(ws);
        writeDoc.Accept(writer);
        fclose(fp);
    }

    void GraphProcessor::startTimer(const string &msg) {
        _timer.start();
        cout << msg << endl;
    }

    void GraphProcessor::stopTimer(const string &msg) {
        cout << msg << " Took " << _timer.elapsed() << " seconds" << endl;
        _timer.reset();
    }

}