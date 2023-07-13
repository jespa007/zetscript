#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	// eval script that creates object of type "Number"
	zs.eval("var number=new Number();");

	return 0;

};


