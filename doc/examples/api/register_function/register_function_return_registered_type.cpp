#include "zetscript.h"

// C++ class to be registered
class Number{
public:
	float value=0;
};

// Defines getter property for Number::value
zetscript::zs_float NumberZs_get_value(
	zetscript::ScriptEngine *_script_engine
	, Number *_this
){
	return _this->value;
}

Number *_number=NULL;

// C function that returns a Number type pointer
Number *returnNumber(
	zetscript::ScriptEngine *_script_engine
){
	// return global _number
    return _number;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Creates number and init its field value
	_number=new Number();
	_number->value=10;

	// Registers class Number as non instantiable type
	script_engine.registerType<Number>("Number");

	// registers property getter Number::value
	script_engine.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);

	// registers C function that returns a Number type pointer
    script_engine.registerFunction("returnNumber",returnNumber);

    // Eval script that C function and prints the result by console
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnNumber());"
 	);

	// Deletes _number
	delete _number;

    return 0;
}
