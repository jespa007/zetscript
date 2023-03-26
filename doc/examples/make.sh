#!/bin/bash

EXTENSION_EXE=""

if [ "$(expr substr $(uname -s) 1 7)" = "MSYS_NT" ]
then
	EXTENSION_EXE=".exe"
fi

the_language_example_dirs=(integer float string function array object class type loop math json system datetime)

# build output for language examples
for i in "${the_language_example_dirs[@]}"
do
	dir="the_language/"$i"/*.zs"
	for j in $dir
	do
		base_file="$(basename $j .zs)"
		../../build/gcc/Release/zs --no-execution-time "the_language/"$i"/"$base_file".zs" > "the_language/"$i"/"$base_file"_out.txt"
		
	done
done


#build output for api examples

if ! [ -d the_api/build ] 
then
	mkdir -p the_api/build
fi

the_api_example_dirs=(eval bind builtin_type)

for i in "${the_api_example_dirs[@]}"
do
	dir_zs="the_api/"$i"/*.zs"
	for j in $dir_zs
	do
		base_file="$(basename $j .zs)"
		echo file:$base_file
		cp "the_api/"$i"/"$base_file".zs" "the_api/build/"$base_file".zs"
	done

	dir_cpp="the_api/"$i"/*.cpp"
	for j in $dir_cpp
	do
		base_file="$(basename $j .cpp)"
		g++ -std=gnu++0x -Wall -pedantic -O2 "the_api/"$i"/"$base_file".cpp" ../../build/gcc/Release/libzetscript-2-0-0-static.a -o "the_api/build/"$base_file$EXTENSION_EXE -I ../../src
		if [ $? -eq 0 ] 
		then
			current=$PWD
			cd "the_api/build"
			./$base_file > "../"$i"/"$base_file"_out.txt"
			cd $current
		fi
	done
done
	




