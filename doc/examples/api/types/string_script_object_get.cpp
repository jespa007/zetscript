#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringScriptObject;

void printString(ZetScript *_zs,StringScriptObject *_string_object){
	printf("%s\n",_string_object->get().toConstChar());
}

int main(){
	zetscript::ZetScript zs;

	// registers printString
	zs.registerFunction("printString",printString);

	// calls printString
	zs.compileAndRun("printString(\"Hello World!\")");
	
	return 0;
}