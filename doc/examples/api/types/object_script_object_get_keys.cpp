#include "zetscript.h"

void printKeys(
	zetscript::ScriptEngine *_script_engine
	,zetscript::ObjectScriptObject *_object
){

	// instance new ObjectScriptObject using ScriptEngine instance
	auto keys=_object->getKeys();
	printf("keys : [");
	for(int i=0; i < keys.length(); i++){
		if(i>0){
			printf(",");
		}
		printf("\"%s\"",keys.get(i).toConstChar());
	}
	printf("]\n");
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers printKeys
	script_engine.registerFunction("printKeys",printKeys);

	script_engine.compileAndRun(
		"var object={"
		"	\"key1\":0"
		"	,\"key2\":\"hello\""
		"	,\"key3\":10.0"
		"	,\"key4\":0"
		"	,\"key5\":1"
		"	,\"key6\":2"
		"	,\"key7\":\"world\"}\n"
		"printKeys(object)\n"

	);
	
	return 0;
}