#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ObjectScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::zs_map;

// c function expects an array of integers and floats
void paramObjectScriptObject(ZetScript *_zs, ObjectScriptObject *_object){
	printf("Values in object:\n");
	auto keys=_object->getKeys();
	for(int i=0; i < keys.size();i++){
        printf(
			"Key: '%s' => Value: %i\n"
			,keys.get(i).c_str()
			,(int)_object->get<zs_int>(keys.get(i))
		);
    }
}

int main(){
	ZetScript zs;

    zs.registerFunction("paramObjectScriptObject",paramObjectScriptObject);

    // eval and call bind function
    zs.eval(
        "paramObjectScriptObject({"
        "    a:0"
        "     ,b:1"
        "     ,c:2"
        "     ,d:3"
        "     ,e:4"

        "});"
 	);


 	return 0;
}

