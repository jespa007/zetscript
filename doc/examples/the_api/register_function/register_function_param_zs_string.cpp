#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_string;

// c function
void paramString(ZetScript *_zs, zs_string *_string){
    printf("Result : '%s' (zs_string *)\n",_string->c_str());
}

int main(){
	ZetScript zs;

    zs.registerFunction("paramString",paramString);

    // call c function 
    zs.eval(
        "paramString(\"Hello world!\");"
 	);

 	return 0;
}