#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		Console::outln("new Number(10) % new Number(20) => " +  (new Number(10) % new Number(20)));
		Console::outln("new Number(10) % 20 => " + (new Number(10) % 20));
		Console::outln("10 % new Number(20) => " + (10 % new Number(20)));
	);

	return 0;
};


