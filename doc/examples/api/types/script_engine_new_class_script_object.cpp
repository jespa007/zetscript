#include "zetscript.h"

// C++ class to be registered
class Number{
public:
	float value;

	Number(){
		value=0;
	}
};

// defines new function for Number object
Number *NumberZs_new(
	zetscript::ScriptEngine *_script_engine
){
	return new  Number();
}

// defines delete function for Number object
void NumberZs_delete(
	zetscript::ScriptEngine *_script_engine
	, Number *_this
){
	delete _this;
}

// C function that returns classObject
zetscript::ClassScriptObject *returnNumber(
	zetscript::ScriptEngine *_script_engine
){
	// instance new ClassScriptObject using ScriptEngine instance and number instance
	return _script_engine->newClassScriptObject(new Number());
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Register class Number as instanciable
	script_engine.registerType<Number>("Number",NumberZs_new,NumberZs_delete);

	// register C function that returns Number ClassScriptObject
    script_engine.registerFunction("returnNumber",returnNumber);

    // Eval script that C function and prints the result by console
    script_engine.compileAndRun(
        "Console::outln(\"return typeOf : \"+(typeof returnNumber()));"
 	);

    return 0;
}