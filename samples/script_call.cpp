
#include "zetscript.h"

using namespace zetscript;

ZetScript *zs=NULL;

void test_callback(ScriptFunction *script_function){
	zs_float param1=1.0;
	zs_int param2=2.0;
	bool param3=true;

	// transform script function to c++ function...
	auto callback_function = zs->bindScriptFunction<ScriptObjectString * (zs_float *, zs_int *, bool *)>(script_function,__FILE__,__LINE__);

	// call the function...
	auto so=(*callback_function)(&param1,&param2,&param3);

	// print the results...
	printf("calling function result: %s\n",so->toString().c_str());

	// unref string object lifetime when is not used anymore
	zs->unrefLifetimeObject(so);

	// delete function...
	delete callback_function;
}


int main(){

	zs = new ZetScript(); // instance ZetScript

	try{

		// bind global function
		zs->registerFunction("test_callback",test_callback);

		zs->eval(
			"class Test{\n"
				"var a\n"
			"	function1(arg){\n"
			"       this.d=10;\n"
			"		Console::outln(\"calling Test.Function:\"+arg+\" a:\"+this.a);\n"
			/*"		test_callback(function(a,b,c){\n"
			"			return \"result a:\"+a+\" b:\"+b+\" c:\"+c+\" this.d:\"+this.d;\n"
			"		});\n"*/
			"	}\n"
			"};\n"
			"\n"
			"function delete_test(){\n"
			"	delete test;\n"
			"	Console::outln(\"test variable was deleted\");\n"
			"}\n"
			"var test=new Test();\n"
			"test_callback(function(a,b,c){\n"
			"		return \"result a:\"+a+\" b:\"+b+\" c:\"+c;\n"
			"});\n"
			""
		,EVAL_OPTION_SHOW_USER_BYTE_CODE);

		std::function<void()>  * delete_test=zs->bindScriptFunction<void ()>("delete_test"); // instance function delete_test function.
		std::function<void(zs_int)> * test_function1=zs->bindScriptFunction<void (zs_int)>("test.function1"); // instance member function test.function1.

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
