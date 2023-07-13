#include "Number.h"
#include "zetscript.h"

using zetscript::ZetScript;

// Number reference
Number *number=NULL;

// function that returns number reference
Number *getNumber(ZetScript *_zs){
	return 	number;
}


int main()
{
	zetscript::ZetScript zs;

	zs.registerType<Number>("Number");
	
	// create new 'Number'
	number=new Number();

 	
 	 //register function 'getNumber' that return 'number' reference
 	zs.registerFunction("getNumber",getNumber);
 	
 	
	// Evaluates ZetScript code that calls 'getNumber' to get 'number' object reference
 	zs.eval(
 		"var number= getNumber();" 
 	);
 	
 	// delete instance
 	delete number;
 	
 	return 0;
 }