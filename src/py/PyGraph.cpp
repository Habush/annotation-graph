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
#include <pybind11/pybind11.h>
#include "../GraphProcessor.h"
#include <string>

namespace py = pybind11;
using namespace annotation_graph;

PYBIND11_MODULE(ann_graph, m) {
    py::class_<GraphProcessor>(m, "GraphProcessor")
            .def(py::init<const std::string&>())
            .def("process", &GraphProcessor::processGraph);
}
