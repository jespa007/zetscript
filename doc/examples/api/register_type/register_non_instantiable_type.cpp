#include "Number.h"
#include "zetscript.h"

// Number reference
Number *number=NULL;

// function that returns number reference
Number *getNumber(
	zetscript::ScriptEngine *_script_engine
){
	return 	number;
}

int main(){
	
	zetscript::ScriptEngine script_engine;

	script_engine.registerType<Number>("Number");
	
	// create new 'Number'
	number=new Number();
 	
 	 //register function 'getNumber' that return 'number' reference
 	script_engine.registerFunction("getNumber",getNumber);
 	
 	
	// Evaluates ZetScript code that calls 'getNumber' to get 'number' object reference
 	script_engine.compileAndRun(
 		"var number= getNumber();" 
 	);
 	
 	// delete instance
 	delete number;
 	
 	return 0;
 }