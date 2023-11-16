-----
Software requirement
-----
gcc 4.4.7 or 4.8.5

OS: Linux, MacOS and Windows

-----------
Graph format basics
----------------------
![Alt text](/images/intr_graph.png)

Given a graph G = (V, E, W), as shown in (a), mainstream storage formats are (b) edge (tuple) list format and (c) *Compressed sparse row* (CSR) format. Note, V, E and W stand for the sets of vertices, edges and weights of the graph G, respectively.

**CSR format**: is firstly introduced to store [sparse matrices](http://netlib.org/utk/papers/templates/node91.html). When it comes to graph, we assume each source vertex to be one row of the matrix, and all the destination vertices (originated from the same source vertex) as column indices for the *non-zero* (nnz) entries of this row. The edge weight resembles the content of the corresponding nnz entry. CSR only stores the destination vertices and edge weights in **Adjacency list** and **Weight list**. Further, the **Begin position** array is used to describe the ranges of destination vertices (also edge weights) for every row. Because the begin index of vertex *n+1* is the end index for vertex *n*. Consequently, a dedicated end index array is unnecessary, except one extra entry at the end of the begin position array to indicate the end index of the destination vertices for the last source vertex, as shown in (c).  

In summary, CSR space consumptions are: 
- Begin position: |V| + 1 entries.
- Adjacency list: |E| entries. 
- Weight list: |E| entries.


-----
Code specification
---------
The overall code structure of this project is:

- **tuple_text_to_binary_csr_mmap**: converting tuple text list into binary CSR with **the following feature**. 
> Directly back the **CSR** and **weight** arrays with files residing on disk, which has to synchronize udpates of **CSR** and **weight** to files on disk. Thus this design suffers from very slow processing speed in network-based file system (e.g., LUSTRE). Fortunately, this design is memory efficient comparing to **tuple_text_to_binary_csr**.
> ```This source code generate **symmetric weights**, that is, weight of a->b = weight of b->a for undirected graph.```




- **tuple_text_to_binary_csr_extreme_graph**: converting tuple text list into binary CSR with **the following feature**. 
> Split tuple list file to enhance parallelism, **fast** and **more complex**. There is a README.md file inside of this folder details the use.
> ```This source code CANNOT generate weights.```

- **tuple_text_to_binary_csr_mem**:
Converting arbitrary *text format* edge tuple list file into Compressed Sparse Row (CSR) format binary file. 
> Allocating **CSR** and **weight** arrays in memory. Once all updates to these two arrays are done, we dump them to disk.



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
This repo serves as the purpose of helping developers to, instead of distracted by coding tools to convert graph, immediately focusing on developing graph algorithms, such as, BFS, SSSP, PageRank, and etc.

For instance, you can directly code up your BFS, SSSP, PageRank and etc algorithms at [line 20-32 of the main.cpp in graph_reader](https://github.com/asherliu/graph_project_start/blob/master/graph_reader/main.cpp#L20-L32) which is detailed as follows:
>
	
	//template <file_vertex_t, file_index_t, file_weight_t
	//new_vertex_t, new_index_t, new_weight_t>
	graph<long, long, int, long, long, char>
	*ginst = new graph
	<long, long, int, long, long, char>
	(beg_file,csr_file,weight_file);
    
    //**line 20 - 32 
    //You can implement your single threaded graph algorithm here.
    //like BFS, SSSP, PageRank and etc.
    
    //for(int i = 0; i < ginst->vert_count+1; i++)
    //{
    //    int beg = ginst->beg_pos[i];
    //    int end = ginst->beg_pos[i+1];
    //    std::cout<<i<<"'s neighor list: ";
    //    for(int j = beg; j < end; j++)
    //        std::cout<<ginst->csr[j]<<" ";
    //    std::cout<<"\n";
    //} 
     
