#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"number-- => {0}\",number--)\n"
	);

	return 0;
}


