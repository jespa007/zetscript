/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "test_arithmetic_common.h"

void test_arithmetic_vars(zetscript::ScriptEngine *_script_engine, bool _show_print=true) {

	int n_test=0;

	// test declare var int/bool/String/number
	if(_show_print) printf("%i. testing primitive var\n",++n_test);

	// decalre vars
	_script_engine->compileAndRun("var i,i1,i2,it1,it2,n1,n2,nt1,nt2;");

	TEST_CONSTANT_INT_EXPRESSION(_script_engine,"i=1;return i;",1);
	TEST_CONSTANT_INT_EXPRESSION(_script_engine,"i++;return i;",2);
	TEST_CONSTANT_INT_EXPRESSION(_script_engine,"++i;return i;",3);
	TEST_CONSTANT_INT_EXPRESSION(_script_engine,"i--;return i;",2);
	TEST_CONSTANT_INT_EXPRESSION(_script_engine,"--i;return i;",1);


	TEST_CONSTANT_INT_EXPRESSION(_script_engine,"i=10;i*=10;return i;",100);
	TEST_CONSTANT_INT_EXPRESSION(_script_engine,"i+=155;return i;",255);
	TEST_CONSTANT_INT_EXPRESSION(_script_engine,"i-=5;return i;",250);
	TEST_CONSTANT_FLOAT_EXPRESSION(_script_engine,"i/=10;return i;",25);
	TEST_CONSTANT_FLOAT_EXPRESSION(_script_engine,"i%=100;return i;",25);

	// test reassign and float
	TEST_CONSTANT_FLOAT_EXPRESSION(_script_engine,"i=2.0;return i;",2.0f);
	TEST_CONSTANT_FLOAT_EXPRESSION(_script_engine,"i++;return i;",3.0f);
	TEST_CONSTANT_FLOAT_EXPRESSION(_script_engine,"--i;return i;",2.0f);

	TEST_CONSTANT_BOOL_EXPRESSION(_script_engine,"i=true;return i;",true);
	TEST_CONSTANT_BOOL_EXPRESSION(_script_engine,"i=!i;return i;",false);
	TEST_CONSTANT_BOOL_EXPRESSION(_script_engine,"return i==i;",true);
	TEST_CONSTANT_BOOL_EXPRESSION(_script_engine,"return i!=i;",false);
	TEST_CONSTANT_BOOL_EXPRESSION(_script_engine,"i=!i;return i;",true);


	if(_show_print) printf("%i. test if-else ...\n",++n_test);
	TEST_CONSTANT_INT_EXPRESSION(_script_engine,"i=0;if(i==0){i=10;}else{i=11;}return i;",10);
	TEST_CONSTANT_INT_EXPRESSION(_script_engine,"if(i==0){i=10;}else{i=11;}return i;",11);

}

void test_arithmetic_vars_no_print(zetscript::ScriptEngine *_script_engine){
	test_arithmetic_vars(_script_engine,false);
}

#ifdef __MAIN__
int main(){
	zetscript::ScriptEngine script_engine;
	try{
		test_arithmetic_vars(&script_engine);
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}

	return 0;
}
#endif
