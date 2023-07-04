#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		var number=new Number(20);
		Console::outln("number.value => "+number.value)
		number.value/=20;
		Console::outln("number.value/=20 => "+number.value)
		number.value/=new Number(30);
		Console::outln("number.value/=new Number(30) => "+number.value)
	);

	return 0;
};


