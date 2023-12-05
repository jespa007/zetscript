#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::ArrayScriptObject;
using zetscript::StringScriptObject;

void modifyArray(ZetScript *_zs,ArrayScriptObject *_array_object){

	for(int i=0; i < _array_object->length(); i++){
		switch(i%3){
		case 0: // set a integer
			_array_object->set<zs_int>(i,i);
			break;
		case 1: // set a random float
			_array_object->set<zs_float>(i,i*10.2);
			break;
		case 2: // set new string
			_array_object->set<StringScriptObject *>(i,new StringScriptObject(_zs,"Hello"));
			break;

		}
	}
}

int main(){
	zetscript::ZetScript zs;

	// registers modifyArray
	zs.registerFunction("modifyArray",modifyArray);

	// create a initialized array, it prints. then prints the 
	// the array after call 'modifyArray'
	zs.compileAndRun(
		"var v=[0,\"hello\",10.0,0,1,2,\"world\"]\n"
		"Console::outln(\"Before call 'modifyArray':{0}\",v);"
		"modifyArray(v)\n"
		"Console::outln(\"After call 'modifyArray':{0}\",v);"
	);
	
	return 0;
}