#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ClassScriptObject;
using zetscript::zs_int;

// Class Number to register
class Number{
public:
	float value;
	Number(){
		value=0;
	}
};

// defines new function Number ClassScriptObject
Number *NumberZs_new(ZetScript *_zs){
	return new  Number();
}

void NumberZs_constructor(ZetScript *_zs, Number *_this, zs_int _value){
	_this->value=_value;
}

// defines getter property Number::x ClassScriptObject
zs_int NumberZs_get_value(ZetScript *_zs, Number *_this){
	return _this->value;
}

// defines delete function Number ClassScriptObject
void NumberZs_delete(ZetScript *_zs, Number *_this){
	delete _this;
}

int main(){
	ZetScript zs;

	// Register class 'Number' as instantiable
	zs.registerType<Number>("Number",NumberZs_new,NumberZs_delete);

	// Register constructor
	zs.registerConstructor<Number>(NumberZs_constructor);

	// register property getter Number::value
	zs.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);

	// Evaluates function 'newNumber' that returns an instance of registered type 'Number'
	zs.eval(
		"// 'newNumber' instances a new 'Number' type\n"
		"function newNumber(){\n"
		"   return new Number(10);\n"
		"}\n"
	);

	// It binds 'newNumber' as '(ClassScriptObject *)(void)'
	auto newNumber=zs.bindScriptFunction<ClassScriptObject *()>("newNumber");

	// Calls ZetScript function which it returns 'ClassScriptObject *' reference
	auto class_script_object_number=newNumber();

	// Prints the contents by console.
	printf("From zetscript object : %s\n",class_script_object_number->toString().c_str());

	// Cast C++ 'Number' type pointer
	auto number=class_script_object_number->to<Number *>();

	// Prints Number's properties by console.
	printf("From C++ pointer type : number->value=%f\n",number->value);

	// 'unrefLifetimeObject' it decreases the reference count of script object to tell is not used anymore
	zs.unrefLifetimeObject(class_script_object_number);
 	return 0;
}