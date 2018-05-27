#include "bin_struct_reader.h"
#include <iostream>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "wtime.h"
#include "beg_pos_2d.hpp"
#include "csr_2d_mem.hpp"
#include "csr_2d_mmap.hpp"
#include "glb_vert_ranger.hpp"
#include "ultra_balanced_2d_partition.hpp"
#include "balanced_2d_partition.hpp"
#include "p2d_partition.hpp"
#include "util.hpp"

int main(int args, char **argv)
{
	std::cout<<"Input: ./exe file_header total_file_count num_rows num_cols num_threads\n";
	assert(args == 6);
	const char* prefix = argv[1];
	const int file_count = atoi(argv[2]);
	const int num_rows = atoi(argv[3]);
	const int num_cols = atoi(argv[4]);
	const int num_thds = atoi(argv[5]);

	typedef long index_t;
	typedef long file_vertex_t;
	typedef unsigned int vertex_t;
	//typedef long vertex_t;

	vertex_t max_vert, min_vert;
	index_t edge_count;
	vertex_t *row_ranger, *col_ranger;
	
	/*figure out vertex and edge count*/
	glb_vert_ranger<index_t, vertex_t, file_vertex_t>
		(prefix, file_count, num_thds, max_vert, min_vert, edge_count);
		
	/*figure out normal 2d partition row and col ranger*/
	double tm_part = wtime();
	//p2d_partition<index_t, vertex_t, file_vertex_t>
	//	(row_ranger, col_ranger, prefix, file_count, num_thds, 
	//	 max_vert + 1, num_rows, num_cols);


	/*ultra balanced 2d partition row and col ranger*/
	ultra_balanced_2d_partition<index_t, vertex_t, file_vertex_t>
		(row_ranger, col_ranger, prefix, file_count, num_thds, 
		 max_vert + 1, edge_count, num_rows, num_cols);
	tm_part = wtime() - tm_part;


	/*produce beg_pos file*/
	double tm_beg = wtime();
	beg_pos_2d<index_t, vertex_t, file_vertex_t>
		(row_ranger, col_ranger,
		 prefix, file_count, num_thds, max_vert + 1, num_rows, num_cols);
	tm_beg = wtime() - tm_beg;

	/*adj list file within memory*/
	double tm_csr = wtime();
	csr_2d_mem<index_t, vertex_t, file_vertex_t>
		(row_ranger, col_ranger,
		 prefix, file_count, num_thds, max_vert + 1, num_rows, num_cols);

	/*adj list file with mmap*/
	//	csr_2d_mmap<index_t, vertex_t, file_vertex_t>
//		(row_ranger, col_ranger,
//		 prefix, file_count, num_thds, max_vert + 1, num_rows, num_cols);

	tm_csr = wtime() - tm_csr;

	std::cout<<"Total time: "<<tm_part<<" "<<tm_beg<<" "<<tm_csr<<"\n";
	return 0;
}
