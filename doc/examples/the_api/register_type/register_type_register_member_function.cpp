#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_float;
using zetscript::zs_int;

typedef struct{
	double value;
}Number;

//-----
// Function to be registered to create new instance of Number
Number *NumberZs_new(ZetScript *_zs){
	return new Number();
}


void NumberZs_constructor(ZetScript *_zs, Number *_this, zs_float * _number){
	_this->value=*_number;
}

zs_int NumberZs_toInteger(ZetScript *_zs, Number *_this){
	return _this->value;
}

// Function to be registered to delete instance of Number
void NumberZs_delete(ZetScript *_zs, Number * _this){
	delete _this;
}
//-----


int main(){

	ZetScript zs;

	// Registers Number type exposed as "Number"
	zs.registerType<Number>("Number",NumberZs_new,NumberZs_delete);//,Number_constructorZs,Number_destructorZs);

	// Register Number constructor
	zs.registerConstructor<Number>(NumberZs_constructor);

	// Register member function toInteger
	zs.registerMemberFunction<Number>("toInteger",NumberZs_toInteger);

	// eval script that creates object of type "Number" and inits x and y as  10 and 20 respectibely
	zs.eval(
		"var number=new Number(10.5);"
		"Console::outln(\"number.toInteger() : \",+number.toInteger())"
	);

	return 0;
};


