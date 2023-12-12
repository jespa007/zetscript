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
Number *NumberZs_new(ScriptEngine *_se){
	return new  Number();
}

// defines getter property for Number::x
zs_float NumberZs_get_value(ScriptEngine *_se, Number *_this){
	return _this->value;
}

// defines delete function for Number object
void NumberZs_delete(ScriptEngine *_se, Number *_this){
	delete _this;
}

// C function that returns classObject
ClassScriptObject *returnNumber(ScriptEngine *_se){

	// Define script class object
	ClassScriptObject *class_object=NULL;

	// Instances number
	Number *number=new Number();

	// initializes value
	number->value=10;

	// instance new ClassScriptObject using ScriptEngine context and number instance
	class_object=_se->newClassScriptObject(number);

	// return class script object
    return class_object;
}

int main(){
	ScriptEngine se;

	// Register class Number as instanciable
	se.registerScriptType<Number>("Number",NumberZs_new,NumberZs_delete);

	// register property getter Number::value
	se.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);

	// register C function that returns Number ClassScriptObject
    se.registerFunction("returnNumber",returnNumber);

    // Eval script that C function and prints the result by console
    se.compileAndRun(
        "Console::outln(\"result : \"+returnNumber());"
 	);

    return 0;
}