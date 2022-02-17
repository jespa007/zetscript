
#include "zetscript.h"

zetscript::ZetScript *zs=NULL;

void test_callback(zetscript::ScriptFunction *_script_function){
	zetscript::zs_float param1=1.0;
	zetscript::zs_int param2=2.0;
	bool param3=true;

	// transform script function to c++ function...
	auto script_function = zs->bindScriptFunction<zetscript::ScriptObjectString * (zetscript::zs_float *, zetscript::zs_int *, bool *)>(_script_function,__FILE__,__LINE__);

	// call the function. The function return a 'ScriptObjectString' ...
	auto so=script_function(&param1,&param2,&param3);

	// print the results...
	printf("FROM CALLBACK 1: calling function result: %s\n",so->toString().c_str());

	// unref 'ScriptObjectString' lifetime
	zs->unrefLifetimeObject(so);

}

void test_callback(zetscript::ScriptObjectMemberFunction *_script_function){
	zetscript::zs_float param1=1.0;
	zetscript::zs_int param2=2.0;
	bool param3=true;

	// transform script function to c++ function...
	auto script_function = zs->bindScriptFunction<zetscript::ScriptObjectString * (zetscript::zs_float *, zetscript::zs_int *, bool *)>(_script_function,__FILE__,__LINE__);

	// call the function. The function return a 'ScriptObjectString' ...
	auto so=script_function(&param1,&param2,&param3);

	// print the results...
	printf("FROM CALLBACK 2: %s\n",so->toString().c_str());

	// unref 'ScriptObjectString' lifetime
	zs->unrefLifetimeObject(so);

}


int main(){

	// instances ZetScript
	zetscript::ZetScript *zs = new zetscript::ZetScript();

	try{

		// bind 'test_callback' receives a 'ScriptFunction' pointer type
		zs->registerFunction("test_callback",static_cast<void (*)(zetscript::ScriptFunction *script_function)>(test_callback));

		// bind 'test_callback' receives a 'ScriptObjectMemberFunction' pointer type
		zs->registerFunction("test_callback",static_cast<void (*)(zetscript::ScriptObjectMemberFunction *script_function)>(test_callback));


		zs->eval(
			// Function say hello
			"function say_hello(){\n"
			"	Console::outln(\"hello!\")\n"
			"}\n"
			"\n"
			// Type 'Test' declaration
			"class Test{\n"
			"	function1(arg){\n"
			//      assign 'this.d=10'
			"       this.d=10;\n"
			// 		calls C function 'test_callback'.
			"		test_callback("
			//      Anonymous function as parameter: The test_callback function expects 'ScriptObjectMemberFunction' in C++ due is created in a class scope
			//		Note: The anonymous function content is reading the 'this.d', so it has access to object 'Test' type scope
			"			function(a,b,c){\n"
			"				return \"result from object Test: a:\"+a+\" b:\"+b+\" c:\"+c+\" this.d:\"+this.d;\n"
			"			}\n"
			"		);\n"
			"	}\n"
			"};\n"
			"\n"

			// test variable declaration and instances 'Text' type
			"var test=new Test();\n"

			// calls C function 'test_callback'.
			"test_callback(\n"
			//  Anonymous function as parameter: The test_callback function expects 'ScriptFunction' in C++ due is created in a main scope
			"	function(a,b,c){\n"
			"		return \"result a:\"+a+\" b:\"+b+\" c:\"+c;\n"
			"});\n"
			""
		);

		// instance function delete_test function.
		auto  say_hello=zs->bindScriptFunction<void ()>("say_hello");

		// it calls "say_hello" function with no parameters
		say_hello();

		// instance member function test.function1.
		auto  test_function1=zs->bindScriptFunction<void (zetscript::zs_int)>("test.function1");

		// it calls "test.function1" member function with 10 as parameter.
		test_function1(10);


	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
	}

	delete zs;
}
