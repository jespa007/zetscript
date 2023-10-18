#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringScriptObject;

void checkStringLength(ZetScript *_zs,StringScriptObject *_string_object){
	printf("The length of '%s' is %i\n",_string_object->toString().c_str(),_string_object->length());
}

int main(){
	zetscript::ZetScript zs;

	// registers checkStringLength
	zs.registerFunction("checkStringLength",checkStringLength);

	// calls checkStringLength
	zs.eval("checkStringLength(\"Hello World!\")");
	
	return 0;
}