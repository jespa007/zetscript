#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	// Evaluates a script that instantiates object type "Number" passing a number to its constructor
	zs.eval("var number=new Number(10.5);");

	return 0;
};


