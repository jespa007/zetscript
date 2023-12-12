#include "Number.h"
#include "zetscript.h"

using zetscript::ScriptEngine;

// Number reference
Number *number=NULL;

// function that returns number reference
Number *getNumber(ScriptEngine *_zs){
	return 	number;
}

int main(){
	
	zetscript::ScriptEngine zs;

	zs.registerScriptType<Number>("Number");
	
	// create new 'Number'
	number=new Number();
 	
 	 //register function 'getNumber' that return 'number' reference
 	zs.registerFunction("getNumber",getNumber);
 	
 	
	// Evaluates ScriptEngine code that calls 'getNumber' to get 'number' object reference
 	zs.compileAndRun(
 		"var number= getNumber();" 
 	);
 	
 	// delete instance
 	delete number;
 	
 	return 0;
 }