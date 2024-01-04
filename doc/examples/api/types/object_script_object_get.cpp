#include "zetscript.h"

// Print contents of an object.
void printObject(
	zetscript::ScriptEngine *_script_engine
	,zetscript::ObjectScriptObject *_object
){

	zetscript::Vector<zetscript::String> keys=_object->getKeys();

	printf("Object contents : {");
	for(int i=0; i < keys.length(); i++){
		if(i>0){
			printf(",");
		}
		if(_object->elementInstanceOf<zetscript::zs_int>(keys.get(i))){
			printf("%s:%i",keys.get(i).toConstChar(),(int)_object->get<zetscript::zs_int>(keys.get(i)));
		}else if(_object->elementInstanceOf<zetscript::zs_float>(keys.get(i))){
			printf("%s:%f",keys.get(i).toConstChar(),_object->get<zetscript::zs_float>(keys.get(i)));
		}else if(_object->elementInstanceOf<zetscript::String>(keys.get(i))){
			printf("%s:'%s'",keys.get(i).toConstChar(),_object->get<zetscript::String>(keys.get(i)).toConstChar());
		}else{
			printf("N/A");
		}
	}
	printf("}\n");
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers 'printObject' function
	script_engine.registerFunction("printObject",printObject);

	// calls printObject
	script_engine.compileAndRun("printObject({i:0,f:10.5,s:\"Hello World\"})");
	
	return 0;
}