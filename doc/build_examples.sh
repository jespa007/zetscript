#!/bin/bash

for i in examples/language/*.zs
do
	base_file="$(basename $i .zs)"
	../build/gcc/Release/zs --no-execution-time "examples/language/"$base_file".zs" > "examples/language/"$base_file"_out.txt"
done


