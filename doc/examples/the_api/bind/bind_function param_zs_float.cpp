#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_float;

// c function
void funParamFloat(ZetScript *_zs, zs_float *_number){
    printf("Number : %f\n",*_number);
}

int main(){
	ZetScript zs;

    zs.bindFunction("funParamFloat",funParamFloat);

    // call c function 
    zs.eval(
        "funParamFloat(10.5);"
 	);


 	return 0;
}

