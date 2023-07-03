#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_float;
using zetscript::zs_string;

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

// Function to be registered as member metamethod _tostring
zs_string NumberZs_tostring(ZetScript *_zs,Number * _this){
	return zetscript::zs_strutils::format(" Number : %f", _this->value);
}

// Function to be registered as member metamethod _set
void NumberZs_set(ZetScript *_zs,Number * _this, zs_float *_value){
	_this->value=*_value;
}

// Function to be registered to delete instance of Number
void NumberZs_delete(ZetScript *_zs, Number * _this){
	delete _this;
}
//-----


int main(){

	ZetScript zs;

	// Register Number type
	zs.registerType<Number>("Number",NumberZs_new,NumberZs_delete);//,Number_constructorZs,Number_destructorZs);

	// Register constructor
	zs.registerConstructor<Number>(NumberZs_constructor);

	// Register member metamethod _tostring
	zs.registerMemberFunction<Number>("_tostring",NumberZs_tostring);


	// Register member metamethod _set
	zs.registerMemberFunction<Number>("_set",NumberZs_set);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		"var number=new Number(10.5);\n"
		"Console::outln(\"-start:\");\n"
		"Console::outln(number);\n"
		"Console::outln(\"-after set:\");\n"
		"number=20;\n"
		"Console::outln(number);\n"
	);

	return 0;
};


