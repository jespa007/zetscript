/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "test_arithmetic_common.h"

void test_register_constants(zetscript::ScriptEngine *_script_engine) {

	//int n_test=0;

	_script_engine->registerConstantInteger("TEST_INT", 100);
	_script_engine->registerConstantFloat("TEST_FLOAT", 1.5f);
	_script_engine->registerConstantBoolean("TEST_BOOL", true);
	_script_engine->registerConstantString("TEST_STRING", "constant_string");

	_script_engine->compileAndRun(
			"System::assert(TEST_INT==100,\"TEST_INT!=100\")\n"
			"System::assert(TEST_FLOAT==1.5,\"TEST_FLOAT!=1.5\")\n"
			"System::assert(TEST_BOOL==true,\"TEST_BOOL!=true\")\n"
			"System::assert(TEST_STRING==\"constant_string\",\"TEST_STRING!=\\\"constant_string\\\"\")\n"
	);
}


#ifdef __MAIN__
int main(){
	zetscript::ScriptEngine script_engine;
	try{
		test_register_constants(&script_engine);
	}catch(std::exception & ex){
		fprintf(stderr,"%s",ex.what());
		return -1;
	}

	return 0;
}
#endif
