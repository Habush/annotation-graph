# Annotation Graph

This library is used for generating hierarchical (using Sugiyama Layout) and Multi-level layout(MLL) for the results of
 [annotation service](https://github.com/MOZI-AI/annotation-service). It takes a JSON file representing a graph
 , parses the JSON into an [OGDF](https://ogdf.uos.de/) [Graph](https://ogdf.uos.de/doc
 /classogdf_1_1_graph
 .html
 ) representation. It then decomposes the graph into
  two subgraph, [GO](http://amigo.geneontology.org/amigo) and Non GO. It then applies hierarchial layout for the GO subgraph and MLL for the non-GO
   subgraph.
   
   #### Prerequisites
   1. [OGDF](https://ogdf.uos.de/)
        * Download the latest version from [here](https://ogdf.uos.de/wp-content/uploads/2020/02/ogdf.v2020.02.zip
        ) and extract the archive
        * Run the following to build the library:
            ```shell script
            $ cd OGDF
            $ cmake .
            $ make -j4 && make install
          ```
    
   2. [Rapidjson](https://github.com/Tencent/rapidjson)
        * Download the source code from [here](https://github.com/Tencent/rapidjson/archive/v1.1.0.tar.gz)
        * Extract it and copy the `include/rapidjson` directory to the system include directory (e.g `/usr/local
        /include`)
          
   3. [Pybind11](https://github.com/pybind/pybind11)
        * Install pybind11 using pip
            ```shell script
              $ pip install pybind11
            ``` 
          
   
  #### Building & Installation
  
  ```shell script
      $ git clone https://github.com/Habush/annotation_graph
      $ cd annotation_graph
      $ mkdir build && cd build
      $ cmake ..
      $ make 
      $ make install
  ```
   