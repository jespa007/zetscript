/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

zetscript::zs_int add(zetscript::zs_int op1, zetscript::zs_int op2){
	return op1+op2;
}


zetscript::zs_float add(zetscript::zs_float *op1, zetscript::zs_float *op2){
	return *op1+*op2;
}


int main(){

	zetscript::ZetScript zs; // instance ZetScript

	zs.registerFunction("add",static_cast<zetscript::zs_int (*)(zetscript::zs_int,zetscript::zs_int)>(add));
	zs.registerFunction("add",static_cast<zetscript::zs_float (*)(zetscript::zs_float *,zetscript::zs_float *)>(add));


	zs.eval(
		"print(\"result 5+4:\"+add(5,4));"       // prints "result 5+4:9"
		"print(\"result 0.5+4.6:\"+add(0.5,4.6));"       // prints "result 5+4:9"
	);

	return 0;
}
