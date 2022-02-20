/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

int main(){
	// instance ZetScript
	zetscript::ZetScript zs;

	// declare some variables
	zetscript::zs_int 		int_var = 10;
	zetscript::zs_float 	float_var = 0.5;
	bool 		bool_var = true;
	zetscript::zs_string	string_var = "in c++";

	zs.registerVariable("int_var",&int_var); // it takes int *
	zs.registerVariable("float_var",&float_var); // it takes float *
	zs.registerVariable("bool_var",&bool_var); // it takes bool *
	zs.registerVariable("string_var",&string_var); // it takes zs_string *

	zs.eval(
		"int_var+=5;"
		"float_var+=5;"
		"bool_var=!bool_var;"
		"string_var+=\" and in script\";"
		"Console::outln(\"int_var:\"+int_var);"       // prints "int_var:0"
		"Console::outln(\"float_var:\"+float_var);"   // prints "float_var:5.500000"
		"Console::outln(\"bool_var:\"+bool_var);"     // prints "bool_var:false"
		"Console::outln(\"string_var:\"+string_var);" // prints "string_var:in c++ and in script"
	);

	return 0;
}
