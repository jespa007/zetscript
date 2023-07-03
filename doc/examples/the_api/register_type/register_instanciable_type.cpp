#include "zetscript.h"

using zetscript::ZetScript;

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

// Function to be registered to delete instance of Number
void NumberZs_delete(ZetScript *_zs, Number * _this){
	delete _this;
}
//-----


int main(){

	ZetScript zs;

	// registers Number exposed as "Number"
	zs.registerType<Number>("Number",NumberZs_new,NumberZs_delete);//,Number_constructorZs,Number_destructorZs);

	// eval script that creates object of type "Number"
	zs.eval("var number=new Number();");

	return 0;

};


