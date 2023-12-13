#include "zetscript.h"
int main(){

	zetscript::ScriptEngine script_engine;

	script_engine.compile(
		"function sum(_a,_b){\n"
		" return _a+_b\n"	
		"}\n"
		"\n"
		"Console::outln(\"result: {0}\",sum(5+10))\n"
	);

	script_engine.printGeneratedCode();
}
