#include "zetscript.h"

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
zetscript::zs_float NumberZs_get_value(
	zetscript::ScriptEngine *_script_engine
	, Number *_this
){
	return _this->value;
}

// REGISTER FUNCTIONS
//------------------------------

int main()
{
	zetscript::ScriptEngine script_engine;

   // Register class Number
	script_engine.registerType<Number>("Number");

	// register property getter Number::x
	script_engine.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);


    // Evaluates ZetScript function 'paramNumber' that prints the contents of '_number'
    script_engine.compile(
 		"function paramNumber(_number){\n"
        "    Console::outln(\"result : \"+_number);\n"
        "}\n"
 	);

    // It binds 'concat' as '(void *)(ArrayScriptObject *, ArrayScriptObject *)'
    auto paramNumber=script_engine.bindScriptFunction<void(Number * _number)>("paramNumber");

    // Prepare parameters
    auto number=Number(10);

    // Calls binded ZetScript function with parameters
    paramNumber(&number);


 	return 0;
}

