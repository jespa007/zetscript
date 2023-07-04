#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		Console::outln("new Number(0x1) << new Number(2) => " +  (new Number(0x1) << new Number(2)));
		Console::outln("new Number(0x1) << 2 => " + (new Number(0x1) << 2));
		Console::outln("0x1 << new Number(2) => " + (0x1 << new Number(2)));
	);

	return 0;
};


