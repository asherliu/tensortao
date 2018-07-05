--------
Converter Specification
-------------

This tool requires all the following four components -- **run.bash**, **split_rename.bash**, **multi_bin_to_2d_csr** and **tuple_to_bin.multithread** to function. Further, one cannot change the relative paths of these files (or folders).

**openmp** is needed to conduct this conversion.

----
Step 1: Compile
---
* **make clean**: clean the binary executable from *tuple_to_bin.multithread* and *multi_bin_to_2d_csr*.
* **make all**: compile binary executable for *tuple_to_bin.multithread* and *multi_bin_to_2d_csr*.


-----
Step 2: Run
--------
- Download a text tuple list
```
wget https://snap.stanford.edu/data/bigdata/communities/com-orkut.ungraph.txt.gz
```
- Uncompress the tuple list file: 
```
gzip -d com-orkut.ungraph.txt.gz
```

- Command to convert the graph into binary CSR, and a brief parameter explanation:
```
./run.bash /path/to/run.bash/ /path/to/text_tuple_file line_skip num_row_partition num_col_partition num_threads reverse_the_edge(1 yes, 2 no) remove_intermediate_file(1 yes, 2 no). Below, we detail the meaning of each parameter.
```

> * **/path/to/run.bash/**: absolute path to "run.bash".
> * **/path/to/text_tuple_file**: absolute path to "text_tuple_file".
> * **line_skip**: number of lines to skip in "text_tuple_file", these lines are often the description lines.
> * **num_row_partition** and  **num_col_partition**: number of partitions in terms of row and column.
> * **num_threads**: number of threads to conduct this conversion.
> * **reverse_the_edge(1 yes, 2 no)**: whether to reverse the edges, 1 means reverse, 2 otherwise.
> * **remove_intermediate_file(1 yes, 2 no)**: whether to remove the intermediate files, 1 means remove, 2 otherwise.

- Example: ./run.bash /path/to/run.bash /path/to/com-orkut.ungraph 4 1 1 20 1 1.


Eventually, this command will tell you the newly generated binary files for your CSR representation.
