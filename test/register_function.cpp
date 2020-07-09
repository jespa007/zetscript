/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "ZetScript.h"

using namespace zetscript;

int add(int op1, int op2){
	return op1+op2;
}


float add(float *op1, float *op2){
	return *op1+*op2;
}


int main(){

	ZetScript *zs = new ZetScript(); // instance ZetScript

	zs->Register_C_Function("add",static_cast<int (*)(int,int)>(add));
	zs->Register_C_Function("add",static_cast<float (*)(float *,float *)>(add));


	zs->evalString(
		"print(\"result 5+4:\"+add(5,4));"       // prints "result 5+4:9"
		"print(\"result 0.5+4.6:\"+add(0.5,4.6));"       // prints "result 5+4:9"
	);

	delete zs;

#ifdef __MEMMANAGER__
  MEM_ViewStatus();
#endif

	return 0;
}
