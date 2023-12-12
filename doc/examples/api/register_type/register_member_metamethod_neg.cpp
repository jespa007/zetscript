#include "NumberZs.h"

int main(){

	ScriptEngine zs;

	NumberZs_register(&zs);

	zs.compileAndRun(
		"var number=new Number(20)\n"
		"Console::outln(\"-number => \"+ (-number))\n"
	);

	return 0;
}


