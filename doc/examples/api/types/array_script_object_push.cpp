#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::ArrayObject;

ArrayObject *returnNewArray(ZetScript *_zs){

	// instance new ArrayObject using ZetScript context
	ArrayObject *array_object=new ArrayObject(_zs);

	// push first value as integer 10
	array_object->push<zs_int>(10);

	// push second value as float 5.5
	array_object->push<zs_float>(5.5);

	// push third value as boolean true
	array_object->push<bool>(true);

	// push 4rth reference string "Hello Word"
	array_object->push<const char *>("Hello World");

	// return object array
    return array_object;
}

int main(){
	zetscript::ZetScript zs;

	// registers returnNewArray
	zs.registerFunction("returnNewArray",returnNewArray);

	// prints the value returned by returnNewArray
	zs.eval("Console::outln(returnNewArray())");
	
	return 0;
}