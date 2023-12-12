#include "zetscript.h"
int main(){
	zetscript::ScriptEngine zs;

	zs.compileFile("file.zs");
	
	return 0;
}
