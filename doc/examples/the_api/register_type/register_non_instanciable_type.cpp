#include "zetscript.h"

using zetscript::ZetScript;

// type 'Number'
typedef struct{
	double number;
}Number;


// Number reference
Number *number=NULL;

// function that returns number reference
Number *getNumber(ZetScript *_zs){
	return 	number;
}


int main()
{
	zetscript::ZetScript zs;
	
	// create new 'Number'
	number=new Number();
	
	
 	//register 'Number' as no instanciable
 	zs.registerType<Number>("Number");
 	
 	 //register function 'getNumber' that return 'number' reference
 	zs.registerFunction("getNumber",getNumber);
 	
 	
	// Evaluates ZetScript code that calls 'getNumber' to get 'number' object reference
 	zs.eval(
 		"var number= getNumber();" 
 	);
 	
 	// delete
 	delete number;
 	
 	return 0;
 }