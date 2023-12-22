#include "zetscript.h"
// Defines  MyType
class MyType{
//...
};

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers MyType as not instantiable
	script_engine.registerType<MyType>("MyType");

	// tries to instance t as MyType where MyType is NOT instantiable
	try{
		script_engine.compile("var t=new MyType()");
	}catch(zetscript::Exception & _ex){
		printf("%s\n",_ex.what());
	}

	return 0;
}