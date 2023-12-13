#include "zetscript.h"

using zetscript::ScriptEngine;


// c function
void paramString(ScriptEngine *_script_engine, const char *_string){
    printf("Result : '%s' (const char *)\n",_string);
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

