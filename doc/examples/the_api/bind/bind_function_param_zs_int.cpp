#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

// c function
void paramInt(ZetScript *_zs, zs_int _number){
    printf("Result : %i\n",(int)_number);
}

int main(){
	ZetScript zs;

    zs.bindFunction("paramInt",paramInt);

    // call c function with int param
    zs.eval(
        "paramInt(10);"
 	);


 	return 0;
}

