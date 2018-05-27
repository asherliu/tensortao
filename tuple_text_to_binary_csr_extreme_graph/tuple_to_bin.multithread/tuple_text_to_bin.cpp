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
#include <omp.h>
#include <assert.h>
#include <ctype.h>

using namespace std;
typedef long vertex_t;
typedef long index_t;

typedef struct packed_edge {
	  vertex_t v0;
	  vertex_t v1;
} packed_edge;

inline off_t fsize(const char *filename) {
    struct stat st; 
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1; 
}

		
int main(int argc, char** argv){

	
	printf("Input: ./exe fileprefix num_files numthreads reverse_edge (1 yes, 2 no) rm_text_file (1 yes, 2 no)\n");
	if(argc!=6){printf("Input format wrong\n");exit(-1);}
	
	const char *prefix = argv[1];
	const int file_count = atoi(argv[2]);
	const int num_thds = atoi(argv[3]);
    const bool is_reverse = (atoi(argv[4]) == 1);
	const int is_rm = atoi(argv[5]);	
#pragma omp parallel num_threads(num_thds)
	{
		char* ss_head;
		char* ss;
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

	

		char filename[256];
		
		while(my_file_beg < my_file_end)
		{
			sprintf(filename, "%s-%05d", prefix, my_file_beg);

			size_t file_size = fsize(filename);
			int fd=open(filename,O_CREAT|O_RDWR,00666 );
			if(fd ==-1) perror("fname open");
			ss_head = (char*)mmap(NULL,file_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
			close(fd);

			printf("Processing %s, size = %ld\n", filename, file_size);
			size_t head_offset=0;
			while(ss_head[head_offset]=='%'){
				while(ss_head[head_offset]!='\n'){
					head_offset++;
				}
				head_offset++;
			}
			ss = &ss_head[head_offset];
			file_size -= head_offset;


			size_t curr=0;
			size_t next=0;

			//step 1. vert_count,edge_count,
			size_t edge_count=0;
			size_t vert_count;
			vertex_t v_max = 0;
			vertex_t v_min = ((int)1<<30);//as infinity
			vertex_t a;
			
			while(next < file_size){
				char* sss = ss + curr;
				a = (vertex_t) atol(sss);/* turn initial portion of sss to an integer/long */

				if(v_max<a){
					v_max = a;
				}
				if(v_min>a){
					v_min = a;
				}
				
				if(next >= file_size) break;
				while((ss[next]!=' ')&&(ss[next]!='\n')&&(ss[next]!='\t')){
					//if(next > 409600086) printf("%d\n",(int)ss[next]);
					next++;
					if(next >= file_size) break;
				}
				
				if(next >= file_size) break;
				while((ss[next]==' ')||(ss[next]=='\n')||(ss[next]=='\t')){
					//if(next > 409600086) printf("%d\n",(int)ss[next]);
					next++;
					if(next >= file_size) break;
				}
				curr = next;
				edge_count++;
			}
			if(!is_reverse) edge_count /=2;
			cout<<"edge count: "<<edge_count<<endl;
			//cout<<"max vertex id: "<<v_max<<endl;
			//cout<<"min vertex id: "<<v_min<<endl;


			curr=0;
			next=0;

			//step 2. each file size
			//step 3. write to bin file
			char fname[256];
			sprintf(fname, "%s.bin",filename);
			int fd1 = open(fname,O_CREAT|O_RDWR,00666 );
			if(fd1 ==-1) perror("fname open");
			assert(ftruncate(fd1, edge_count*sizeof(packed_edge)) ==0);
			packed_edge* edge = (packed_edge*)mmap(NULL,edge_count*sizeof(packed_edge),PROT_READ|PROT_WRITE,MAP_SHARED,fd1,0);
			close(fd1);
			size_t offset=0;

			//cout<<"step2"<<endl;
			while(offset<edge_count){
				char* sss=ss+curr;
				edge[offset].v0 = (vertex_t) atol(sss);
				edge[offset+1].v1 = (vertex_t) atol(sss);
				//		cout<<curr<<"~"<<edge[offset].v0<<"  ";
				while((ss[next]!=' ')&&(ss[next]!='\n')&&(ss[next]!='\t')){
					//if(next > 409600086) printf("%d\n",(int)ss[next]);
					next++;
					//assert(next < file_size);
				}
				
				while((ss[next]==' ')||(ss[next]=='\n')||(ss[next]=='\t')){
					//if(next > 409600086) printf("%d\n",(int)ss[next]);
					next++;
					//assert(next < file_size);
				}
				curr = next;

				char* sss1=ss+curr;
				edge[offset].v1 = (vertex_t)atol(sss1);
				edge[offset+1].v0 = (vertex_t)atol(sss1);
				//		cout<<curr<<"~"<<edge[offset].v1<<endl;
				while((ss[next]!=' ')&&(ss[next]!='\n')&&(ss[next]!='\t')){
					//if(next > 409600086) printf("%d\n",(int)ss[next]);
					next++;
					//assert(next < file_size);
				}
				while((ss[next]==' ')||(ss[next]=='\n')||(ss[next]=='\t')){
					//if(next > 409600086) printf("%d\n",(int)ss[next]);
					next++;
					//assert(next < file_size);
				}
				curr = next;

				offset+=2;
				//cout<<"step2.5"<<endl;
			}
			munmap(ss,sizeof(char)*file_size );
			munmap(edge,sizeof(packed_edge)*edge_count );
			
			if(is_rm == 1) unlink(filename);
			my_file_beg ++;
		}
	}
}
