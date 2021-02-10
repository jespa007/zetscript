/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

using namespace zetscript;

void say_helloworld(){
	printf("Hello World!");
}

int main(){

	int 	int_var = 10;
	zs_float 	float_var = 0.5;
	bool 	bool_var = true;
	std::string	string_var = "in c++";

	ZetScript *zs = new ZetScript(); // instance ZetScript

	zs->registerNativeGlobalVariable("int_var",&int_var); // it takes int *
	zs->registerNativeGlobalVariable("float_var",&float_var); // it takes float *
	zs->registerNativeGlobalVariable("bool_var",&bool_var); // it takes bool *
	zs->registerNativeGlobalVariable("string_var",&string_var); // it takes std::string *

	zs->eval(
		"int_var+=5;"
		"float_var+=5;"
		"bool_var=!bool_var;"
		"string_var+=\" and in script\";"
		"print(\"int_var:\"+int_var);"       // prints "int_var:0"
		"print(\"float_var:\"+float_var);"   // prints "float_var:5.500000"
		"print(\"bool_var:\"+bool_var);"     // prints "bool_var:false"
		"print(\"string_var:\"+string_var);" // prints "string_var:in c++ and in script"
	);

	delete zs;

#ifdef __MEMMANAGER__
	MEMMGR_print_status();
#endif

	return 0;
}
