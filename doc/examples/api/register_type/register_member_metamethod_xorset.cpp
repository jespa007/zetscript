
#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(0);\n"
		"Console::outln(\"number^=0xa => {0}\",number^=0xa);\n"
		"Console::outln(\"number^=new Number(0x9) => {0}\",number^=new Number(0x9))\n"
	);

	return 0;
}


