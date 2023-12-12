#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ObjectScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::MapString;

// c function expects an array of integers and floats
void paramObjectScriptObject(ScriptEngine *_zs, ObjectScriptObject *_object){
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
	ScriptEngine zs;

    zs.registerFunction("paramObjectScriptObject",paramObjectScriptObject);

    // eval and call bind function
    zs.compileAndRun(
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

