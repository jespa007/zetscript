#include "zetscript.h"

int main(){
	zetscript::ScriptEngine se;

	se.compileFileAndRun("file.zs");
	
	return 0;
}
