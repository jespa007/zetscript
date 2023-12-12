#include "NumberZs.h"

int main(){

	ScriptEngine zs;

	NumberZs_register(&zs);
	
	zs.compileAndRun(
		"var number=new Number(20);\n"
		"Console::outln(\"number.value+=10 => {0}\",number.value+=10)\n"
	);

	return 0;
}


