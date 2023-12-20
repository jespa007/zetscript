#include "zetscript.h"

using zetscript::ScriptEngine;

// Defines  MyType
class MyType{
//...
};

// defines global variable my_type
MyType *my_type=NULL;

MyType *MyTypeZs_new(ScriptEngine *_script_engine){
	return new MyType();
}

void MyTypeZs_delete(ScriptEngine *_script_engine,MyType *_this){
	delete _this;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Instances MyType
	my_type=new MyType();

	// Registers type 'MyType' as instantiable
	script_engine.registerType<MyType>("MyType",MyTypeZs_new,MyTypeZs_delete);

	// Instances an object of type 'MyType' and stores in 't'
	script_engine.compileAndRun(
		"var t=new MyType()"
		"Console::outln(\"typeof t =>\"+(typeof t));"
	);

	delete my_type;

	return 0;
}