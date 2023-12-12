#include "NumberZs.h"

int main(){

	ScriptEngine zs;

	NumberZs_register(&zs);

	zs.compileAndRun(
		"var number=new Number(2);\n"
		"Console::outln(\"number*=2 => {0}\",number*=2)\n"
		"Console::outln(\"number*=new Number(2) => {0}\",number*=new Number(2))\n"
	);
	return 0;
}


