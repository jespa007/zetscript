#include "zetscript.h"
#include <math.h>

class Number{
	//...
};

zetscript::zs_float NumberZs_pow(
	zetscript::ScriptEngine *_script_engine
	,zetscript::zs_float *_base
	,zetscript::zs_float *_exp
){

	return pow(*_base,*_exp);
}


int main(){
	zetscript::ScriptEngine script_engine;

	script_engine.registerType<Number>("Number");

	script_engine.registerStaticMemberFunction<Number>("pow",NumberZs_pow);

	script_engine.compileAndRun(

		"Console::outln(\"Number::pow(2,2) => {0}\",Number::pow(2,2));"
	);
	
	return 0;
}