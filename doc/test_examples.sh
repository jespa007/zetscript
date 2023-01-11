#!bin/bash

var=$(../build/gcc/Release/zs --no-execution-time examples/string_length.zs)
echo "$var"
if [ "$var" = "`cat examples/string_length.output`" ]
then
	echo OK
else
	echo ERROR
fi
var=$(../build/gcc/Release/zs --no-execution-time examples/loop_for_1.zs)
if [ "$var" = "`cat examples/loop_for_1.output`" ]
then
	echo OK
else
	echo ERROR
fi
echo "$var"