#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

// c function
void paramString(ScriptEngine *_script_engine, StringScriptObject *_string){
    printf("String : '%s' (StringScriptObject *)\n",_string->get().toConstChar());
}

int main(){
	ScriptEngine script_engine;

    script_engine.registerFunction("paramString",paramString);

    // call c function with string param
    script_engine.eval(
        "paramString(\"Hello World\");"
 	);


 	return 0;
}

