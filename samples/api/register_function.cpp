/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

zetscript::zs_int add(zetscript::ScriptEngine *_zs, zetscript::zs_int op1, zetscript::zs_int op2){
	ZS_UNUSUED_PARAM(_zs);
	return op1+op2;
}


zetscript::zs_float add(zetscript::ScriptEngine *_zs,zetscript::zs_float *op1, zetscript::zs_float *op2){
	ZS_UNUSUED_PARAM(_zs);
	return *op1+*op2;
}


int main(){

	zetscript::ScriptEngine zs; // instance ScriptEngine

	zs.registerFunction("add",static_cast<zetscript::zs_int (*)(zetscript::ScriptEngine *_zs,zetscript::zs_int,zetscript::zs_int)>(add));
	zs.registerFunction("add",static_cast<zetscript::zs_float (*)(zetscript::ScriptEngine *_zs,zetscript::zs_float *,zetscript::zs_float *)>(add));


	zs.compileAndRun(
		"Console::outln(\"result 5+4:\"+add(5,4));"       // prints "result 5+4:9"
		"Console::outln(\"result 0.5+4.6:\"+add(0.5,4.6));"       // prints "result 5+4:9"
	);

	return 0;
}
