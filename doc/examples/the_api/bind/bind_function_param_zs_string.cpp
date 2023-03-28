#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_string;

// c function
void funParamString(ZetScript *_zs, zs_string *_string){
    printf("String : '%s'\n",_string->c_str());
}

int main(){
	ZetScript zs;

    zs.bindFunction("funParamString",funParamString);

    // call c function 
    zs.eval(
        "funParamString(\"Hello world\");"
 	);

 	return 0;
}

