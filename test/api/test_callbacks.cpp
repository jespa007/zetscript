/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

std::function<void ()> * test_2nd_script_call = NULL;
int idx_test_function_1st_c_call=0;
bool test_function_1st_c_call_print=true;

void test_function_1st_c_call(zetscript::ScriptEngine *_script_engine){
	ZS_UNUSUED_PARAM(_script_engine);
	if(test_function_1st_c_call_print==true){
		printf("C ScriptFunction 1st call from script\n");
	}
	 idx_test_function_1st_c_call++;

	 if(test_2nd_script_call != NULL){
		 (*test_2nd_script_call)();
	 }
}


void test_callback(
	zetscript::ScriptEngine *_script_engine
	,zetscript::ScriptFunction *_script_function
){
	zetscript::zs_float param1=1.5;
	zetscript::zs_int param2=2;
	bool param3=true;
	// get zetscript instance
	// transform script function to c++ function...
	auto script_function = _script_engine->bindScriptFunction<zetscript::StringScriptObject * (zetscript::zs_float *, zetscript::zs_int *, bool *)>(_script_function,__FILE__,__LINE__);

	// call the function with params 1,2,3. The function returns a 'StringScriptObject' ...
	auto so=script_function(&param1,&param2,&param3);

	// print the results...
	if(test_function_1st_c_call_print==true){
		printf("FROM CALLBACK 1: calling function result: %s\n",so->toString().toConstChar());
	}

	// unref 'StringScriptObject' lifetime
	_script_engine->unrefLifetimeObject(so);

}

void test_callback(
	zetscript::ScriptEngine *_script_engine
	,zetscript::ScriptFunction *_script_function
	,zetscript::ObjectScriptObject *_user_params
){
	zetscript::zs_float param1=1.0;
	zetscript::zs_int param2=2.0;
	bool param3=true;

	// transform script function to c++ function...
	auto script_function = _script_engine->bindScriptFunction<zetscript::StringScriptObject * (zetscript::zs_float *, zetscript::zs_int *, bool *,zetscript::ObjectScriptObject *)>(_script_function,__FILE__,__LINE__);

	// call the function with params 1,2,3. The function returns a 'StringScriptObject' ...
	auto so=script_function(&param1,&param2,&param3,_user_params);

	// print the results...
	if(test_function_1st_c_call_print==true){
		printf("FROM CALLBACK 2: %s\n",so->toString().toConstChar());
	}

	// unref 'StringScriptObject' lifetime
	_script_engine->unrefLifetimeObject(so);

}

void test_call_script_c_script(zetscript::ScriptEngine *_script_engine, bool _show_print=true){

	test_function_1st_c_call_print=_show_print;

	// 1s combination: Script -> C -> Script
	// bind 'test_callback' receives a 'ScriptFunction' pointer type
	_script_engine->registerFunction("test_callback",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,zetscript::ScriptFunction *_script_function)>(test_callback));

	// bind 'test_callback' receives a 'ScriptFunction' and 'ObjectScriptObject' pointer type
	_script_engine->registerFunction("test_callback",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,zetscript::ScriptFunction *_script_function,zetscript::ObjectScriptObject *_params)>(test_callback));


	_script_engine->compileAndRun(
		// ScriptType 'Test' declaration
		"class Test{\n"
		"	constructor(){\n"
		//      assign 'this.d=10'
		"       this.d=10;\n"
		// 		calls C function 'test_callback'.
		"		test_callback("
		//      1st parameter: Anonymous function ('ScriptFunction *' in C aka 'function(){}')
		"			function("
		"				 _a\n"
		"				,_b\n"
		"				,_c\n"
		"				,_user_params" //--> params: Include aditional parameters to
		"			){\n"
		"				return \"result from object Test: a:\"+_a+\" b:\"+_b+\" c:\"+_c+\" d:\"+ _user_params.that.d;\n"
		"			}"
		//		2nd parameter: ScriptObject with user params ('ObjectScriptObject *' in C aka '{}')
		"			,{"
		"				that:this"
		"			}"
		"		);\n"
		"	}\n"
		"};\n"
		"\n"

		// test variable declaration and instances 'Text' type
		"var test=new Test();\n"

		// calls C function 'test_callback'.
		"test_callback(\n"
		//   Anonymous function ('ScriptFunction *' in C aka 'function(){}')
		"	function(a,b,c){\n"
		"		return \"result a:\"+a+\" b:\"+b+\" c:\"+c;\n"
		"});\n"
		"\n"
		""
	);
}

void test_call_c_script_c(zetscript::ScriptEngine *_script_engine, bool _show_print=true){


	// 2nd test calling from C->Script->C
	_script_engine->registerFunction("test_function_1st_c_call",test_function_1st_c_call);
	// test calling script-c-script-c

	_script_engine->compileAndRun(
			zetscript::String::format(
			"function test_1st_script_call(){\n"
				"if(%s){\n"
					"Console::outln (\"Hello from script\");\n"
				"}\n"
				"test_function_1st_c_call();\n"
			"}\n"
			"function test_2nd_script_call(){\n"
				"if(%s){\n"
					"Console::outln(\"2nd call script\");\n"
				"}\n"
			"}"
			,_show_print?"true":"false"
			,_show_print?"true":"false"
		)
	);

	auto  test_1st_script_call=_script_engine->bindScriptFunction<void ()>("test_1st_script_call");
	test_2nd_script_call=new std::function<void()>(_script_engine->bindScriptFunction<void ()>("test_2nd_script_call"));

	if(test_1st_script_call){
		test_1st_script_call();
	}

	delete test_2nd_script_call;
}

void test_call_c_script_c_no_print(zetscript::ScriptEngine *_script_engine){
	 test_call_c_script_c(_script_engine,false);
}

void test_call_script_c_script_no_print(zetscript::ScriptEngine *_script_engine){
	 test_call_script_c_script(_script_engine,false);
}

#ifdef __MAIN__
int main(){
	zetscript::ScriptEngine script_engine;
	try{
		test_call_script_c_script(&script_engine);
		test_call_c_script_c(&script_engine);
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
	}
	return 0;
}
#endif
