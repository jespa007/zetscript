#include "zetscript.h"

// c function
void paramString(
    zetscript::ScriptEngine *_script_engine
    ,zetscript::StringScriptObject *_string
){
    printf("String : '%s' (StringScriptObject *)\n",_string->get().toConstChar());
}

int main(){
	zetscript::ScriptEngine script_engine;

    script_engine.registerFunction("paramString",paramString);

    // call c function with string param
    script_engine.compileAndRun(
        "paramString(\"Hello World\");"
 	);


 	return 0;
}

