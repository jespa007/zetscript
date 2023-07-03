#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_float;

class Number{
public:
	double value;

	Number(){
		value = 0;
	}
};

//-----
// Function to be registered to create new instance of Number
Number *NumberZs_new(ZetScript *_zs){
	return new Number();
}

// Function to be registered as constructor
void NumberZs_constructor(ZetScript *_zs, Number *_this, zs_float * _number){
	_this->value=*_number;
}

// Function to be registered to delete instance of Number
void NumberZs_delete(ZetScript *_zs, Number * _this){
	delete _this;
}
//-----


int main(){

	ZetScript zs;

	// registers Number exposed as "Number"
	zs.registerType<Number>("Number",NumberZs_new,NumberZs_delete);//,Number_constructorZs,Number_destructorZs);

	// register Number constructor
	zs.registerConstructor<Number>(NumberZs_constructor);

	// eval script that creates object of type "Number" and inits x and y as  10 and 20 respectibely
	zs.eval("var number=new Number(10.5);");

	return 0;
};


