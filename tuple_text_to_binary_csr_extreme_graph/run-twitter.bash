../graph_project_start/tuple_text_to_binary_csr_extreme_graph/split_rename.bash ./com-friendster.ungraph.nohead
../graph_project_start/tuple_text_to_binary_csr_extreme_graph/tuple_to_bin.multithread/text_to_bin.bin ./com-friendster.ungraph.nohead-split 200 20 1 1
../graph_project_start/tuple_text_to_binary_csr_extreme_graph/multi_bin_to_2d_csr/multi_bin_to_2d_csr.bin ./com-friendster.ungraph.nohead-split 200 1 1 20
