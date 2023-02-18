#!/bin/bash

EXTENSION_EXE=""

if [ "$(expr substr $(uname -s) 1 7)" = "MSYS_NT" ]
then
	EXTENSION_EXE=".exe"
fi

the_language_example_dirs=(integer float string function vector object class type loop)

# build output for language examples
for i in "${the_language_example_dirs[@]}"
do
	dir="examples/the_language/"$i"/*.zs"
	for j in $dir
	do
		base_file="$(basename $j .zs)"
		../build/gcc/Release/zs --no-execution-time "examples/the_language/"$i"/"$base_file".zs" > "examples/the_language/"$i"/"$base_file"_out.txt"
		
	done
done


#build output for api examples

if ! [ -d examples/the_api/build ] 
then
	mkdir -p examples/the_api/build
fi

the_api_example_dirs=(function type)

for i in "${the_api_example_dirs[@]}"
do
	dir="examples/the_api/"$i"/*.cpp"
	for j in $dir
	do
		base_file="$(basename $j .cpp)"
		g++ -std=gnu++0x -Wall -pedantic -O2 "examples/the_api/"$i"/"$base_file".cpp" ../build/gcc/Release/libzetscript-2-0-0-static.a -o "examples/the_api/build/"$base_file$EXTENSION_EXE -I ../src
		if [ $? -eq 0 ] 
		then
			"examples/the_api/build/"$base_file > "examples/the_api/"$i"/"$base_file"_out.txt" 
		fi
	done
done
	




