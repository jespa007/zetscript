#include "zetscript.h"

void returnStackElement(zetscript::ScriptEngine *_script_engine){

    // Converts primitive type to stack element
    zetscript::StackElement r1=_script_engine->toStackElement<zetscript::zs_int>(10);
    zetscript::StackElement r2=_script_engine->toStackElement<zetscript::zs_float>(10.5);
    zetscript::StackElement r3=_script_engine->toStackElement<zetscript::StringScriptObject *>(
        new zetscript::StringScriptObject(_script_engine,"Hello World!!!")
    );

    // Push stack stack elements FIFO order (First into stack First out to be readed in ZetScript)
    _script_engine->pushStackElement(r1);
    _script_engine->pushStackElement(r2);
    _script_engine->pushStackElement(r3);
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers function interface
    script_engine.registerFunction("returnStackElement",returnStackElement);

    // Evaluates a script that calls the registered function and prints its result by console
    script_engine.compileAndRun(
        "var r1,r2,r3;\n"
        "r3,r2,r1 = returnStackElement();\n"
        "Console::outln(\"r1:{0} r2:{1} r3:{2}\",r1,r2,r3);"
 	);
    return 0;
}