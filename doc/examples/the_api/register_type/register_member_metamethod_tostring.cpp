#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	// Eval script that creates object of type "Number" and later is printed to console 
	zs.eval(
		"Console::outln(\"Result _tostring => \"+new Number(10))"
	);

	return 0;
};


