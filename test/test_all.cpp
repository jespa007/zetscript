/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"
#include "test_all_config.h"

#include "test_arithmetic_constants.cpp"
#include "test_arithmetic_metamethods.cpp"
#include "test_arithmetic_common.cpp"
#include "test_arithmetic_vars.cpp"
#include "test_call_native_functions.cpp"
#include "test_callbacks.cpp"
#include "test_consecutive_evals.cpp"
#include "test_register_constants.cpp"



#define MAX_TEST_NAME_FIXED_LENGTH 30

#ifdef __DEBUG__
const char *post_exe_name="_d";
#else
const char *post_exe_name="";
#endif

int main(int argc, char * argv[]) {
	ZS_UNUSUED_2PARAMS(argc,argv);
	zetscript::ZetScript zs;


	printf("======================================\n\n");
	printf("Testing native samples\n\n");

	// get all files in the path
	typedef struct{
		const char *name;
		void (* fun)(zetscript::ZetScript *_zs);
	}TestNativeFunctionIterator;

	TestNativeFunctionIterator test_native_functions[]={
		 {"test_arithmetic_constants",test_arithmetic_constants_no_print}				// 1
		,{"test_arithmetic_metamethods",test_arithmetic_metamethods_no_print}			// 2
		,{"test_arithmetic_vars",test_arithmetic_vars_no_print}							// 3
		,{"test_call_native_function_with_nulls",test_call_native_function_with_nulls}	// 4
		,{"test_call_script_c_script",test_call_script_c_script}						// 5
		,{"test_call_c_script_c",test_call_c_script_c}									// 6
		,{"test_anonymous_scopes",test_anonymous_scopes_no_print}						// 7
		,{"test_consistency_function_override",test_consistency_function_override}		// 8
		,{"test_register_constants",test_register_constants}							// 9
		,{0,0}
	};
	char name[MAX_TEST_NAME_FIXED_LENGTH+20]={0};
	TestNativeFunctionIterator *it_test_native_functions=(TestNativeFunctionIterator *)test_native_functions;
	int test_native_total=sizeof(test_native_functions)/sizeof(TestNativeFunctionIterator)-1;
	int n=0;
	int n_test_native_success=0;
	int n_test_native_failed=0;

	while(it_test_native_functions->name!=0){
		sprintf(name,"%s",it_test_native_functions->name);
		size_t len=strlen(name);
		for(size_t i=len; i < MAX_TEST_NAME_FIXED_LENGTH; i++){
			name[i]=' ';
		}
		name[MAX_TEST_NAME_FIXED_LENGTH]=0;


		printf("* Test native %2i/%i - %s ... ",++n,test_native_total,name);
		try{
			it_test_native_functions->fun(&zs);
			n_test_native_success++;
			printf("OK\n");
		}catch(std::exception & ex){
			fprintf(stderr,"Failed: %s\n",ex.what());
			n_test_native_failed++;
		}
		it_test_native_functions++;
	}


	printf("======================================\n\n");
	printf("Testing script samples\n\n");
	printf("-Script test path: \"%s\"\n\n",ZS_TEST_ALL_SCRIPT_TEST_PATH);

	// test all external tests...
	const char *test_script_files[]={
		//"samples/assert_error.zs"
		"test_assign"
		,"test_class_property"
		,"test_call_member_function"
		,"test_class_metamethod"
		,"test_const"
		,"test_eval"
		,"test_for_in_object"
		,"test_for_in_string"
		,"test_for_in_vector"
		,"test_function_arg_by_ref"
		,"test_function_arg_default"
		,"test_if_else"
		,"test_import"
		,"test_in"
		,"test_instanceof"
		,"test_instance_from_var"
		,"test_iterator_object"
		,"test_iterator_string"
		,"test_iterator_vector"
		,"test_json"
		,"test_loops_break_continue"
		,"test_loops"
		,"test_object"
		,"test_string"
		,"test_switch"
		,"test_ternary"
		,"test_typeof"
		,"test_vector"
		,"test_integer_values"
		,"test_constant_operations"
		,0
	};

	char **it_test_script_files=(char **)test_script_files;

	n=0;
	int test_script_total=sizeof(test_script_files)/sizeof(char **)-1;
	int n_test_script_success=0;
	int n_test_script_failed=0;

	while(*it_test_script_files!=0){
		sprintf(name,"%s",*it_test_script_files);
		size_t len=strlen(name);
		for(size_t i=len; i < MAX_TEST_NAME_FIXED_LENGTH; i++){
			name[i]=' ';
		}
		name[MAX_TEST_NAME_FIXED_LENGTH]=0;

		// clear all vars in order to no have conflict with previous evaluations
		zs.clear();
		printf("* Test script %2i/%i - %s ... ",++n,test_script_total,name);
		try{
			zetscript::zs_string filename=zetscript::zs_strutils::format("%s/%s.zs",ZS_TEST_ALL_SCRIPT_TEST_PATH,*it_test_script_files);

			if(zetscript::zs_file::exists(filename.c_str())==false){
				throw std::runtime_error("file not exist");
			}

			zs.evalFile(filename.c_str());
			n_test_script_success++;
			printf("OK\n");
		}catch(std::exception & ex){
			printf("Failed: %s\n",ex.what());
			n_test_script_failed++;
		}
		it_test_script_files++;
	}


	printf("\n");
	printf("Native tests success: %i of %i!\n",n_test_native_success,test_native_total);
	printf("Native tests failed: %i of %i!\n",n_test_native_failed,test_native_total);

	printf("\n");
	printf("Script tests success: %i of %i!\n",n_test_script_success,test_script_total);
	printf("Script tests failed: %i of %i!\n",n_test_script_failed,test_script_total);
}
