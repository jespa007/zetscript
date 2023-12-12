#include "zetscript.h"

int main(){
	zetscript::ScriptEngine script_engine;

	script_engine.evalFile("file.zs");
	
	return 0;
}
