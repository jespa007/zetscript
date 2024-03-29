#include "zetscript.h"

void modifyObject(
	zetscript::ScriptEngine *_script_engine
	,zetscript::ObjectScriptObject *_object
){

	auto keys=_object->getKeys();
	for(int i=0; i < keys.length(); i++){
		switch(i%3){
		case 0: // set a integer
			_object->set<zetscript::zs_int>(keys.get(i),i);
			break;
		case 1: // set a random float
			_object->set<zetscript::zs_float>(keys.get(i),i*10.2);
			break;
		case 2: // set new string
			_object->set<zetscript::StringScriptObject *>(keys.get(i),new zetscript::StringScriptObject(_script_engine,"Hello"));
			break;

		}
	}
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers modifyObject
	script_engine.registerFunction("modifyObject",modifyObject);

	// create a initialized array, it prints. then prints the 
	// the array after call 'modifyObject'
	script_engine.compileAndRun(
		"var o={"
		"	\"key1\":0"
		"	,\"key2\":\"hello\""
		"	,\"key3\":10.0"
		"	,\"key4\":0"
		"	,\"key5\":1"
		"	,\"key6\":2"
		"	,\"key7\":\"world\"}\n"
		"Console::outln(\"Before call 'modifyObject':{0}\",o);"
		"modifyObject(o)\n"
		"Console::outln(\"After call 'modifyObject':{0}\",o);"
	);
	
	return 0;
}