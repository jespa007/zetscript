#include "zetscript.h"

// The C function to register that prints the result of adding (_numbe1) + (*_number2)
void add(
    zetscript::ScriptEngine *_script_engine
    ,zetscript::zs_int _number1
    ,zetscript::zs_int  *_number2
){
    printf("Result _number1 + _number2: %ld\n",(long int)_number1 + *_number2);
}

int main(){
	zetscript::ScriptEngine script_engine;

    // Registers C function
    script_engine.registerFunction("add",add);

    // Evaluates the script that calls C function passing '10' and '20' values as arguments
    script_engine.compileAndRun(
        "add(10,20);"
 	);


 	return 0;
}

