#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_float;
using zetscript::ClassScriptObject;

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

// defines getter property for Number::x
zs_float NumberZs_get_value(ScriptEngine *_script_engine, Number *_this){
	return _this->value;
}

// defines delete function for Number object
void NumberZs_delete(ScriptEngine *_script_engine, Number *_this){
	delete _this;
}

// C function that returns classObject
ClassScriptObject *returnNumber(ScriptEngine *_script_engine){

	// Define script class object
	ClassScriptObject *class_object=NULL;

	// Instances number
	Number *number=new Number();

	// initializes value
	number->value=10;

	// instance new ClassScriptObject using ScriptEngine instance and number instance
	class_object=new ClassScriptObject(_script_engine,number);

	// return class script object
    return class_object;
}

int main(){
	ScriptEngine script_engine;

	// Register class Number as instanciable
	script_engine.registerScriptType<Number>("Number",NumberZs_new,NumberZs_delete);

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