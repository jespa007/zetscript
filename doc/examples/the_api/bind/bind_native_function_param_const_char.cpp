#include "zetscript.h"

using zetscript::ZetScript;


// c function
void paramString(ZetScript *_zs, const char *_string){
    printf("Result : %s (const char *)\n",_string);
}

int main(){
	ZetScript zs;

    zs.bindFunction("paramString",paramString);

    // call c function 
    zs.eval(
        "paramString(\"Hello world!\");"
 	);


 	return 0;
}

