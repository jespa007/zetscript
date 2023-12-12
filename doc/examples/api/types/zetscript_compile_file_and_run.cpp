#include "zetscript.h"

int main(){
	zetscript::ScriptEngine zs;

	zs.compileFileAndRun("file.zs");
	
	return 0;
}
