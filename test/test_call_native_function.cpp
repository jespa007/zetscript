/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

class A{

};

//------------------------------
// TEST CALLBACK WITH NULLS
void test_native_function_with_nulls(
		zetscript::ZetScript *_zs
		,zetscript::zs_int _p1
		,zetscript::zs_float *_p2
		,bool *_p3
		,A *_p4
){
	if(_p1 != 0){std::runtime_error("_p1 not 0");}
	if(_p2 != NULL){std::runtime_error("_p2 not NULL");}
	if(_p3 != NULL){std::runtime_error("_p3 not NULL");}
	if(_p4 != NULL){std::runtime_error("_p4 not NULL");}

}

void test_call_native_function_with_nulls(zetscript::ZetScript *_zs){
	// bind type A
	_zs->bindType<A>("A");

	// bind 'bindFunction'
	_zs->bindFunction("test_native_function_with_nulls",test_native_function_with_nulls);

	_zs->eval(
		// calls C function 'test_native_function_with_nulls' passing all parameters as null.
		"test_native_function_with_nulls(\n"
			"null\n"
			",null\n"
			",null\n"
			",null\n"
		");\n"
		""
	);
}


int main(){

	// instances ZetScript
	zetscript::ZetScript zs;

	try{

		test_call_native_function_with_nulls(&zs);

	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
	}

}
