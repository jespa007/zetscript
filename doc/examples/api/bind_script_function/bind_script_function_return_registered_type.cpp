#include "zetscript.h"

// Class Number to register
class Number{
public:
	float value;
	Number(){
		value=0;
	}
};

// defines new function Number ClassScriptObject
Number *NumberZs_new(
	zetscript::ScriptEngine *_script_engine
){
	return new  Number();
}

void NumberZs_constructor(
	zetscript::ScriptEngine *_script_engine
	, Number *_this
	, zetscript::zs_int _value
){
	_this->value=_value;
}

// defines getter property Number::x ClassScriptObject
zetscript::zs_int NumberZs_get_value(
	zetscript::ScriptEngine *_script_engine
	, Number *_this
){
	return _this->value;
}

// defines delete function Number ClassScriptObject
void NumberZs_delete(
	zetscript::ScriptEngine *_script_engine
	, Number *_this
){
	delete _this;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Register class 'Number' as instantiable
	script_engine.registerType<Number>("Number",NumberZs_new,NumberZs_delete);

	// Register constructor
	script_engine.registerConstructor<Number>(NumberZs_constructor);

	// register property getter Number::value
	script_engine.registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_get_value);

	// Evaluates function 'newNumber' that returns an instance of registered type 'Number'
	script_engine.compile(
		"// 'newNumber' instances a new 'Number' type\n"
		"function newNumber(){\n"
		"   return new Number(10);\n"
		"}\n"
	);

	// It binds 'newNumber' as '(ClassScriptObject *)(void)'
	auto newNumber=script_engine.bindScriptFunction<
		zetscript::ClassScriptObject *()
	>("newNumber");

	// Calls ZetScript function which it returns 'ClassScriptObject *' reference
	auto class_object_number=newNumber();

	// Prints the contents by console.
	printf("From zetscript object : %s\n",class_object_number->toString().toConstChar());

	// Cast C++ 'Number' type pointer
	auto number=class_object_number->to<Number *>();

	// Prints Number's properties by console.
	printf("From C++ pointer type : number->value=%f\n",number->value);

	// 'unrefLifetimeObject' it decreases the reference count of script object to tell is not used anymore
	script_engine.unrefLifetimeObject(class_object_number);
 	return 0;
}