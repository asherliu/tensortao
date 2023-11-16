#!/bin/bash

echo "Input: /path/to/bash /path/to/this_bash/ /path/to/text_tuple_file line_skip num_row_partition num_col_partition num_threads reverse_the_edge(1 yes, 2 no) remove_intermediate_file(1 yes, 2 no)"

if [ ! $# -eq 8 ];
then
    echo Input wrong
    exit
fi

path=$1
file=$2
skip_line=$3
num_row=$4
num_col=$5
num_threads=$6
is_reverse=$7
is_remove=$8


if [ ! -f $file ];
then
    echo File **$file** does not exist
    exit
fi

if [ ! -f "$path/split_rename.bash" ];
then
    echo **split_rename.bash** is not at $path/split_rename.bash!
    exit
fi

if [ ! -f "$path/tuple_to_bin.multithread/text_to_bin.bin" ];
then
    echo $path/tuple_to_bin.multithread/text_to_bin.bin not exist!
    exit
fi

if [ ! -f "$path/tuple_to_bin.multithread/text_to_bin.bin" ];
then
    echo $path/tuple_to_bin.multithread/text_to_bin.bin not exist!
    exit
fi


echo "Skip lines ..."

rm -rf "$file".skip
skip_line=$((skip_line+1))
echo "tail -n +$skip_line $file"
tail -n +$skip_line $file &>> "$file".skip


$path/split_rename.bash "$file".skip 
if [ $is_remove -eq 1 ];
then
    rm -rf "$file".skip
fi


$path/tuple_to_bin.multithread/text_to_bin.bin "$file".skip-split 200 $num_threads $is_reverse $is_remove



$path/multi_bin_to_2d_csr/multi_bin_to_2d_csr.bin "$file".skip-split 200 $num_row $num_col $num_threads
if [ $is_remove -eq 1 ];
then
    #rm -rf "$file".skip-split-\d{5}.bin
    rm -rf "$file".skip-split-*.bin
fi

echo
echo The CSR format files are below: 
for((i = 0; i< num_row; i++));do
    for((j=0;j<num_col;j++));do
        echo "$file".skip-split_beg."$i"_"$j"_of_"$num_row"x"$num_col".bin
        echo "$file".skip-split_csr."$i"_"$j"_of_"$num_row"x"$num_col".bin
    done
done

