#include "zetscript.h"

void printStackElementType(
    zetscript::ScriptEngine *_script_engine
    ,zetscript::StackElement *_stack_element
){
    printf("StackElement type is '%s'\n"
        ,_script_engine->stackElementTypeToString(_stack_element).toConstChar()
    );
}

int main(){
	zetscript::ScriptEngine script_engine;

    // Register C function
    script_engine.registerFunction("printStackElementType",printStackElementType);

    // Compiles and runs script
    script_engine.compileAndRun(
        "printStackElementType(1); // pass Integer value to printStackElementType\n"
        "printStackElementType(5.2); // pass Float value to printStackElementType \n"
        "printStackElementType(\"Hello world!\"); // pass String value to printStackElementType \n"
 	);


 	return 0;
}

