--------
Converter Specification
-------------

This converter tells you how to convert a text edge tuple into binary CSR format in parallel

----
Step 1: Convert and Partition a Graph
---
- Create a folder to store dataset: **mkdir -p sample_dataset; cd sample_dataset**
- Download a text tuple list: **wget *https://snap.stanford.edu/data/bigdata/communities/com-orkut.ungraph.txt.gz***
- Uncompress the tuple list file: **gzip -d com-orkut.ungraph.txt.gz**
- To improve processing parallelism, you should split the file into a number of smaller files with our provided bash script: **../split_rename.bash com-orkut.ungraph.txt**
- Count number of smaller files we get from prior step: **ls -l ./com-orkut.ungraph.txt-split\* | wc -l**
- Convert those files into binary files: **../tuple_to_bin.multithread/text_to_bin.bin ./com-orkut.ungraph.txt-split 201 32 1**. To understand this command, you can type **../tuple_to_bin.multithread/text_to_bin.bin** to check the meanings of all input parameters which are **./exe fileprefix num_files numthreads rm_text_file (1 yes, 2 no)**. As such this commandline means, you have split the entire edge list file into **201** subfiles, you want to use **32** threads to convert this 201 text files into 201 binary subfiles. And you want to remove the text file after the conversion.
- Convert those binary edge subfiles into binary CSR: **../multi_bin_to_2d_csr/multi_bin_to_2d_csr.bin ./com-orkut.ungraph.txt-split 201 2 2 32**. Again, you can type **../multi_bin_to_2d_csr/multi_bin_to_2d_csr.bin** to see the meaning of each input parameter. In this case, you will get **./exe file_header total_file_count num_rows num_cols num_threads**. Here **file_header**=./com-orkut.ungraph.txt-split, meaning the common header substring across all 201 binary subfiles, **total_file_count**=201, **num_rows**=2, **num_cols**=2 and **num_threads**=32.

Eventually, you should get the following **8** files: **./com-orkut.ungraph.txt-split_beg.0_0_of_2x2.bin**, **./com-orkut.ungraph.txt-split_beg.0_1_of_2x2.bin**, **./com-orkut.ungraph.txt-split_beg.1_0_of_2x2.bin** and **./com-orkut.ungraph.txt-split_beg.1_1_of_2x2.bin**. **./com-orkut.ungraph.txt-split_csr.0_0_of_2x2.bin**, **./com-orkut.ungraph.txt-split_csr.0_1_of_2x2.bin**, **./com-orkut.ungraph.txt-split_csr.1_0_of_2x2.bin** and **./com-orkut.ungraph.txt-split_csr.1_1_of_2x2.bin**.
