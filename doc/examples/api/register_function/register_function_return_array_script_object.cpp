#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ArrayObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;


// Definition of the native function interface returnArray
ArrayObject *returnArray(ZetScript *_zs){

	// instance new ArrayObject using ZetScript context
	ArrayObject *array=_zs->newArrayObject();

	// push first value as integer 10
	array->push<zs_int>(10);

	// push second value as float 5.5
	array->push<zs_float>(5.5);

	// push third value as boolean true
	array->push<bool>(true);

	// push 4rth reference string "Hello Word"
	array->push<const char *>("Hello World");

	// return object array
    return array;
}

int main(){
	ZetScript zs;

	// bind native function returnArray named as 'returnArray'
    zs.registerFunction("returnArray",returnArray);

    // Eval script that calls native function 'returnArray'
    zs.eval(
        "Console::outln(\"result : \"+returnArray());"
 	);

    return 0;
}


