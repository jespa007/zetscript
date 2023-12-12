#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_int;

// The C function to register that prints the result of adding (_numbe1) + (*_number2)
void add(ScriptEngine *_zs, zs_int _number1, zs_int  *_number2){
    printf("Result _number1 + _number2: %ld\n",_number1 + *_number2);
}

int main(){
	ScriptEngine zs;

    // Registers C function
    zs.registerFunction("add",add);

    // Evaluates the script that calls C function passing '10' and '20' values as arguments
    zs.compileAndRun(
        "add(10,20);"
 	);


 	return 0;
}

