#include "zetscript.h"

using zetscript::ZetScript;
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
Number *NumberZs_new(ZetScript *_zs){
	return new  Number();
}

// defines getter property for Number::x
zs_float NumberZs_get_value(ZetScript *_zs, Number *_this){
	return _this->value;
}

// defines delete function for Number object
void NumberZs_delete(ZetScript *_zs, Number *_this){
	delete _this;
}

// C function that returns classScriptObject
ClassScriptObject *returnNumber(ZetScript *_zs){

	// Define script class object
	ClassScriptObject *class_script_object=NULL;

	// Instances number
	Number *number=new Number();

	// initializes value
	number->value=10;

	// instance new ClassScriptObject using ZetScript context and number instance
	class_script_object=_zs->newClassScriptObject(number);

	// return class script object
    return class_script_object;
}

int main(){
	ZetScript zs;

	// Register class Number as instanciable
	zs.registerType<Number>("Number",NumberZs_new,NumberZs_delete);

	// register property getter Number::value
	zs.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);

	// register C function that returns Number ClassScriptObject
    zs.registerFunction("returnNumber",returnNumber);

    // Eval script that C function and prints the result by console
    zs.eval(
        "Console::outln(\"result : \"+returnNumber());"
 	);

    return 0;
}