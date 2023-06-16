#include "zetscript.h"

using zetscript::ZetScript;

// type 'MyType'
typedef struct{
	int x;
	int y;
}MyType;


// MyType reference
MyType *my_type=NULL;

// function that returns my_type reference
MyType *getMyType(ZetScript *_zs){
	return 	my_type;
}


void main(int argc, char *argv[])
{
	zetscript::ZetScript zs;
	
	// create new 'MyType'
	my_type=new MyType();
	
	
 	//register 'MyType' as no instanciable
 	zs.registerType<MyType>("MyType");
 	
 	 //register function 'getMyType' that return 'my_type' reference
 	zs.registerFunction("getMyType",getMyType);
 	
 	
	// Evaluates ZetScript code that calls 'getMyType' to get 'my_type' object reference
 	zs.eval(
 		"var my_type= getMyClass();" 
 	);
 	
 	// delete
 	delete my_type;
 	
 	return 0;
 }