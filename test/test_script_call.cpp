
#include "zetscript.h"

using namespace zetscript;

ZetScript *zs=NULL;

void test_callback(ScriptFunction *script_function){
	float param1=1.0;
	int param2=2.0;
	bool param3=true;
	std::string *s;

	auto callback_function = zs->bindScriptFunction<std::string * (float *, int *, bool *)>(script_function);

	// call the function...
	s=(*callback_function)(&param1,&param2,&param3);

	printf("string result %s",s->c_str());

	// delete function...
	delete callback_function;
}


int main(){

	zs = new ZetScript(); // instance ZetScript

	try{

		zs->registerFunction("test_callback",test_callback);

		zs->eval(
			"class Test{"
			"	function1(arg){"
			"		print(\"calling Test.Function:\"+arg);"
			"	}"
			"};"
			""
			"function delete_test(){"
			"	delete test;"
			"	print(\"test variable was deleted\");"
			"}"
			"var test=new Test();"
			"test_callback(function(a,b,c){return \"result a:\"+a+\"b:\"+b+\"c:\"+c;});"
			""
		);

		std::function<void()>  * delete_test=zs->bindScriptFunction<void ()>("delete_test"); // instance function delete_test function.
		std::function<void(int)> * test_function1=zs->bindScriptFunction<void (int)>("test.function1"); // instance member function test.function1.

		(*test_function1)(10); // it calls "test.function" member function with 10 as parameter.
		(*delete_test)(); // it calls "delete_test" function with no parameters

		// delete functions when they are used anymore
		delete 	test_function1;
		delete 	delete_test;

	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
	}

	delete zs;

#ifdef __MEMMANAGER__
	MEMMGR_print_status();
#endif
}
