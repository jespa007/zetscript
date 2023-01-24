#!/bin/bash

dirs=("string")

for i in ${dirs[@]}
do
	for j in "examples/the_language/"$i"/*.zs"
	do
		base_file="$(basename $j .zs)"
		echo "$base_file"
		../build/gcc/Release/zs --no-execution-time "examples/the_language/"$i"/"$base_file".zs" > "examples/the_language/"$i"/"$base_file"_out.txt"
		
	done
done


