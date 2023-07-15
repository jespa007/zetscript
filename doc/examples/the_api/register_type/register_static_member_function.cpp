#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"Console::outln(\"Number::pow(2,10) : \"+Number::pow(2,10))"
	);

	return 0;
};


