#include "NumberZs.h"

int main(){

	ScriptEngine se;

	NumberZs_register(&zs);

	se.compileAndRun(
		"var number=new Number();\n"
		"Console::outln(\"number::MAX_VALUE => {0}\",number::MAX_VALUE)\n"
	);

	return 0;
}


