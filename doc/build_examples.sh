#!/bin/bash

for i in examples/the_language/*.zs
do
	base_file="$(basename $i .zs)"
	../build/gcc/Release/zs --no-execution-time "examples/the_language/"$base_file".zs" > "examples/the_language/"$base_file"_out.txt"
done


