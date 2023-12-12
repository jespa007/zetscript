#include "zetscript.h"

using zetscript::ScriptEngine;


// c function
void paramString(ScriptEngine *_zs, const char *_string){
    printf("Result : '%s' (const char *)\n",_string);
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

