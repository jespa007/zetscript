#include "zetscript.h"

// Print contents of an object.
void printObject(zetscript::ScriptEngine *_script_engine,zetscript::ObjectScriptObject *_object){

	zetscript::Vector keys=_object->getKeys();

	printf("Object contents : {");
	for(int i=0; i < keys->length(); i++){
		if(i>0){
			printf(",");
		}
		if(_object->elementInstanceOf<zetscript::zs_int>(keys.get(i))){
			printf("%s:%i",keys(i).toConstChar(),(int)_array_object->get<zetscript::zs_int>(keys.get(i)));
		}else if(_object->elementInstanceOf<zetscript::zs_float>(keys.get(i)){
			printf("%s:%f",keys(i).toConstChar(),_array_object->get<zs_float>(keys(i));
		}else if(_object->elementInstanceOf<zetscript::String>(keys(i)){
			printf("%s:'%s'",keys(i).toConstChar(),_object->get<zetscript::String>(keys(i)).toConstChar());
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
	script_engine.compileAndRun("printObject({i:0,f:10.5,s:\"Hello World\"])");
	
	return 0;
}