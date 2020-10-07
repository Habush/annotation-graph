add_library(annotation_graph SHARED GraphProcessor.cpp GraphProcessor.h NodeInfo.h EdgeInfo.h Utils.h)
add_executable(annotation_graph_main main.cpp)
find_package(OGDF REQUIRED)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
add_definitions(-DRAPIDJSON_HAS_STDSTRING)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/py})
target_link_libraries(annotation_graph
        ${COGUTIL_LIBRARIES}
        ${Boost_LIBRARIES}
        OGDF)

target_link_libraries(annotation_graph_main PRIVATE annotation_graph)

install(TARGETS annotation_graph DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

pybind11_add_module(ann_graph SHARED py/PyGraph.cpp)
target_link_libraries(ann_graph PRIVATE annotation_graph)
install(TARGETS ann_graph DESTINATION ${Python_STDLIB})