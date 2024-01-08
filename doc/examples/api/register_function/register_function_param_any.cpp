#include "zetscript.h"

void paramAny(
    zetscript::ScriptEngine *_script_engine
    ,zetscript::StackElement *_stack_element
){
    printf("StackElement passed is type '%s' with value of '%s'\n"
        ,_script_engine->stackElementTypeToString(_stack_element).toConstChar()
        ,_script_engine->stackElementValueToString(_stack_element).toConstChar()
    );
}

int main(){
	zetscript::ScriptEngine script_engine;

    // Register C function
    script_engine.registerFunction("paramAny",paramAny);

    // Compiles and runs script
    script_engine.compileAndRun(
        "paramAny(1); // pass Integer value to paramAny\n"
        "paramAny(5.2); // pass Float value to paramAny \n"
        "paramAny(\"Hello world!\"); // pass String value to paramAny \n"
 	);


 	return 0;
}

