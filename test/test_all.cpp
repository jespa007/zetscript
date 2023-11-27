/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"
#include "test_all_config.h"

#include "api/test_arithmetic_constants.cpp"
#include "api/test_arithmetic_metamethods.cpp"
#include "api/test_arithmetic_common.cpp"
#include "api/test_arithmetic_vars.cpp"
#include "api/test_call_native_functions.cpp"
#include "api/test_callbacks.cpp"
#include "api/test_consecutive_evals.cpp"
#include "api/test_register_constants.cpp"



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
		 {"test_arithmetic_constants",test_arithmetic_constants_no_print}						// 1
		,{"test_arithmetic_metamethods",test_arithmetic_metamethods_no_print}					// 2
		,{"test_arithmetic_vars",test_arithmetic_vars_no_print}									// 3
		,{"test_call_native_function",test_call_native_function_no_print}						// 4
		,{"test_call_script_c_script",test_call_script_c_script_no_print}						// 5
		,{"test_call_c_script_c",test_call_c_script_c_no_print}									// 6
		,{"test_anonymous_scopes",test_anonymous_scopes_no_print}								// 7
		,{"test_consistency_function_override",test_consistency_function_override_no_print}		// 8
		,{"test_register_constants",test_register_constants}									// 9
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


		zs.clear();
		printf("* Test native %2i/%i - %s ... ",++n,test_native_total,name);
		try{
			it_test_native_functions->fun(&zs);
			n_test_native_success++;
			printf("OK\n");
		}catch(std::exception & ex){
			printf("Failed: %s\n",ex.what());
			n_test_native_failed++;
		}
		it_test_native_functions++;
	}


	printf("======================================\n\n");
	printf("Testing language samples\n\n");
	printf("-Script test path: \"%s\"\n\n",ZS_TEST_ALL_SCRIPT_TEST_PATH);

	// test all external tests...
	const char *test_script_files[]={
		//"samples/assert_error.zs"
		"test_assign"								//1.
		,"test_call"								//2.
		,"test_arithmetic_metamethods"				//3.
		,"test_const"								//4.
		,"test_eval"								//5.
		,"test_for_in_object"						//6.
		,"test_for_in_string"						//7.
		,"test_for_in_array"						//8.
		,"test_function_arg_by_ref"					//9.
		,"test_function_arg_default"				//10.
		,"test_if_else"								//11.
		,"test_import"								//12.
		,"test_in"									//13.
		,"test_instanceof"							//14.
		,"test_instance_from_var_type"				//15.
		,"test_iterator_object"						//16.
		,"test_iterator_string"						//17.
		,"test_iterator_array"						//18.
		,"test_json"								//19.
		,"test_loops_break_continue"				//20.
		,"test_loops"								//21.
		,"test_object"								//22.
		,"test_string"								//23.
		,"test_switch"								//24.
		,"test_ternary"								//25.
		,"test_typeof"								//26.
		,"test_array"								//27.
		,"test_integer_values"						//28.
		,"test_constant_operations"					//29.
		,"test_member_functions"					//30.
		,"test_member_variables"					//31.
		,"test_postinc"								//32.
		,"test_cyclic_references"					//33.
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
			zetscript::String filename=zetscript::String::format("%s/%s.zs",ZS_TEST_ALL_SCRIPT_TEST_PATH,*it_test_script_files);

			if(zetscript::File::exists(filename.toConstChar())==false){
				throw std::runtime_error("file not exist");
			}

			zs.evalFile(filename.toConstChar());


			if(strcmp(*it_test_script_files,"test_cyclic_references")==0){

				// remove cyclic container instances
				vm_deref_cyclic_references(zs.getVirtualMachine());

				vm_remove_empty_shared_pointers(
						zs.getVirtualMachine()
					,vm_get_scope_block_main(zs.getVirtualMachine())
				);

				auto cyclic_container_instances=vm_get_cyclic_container_instances(zs.getVirtualMachine());

				if(cyclic_container_instances->count() > 0){
					throw std::runtime_error("Some cyclic container instances still not freed");
				}
			}

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
