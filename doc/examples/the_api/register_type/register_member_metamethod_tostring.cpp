#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"Console::outln(\"Result _tostring => \"+new Number(10))"
	);

	return 0;
}


