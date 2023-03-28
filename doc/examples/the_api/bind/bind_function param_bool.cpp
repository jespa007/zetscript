#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

// c function
void paramBool(ZetScript *_zs, bool *_bool){
    printf("Result : %s\n",*_bool?"true":"false");
}

int main(){
	ZetScript zs;

    zs.bindFunction("paramBool",paramBool);

    // call c function with int param
    zs.eval(
        "paramBool(true);"
 	);


 	return 0;
}

