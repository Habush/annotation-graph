/**
Created by Abdulrahman Semrie<hsamireh@gmail.com> on 10/2/20
Copyright © 2020 Abdulrahman Semrie
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

#include <string>
#include <vector>
using namespace rapidjson;
namespace annotation_graph {
    struct EdgeInfo {

        EdgeInfo(const std::string &id, const std::string &name ,const std::string &source, const std::string &target,
                 const std::string &pubmed, const std::string &subgroup, const std::vector<std::string> &groups) :
                 id(id), name(name), source(source), target(target), pubmed(pubmed), subgroup(subgroup), groups(groups) {}

        std::string id;
        std::string name;
        std::string source;
        std::string target;
        std::string pubmed;
        std::string subgroup;
        std::vector<std::string> groups;

        void serialize(Document& doc, Value& obj){
            Value data(kObjectType);
            data.AddMember("id", id, doc.GetAllocator());
            data.AddMember("name", name, doc.GetAllocator());
            data.AddMember("source", source, doc.GetAllocator());
            data.AddMember("target", target, doc.GetAllocator());
            data.AddMember("pubmedId", pubmed, doc.GetAllocator());
            data.AddMember("subgroup", subgroup, doc.GetAllocator());
            Value grs(kArrayType);
            for(const std::string& gr: groups){
                Value val;
                val.SetString(gr, doc.GetAllocator());
                grs.PushBack(val, doc.GetAllocator());
            }
            data.AddMember("group", grs, doc.GetAllocator());
            obj.AddMember("data", data, doc.GetAllocator());
            obj.AddMember("group", "edges", doc.GetAllocator());

        }

    };
}

