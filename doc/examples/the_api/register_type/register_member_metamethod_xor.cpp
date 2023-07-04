#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		Console::outln("new Number(0xa) ^ new Number(0x9) => " +  (new Number(0xa) ^ new Number(0x9)));
		Console::outln("new Number(0xa) ^ 0x9 => " + (new Number(0xa) ^ 0x9));
		Console::outln("0xa ^ new Number(0x9) => " + (0xa ^ new Number(0x9)));
	);

	return 0;
};


