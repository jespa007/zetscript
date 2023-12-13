#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::String;

// c function
void paramString(ScriptEngine *_script_engine, String *_string){
    printf("Result : '%s' (String *)\n",_string->toConstChar());
}

int main(){
	ScriptEngine script_engine;

    script_engine.registerFunction("paramString",paramString);

    // call c function 
    script_engine.compileAndRun(
        "paramString(\"Hello world!\");"
 	);

 	return 0;
}