#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ObjectScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;

// print array contents
void printObject(ZetScript *_zs,ObjectScriptObject *_object){
	printf("ScriptObject contents : %s",_object->toString().toConstChar());
}

int main(){
	zetscript::ZetScript zs;

	// registers printObject
	zs.registerFunction("printObject",printObject);

	// calls printObject
	zs.compileAndRun(
        "printObject({"
            "\"key1\":0"                 
            ",\"key2\":10.5"             
            ",\"key3\":\"Hello World!\"" 
        "})");
	
	return 0;
}