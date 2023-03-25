#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringScriptObject;

// c function
void funParamStringScriptObject(ZetScript *_zs, StringScriptObject *_string){
    printf("String : %i\n",_string->c_str());
}

int main(int argc, char *argv[]){
	ZetScript zs;

    zs.bindFunction("funParamStringScriptObject",funParamStringScriptObject);

    // call c function with string param
    zs.eval(
        "funParamStringScriptObject(\"Hello World\");"
 	);


 	return 0;
}

