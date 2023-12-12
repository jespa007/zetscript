#include "zetscript.h"

using zetscript::ScriptEngine;


// c function
void paramString(ScriptEngine *_se, const char *_string){
    printf("Result : '%s' (const char *)\n",_string);
}

int main(){
	ScriptEngine se;

    se.registerFunction("paramString",paramString);

    // call c function 
    se.compileAndRun(
        "paramString(\"Hello world!\");"
 	);


 	return 0;
}

