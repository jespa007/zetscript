#include "zetscript.h"

// c function expects an array of integers and floats
void paramObjectScriptObject(
    zetscript::ScriptEngine *_script_engine
    ,zetscript::ObjectScriptObject *_object
){
	printf("Values in object:\n");
	auto keys=_object->getKeys();
	for(int i=0; i < keys.length();i++){
        printf(
			"Key: '%s' => Value: %i\n"
			,keys.get(i).toConstChar()
			,(int)_object->get<zetscript::zs_int>(keys.get(i))
		);
    }
}

int main(){
	zetscript::ScriptEngine script_engine;

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

