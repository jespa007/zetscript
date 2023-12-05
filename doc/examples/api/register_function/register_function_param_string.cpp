#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::String;

// c function
void paramString(ZetScript *_zs, String *_string){
    printf("Result : '%s' (String *)\n",_string->toConstChar());
}

int main(){
	ZetScript zs;

    zs.registerFunction("paramString",paramString);

    // call c function 
    zs.compileAndRun(
        "paramString(\"Hello world!\");"
 	);

 	return 0;
}