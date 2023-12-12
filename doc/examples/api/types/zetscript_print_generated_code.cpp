#include "zetscript.h"
int main(){

	zetscript::ScriptEngine script_engine;

	script_engine.compileFileAndRun(
		"file.zs"
	);

	script_engine.printGeneratedCode();
}
