/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

zetscript::zs_int add(zetscript::ScriptEngine *_script_engine, zetscript::zs_int op1, zetscript::zs_int op2){
	ZS_UNUSUED_PARAM(_script_engine);
	return op1+op2;
}


zetscript::zs_float add(zetscript::ScriptEngine *_script_engine,zetscript::zs_float *op1, zetscript::zs_float *op2){
	ZS_UNUSUED_PARAM(_script_engine);
	return *op1+*op2;
}


int main(){

	zetscript::ScriptEngine script_engine; // instance ScriptEngine

	script_engine.registerFunction("add",static_cast<zetscript::zs_int (*)(zetscript::ScriptEngine *_script_engine,zetscript::zs_int,zetscript::zs_int)>(add));
	script_engine.registerFunction("add",static_cast<zetscript::zs_float (*)(zetscript::ScriptEngine *_script_engine,zetscript::zs_float *,zetscript::zs_float *)>(add));


	script_engine.compileAndRun(
		"Console::outln(\"result 5+4:\"+add(5,4));"       // prints "result 5+4:9"
		"Console::outln(\"result 0.5+4.6:\"+add(0.5,4.6));"       // prints "result 5+4:9"
	);

	return 0;
}
