#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Eval script that instances "Number" with 10.5 and later is set to 20
	zs.eval(
		var n=new Number ();
		Console::outln("n => "+n)

		n=10;
		Console::outln("n=10 => "+n)

		n=new Number(20);
		Console::outln("n=new Number(20) => "+n)
	);

	return 0;
};


