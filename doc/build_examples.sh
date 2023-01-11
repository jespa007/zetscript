#!/bin/bash

for i in examples/*.zs
do
	base_file="$(basename -- $i)"
	../build/gcc/Release/zs --no-execution-time $base_file".zs" > "examples/"$base_file".output"
done


