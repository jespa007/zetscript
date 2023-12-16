#include "zetscript.h"
int main(){

	zetscript::ScriptEngine script_engine;

	script_engine.compile(
		"function add(_o1,_o2){\n"
		"	return _o1+_o2;\n"
		"}\n"
	);

	script_engine.compile(
		"var b=add(2,3)"
	);

	script_engine.compile(
		"Console::outln(\"b = \"+b)"
	);

	script_engine.run();

	return 0;
}