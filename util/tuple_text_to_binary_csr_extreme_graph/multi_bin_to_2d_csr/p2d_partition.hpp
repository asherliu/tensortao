#include "bin_struct_reader.h"
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "wtime.h"

template<typename index_t, typename vertex_t, typename file_vertex_t>
void p2d_partition (
		vertex_t* &row_ranger,
		vertex_t* &col_ranger,
		const char *prefix,
		const int file_count,
		const int num_thds,
		const vertex_t vert_count,//max_vert + 1
		const int num_rows,
		const int num_cols
){
	printf("\n\n\n===========\nConventional-2d computation\n------\n");
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
	
	vertex_t row_step = vert_count / num_rows;
	vertex_t col_step = vert_count / num_cols;

	for(int i = 0; i < num_rows; i ++)
	{
		row_ranger[i] = row_step * i;
		for(int m = 0; m < num_cols; m++)
			col_ranger[i*(num_cols+1) + m] = col_step * m;
	}
	
	row_ranger[num_rows] = vert_count;
	for(int m = 0; m < num_rows; m++)
		col_ranger[m*(num_cols+1) + num_cols] = vert_count;

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

	return ;
}
