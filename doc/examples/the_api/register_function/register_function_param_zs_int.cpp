#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

// The C function to register that prints parameter value by console
void paramInt(ZetScript *_zs, zs_int _number){
    printf("Result : %ld\n",_number);
}

int main(){
	ZetScript zs;

    // Register C function
    zs.registerFunction("paramInt",paramInt);

    // Evaluates the script that calls C function with '10' value as argument
    zs.eval(
        "paramInt(10);"
 	);


 	return 0;
}

