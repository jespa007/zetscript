#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::String;

// c function
void paramString(ScriptEngine *_se, String *_string){
    printf("Result : '%s' (String *)\n",_string->toConstChar());
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