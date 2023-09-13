
#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(20);\n"
		"Console::outln(\"number-=20 => {0}\",number-=20)\n"
		"Console::outln(\"number-=new Number(30) => {0}\",number-=new Number(30))\n"
	);
	return 0;
}