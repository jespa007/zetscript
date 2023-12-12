#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_int;

// The C function to register that prints the result of adding (_numbe1) + (*_number2)
void add(ScriptEngine *_script_engine, zs_int _number1, zs_int  *_number2){
    printf("Result _number1 + _number2: %ld\n",_number1 + *_number2);
}

int main(){
	ScriptEngine script_engine;

    // Registers C function
    script_engine.registerFunction("add",add);

    // Evaluates the script that calls C function passing '10' and '20' values as arguments
    script_engine.eval(
        "add(10,20);"
 	);


 	return 0;
}

