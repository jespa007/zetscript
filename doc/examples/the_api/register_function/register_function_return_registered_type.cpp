#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_float;

// C++ class to be registered
class Number{
public:
	float value;

	Number(){
		value=0;
	}
};

//------------------------------
// REGISTER FUNCTIONS

// defines getter property for Number::value
zs_float NumberZs_get_value(ZetScript *_zs, Number *_this){
	return _this->value;
}

// REGISTER FUNCTIONS
//------------------------------

Number *_number=NULL;

// C function that returns a Number type pointer
Number *returnNumber(ZetScript *_zs){

	// return global _number
    return _number;
}

int main(){
	ZetScript zs;

	// Creates number and init its field value
	_number=new Number();
	_number->value=10;

	// Registers class Number as non instantiable type
	zs.registerType<Number>("Number");

	// registers property getter Number::value
	zs.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);

	// registers C function that returns a Number type pointer
    zs.registerFunction("returnNumber",returnNumber);

    // Eval script that C function and prints the result by console
    zs.eval(
        "Console::outln(\"result : \"+returnNumber());"
 	);

	// deletes _number
	delete _number;

    return 0;
}
