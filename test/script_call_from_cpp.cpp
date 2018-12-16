
#include "CZetScript.h"

using namespace zetscript;

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	zs->evalString(
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
	);


	std::function<void()>  * delete_test=bind_function<void ()>("delete_test"); // instance function delete_test function.
	std::function<void(int)> * test_function1=bind_function<void (int)>("test.function1"); // instance member function test.function1.

	(*test_function1)(10); // it calls "test.function" member function with 10 as parameter.
	(*delete_test)(); // it calls "delete_test" function with no parameters

	// delete functions when they are used anymore
	delete 	test_function1;
	delete 	delete_test;


	CZetScript::destroy();

#ifdef __MEMMANAGER__
  MEM_ViewStatus();
#endif
}
