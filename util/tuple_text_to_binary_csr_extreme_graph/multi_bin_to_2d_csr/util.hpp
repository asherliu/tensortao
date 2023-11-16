#ifndef _UTIL_H_
#define _UTIL_H_
template<typename index_t>
struct struct_ranger_beg
{
	index_t row_ranger_beg;
	index_t col_ranger_beg;
};


//assuming both row and colum are aligned
template<typename vertex_t, typename index_t>
inline int aligned_par(vertex_t *ranger, int num_par, vertex_t vert )
{
	for(int i = 0; i < num_par+1; i ++)
		if(vert < ranger[i]) return i-1;
	printf("err\n");
}

//assuming both row and colum are aligned
template<typename vertex_t, typename index_t>
inline int misaligned_col(vertex_t *col_ranger, int num_cols, vertex_t dest , int aligned_row)
{
	const int base = aligned_row * (num_cols + 1);
	for(int i = 0; i < num_cols + 1; i ++)
		if(dest < col_ranger[base + i]) return i-1;
	printf("err\n");
}

//assuming row-wise aligned
template<typename vertex_t, typename index_t>
inline int aligned_row(struct_ranger_beg<index_t> **st_beg, int num_rows, vertex_t src)
{
	for(int i = 0; i < num_rows + 1; i ++)
		if(src < st_beg[i][0].row_ranger_beg) return i - 1;
	printf("err\n");
}

//assuming row-wise aligned
template<typename vertex_t, typename index_t>
inline void my_row_col(struct_ranger_beg<index_t> **st_beg, int num_rows, int num_cols, vertex_t src, vertex_t dest, int &my_row, int &my_col)
{
	for(int i = 0; i < num_rows + 1; i ++)
		if(src < st_beg[i][0].row_ranger_beg)
		{
			my_row = i - 1;
			for(int j = 0; j < num_cols + 1; j ++)
			{
				if(dest < st_beg[i][j].col_ranger_beg)
				{
					my_col = j - 1;
					return;
				}
			}
		}
	printf("err\n");
}

#endif

