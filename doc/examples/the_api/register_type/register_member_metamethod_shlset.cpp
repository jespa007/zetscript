
#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Evaluates a script that instances "Number" with 10.5 and later it adds 20
	zs.eval(
		var number=new Number(0x1);
		Console::outln("number => "+number)
		number<<=1;
		Console::outln("number<<=1 => "+number)
		number<<=new Number(1);
		Console::outln("number<<=new Number(1) => "+number)
	);

	return 0;
};


