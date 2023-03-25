#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

// c function
void funParamInt(ZetScript *_zs, zs_int _number){
    printf("Number : %lu\n",_number);
}

int main(int argc, char *argv[]){
	ZetScript zs;

    zs.bindFunction("funParamInt",funParamInt);

    // call c function with int param
    zs.eval(
        "funParamInt(10);"
 	);


 	return 0;
}

