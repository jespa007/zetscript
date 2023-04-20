#include "zetscript.h"

using zetscript::ZetScript;

// The C function to register that prints parameter value by console
void paramBool(ZetScript *_zs, bool *_bool){
    printf("Result : %s\n",*_bool?"true":"false");
}

int main(){
	ZetScript zs;

    // Register C function
    zs.registerFunction("paramBool",paramBool);

    // Evaluates the script that calls C function with 'true' value as argument
    zs.eval(
        "paramBool(true);"
 	);


 	return 0;
}

