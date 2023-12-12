#include "NumberZs.h"

int main(){

	ScriptEngine zs;

	NumberZs_register(&zs);

	zs.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value => {0}\",number.value)\n"
	);

	return 0;
}


