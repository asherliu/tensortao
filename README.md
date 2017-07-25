-----
Software requirement
-----
gcc 4.4.7 or 4.8.5

OS: Linux, MacOS and Windows

-----
Code specification
---------
The overall code structure of this project is:

- **tuple_text_to_binary_csr**: Converting arbitrary edge tuple file into Compressed Sparse Row (CSR) format binary file

- **graph_reader**: Reading the binary format CSR file into memory for graph computing.


-----
Converter: edge tuples to CSR
----
- Compile: make
- To execute: type "./text_to_bin.bin", it will show you what is needed
- One example is provided in toy_graph folder. The user can use **./text_to_bin.bin ./toy_graph/toy.dat 1 2** to run the converter.


**Real example**:
- Download https://snap.stanford.edu/data/com-Orkut.html file. **unzip** it. 
- **./text_to_bin.bin soc-orkut.mtx 1 2(could change, depends on the number of lines are not edges)**
- You will get *soc-orkut.mtx_beg_pos.bin* and *soc-orkut.mtx_csr.bin*. 
- You could use these two files to run enterprise.


-----
Graph reader
----
- Compile: make
- To execute: type "./graph_loader.bin", it will show you what is needed
- You can use the converter converted graph to run this command. 

**Toy example from converter**:
- ./graph_loader.bin ../tuple_text_to_binary_csr/toy_graph/toy.dat_beg_pos.bin  ../tuple_text_to_binary_csr/toy_graph/toy.dat_csr.bin ../tuple_text_to_binary_csr/toy_graph/toy.dat_weight.bin 

--------
Further Development
--------------
This repo serves as the purpose of helping developers to, instead of distracted by coding tools to convert graph, immediately focusing on developing graph algorithms. 

One example of using graph reader to develop BFS can be found here: https://github.com/mteverfresh/Multi-threaded-BFS


----
Acknowledgement
----
The toy graph is based on the example of Figure 1 in the following paper:

[SC '15] Enterprise: Breadth-First Graph Traversal on GPUs [[PDF](http://hang-liu.com/publication/enterprise_sc15.pdf)] [[Slides](http://hang-liu.com/publication/enterprise_slides.pdf)] [[Blog](http://hang-liu.com/enterprise_blog.html)]

