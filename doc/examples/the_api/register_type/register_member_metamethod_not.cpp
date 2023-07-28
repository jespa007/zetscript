#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);
	
	zs.eval(
		"var number=new Number()\n"
		"if(!number){\n"
		"	Console::outln(\"Number is empty\")	\n"
		"}\n"
	);

	return 0;
};


