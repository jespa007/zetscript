
#include "zetscript.h"

using namespace zetscript;

std::function<std::string * (*float, *int, *bool)> *callback = NULL;

std::function<std::string * (*float, *int, *bool)> * create_callback(ZetScript *zs, ScriptVarFunction *script_function){
	return zs->bindScriptFunction<std::string * (*float, *int, *bool)>(script_function->)
}


int main(){

	ZetScript *zs = new ZetScript(); // instance ZetScript

	zs->bindFunctionMember(create_callback,"create_callback");

	zs->eval(
		"class Test{"
		"	var data1;"
		"	function function1(arg){"
		"		print(\"calling Test.Function:\"+arg);"
		"	}"
		"};"
		""
		"function delete_test(){"
		"	delete test;"
		"	print(\"test variable was deleted\");"
		"}"
		""
		"var test=new Test();"
		"create_callback(function(a,b,c){return \"result a:\"+a+\"b:\"+b+\"c:\"+c);})"
		""
	);


	std::function<void()>  * delete_test=zs->bindScriptFunction<void ()>("delete_test"); // instance function delete_test function.
	std::function<void(int)> * test_function1=zs->bindScriptFunction<void (int)>("test.function1"); // instance member function test.function1.

	(*test_function1)(10); // it calls "test.function" member function with 10 as parameter.
	(*delete_test)(); // it calls "delete_test" function with no parameters

	// delete functions when they are used anymore
	delete 	test_function1;
	delete 	delete_test;


	delete zs;

#ifdef __MEMMANAGER__
	MEMMGR_print_status();
#endif
}
