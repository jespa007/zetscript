#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		var number=new Number(0x0);
		Console::outln("number.value => "+number.value)
		number.value^=0xa;
		Console::outln("number.value^=0xa => "+number.value)
		number.value^=new Number(0x9);
		Console::outln("number.value^=new Number(0x9) => "+number.value)
	);

	return 0;
};


