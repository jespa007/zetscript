#include "CZetScript.h"

using namespace zetscript;

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	zs->eval(
		"class Test{"
		"	var data1;"
		"	function function1(){"
		"		print(\"calling Test.Function\");"
		"	}"
		"};"
		""
		"function delete_test(){"
		"	delete test;"
		"	print(\"test variable was deleted\");"
		"}"
		""
		"var test=new Test();"
	);



	auto delete_test=zs->bind_function("delete_test");
	auto test_function1=zs->bind_function("test.function1");



	if(test_function1){
		if(!(*test_function1)(NO_PARAMS)){
			fprintf(stderr,"%s\n",ZS_GET_ERROR_MSG());
			return -1;
		}
	}

	if(delete_test){ // if the function exist,
		if(!(*delete_test)(NO_PARAMS)){
			fprintf(stderr,"%s\n",ZS_GET_ERROR_MSG());
			return -1;
		}
	}

	// delete function obj
	delete 	test_function1;
	delete 	delete_test;

}
