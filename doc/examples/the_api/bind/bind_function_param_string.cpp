#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_string;

// c function
void fun_param_zs_string(ZetScript *_zs, zs_string *_string){
    printf("String : %i\n",_string->c_str());
}

int main(int argc, char *argv[]){
	ZetScript zs;

    zs.bindFunction("fun_param_zs_string",fun_param_zs_string);

    // call c function 
    zs.eval(
        "fun_param_zs_string(10);"
 	);


 	return 0;
}

