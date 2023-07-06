#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	// eval script that creates object of type "Number" and inits x and y as  10 and 20 respectibely
	zs.eval(
		"Console::outln(\"Number::pow(2,10) : \",+Number::pow(2,10))"
	);

	return 0;
};


