#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_float;

// c function
void funParamFloat(ZetScript *_zs, zs_float *_number){
    printf("Result : %f\n",*_number);
}

int main(){
	ZetScript zs;

    zs.registerFunction("funParamFloat",funParamFloat);

    // call c function 
    zs.eval(
        "funParamFloat(10.5);"
 	);


 	return 0;
}

