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

	Number(float _value){
		value=_value;
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

int main()
{
	ZetScript zs;

   // Register class Number
	zs.registerScriptType<Number>("Number");

	// register property getter Number::x
	zs.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);


    // Evaluates ZetScript function 'paramNumber' that prints the contents of '_number'
    zs.eval(
 		"function paramNumber(_number){\n"
        "    Console::outln(\"result : \"+_number);\n"
        "}\n"
 	);

    // It binds 'concat' as '(void *)(ArrayScriptObject *, ArrayScriptObject *)'
    auto paramNumber=zs.bindFunction<void(Number * _number)>("paramNumber");

    // Prepare parameters
    auto number=Number(10);

    // Calls binded ZetScript function with parameters
    paramNumber(&number);


 	return 0;
}

