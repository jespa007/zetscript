#include "zetscript.h"

using zetscript::ScriptEngine;
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
Number *NumberZs_new(ScriptEngine *_se){
	return new  Number();
}

void NumberZs_constructor(ScriptEngine *_se, Number *_this, zs_int _value){
	_this->value=_value;
}

// defines getter property Number::x ClassScriptObject
zs_int NumberZs_get_value(ScriptEngine *_se, Number *_this){
	return _this->value;
}

// defines delete function Number ClassScriptObject
void NumberZs_delete(ScriptEngine *_se, Number *_this){
	delete _this;
}

int main(){
	ScriptEngine se;

	// Register class 'Number' as instantiable
	se.registerScriptType<Number>("Number",NumberZs_new,NumberZs_delete);

	// Register constructor
	se.registerConstructor<Number>(NumberZs_constructor);

	// register property getter Number::value
	se.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);

	// Evaluates function 'newNumber' that returns an instance of registered type 'Number'
	se.compile(
		"// 'newNumber' instances a new 'Number' type\n"
		"function newNumber(){\n"
		"   return new Number(10);\n"
		"}\n"
	);

	// It binds 'newNumber' as '(ClassScriptObject *)(void)'
	auto newNumber=se.bindScriptFunction<ClassScriptObject *()>("newNumber");

	// Calls ScriptEngine function which it returns 'ClassScriptObject *' reference
	auto class_object_number=newNumber();

	// Prints the contents by console.
	printf("From zetscript object : %s\n",class_object_number->toString().toConstChar());

	// Cast C++ 'Number' type pointer
	auto number=class_object_number->to<Number *>();

	// Prints Number's properties by console.
	printf("From C++ pointer type : number->value=%f\n",number->value);

	// 'unrefLifetimeObject' it decreases the reference count of script object to tell is not used anymore
	se.unrefLifetimeObject(class_object_number);
 	return 0;
}