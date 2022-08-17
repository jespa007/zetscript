/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

void test_register_variables(zetscript::ZetScript *_zs) {

	int n_test=0;

	// declare some variables
	zetscript::zs_int 		int_var = 10;
	zetscript::zs_float 	float_var = 0.5;
	bool 		bool_var = true;
	std::string	string_var = "in c++";

	_zs->bindVariable("int_var",&int_var); // it takes int *
	_zs->bindVariable("float_var",&float_var); // it takes float *
	_zs->bindVariable("bool_var",&bool_var); // it takes bool *
	_zs->bindVariable("string_var",&string_var); // it takes std::string *

	_zs->eval(
		"int_var+=5;"
		"float_var+=5;"
		"bool_var=!bool_var;"
		"string_var+=\" and in script\";"
		"Console::outln(\"int_var:\"+int_var);"       // prints "int_var:0"
		"Console::outln(\"float_var:\"+float_var);"   // prints "float_var:5.500000"
		"Console::outln(\"bool_var:\"+bool_var);"     // prints "bool_var:false"
		"Console::outln(\"string_var:\"+string_var);" // prints "string_var:in c++ and in script"
	);

}


#ifdef __MAIN__
int main(){
	zetscript::ZetScript zs;
	try{
		test_register_constants(&zs);
	}catch(std::exception & ex){
		fprintf(stderr,"%s",ex.what());
		return -1;
	}

	return 0;
}
#endif
