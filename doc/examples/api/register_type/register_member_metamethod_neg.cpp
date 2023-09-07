#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(20)\n"
		"Console::outln(\"-number => \"+ (-number))\n"
	);

	return 0;
}


