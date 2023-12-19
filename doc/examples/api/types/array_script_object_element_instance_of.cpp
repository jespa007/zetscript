#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;

// Check whether the array element is type integer, float or string
void testArrayElementInstanceOf(ScriptEngine *_script_engine,ArrayScriptObject *_array_object){
    
	for(int i=0; i < _array_object->length(); i++){
        printf("Element at position '%i' is type '",i);

        if(_array_object->elementInstanceOf<zs_int>(i)){
            printf("Integer");
        }

        if(_array_object->elementInstanceOf<zs_float>(i)){
            printf("Float");
        }
        
        if(_array_object->elementInstanceOf<String>(i)){
            printf("String");
        }
        printf("'\n");
	}
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers testArrayElementInstanceOf
	script_engine.registerFunction("testArrayElementInstanceOf",testArrayElementInstanceOf);

	// calls printArrayElementTypes
	script_engine.compileAndRun(
        "testArrayElementInstanceOf(["
            "0"                 // Element at position '1' is type 'Integer'
            ",10.5"             // Element at position '2: is type 'Float'
            ",\"Hello World!\"" // Element at position '3: is type 'String'
        "])");
	
	return 0;
}