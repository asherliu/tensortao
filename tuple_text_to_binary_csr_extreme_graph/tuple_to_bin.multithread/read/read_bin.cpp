#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

using namespace std;

typedef struct packed_edge {
	  long int v0;
	  long int v1;
} packed_edge;

inline off_t fsize(const char *filename) {
    struct stat st; 
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1; 
}

		
main(int argc, char** argv){
	int fd,i;
	char* ss;

	size_t file_size = fsize(argv[1]);
	size_t edge_count = file_size/sizeof(packed_edge);
//	long int** bin = (long int**)malloc(N*sizeof(long int*));//ptr to the N new files


	fd = open( argv[1] ,O_CREAT|O_RDWR,00777 );




	cout<<"edge count: "<<edge_count<<endl;
	//step 2. each file size
	//step 3. write to bin file
	packed_edge* edge = (packed_edge*)mmap(NULL,edge_count*sizeof(packed_edge),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	size_t offset=0;
	
	while(offset<edge_count){
		cout<<edge[offset].v0<<"  ";

		cout<<edge[offset].v1<<endl;

		offset++;
	}


	munmap( edge,sizeof(packed_edge)*edge_count );
}
