#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_float;

// C++ class to be registered
class Number{
public:
	float value;

	Number(){
		value=0;
	}
};

// defines getter property for Number::value
zs_float NumberZs_get_value(ScriptEngine *_zs, Number *_this){
	return _this->value;
}

Number *_number=NULL;

// C function that returns a Number type pointer
Number *returnNumber(ScriptEngine *_zs){

	// return global _number
    return _number;
}

int main(){
	ScriptEngine zs;

	// Creates number and init its field value
	_number=new Number();
	_number->value=10;

	// Registers class Number as non instantiable type
	zs.registerScriptType<Number>("Number");

	// registers property getter Number::value
	zs.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);

	// registers C function that returns a Number type pointer
    zs.registerFunction("returnNumber",returnNumber);

    // Eval script that C function and prints the result by console
    zs.compileAndRun(
        "Console::outln(\"result : \"+returnNumber());"
 	);

	// deletes _number
	delete _number;

    return 0;
}
