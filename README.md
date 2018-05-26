-----
Software requirement
-----
gcc 4.4.7 or 4.8.5

OS: Linux, MacOS and Windows

-----
Code specification
---------
The overall code structure of this project is:

- **tuple_text_to_binary_csr**: Converting arbitrary *text format* edge tuple list file into Compressed Sparse Row (CSR) format binary file. 
> Allocating **CSR** and **weight** in memory.

- **tuple_text_to_binary_csr_mmap**: the same as **tuple_text_to_binary_csr** but with **the following difference**. 
> Directly back the **CSR** and **weight** arrays with files residing on disk. Thus this method has to synchronize udpates of **CSR** and **weight** to files on disk. This design suffers from very slow processing speed when in network-based file system (e.g., LUSTRE). However, this design is memory efficient comparing to prior method.

- **graph_reader**: Reading the binary format CSR file into memory for graph computing.


-----
Converter: edge tuples to CSR
----
- Compile: make
- To execute: type "./text_to_bin.bin", it will show you what is needed
- One example is provided in toy_graph folder. The user can use **./text_to_bin.bin ./toy_graph/toy.dat 1 2** to run the converter.


**Real example**:
- Download a graph file, e.g., **wget https://snap.stanford.edu/data/bigdata/communities/com-orkut.ungraph.txt.gz** file. 
- Decompress the file, e.g., **gzip -d /path/to/com-orkut.ungraph.txt.gz**. 
- Convert the edge list into binary CSR, e.g., **/path/to/text_to_bin.bin /path/to/com-orkut.ungraph.txt 1 0**. Note, the first number '1' means we want to reverse the edge, the second number '0' means we will skip 0 lines from /path/to/com-orkut.ungraph.txt. Eventually, this commandline will yield two files: */path/to/com-orkut.ungraph.txt_beg_pos.bin* and */path/to/com-orkut.ungraph.txt_csr.bin*. 
- Then you can use these two files to run [enterprise](https://github.com/iHeartGraph/Enterprise).


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

One example of using graph reader to develop BFS can be found here: https://github.com/asherliu/Multi-threaded-BFS


----
Acknowledgement
----
The toy graph is based on the example of Figure 1 in the following paper:

[SC '15] Enterprise: Breadth-First Graph Traversal on GPUs [[PDF](http://hang-liu.com/publication/enterprise_sc15.pdf)] [[Slides](http://hang-liu.com/publication/enterprise_slides.pdf)] [[Blog](http://hang-liu.com/enterprise_blog.html)]

