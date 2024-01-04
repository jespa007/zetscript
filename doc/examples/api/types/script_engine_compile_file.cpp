#include "zetscript.h"
int main(){
	zetscript::ScriptEngine script_engine;

	script_engine.compileFile("file.zs");
	
	return 0;
}
