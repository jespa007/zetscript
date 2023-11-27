#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::DictionaryObject;
using zetscript::StringObject;

void printKeys(ZetScript *_zs,DictionaryObject *_object){

	// instance new DictionaryObject using ZetScript context
	auto keys=_object->getKeys();
	printf("keys : [");
	for(int i=0; i < keys.size(); i++){
		if(i>0){
			printf(",");
		}
		printf("\"%s\"",keys.get(i).toConstChar());
	}
	printf("]\n");
}

int main(){
	zetscript::ZetScript zs;

	// registers printKeys
	zs.registerFunction("printKeys",printKeys);

	zs.eval(
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