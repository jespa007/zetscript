
#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Evaluates a script that instances "Number" with 10.5 and later it adds 20
	zs.eval(
		var number=new Number(20);
		Console::outln("number => "+number)
		number/=20;
		Console::outln("number/=20 => "+number)
		number/=new Number(30);
		Console::outln("number/=new Number(30) => "+number)
	);

	return 0;
};


