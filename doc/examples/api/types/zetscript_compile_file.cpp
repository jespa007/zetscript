#include "zetscript.h"
int main(){
	zetscript::ScriptEngine se;

	se.compileFile("file.zs");
	
	return 0;
}
