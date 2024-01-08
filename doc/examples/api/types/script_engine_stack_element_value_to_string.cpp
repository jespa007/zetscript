#include "zetscript.h"

void printStackElementValue(
    zetscript::ScriptEngine *_script_engine
    ,zetscript::StackElement *_stack_element
){
    printf("StackElement value is '%s'\n"
        ,_script_engine->stackElementValueToString(_stack_element).toConstChar()
    );
}

int main(){
	zetscript::ScriptEngine script_engine;

    // Register C function
    script_engine.registerFunction("printStackElementValue",printStackElementValue);

    // Compiles and runs script
    script_engine.compileAndRun(
        "printStackElementValue(1); // pass Integer value to printStackElementValue\n"
        "printStackElementValue(5.2); // pass Float value to printStackElementValue \n"
        "printStackElementValue(\"Hello world!\"); // pass String value to printStackElementValue \n"
 	);


 	return 0;
}

