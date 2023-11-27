#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::DictionaryObject;
using zetscript::StringObject;

void modifyObject(ZetScript *_zs,DictionaryObject *_object){

	auto keys=_object->getKeys();
	for(int i=0; i < keys.size(); i++){
		switch(i%3){
		case 0: // set a integer
			_object->set<zs_int>(keys.get(i),i);
			break;
		case 1: // set a random float
			_object->set<zs_float>(keys.get(i),i*10.2);
			break;
		case 2: // set new string
			_object->set<StringObject *>(keys.get(i),new StringObject(_zs,"Hello"));
			break;

		}
	}
}

int main(){
	zetscript::ZetScript zs;

	// registers modifyObject
	zs.registerFunction("modifyObject",modifyObject);

	// create a initialized array, it prints. then prints the 
	// the array after call 'modifyObject'
	zs.eval(
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