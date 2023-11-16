#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#ifndef BIN_STRUCT_READER
#define BIN_STRUCT_READER

#define INFTY_MAX (int)(1<<30)
template<typename vertex_t>
struct edge{
   vertex_t src;
   vertex_t dest;
};

template<typename vertex_t, typename index_t>
class bin_struct_reader
{
    public:
        index_t num_edges;
        vertex_t min_vert;
        vertex_t max_vert;
        edge<vertex_t> *edge_list;
		int fd;

    public:
        bin_struct_reader(const char* filename);
        virtual ~bin_struct_reader()
        {
            //delete this;
            munmap(edge_list, sizeof(edge<vertex_t>)*num_edges);
			close(fd);
        };
    
    public:
        void vert_ranger();
};

#include "bin_struct_reader.hpp"
#endif
