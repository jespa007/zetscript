#include "NumberZs.h"

int main(){

	ScriptEngine zs;

	NumberZs_register(&zs);

	zs.compileAndRun("var number=new Number();");

	return 0;
}


