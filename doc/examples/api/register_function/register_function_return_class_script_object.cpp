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

// defines getter property for Number::x
zetscript::zs_float NumberZs_get_value(
	zetscript::ScriptEngine *_script_engine
	, Number *_this
){
	return _this->value;
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
	// Define script class object
	zetscript::ClassScriptObject *class_object=NULL;

	// Instances number
	Number *number=new Number();

	// initializes value
	number->value=10;

	// instance new ClassScriptObject using ScriptEngine instance and number instance
	class_object=_script_engine->newClassScriptObject(number);

	// return class script object
    return class_object;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Register class Number as instanciable
	script_engine.registerType<Number>("Number",NumberZs_new,NumberZs_delete);

	// register property getter Number::value
	script_engine.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);

	// register C function that returns Number ClassScriptObject
    script_engine.registerFunction("returnNumber",returnNumber);

    // Eval script that C function and prints the result by console
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnNumber());"
 	);

    return 0;
}