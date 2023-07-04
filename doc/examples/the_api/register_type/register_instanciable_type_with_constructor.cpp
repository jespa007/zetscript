#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	// eval script that creates object of type "Number" and inits x and y as  10 and 20 respectibely
	zs.eval("var number=new Number(10.5);");

	return 0;
};


