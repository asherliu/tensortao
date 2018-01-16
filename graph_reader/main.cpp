#include <iostream>
#include "graph.h"

int main(int args, char **argv)
{
	std::cout<<"Input: ./exe beg csr weight\n";
	if(args!=4){std::cout<<"Wrong input\n"; return -1;}
	
	const char *beg_file=argv[1];
	const char *csr_file=argv[2];
	const char *weight_file=argv[3];
	
	//template <file_vertex_t, file_index_t, file_weight_t
	//new_vertex_t, new_index_t, new_weight_t>
	graph<long, long, int, long, int, char>
	*ginst = new graph
	<long, long, int, long, int, char>
	(beg_file,csr_file,weight_file);
    
    //**
    //You can implement your single threaded graph algorithm here.
    //like BFS, SSSP, PageRank and etc.
    
    for(int i = 0; i < ginst->vert_count+1; i++)
    {
        int beg = ginst->beg_pos[i];
        int end = ginst->beg_pos[i+1];
        std::cout<<i<<"'s neighor list: ";
        for(int j = beg; j < end; j++)
            std::cout<<ginst->csr[j]<<" ";
        std::cout<<"\n";
    } 
     

	return 0;	
}
