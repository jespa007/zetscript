#include "zetscript.h"
int main(){
	zetscript::ScriptEngine script_engine;

	// declares and initializes variable 'i'
	script_engine.compileAndRun("var i=10;");

	// saves current state, so variable 'i' is saved
	script_engine.saveState();

	// Clears current state. It doesn't clear symbol 'i' because it was saved
	script_engine.clear();

	// prints 'i' after clear state
	script_engine.compileAndRun("Console::outln(\"i after clear => \"+i);");
	
	return 0;
}