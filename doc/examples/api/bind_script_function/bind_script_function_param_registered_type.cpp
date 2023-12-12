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

	Number(float _value){
		value=_value;
	}
};

//------------------------------
// REGISTER FUNCTIONS

// defines getter property for Number::value
zs_float NumberZs_get_value(ScriptEngine *_se, Number *_this){
	return _this->value;
}

// REGISTER FUNCTIONS
//------------------------------

int main()
{
	ScriptEngine se;

   // Register class Number
	se.registerScriptType<Number>("Number");

	// register property getter Number::x
	se.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);


    // Evaluates ScriptEngine function 'paramNumber' that prints the contents of '_number'
    se.compile(
 		"function paramNumber(_number){\n"
        "    Console::outln(\"result : \"+_number);\n"
        "}\n"
 	);

    // It binds 'concat' as '(void *)(ArrayScriptObject *, ArrayScriptObject *)'
    auto paramNumber=se.bindScriptFunction<void(Number * _number)>("paramNumber");

    // Prepare parameters
    auto number=Number(10);

    // Calls binded ScriptEngine function with parameters
    paramNumber(&number);


 	return 0;
}

