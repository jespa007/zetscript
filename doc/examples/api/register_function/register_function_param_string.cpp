#include "zetscript.h"

// c function
void paramString(
    zetscript::ScriptEngine *_script_engine, zetscript::String *_string
){
    printf("Result : '%s' (String *)\n",_string->toConstChar());
}

int main(){
	zetscript::ScriptEngine script_engine;

    script_engine.registerFunction("paramString",paramString);

    // call c function 
    script_engine.compileAndRun(
        "paramString(\"Hello world!\");"
 	);

 	return 0;
}