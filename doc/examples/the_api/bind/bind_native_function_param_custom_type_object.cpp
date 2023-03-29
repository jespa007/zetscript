#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ObjectScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::zs_map;

// c function expects an array of integers and floats
void paramObjectScriptObject(ZetScript *_zs, ObjectScriptObject *_object){
	printf("Values in object:\n");
	zs_map *map=_object->getMapFields();
	for(auto it=map->begin(); !it.end();it.next()){
        printf(
			"Key: '%s' => Value: %i\n"
			,it.key
			,(int)_object->get<zs_int>(it.key)
		);
    }
}

int main(){
	ZetScript zs;

    zs.bindFunction("paramObjectScriptObject",paramObjectScriptObject);

    // call c function with string param
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

