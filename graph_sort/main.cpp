#include <iostream>
#include <omp.h>
#include <stdlib.h>

#include "graph.h"



int comp (const void *p, const void *q)
{
    long a = *(const long *)p - *(const long *)q;
    if (a > 0) return 1;
    else return -1;
}


int main(int args, char **argv)
{
	std::cout<<"Input: ./exe beg csr weight thread_count\n";
	if(args!=5){std::cout<<"Wrong input\n"; return -1;}
	
	const char *beg_file=argv[1];
	const char *csr_file=argv[2];
	const char *weight_file=argv[3];
	int thread_count=atoi(argv[4]);
    
    typedef long file_vert_t;

	//template <file_vertex_t, file_index_t, file_weight_t
	//new_vertex_t, new_index_t, new_weight_t>
	graph<file_vert_t, long, int, file_vert_t, long, char>
	*ginst = new graph
    <file_vert_t, long, int, file_vert_t, long, char>
    (beg_file,csr_file,weight_file);

    //**
    //You can implement your single threaded graph algorithm here.
    //like BFS, SSSP, PageRank and etc.


std::cout<<"Start sorting the graph ... \n";
#pragma omp parallel num_threads (thread_count)
    {
        long progress = 1;
        long step = (ginst->vert_count - 1)/thread_count + 1;
        int tid = omp_get_thread_num();

        long range_beg = tid * step;
        long range_end = (tid + 1)* step;

        if(tid == thread_count - 1) range_end = ginst->vert_count;


        while(range_beg < range_end)
        {
            
            if (range_beg > progress && tid == 0)
            {
                std::cout<<range_beg * 100.0/range_end<<"% done!\n";
                progress <<=1;
            }

            int beg = ginst->beg_pos[range_beg];
            int end = ginst->beg_pos[range_beg + 1];

            qsort(ginst->csr + beg, end - beg, sizeof(file_vert_t), &comp);
            range_beg ++;

            //std::cout << i << "'s neighor list: ";
            //for (int j = beg; j < end; j++)
            //    std::cout << ginst->csr[j] << " ";
            //std::cout << "\n";
        }


    }
        char filename[256];
        sprintf(filename, "%s_sorted.bin", argv[2]);

        FILE *fid = fopen(filename, "wb");
        assert(fid != NULL);

        assert(fwrite(ginst->csr, sizeof(file_vert_t), ginst->edge_count, fid) == ginst->edge_count);
        fclose(fid);
        
        std::cout<<"Check the "<<filename<<" for the sorted binary CSR\n";
    return 0;	
}
