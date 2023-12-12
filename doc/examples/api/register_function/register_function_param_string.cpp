#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::String;

// c function
void paramString(ScriptEngine *_zs, String *_string){
    printf("Result : '%s' (String *)\n",_string->toConstChar());
}

int main(){
	ScriptEngine zs;

    zs.registerFunction("paramString",paramString);

    // call c function 
    zs.compileAndRun(
        "paramString(\"Hello world!\");"
 	);

 	return 0;
}