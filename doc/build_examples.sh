#!/bin/bash

for i in examples/*.zs
do
	base_file="$(basename $i .zs)"
	../build/gcc/Release/zs --no-execution-time "examples/"$base_file".zs" > "examples/"$base_file".output"
done


