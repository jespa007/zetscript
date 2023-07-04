
#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Evaluates a script that instances "Number" with 10.5 and later it adds 20
	zs.eval(
		var number=new Number(0);
		Console::outln("number => "+number)
		number^=0xa;
		Console::outln("number^=0xa => "+number)
		number^=new Number(0x9);
		Console::outln("number^=new Number(0x9) => "+number)
	);

	return 0;
};


