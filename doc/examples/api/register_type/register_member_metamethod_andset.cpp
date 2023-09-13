#include "NumberZs.h"

int main(){

	ZetScript zs;

	NumberZs_register(&zs);

	zs.eval(
		"var number=new Number(0x7);\n"
		"Console::outln(\"number&=0x3 => {0}\",number&=0x3);\n"
		"Console::outln(\"number&=new Number(0x1) => {0}\",number&=new Number(0x1));\n"
	);

	return 0;
}