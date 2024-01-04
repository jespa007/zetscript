#include "zetscript.h"

// c function
void paramString(
    zetscript::ScriptEngine *_script_engine
    , const char *_string
){
    printf("Result : '%s' (const char *)\n",_string);
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

