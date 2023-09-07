#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);
	
	zs.eval(
		"var number=new Number(0x0);\n"
		"Console::outln(\"number.value^=0xa => {0}\",number.value^=0xa)\n"
	);

	return 0;
}


