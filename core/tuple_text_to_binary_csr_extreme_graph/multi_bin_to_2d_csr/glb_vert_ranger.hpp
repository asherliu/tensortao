#include "bin_struct_reader.h"
#include <iostream>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "wtime.h"

template<typename index_t, typename vertex_t, typename file_vertex_t>
void 
glb_vert_ranger(
		const char *prefix,
		const int file_count,
		const int num_thds,
		vertex_t  &glb_max_vert,
		vertex_t  &glb_min_vert,
		index_t &glb_edge_count
){
	printf("\n\n\n===========\nVert-ranger computation\n------\n");

	char name[256];
	sprintf(name, "%s-glb-vert-edge-count.bin", prefix);
	FILE *file = fopen(name, "rb");
	size_t ret = 0;	
	bool is_config = true;
	
	if(file != NULL)
	{
		printf("glb-vert-edge-count file is found\n");
		ret = fread(&glb_min_vert, sizeof(vertex_t), 1, file);
		if(ret != 1)
		{
			perror("fread");
			is_config = false;
			printf("glb_min_vert: %u wrong, ret = %d\n", glb_min_vert, ret);
		}

		ret = fread(&glb_max_vert, sizeof(vertex_t), 1, file);
		if(ret != 1 || glb_min_vert >= glb_max_vert)
		{
			perror("fread");
			is_config = false;
			printf("glb_max_vert: %u wrong, ret = %d\n", glb_max_vert, ret);
		}

		ret = fread(&glb_edge_count, sizeof(index_t), 1, file);
		if(ret != 1 || glb_edge_count <= 0)
		{
			perror("fread");
			is_config = false;
			printf("glb_edge_count: %ld wrong, ret = %d\n", glb_edge_count, ret);
		}

		fclose(file);
	}else{
		is_config = false;
		printf("glb-vert-edge-count cannot open\n");
	}
	
	if(is_config)
	{
		printf("glb-vert-edge-count works\n");
		printf("min: %u, max:%u, total edge count: %ld\n", 
			glb_min_vert, glb_max_vert, glb_edge_count);
		return;
	}


	index_t *comm_num_edges = new index_t[num_thds];
	vertex_t *comm_min_vert = new vertex_t[num_thds];
	vertex_t *comm_max_vert = new vertex_t[num_thds];

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
		else
			my_file_beg = tid * step + remainder;

		int my_file_end = my_file_beg + step;

		// for(int i = 0; i < num_thds; i++)
		// {
		//     if(tid == i)
		//         printf("thd-%d: %d ~ %d\n", i, my_file_beg, my_file_end);
		//#pragma omp barrier
		// }
		 index_t my_edge_count = 0;
		 vertex_t min_vert = (int)(1<<30);
		 vertex_t max_vert = 0;
		 char filename[256];

		 while(my_file_beg < my_file_end)
		 {
		     sprintf(filename, "%s-%05d.bin", prefix, my_file_beg);
		     if(tid==0) 
		         printf("Processing %s, %lf seconds\n", filename, wtime()-tm);

		     bin_struct_reader<file_vertex_t, index_t>
		         *inst = new bin_struct_reader<file_vertex_t, index_t>
		         ((const char *)filename);
		     inst->vert_ranger();

		     ///
		     if(min_vert > inst->min_vert) min_vert = inst->min_vert;
		     if(max_vert < inst->max_vert) max_vert = inst->max_vert;
		     
		     my_edge_count += inst->num_edges;
		     delete inst;
		     my_file_beg++;
		 }

		 comm_num_edges[tid] = my_edge_count;
		 comm_min_vert[tid] = min_vert;
		 comm_max_vert[tid] = max_vert;

	}

	glb_min_vert = INFTY_MAX;
	glb_max_vert = 0;
	glb_edge_count = 0;
	for(int i = 0; i < num_thds; i ++)
	{
		if(glb_min_vert > comm_min_vert[i]) glb_min_vert = comm_min_vert[i];
		if(glb_max_vert < comm_max_vert[i]) glb_max_vert = comm_max_vert[i];
		glb_edge_count += comm_num_edges[i];
	}
	printf("min: %u, max:%u, total edge count: %ld\n", 
			glb_min_vert, glb_max_vert, glb_edge_count);
	
	file=fopen(name, "wb");
	assert(file != NULL);
	assert(fwrite(&glb_min_vert, sizeof(vertex_t), 1, file) == 1);
	assert(fwrite(&glb_max_vert, sizeof(vertex_t), 1, file) == 1);
	assert(fwrite(&glb_edge_count, sizeof(index_t), 1, file) == 1);
	fclose(file);
}
