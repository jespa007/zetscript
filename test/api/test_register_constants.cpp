/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "test_arithmetic_common.h"

void test_register_constants(zetscript::ScriptEngine *_zs) {

	//int n_test=0;

	_zs->registerConstant("TEST_INT", 100);
	_zs->registerConstant("TEST_FLOAT", 1.5f);
	_zs->registerConstant("TEST_BOOL", true);
	_zs->registerConstant("TEST_STRING", "constant_string");

	_zs->compileAndRun(
			"System::assert(TEST_INT==100,\"TEST_INT!=100\")\n"
			"System::assert(TEST_FLOAT==1.5,\"TEST_FLOAT!=1.5\")\n"
			"System::assert(TEST_BOOL==true,\"TEST_BOOL!=true\")\n"
			"System::assert(TEST_STRING==\"constant_string\",\"TEST_STRING!=\\\"constant_string\\\"\")\n"
	);
}


#ifdef __MAIN__
int main(){
	zetscript::ScriptEngine zs;
	try{
		test_register_constants(&zs);
	}catch(std::exception & ex){
		fprintf(stderr,"%s",ex.what());
		return -1;
	}

	return 0;
}
#endif
