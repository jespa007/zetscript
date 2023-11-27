#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringObject;

// c function
void paramString(ZetScript *_zs, StringObject *_string){
    printf("String : '%s' (StringObject *)\n",_string->get().toConstChar());
}

int main(){
	ZetScript zs;

    zs.registerFunction("paramString",paramString);

    // call c function with string param
    zs.eval(
        "paramString(\"Hello World\");"
 	);


 	return 0;
}

