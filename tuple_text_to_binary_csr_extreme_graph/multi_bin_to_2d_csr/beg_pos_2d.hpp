#include "bin_struct_reader.h"
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "wtime.h"
#include "util.hpp"

template<typename index_t, typename vertex_t, typename file_vertex_t>
void beg_pos_2d (
		vertex_t *row_ranger,
		vertex_t *col_ranger,
		const char *prefix,
		const int file_count,
		const int num_thds,
		const vertex_t vert_count,//max_vert + 1
		const int num_rows,
		const int num_cols
){
	printf("\n\n\n===========\nBeg_pos computation\n------\n");
	
	//alloc beg_pos array
	index_t **degree = new index_t*[num_rows*num_cols];
	for(int i = 0; i < num_rows*num_cols; i ++)
	{
		int my_row = i/num_cols;
		index_t row_count = row_ranger[my_row+1]-row_ranger[my_row];
		degree[i] = new index_t[row_count];
		memset(degree[i], 0, sizeof(index_t)*row_count);
		printf("part-%d-%d row vert-count: %ld\n", my_row, i%num_cols, row_count);
	}
	
	//check if beg_pos is already computed
	bool is_beg_done = true;
	char filename[256];
	struct stat st;
	for(int i = 0; i < num_rows; i++)
	{
		for(int j = 0; j < num_cols; j++)
		{
			sprintf(filename, "%s_beg.%d_%d_of_%dx%d.bin", 
					prefix, i, j, num_rows, num_cols);
			if(stat(filename, &st)!=0)
			{
				is_beg_done = false;
				printf("%s is not found\n", filename);
				break;
			}

			if(st.st_size != (row_ranger[i+1] - row_ranger[i] + 1) * sizeof(index_t))
			{
				is_beg_done = false;
				printf("%s is not found\n", filename);
				break;
			}
			
			printf("%s found\n", filename);
		}
	}
	
	if(is_beg_done == true)
	{
		printf("beg_pos is already computed\n");
		return;
	}

	double tm = wtime();
#pragma omp parallel \
	num_threads(num_thds)
	{
		int tid = omp_get_thread_num();
		int step = file_count/num_thds;
		int remainder = file_count - num_thds * step;
		int my_file_beg;

		if(tid < remainder) 
		{
			step++;
			my_file_beg = tid * step;
		}
		else my_file_beg = tid * step + remainder;

		int my_file_end = my_file_beg + step;
		// for(int i = 0; i < num_thds; i++)
		// {
		//     if(tid == i)
		//         printf("thd-%d: %d ~ %d\n", i, my_file_beg, my_file_end);
		//#pragma omp barrier
		// }
		 char filename[256];

		 while(my_file_beg < my_file_end)
		 {
			 sprintf(filename, "%s-%05d.bin", prefix, my_file_beg);
			 if(tid==0) 
				 printf("Processing %s, %lf seconds\n", filename, wtime()-tm);

			 bin_struct_reader<file_vertex_t, index_t>
				 *inst = new bin_struct_reader<file_vertex_t, index_t>
				 ((const char *)filename);

			 for(index_t i = 0; i < inst->num_edges; i++)
			 {
				 vertex_t src = (vertex_t) inst->edge_list[i].src;
				 vertex_t dest= (vertex_t) inst->edge_list[i].dest;

				 int my_row = aligned_par<vertex_t, index_t>
					 (row_ranger, num_rows, src);

				 int my_col = misaligned_col<vertex_t, index_t>
					(col_ranger,  num_cols, dest, my_row);
				
				 index_t *my_degree = degree[my_row * num_cols + my_col];
				 __sync_fetch_and_add(my_degree + 
						 src - row_ranger[my_row], 1);
			 }

			 delete inst;
			 my_file_beg++;
		 }
	}

	printf("Finished count degree\nStart compute beg_pos\n");

	//use degree to compute inclusive beg_pos
	//for exclusive beg_pos, we add a 0 ahead
#pragma omp parallel num_threads(num_rows * num_cols)
		{
			int tid = omp_get_thread_num();
			int my_row = tid / num_cols;
			for(index_t i = 1; i < row_ranger[my_row+1] - row_ranger[my_row];
						i++)
			{
				degree[tid][i] += degree[tid][i-1];	
			}
		}
	
	printf("Dump beg_pos to disk\n");

	for(int i = 0; i < num_rows; i++)
	{
		for(int j = 0; j < num_cols; j++)
		{
			sprintf(filename, "%s_beg.%d_%d_of_%dx%d.bin", 
					prefix, i, j, num_rows, num_cols);
			FILE *fd = fopen(filename, "wb");
			assert(fd != NULL);

			//write the beg_pos of the first vertex, -- 0.
			index_t init_beg[1]; init_beg[0] = 0;
			fwrite(init_beg, sizeof(index_t), 1, fd);
			fwrite(degree[i*num_cols + j], sizeof(index_t), 
					row_ranger[i+1] - row_ranger[i], fd);
			fclose(fd);
			printf("row-%d-col-%d, edgecount: %ld\n", i, j, 
					degree[i*num_cols + j]
					[row_ranger[i+1] - row_ranger[i]-1]);
		}
	}
	

	//dealloc arrays
	for(int i = 0; i < num_rows; i++)
		for(int j = 0; j < num_cols; j++)
			delete degree[i*num_cols + j];
	
	delete[] degree;
	return ;
}
