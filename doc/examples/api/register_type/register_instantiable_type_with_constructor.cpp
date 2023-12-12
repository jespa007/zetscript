#include "NumberZs.h"

int main(){

	ScriptEngine script_engine;

	NumberZs_register(&zs);

	script_engine.eval("var number=new Number(10.5);");

	return 0;
}


