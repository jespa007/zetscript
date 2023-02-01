#!/bin/bash

dirs=(integer float string vector object class)

for i in "${dirs[@]}"
do
	dir="examples/the_language/"$i"/*.zs"
	for j in $dir
	do
		base_file="$(basename $j .zs)"
		../build/gcc/Release/zs --no-execution-time "examples/the_language/"$i"/"$base_file".zs" > "examples/the_language/"$i"/"$base_file"_out.txt"
		
	done
done


