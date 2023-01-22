#!/bin/bash

dirs=("string")

for i in ${dirs[@]}
do
	echo $i
	echo "examples/the_language/"$i"/*.zs"
	for j in "examples/the_language/"$i"/*.zs"
	do
		echo $j
		base_file="$(basename $j .zs)"
		echo $base_file
		#echo "examples/the_language/"$i"/"$base_file"_out.txt"
		#../build/gcc/Release/zs --no-execution-time "examples/the_language/"$i"/"$base_file".zs" > "examples/the_language/"$i"/"$base_file"_out.txt"
		
	done
done


