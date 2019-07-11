/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

using namespace zetscript;

void say_helloworld(){
	printf("Hello World!");
}

int main(){

	int 	int_var = 10;
	float 	float_var = 0.5;
	bool 	bool_var = true;
	std::string	string_var = "in c++";

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	REGISTER_C_VARIABLE("int_var",int_var); // it takes int *
	REGISTER_C_VARIABLE("float_var",float_var); // it takes float *
	REGISTER_C_VARIABLE("bool_var",bool_var); // it takes bool *
	REGISTER_C_VARIABLE("string_var",string_var); // it takes std::string *

	zetscript::evalString(
		"int_var+=5;"
		"float_var+=5;"
		"bool_var=!bool_var;"
		"string_var+=\" and in script\";"
		"print(\"int_var:\"+int_var);"       // prints "int_var:0"
		"print(\"float_var:\"+float_var);"   // prints "float_var:5.500000"
		"print(\"bool_var:\"+bool_var);"     // prints "bool_var:false"
		"print(\"string_var:\"+string_var);" // prints "string_var:in c++ and in script"
	);

	CZetScript::destroy();

#ifdef __MEMMANAGER__
  MEM_ViewStatus();
#endif

	return 0;
}
