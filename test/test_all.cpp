/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"


int main(int argc, char * argv[]) {
	zetscript::ZetScript zs;

	// test all external tests...
	const char *test_files[]={
		//"samples/assert_error.zs"
		"samples/assign.zs"
		,"samples/class_property.zs"
		,"samples/class_inheritance_call.zs"
		,"samples/class_metamethod.zs"
		,"samples/const.zs"
		,"samples/datetime.zs"
		,"samples/eval.zs"
		,"samples/for_in_object.zs"
		,"samples/for_in_string.zs"
		,"samples/for_in_vector.zs"
		,"samples/function_arg_by_ref.zs"
		,"samples/function_arg_default.zs"
		,"samples/function.zs"
		,"samples/if_else.zs"
		,"samples/import.zs"
		,"samples/in.zs"
		,"samples/instanceof.zs"
		,"samples/iterator_object.zs"
		,"samples/iterator_string.zs"
		,"samples/iterator_vector.zs"
		,"samples/json.zs"
		,"samples/loops_break_continue.zs"
		,"samples/loops.zs"
		,"samples/object.zs"
		,"samples/return.zs"
		,"samples/string.zs"
		,"samples/switch.zs"
		,"samples/ternary.zs"
		,"samples/typeof.zs"
		,"samples/vector.zs"
		,"samples/zs_int.zs"
		,0
	};

	printf("======================================\n");
	printf("Testing all zetscript samples\n");

	char **it=(char **)test_files;
	int total=sizeof(test_files)/sizeof(char **);
	int n=1;

	while(*it!=0){
		// clear all vars in order to no have conflict with previous evaluations
		zs.clear();
		printf("Evaluating %i/%i:'%s'\n",n++,total,*it);
		zs.evalFile(*it);
		it++;
	}

	printf("All tests passed OK!\n");
}
