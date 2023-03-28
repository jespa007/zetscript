#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::zs_string;
using zetscript::ArrayScriptObject;
using zetscript::ObjectScriptObject;

// Definition of the native function interface returnObject
ObjectScriptObject *returnObject(ZetScript *_zs){
	// instance new ObjectScriptObject using ZetScript context
	ObjectScriptObject *object=_zs->newObjectScriptObject();

	// instance new ArrayScriptObject using ZetScript context
	ArrayScriptObject *array=_zs->newArrayScriptObject();

	// set field "a" as integer 10
	object->set<zs_int>("a",10);

	// set field "b" as float 5.5
	object->set<zs_float>("b",5.5);

	// set field "c" as boolean true
	object->set<bool>("c",true);

	// set field "d" as string
	object->set<const char *>("d","Hello World");

	// initialize vector object and set as "d"
	for(int i=0; i < 10; i++){
		array->push<zs_int>(i);
	}

	object->set<ArrayScriptObject *>("d",array);


    return object;
}

int main(){
	ZetScript zs;

	// bind native function returnObject named as 'returnObject'
    zs.bindFunction("returnObject",returnObject);

    // Eval script that calls native function 'returnObject'
    zs.eval(
        "Console::outln(\"result : \"+returnObject());"
 	);

    return 0;
}


