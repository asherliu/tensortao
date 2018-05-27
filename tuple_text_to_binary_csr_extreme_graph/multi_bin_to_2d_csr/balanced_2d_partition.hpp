#include "bin_struct_reader.h"
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "wtime.h"

template<typename index_t, typename vertex_t, typename file_vertex_t>
void balanced_2d_partition (
		index_t* &row_ranger,
		index_t* &col_ranger,
		const char *prefix,
		const int file_count,
		const int num_thds,
		const vertex_t vert_count,//max_vert + 1
		const index_t edge_count, 
		const int num_rows,
		const int num_cols
){
	printf("\n\n\n===========\nBalanced-2d computation\n------\n");
	//alloc balanced partition ranger_beg matrix
	row_ranger = new vertex_t[num_rows + 1];
	col_ranger = new vertex_t[(num_cols + 1) * num_rows];
	
	
	char name[256];	
	sprintf(name, "%s_beg-%dx%d-row-ranger.bin", prefix, num_rows, num_cols);
	FILE *file = fopen(name, "rb");
	size_t ret = 0;	
	if(file != NULL)
	{
		printf("row-ranger file is found\n");
		ret = fread(row_ranger, sizeof(vertex_t), num_rows + 1, file);
		assert(ret == num_rows + 1);
		fclose(file);
	}

	sprintf(name, "%s_beg-%dx%d-col-ranger.bin", prefix, num_rows, num_cols);
	file = fopen(name, "rb");

	if(file != NULL)
	{
		printf("col-ranger file is found\n");
		ret = fread(col_ranger, sizeof(vertex_t), (num_cols + 1) * num_rows, file);
		assert(ret == (num_cols + 1) * num_rows);
		fclose(file);

		printf("Will not do ultra-balanced partition again\n");
		return;
	}
	
	printf("No precomputed ultra balanced partition, we need to compute\n");
	memset(row_ranger, 0, sizeof(vertex_t)*(num_rows + 1));
	memset(col_ranger, 0, sizeof(vertex_t)*(num_cols + 1) * num_rows);
	
	//alloc beg_pos array
	index_t *indegree = new index_t[vert_count];
	index_t *outdegree = new index_t[vert_count];
	memset(indegree, 0, sizeof(index_t)*vert_count);
	memset(outdegree, 0, sizeof(index_t)*vert_count);

	printf("Degree data struct allocated\n");
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
				 __sync_fetch_and_add(outdegree + src, 1);
				 __sync_fetch_and_add(indegree + dest, 1);
			 }

			 delete inst;
			 my_file_beg++;
		 }
	}

	printf("Finished count degree\nStart compute in/out inclusive beg_pos.\n");
	printf("And search through to find balanced row/col partition\n");
	
	index_t row_par_step = edge_count / num_rows;
	index_t col_par_step = edge_count / num_cols;
	index_t curr_row = 1;
	index_t curr_col = 1;

	printf("Col step: %ld, Row step: %ld\n", row_par_step, col_par_step);
	//use degree to compute inclusive beg_pos
	//for exclusive beg_pos, we add a 0 ahead
#pragma omp parallel num_threads(2)
	{
		int tid = omp_get_thread_num();
		if(tid == 0)
			for(index_t i = 1; i < vert_count; i++)
			{
				outdegree[i] += outdegree[i-1];	
				
				if((outdegree[i-1] <= (curr_row * row_par_step)) &&
					(outdegree[i]  >= (curr_row * row_par_step)))
				{
					printf("curr_row: %ld\n", curr_row);
					row_ranger[curr_row++] = i-1;
				}
			}
		else	
			for(index_t i = 1; i < vert_count; i++)
			{
				indegree[i] += indegree[i-1];	
				if((indegree[i-1] <= (curr_col * col_par_step)) &&
					(indegree[i]  >= (curr_col * col_par_step)))
				{
					printf("curr_col: %ld\n", curr_col);
					for(int m = 0; m < num_rows; m++)
						col_ranger[m*(num_cols+1) + curr_col] = i - 1;

					curr_col ++;
				}
			}
	}
	
	row_ranger[num_cols] = vert_count;
	for(int m = 0; m < num_rows; m++)
		col_ranger[m*(num_cols+1) + num_rows] = vert_count;

	//dump the row_ranger and col_ranger to disk -- checkpointing
	sprintf(name, "%s_beg-%dx%d-row-ranger.bin", prefix, num_rows, num_cols);
	file = fopen(name, "wb");
	ret = fwrite(row_ranger, sizeof(vertex_t), num_rows + 1, file);
	assert(ret == num_rows + 1);
	fclose(file);
	
	sprintf(name, "%s_beg-%dx%d-col-ranger.bin", prefix, num_rows, num_cols);
	file = fopen(name, "wb");
	ret = fwrite(col_ranger, sizeof(vertex_t), (num_cols + 1) * num_rows, file);
	assert(ret == (num_cols + 1) * num_rows);
	fclose(file);
	delete[] indegree;
	delete[] outdegree;

	return ;
}
