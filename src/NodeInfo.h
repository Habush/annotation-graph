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

//
#include <string>
#include <vector>
#include "rapidjson/document.h"

using namespace rapidjson;
namespace annotation_graph {
    struct NodeInfo {
        NodeInfo(const std::string &id, const std::string &type ,const std::string &name, const std::string &definition,
                 const std::string &location, const std::vector<std::string> &groups)
                 : id(id), name(name), type(type),
                    definition(definition),
                    location(location),
                    groups(groups) {}

        std::string id;
        std::string name;
        std::string type;
        std::string definition;
        std::string location;
        std::vector<std::string> groups;

        void serialize(Document& doc, Value& obj, const double x, const double y){
            Value data(kObjectType);
            data.AddMember("id", id, doc.GetAllocator());
            data.AddMember("type", type, doc.GetAllocator());
            data.AddMember("name", name, doc.GetAllocator());
            data.AddMember("definition", definition, doc.GetAllocator());
            data.AddMember("location", location, doc.GetAllocator());

            Value grs(kArrayType);
            for(const std::string& gr: groups){
                Value val;
                val.SetString(gr, doc.GetAllocator());
                grs.PushBack(val, doc.GetAllocator());
            }
            data.AddMember("group", grs, doc.GetAllocator());
            obj.AddMember("data", data, doc.GetAllocator());

            Value pos(kObjectType);
            pos.AddMember("x", x, doc.GetAllocator());
            pos.AddMember("y", y, doc.GetAllocator());
            obj.AddMember("position", pos, doc.GetAllocator());

            obj.AddMember("group", "nodes", doc.GetAllocator());
        }
    };
}