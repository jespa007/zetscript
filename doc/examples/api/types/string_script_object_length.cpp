#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

void checkStringLength(ScriptEngine *_zs,StringScriptObject *_string_object){
	printf("The length of '%s' is %i\n",_string_object->toString().toConstChar(),_string_object->length());
}

int main(){
	zetscript::ScriptEngine zs;

	// registers checkStringLength
	zs.registerFunction("checkStringLength",checkStringLength);

	// calls checkStringLength
	zs.compileAndRun("checkStringLength(\"Hello World!\")");
	
	return 0;
}