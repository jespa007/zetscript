#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

void checkStringLength(ScriptEngine *_se,StringScriptObject *_string_object){
	printf("The length of '%s' is %i\n",_string_object->toString().toConstChar(),_string_object->length());
}

int main(){
	zetscript::ScriptEngine se;

	// registers checkStringLength
	se.registerFunction("checkStringLength",checkStringLength);

	// calls checkStringLength
	se.compileAndRun("checkStringLength(\"Hello World!\")");
	
	return 0;
}