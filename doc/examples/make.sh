#!/bin/bash

EXTENSION_EXE=""

if [ "$(expr substr $(uname -s) 1 7)" = "MSYS_NT" ]
then
	EXTENSION_EXE=".exe"
fi

language_example_dirs=(console integer float string function array object class type loop math json system datetime)

# build output for language examples
for i in "${language_example_dirs[@]}"
do
	dir="language/"$i"/*.zs"
	for j in $dir
	do
		base_file="$(basename $j .zs)"
		../../bin/gcc/Release/zs --no-execution-time "language/"$i"/"$base_file".zs" > "language/"$i"/"$base_file"_out.txt" 
	done
done


#build output for api examples

if ! [ -d api/build ] 
then
	mkdir -p api/build
fi

api_example_dirs=(types bind_script_function register_function register_type)
#api_example_dirs=(types)

dir_zs="api/types/*.zs"
for j in $dir_zs
do
	base_file="$(basename $j .zs)"
	cp "api/types/"$base_file".zs" "api/build/"$base_file".zs"
done

for i in "${api_example_dirs[@]}"
do

	dir_cpp="api/"$i"/*.cpp"
	#dir_cpp="api/"$i"/script_engine*.cpp"
	for j in $dir_cpp
	do
		base_file="$(basename $j .cpp)"
		echo "processing "$base_file".cpp"
		# DEBUG
		#g++ -std=gnu++0x -Wall -pedantic -O0 -g -D__DEBUG__ -D__MEMMANAGER__ "api/"$i"/"$base_file".cpp" "../../../memmgr/memmgr.cpp"  ../../bin/gcc/Debug/libzetscript-2-0-0-static_d.a -o "api/build/"$base_file$EXTENSION_EXE -I ../../src -I ../../../memmgr
		# RELEASE
		g++ -std=gnu++0x -Wall -pedantic -O2 "api/"$i"/"$base_file".cpp" ../../bin/gcc/Release/libzetscript-2-0-0-static.a -o "api/build/"$base_file$EXTENSION_EXE -I ../../src
		if [ $? -eq 0 ] 
		then
			current=$PWD
			cd "api/build"
			./$base_file > "../"$i"/"$base_file"_out.txt"
			cd $current
		fi
	done
done
	




