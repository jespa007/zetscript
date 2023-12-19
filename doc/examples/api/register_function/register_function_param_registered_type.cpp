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

// defines new function for Number object
Number *NumberZs_new(ScriptEngine *_script_engine){
	return new  Number();
}

// defines setter property for Number::x
void NumberZs_set_value(ScriptEngine *_script_engine, Number *_this, zs_float *_value){
	_this->value=*_value;
}

// defines getter property for Number::x
zs_float NumberZs_get_value(ScriptEngine *_script_engine, Number *_this){
	return _this->value;
}

// defines delete function for Number object
void NumberZs_delete(ScriptEngine *_script_engine, Number *_this){
	delete _this;
}

// C function the accepts native Number
void mul10Number(ScriptEngine *_script_engine, Number *_number){
	// initialize x and y
	_number->value*=10;
}

int main(){
	ScriptEngine script_engine;

	// Register class Number
	script_engine.registerType<Number>("Number",NumberZs_new,NumberZs_delete);

	// Register property setter Number::x
	script_engine.registerMemberPropertyMetamethod<Number>("value","_set",NumberZs_set_value);

	// Register property getter Number::x
	script_engine.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);

	// Register native function mulNumber named as 'mulNumber'
    script_engine.registerFunction("mul10Number",mul10Number);

    // Eval script that calls native function 'mulNumber'
    script_engine.compileAndRun(
        "var number=new Number();\n"
        "number.value=10;\n"
        "Console::outln(\"before : \"+number);\n"
        "mul10Number(number)\n"
        "Console::outln(\"after call 'mul10Number': \"+number);"
 	);

    return 0;
}