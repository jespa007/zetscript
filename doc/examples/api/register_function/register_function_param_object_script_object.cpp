#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ObjectScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::MapString;

// c function expects an array of integers and floats
void paramObjectScriptObject(ScriptEngine *_script_engine, ObjectScriptObject *_object){
	printf("Values in object:\n");
	auto keys=_object->getKeys();
	for(int i=0; i < keys.length();i++){
        printf(
			"Key: '%s' => Value: %i\n"
			,keys.get(i).toConstChar()
			,(int)_object->get<zs_int>(keys.get(i))
		);
    }
}

int main(){
	ScriptEngine script_engine;

    script_engine.registerFunction("paramObjectScriptObject",paramObjectScriptObject);

    // Calls registered "paramObjectScriptObject" with an Object as a parameter
    script_engine.compileAndRun(
        "paramObjectScriptObject({"
        "    a:0"
        "     ,b:1"
        "     ,c:2"
        "     ,d:3"
        "     ,e:4"

        "});"
 	);


 	return 0;
}
