#include "bin_struct_reader.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

template<typename vertex_t, typename index_t>
bin_struct_reader<vertex_t, index_t>::
bin_struct_reader(const char* filename)
{
    struct stat st;
    if(stat(filename, &st)!=0)
    {
        printf("%s access error\n", filename);
        exit(-1);
    }

    size_t fbytes=st.st_size;
    num_edges=fbytes/sizeof(edge<vertex_t>);
    assert(num_edges*sizeof(edge<vertex_t>) == fbytes);
    
    //init min max vert
    min_vert = INFTY_MAX;
    max_vert = 0;

    fd = open(filename, O_RDONLY);
    if(fd==-1){perror("open"); printf("File %s open error\n", filename);}
    edge_list = (edge<vertex_t> *)mmap(0, fbytes, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(edge_list != MAP_FAILED);
}
    
template<typename vertex_t, typename index_t>
void bin_struct_reader<vertex_t, index_t>::
vert_ranger()
{
    min_vert = edge_list[0].src;
    max_vert = edge_list[0].src;

    for(index_t i = 0;i < num_edges; i ++)
    {
        vertex_t src = edge_list[i].src;
        vertex_t dest= edge_list[i].dest;

        if(src < min_vert) min_vert = src;
        if(src > max_vert) max_vert = src;
        if(dest< min_vert) min_vert = dest;
        if(dest> max_vert) max_vert = dest;

    }
}
