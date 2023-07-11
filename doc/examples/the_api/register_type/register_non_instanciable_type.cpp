#include "NumberZs.h"

// Number reference
Number *number=NULL;

// function that returns number reference
Number *getNumber(ZetScript *_zs){
	return 	number;
}


int main()
{
	zetscript::ZetScript zs;

	registerNumberZs(&zs);
	
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