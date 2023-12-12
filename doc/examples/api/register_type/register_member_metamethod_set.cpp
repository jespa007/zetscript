#include "NumberZs.h"

int main(){

	ScriptEngine zs;

	NumberZs_register(&zs);

	zs.compileAndRun(
		"var n=new Number(0);\n"
		"Console::outln(\"n=10 => {0}\",n=10)\n"
		"Console::outln(\"n=new Number(20) => {0}\",n=new Number(20))\n"
	);

	return 0;
}


