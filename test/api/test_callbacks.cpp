/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

std::function<void ()> * test_2nd_script_call = NULL;
int idx_test_function_1st_c_call=0;
bool test_function_1st_c_call_print=true;

void test_function_1st_c_call(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	if(test_function_1st_c_call_print==true){
		printf("C Function 1st call from script\n");
	}
	 idx_test_function_1st_c_call++;

	 if(test_2nd_script_call != NULL){
		 (*test_2nd_script_call)();
	 }
}


void test_callback(
	zetscript::ZetScript *_zs
	,zetscript::Function *_script_function
){
	zetscript::zs_float param1=1.5;
	zetscript::zs_int param2=2;
	bool param3=true;
	// get zetscript instance
	// transform script function to c++ function...
	auto script_function = _zs->bindFunction<zetscript::StringObject * (zetscript::zs_float *, zetscript::zs_int *, bool *)>(_script_function,__FILE__,__LINE__);

	// call the function with params 1,2,3. The function returns a 'StringObject' ...
	auto so=script_function(&param1,&param2,&param3);

	// print the results...
	if(test_function_1st_c_call_print==true){
		printf("FROM CALLBACK 1: calling function result: %s\n",so->toString().toConstChar());
	}

	// unref 'StringObject' lifetime
	_zs->unrefLifetimeObject(so);

}

void test_callback(
	zetscript::ZetScript *_zs
	,zetscript::Function *_script_function
	,zetscript::DictionaryObject *_user_params
){
	zetscript::zs_float param1=1.0;
	zetscript::zs_int param2=2.0;
	bool param3=true;

	// transform script function to c++ function...
	auto script_function = _zs->bindFunction<zetscript::StringObject * (zetscript::zs_float *, zetscript::zs_int *, bool *,zetscript::DictionaryObject *)>(_script_function,__FILE__,__LINE__);

	// call the function with params 1,2,3. The function returns a 'StringObject' ...
	auto so=script_function(&param1,&param2,&param3,_user_params);

	// print the results...
	if(test_function_1st_c_call_print==true){
		printf("FROM CALLBACK 2: %s\n",so->toString().toConstChar());
	}

	// unref 'StringObject' lifetime
	_zs->unrefLifetimeObject(so);

}

void test_call_script_c_script(zetscript::ZetScript *_zs, bool _show_print=true){

	test_function_1st_c_call_print=_show_print;

	// 1s combination: Script -> C -> Script
	// bind 'test_callback' receives a 'Function' pointer type
	_zs->registerFunction("test_callback",static_cast<void (*)(zetscript::ZetScript *_zs,zetscript::Function *_script_function)>(test_callback));

	// bind 'test_callback' receives a 'Function' and 'DictionaryObject' pointer type
	_zs->registerFunction("test_callback",static_cast<void (*)(zetscript::ZetScript *_zs,zetscript::Function *_script_function,zetscript::DictionaryObject *_params)>(test_callback));


	_zs->eval(
		// Type 'Test' declaration
		"class Test{\n"
		"	constructor(){\n"
		//      assign 'this.d=10'
		"       this.d=10;\n"
		// 		calls C function 'test_callback'.
		"		test_callback("
		//      1st parameter: Anonymous function ('Function *' in C aka 'function(){}')
		"			function("
		"				 _a\n"
		"				,_b\n"
		"				,_c\n"
		"				,_user_params" //--> params: Include aditional parameters to
		"			){\n"
		"				return \"result from object Test: a:\"+_a+\" b:\"+_b+\" c:\"+_c+\" d:\"+ _user_params.that.d;\n"
		"			}"
		//		2nd parameter: Object with user params ('DictionaryObject *' in C aka '{}')
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
		//   Anonymous function ('Function *' in C aka 'function(){}')
		"	function(a,b,c){\n"
		"		return \"result a:\"+a+\" b:\"+b+\" c:\"+c;\n"
		"});\n"
		"\n"
		""
	);
}

void test_call_c_script_c(zetscript::ZetScript *_zs, bool _show_print=true){


	// 2nd test calling from C->Script->C
	_zs->registerFunction("test_function_1st_c_call",test_function_1st_c_call);
	// test calling script-c-script-c

	_zs->eval(
			zetscript::StringUtils::format(
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

	auto  test_1st_script_call=_zs->bindFunction<void ()>("test_1st_script_call");
	test_2nd_script_call=new std::function<void()>(_zs->bindFunction<void ()>("test_2nd_script_call"));

	if(test_1st_script_call){
		test_1st_script_call();
	}

	delete test_2nd_script_call;
}

void test_call_c_script_c_no_print(zetscript::ZetScript *_zs){
	 test_call_c_script_c(_zs,false);
}

void test_call_script_c_script_no_print(zetscript::ZetScript *_zs){
	 test_call_script_c_script(_zs,false);
}

#ifdef __MAIN__
int main(){
	zetscript::ZetScript zs;
	try{
		test_call_script_c_script(&zs);
		test_call_c_script_c(&zs);
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
	}
	return 0;
}
#endif
