/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

using namespace zetscript;

int add(int op1, int op2){
	return op1+op2;
}


zs_float add(zs_float *op1, zs_float *op2){
	return *op1+*op2;
}


int main(){

	ZetScript *zs = new ZetScript(); // instance ZetScript

	zs->registerNativeGlobalFunction("add",static_cast<int (*)(int,int)>(add));
	zs->registerNativeGlobalFunction("add",static_cast<zs_float (*)(zs_float *,zs_float *)>(add));


	zs->eval(
		"print(\"result 5+4:\"+add(5,4));"       // prints "result 5+4:9"
		"print(\"result 0.5+4.6:\"+add(0.5,4.6));"       // prints "result 5+4:9"
	);

	delete zs;

#ifdef __MEMMANAGER__
	MEMMGR_print_status();
#endif

	return 0;
}
