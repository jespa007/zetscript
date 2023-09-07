#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number();\n"
		"Console::outln(\"number::MAX_VALUE => {0}\",number::MAX_VALUE)\n"
	);

	return 0;
}


