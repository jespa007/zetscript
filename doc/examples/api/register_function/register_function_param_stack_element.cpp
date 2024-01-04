#include "zetscript.h"

// This function prints StackElement info
void printStackelementInfo(zetscript::ScriptEngine *_script_engine, zetscript::StackElement * _stk){
    if(_stk->properties & zetscript::STACK_ELEMENT_PROPERTY_INT){
        printf("StackElement type is type 'Integer' with value of '%s'\n",_script_engine->stackElementValueToString(_stk).toConstChar());
    }else if(_stk->properties & zetscript::STACK_ELEMENT_PROPERTY_FLOAT){
        printf("StackElement type is type 'Float' with value of '%s'\n",_script_engine->stackElementValueToString(_stk).toConstChar());
    }else if(_stk->properties & zetscript::STACK_ELEMENT_PROPERTY_OBJECT){
        printf("StackElement type 'Object' with value of '%s'\n",_script_engine->stackElementValueToString(_stk).toConstChar());
    }else{
        fprintf(stderr,"Unsupported type\n");
    }
}

int main(){
	zetscript::ScriptEngine script_engine;

    // Registers C function
    script_engine.registerFunction("printStackelementInfo",printStackelementInfo);

    // Compile and runs script
    script_engine.compileAndRun(
        "printStackelementInfo(10);\n"
        "printStackelementInfo(20.5);\n"
        "printStackelementInfo([0,1,2]);\n"
 	);


 	return 0;
}

