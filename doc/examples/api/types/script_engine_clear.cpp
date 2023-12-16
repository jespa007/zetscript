#include "zetscript.h"
int main(){
	zetscript::ScriptEngine script_engine;

	script_engine.compileAndRun("var i=10;");

	// Prints the value of symbol 'i'
	script_engine.compileAndRun("Console::outln(\"i => \"+i);");

	// Clears current state. After clear, symbol 'i' doesn't exist anymore
	script_engine.clear();

	// throws an error that symbol 'i' does not exist
	try{
		script_engine.compileAndRun("Console::outln(\"i => \"+i);");
	}catch(zetscript::Exception & ex){
		printf("Error: %s\n",ex.what());
	}
	
	return 0;
}