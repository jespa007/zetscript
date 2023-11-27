#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringObject;

void printString(ZetScript *_zs,StringObject *_string_object){
	printf("%s\n",_string_object->getConstChar());
}

int main(){
	zetscript::ZetScript zs;

	// registers printString
	zs.registerFunction("printString",printString);

	// calls printString
	zs.eval("printString(\"Hello World!\")");
	
	return 0;
}