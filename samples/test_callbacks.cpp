/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

std::function<void ()> * test_2nd_script_call = NULL;

void test_function_1st_c_call(zetscript::ZetScript *_zs){
	 printf("C Function 1st call from script\n");

	 if(test_2nd_script_call != NULL){
		 (*test_2nd_script_call)();
	 }
}



void test_callback(
	zetscript::ZetScript *_zs
	,zetscript::ScriptFunction *_script_function
){
	zetscript::zs_float param1=1.0;
	zetscript::zs_int param2=2.0;
	bool param3=true;
	// get zetscript instance
	// transform script function to c++ function...
	auto script_function = _zs->bindScriptFunction<zetscript::ScriptObjectString * (zetscript::zs_float *, zetscript::zs_int *, bool *)>(_script_function,__FILE__,__LINE__);

	// call the function with params 1,2,3. The function returns a 'ScriptObjectString' ...
	auto so=script_function(&param1,&param2,&param3);

	// print the results...
	printf("FROM CALLBACK 1: calling function result: %s\n",so->toString().c_str());

	// unref 'ScriptObjectString' lifetime
	_zs->unrefLifetimeObject(so);

}

void test_callback(
	zetscript::ZetScript *_zs
	,zetscript::ScriptObjectMemberFunction *_script_function
){
	zetscript::zs_float param1=1.0;
	zetscript::zs_int param2=2.0;
	bool param3=true;

	// transform script function to c++ function...
	auto script_function = _zs->bindScriptFunction<zetscript::ScriptObjectString * (zetscript::zs_float *, zetscript::zs_int *, bool *)>(_script_function,__FILE__,__LINE__);

	// call the function with params 1,2,3. The function returns a 'ScriptObjectString' ...
	auto so=script_function(&param1,&param2,&param3);

	// print the results...
	printf("FROM CALLBACK 2: %s\n",so->toString().c_str());

	// unref 'ScriptObjectString' lifetime
	_zs->unrefLifetimeObject(so);

}


int main(){

	// instances ZetScript
	zetscript::ZetScript zs;

	try{

		// bind 'test_callback' receives a 'ScriptFunction' pointer type
		zs.registerFunction("test_callback",static_cast<void (*)(zetscript::ZetScript *_zs,zetscript::ScriptFunction *script_function)>(test_callback));

		// bind 'test_callback' receives a 'ScriptObjectMemberFunction' pointer type
		zs.registerFunction("test_callback",static_cast<void (*)(zetscript::ZetScript *_zs,zetscript::ScriptObjectMemberFunction *script_function)>(test_callback));


		zs.eval(
			// Type 'Test' declaration
			"class Test{\n"
			"	test_callback(){\n"
			//      assign 'this.d=10'
			"       this.d=10;\n"
			// 		calls C function 'test_callback'.
			"		test_callback("
			// 		passes current zetscript instance as 1st parameters
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
			"\n"
			"test.test_callback()"
			""
		);

		// 2nd test calling from script->C->script
		zs.registerFunction("test_function_1st_c_call",test_function_1st_c_call);
		// test calling script-c-script-c

		zs.eval("function test_1st_script_call(){\n"
					"Console::outln (\"Hello from script\");\n"
					"test_function_1st_c_call();\n"
				"}\n"
				"function test_2nd_script_call(){\n"
					"Console::outln(\"2nd call script\");\n"
				"}");

		auto  test_1st_script_call=zs.bindScriptFunction<void ()>("test_1st_script_call");
		test_2nd_script_call=new std::function<void()>(zs.bindScriptFunction<void ()>("test_2nd_script_call"));

		if(test_1st_script_call){
			test_1st_script_call();
		}

		delete test_2nd_script_call;

	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
	}

}
