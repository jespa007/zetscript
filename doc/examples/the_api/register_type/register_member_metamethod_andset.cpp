
#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Evaluates a script that instances "Number" with 10.5 and later it adds 20
	zs.eval(
		var number=new Number(0x7);
		Console::outln("number => "+number)
		number&=0x3;
		Console::outln("number&=0x3 => "+number)
		number&=new Number(0x1);
		Console::outln("number&=new Number(0x1) => "+number)
	);

	return 0;
};


