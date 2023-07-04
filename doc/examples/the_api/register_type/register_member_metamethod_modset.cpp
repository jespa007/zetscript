
#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Evaluates a script that instances "Number" with 10.5 and later it adds 20
	zs.eval(
		var number=new Number(250);
		Console::outln("number => "+number)
		number%=30;
		Console::outln("number%=30 => "+number)
		number%=new Number(100);
		Console::outln("number%=new Number(100) => "+number)
	);

	return 0;
};


