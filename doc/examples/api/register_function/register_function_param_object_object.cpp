#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ObjectObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::MapString;

// c function expects an array of integers and floats
void paramObjectObject(ZetScript *_zs, ObjectObject *_object){
	printf("Values in object:\n");
	auto keys=_object->getKeys();
	for(int i=0; i < keys.size();i++){
        printf(
			"Key: '%s' => Value: %i\n"
			,keys.get(i).toConstChar()
			,(int)_object->get<zs_int>(keys.get(i))
		);
    }
}

int main(){
	ZetScript zs;

    zs.registerFunction("paramObjectObject",paramObjectObject);

    // eval and call bind function
    zs.eval(
        "paramObjectObject({"
        "    a:0"
        "     ,b:1"
        "     ,c:2"
        "     ,d:3"
        "     ,e:4"

        "});"
 	);


 	return 0;
}

