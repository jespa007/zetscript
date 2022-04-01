/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"
#include "test_all_config.h"

#define FILENAME_FIXED_LENGTH_FORMAT 30

int main(int argc, char * argv[]) {
	zetscript::ZetScript zs;

	// get all files in the path

	// test all external tests...
	const char *test_files[]={
		//"samples/assert_error.zs"
		"test_assign.zs"
		,"test_class_property.zs"
		,"test_class_inheritance_call.zs"
		,"test_class_metamethod.zs"
		,"test_const.zs"
		,"test_datetime.zs"
		,"test_eval.zs"
		,"test_for_in_object.zs"
		,"test_for_in_string.zs"
		,"test_for_in_vector.zs"
		,"test_function_arg_by_ref.zs"
		,"test_function_arg_default.zs"
		,"test_function.zs"
		,"test_if_else.zs"
		,"test_import.zs"
		,"test_in.zs"
		,"test_instanceof.zs"
		,"test_iterator_object.zs"
		,"test_iterator_string.zs"
		,"test_iterator_vector.zs"
		,"test_json.zs"
		,"test_loops_break_continue.zs"
		,"test_loops.zs"
		,"test_object.zs"
		,"test_return.zs"
		,"test_string.zs"
		,"test_switch.zs"
		,"test_ternary.zs"
		,"test_typeof.zs"
		,"test_vector.zs"
		,"test_integer_values.zs"
		,0
	};

	printf("======================================\n\n");
	printf("Testing all zetscript samples\n\n");
	printf("-Script test path: \"%s\"\n\n",ZS_TEST_ALL_SCRIPT_TEST_PATH);

	char file[FILENAME_FIXED_LENGTH_FORMAT+20]={0};
	char **it=(char **)test_files;
	int total=sizeof(test_files)/sizeof(char **);
	int n=0;
	int n_success=0;
	int n_failed=0;

	while(*it!=0){
		sprintf(file,"%s",*it);
		size_t len=strlen(file);
		for(size_t i=len; i < FILENAME_FIXED_LENGTH_FORMAT; i++){
			file[i]=' ';
		}
		file[FILENAME_FIXED_LENGTH_FORMAT]=0;

		zetscript::zs_string filename=zetscript::zs_strutils::format("%s/%s",ZS_TEST_ALL_SCRIPT_TEST_PATH,*it);
		// clear all vars in order to no have conflict with previous evaluations
		zs.clear();
		printf("* Test %2i/%02i - %s ... ",++n,total,file);
		try{
			if(zetscript::zs_file::exists(filename.c_str())==false){
				throw std::runtime_error("file not exist");
			}

			zs.evalFile(filename.c_str());
			n_success++;
			printf("OK\n",++n,total,*it);
		}catch(std::exception & ex){
			fprintf(stderr,"Failed: %s\n",ex.what());
			n_failed++;
		}
		it++;
	}

	printf("\n");
	printf("Tests success: %i of %i!\n",n_success,n);
	printf("Tests failed: %i of %i!\n",n_failed,n);
}
