#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ObjectObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;

// print array contents
void printObject(ZetScript *_zs,ObjectObject *_object){
	printf("Object contents : %s",_object->toString().toConstChar());
}

int main(){
	zetscript::ZetScript zs;

	// registers printObject
	zs.registerFunction("printObject",printObject);

	// calls printObject
	zs.eval(
        "printObject({"
            "\"key1\":0"                 
            ",\"key2\":10.5"             
            ",\"key3\":\"Hello World!\"" 
        "})");
	
	return 0;
}