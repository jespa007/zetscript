#include "zetscript.h"

int main(){
	zetscript::ScriptEngine script_engine;

	script_engine.compileFileAndRun("file.zs");
	
	return 0;
}
