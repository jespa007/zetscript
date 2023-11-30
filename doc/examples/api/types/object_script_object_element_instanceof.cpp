#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::DictionaryScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;

// Check whether the object element is type integer, float or string
void testObjectElementInstanceOf(ZetScript *_zs,DictionaryScriptObject *_object){

    auto keys=_object->getKeys();

	for(int i=0; i < keys.size(); i++){
               
        printf("Element at key '%s' is type '",keys.get(i).toConstChar());

        if(_object->elementInstanceOf<zs_int>(keys.get(i))){
            printf("Integer");
        }

        if(_object->elementInstanceOf<zs_float>(keys.get(i))){
            printf("Float");
        }
        
        if(_object->elementInstanceOf<String>(keys.get(i))){
            printf("String");
        }

        printf("'\n");
	}
}

int main(){
	zetscript::ZetScript zs;

	// registers testObjectElementInstanceOf
	zs.registerFunction("testObjectElementInstanceOf",testObjectElementInstanceOf);

	// calls printObjectElementTypes
	zs.eval(
        "testObjectElementInstanceOf({"
            "\"key1\":0"                // Element at key 'key1' is type 'Integer'
            ",\"key2\":10.5"             // Element at key 'key2': is type 'Float'
            ",\"key3\":\"Hello World!\"" // Element at key 'key3': is type 'String'
        "})");
	
	return 0;
}