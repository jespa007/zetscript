/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "test_arithmetic_common.h"

void test_arithmetic_vars(zetscript::ZetScript *_zs, bool _show_print=true) {

	int n_test=0;

	// test declare var int/bool/std::string/number
	if(_show_print) printf("%i. testing primitive var\n",++n_test);

	// decalre vars
	_zs->eval("var i,i1,i2,it1,it2,n1,n2,nt1,nt2;");

	TEST_CONSTANT_INT_EXPRESSION(_zs,"i=1;return i;",1);
	TEST_CONSTANT_INT_EXPRESSION(_zs,"i++;return i;",2);
	TEST_CONSTANT_INT_EXPRESSION(_zs,"++i;return i;",3);
	TEST_CONSTANT_INT_EXPRESSION(_zs,"i--;return i;",2);
	TEST_CONSTANT_INT_EXPRESSION(_zs,"--i;return i;",1);


	TEST_CONSTANT_INT_EXPRESSION(_zs,"i=10;i*=10;return i;",100);
	TEST_CONSTANT_INT_EXPRESSION(_zs,"i/=10;return i;",10);
	TEST_CONSTANT_INT_EXPRESSION(_zs,"i+=10;return i;",20);
	TEST_CONSTANT_INT_EXPRESSION(_zs,"i-=5;return i;",15);
	TEST_CONSTANT_INT_EXPRESSION(_zs,"i%=10;return i;",5);

	// test reassign and float
	TEST_CONSTANT_FLOAT_EXPRESSION(_zs,"i=2.0;return i;",2.0f);
	TEST_CONSTANT_FLOAT_EXPRESSION(_zs,"i++;return i;",3.0f);
	TEST_CONSTANT_FLOAT_EXPRESSION(_zs,"--i;return i;",2.0f);

	TEST_CONSTANT_BOOL_EXPRESSION(_zs,"i=true;return i;",true);
	TEST_CONSTANT_BOOL_EXPRESSION(_zs,"i=!i;return i;",false);
	TEST_CONSTANT_BOOL_EXPRESSION(_zs,"return i==i;",true);
	TEST_CONSTANT_BOOL_EXPRESSION(_zs,"return i!=i;",false);
	TEST_CONSTANT_BOOL_EXPRESSION(_zs,"i=!i;return i;",true);


	if(_show_print) printf("%i. test if-else ...\n",++n_test);
	TEST_CONSTANT_INT_EXPRESSION(_zs,"i=0;if(i==0){i=10;}else{i=11;}return i;",10);
	TEST_CONSTANT_INT_EXPRESSION(_zs,"if(i==0){i=10;}else{i=11;}return i;",11);

}

void test_arithmetic_vars_no_print(zetscript::ZetScript *_zs){
	test_arithmetic_vars(_zs,false);
}

#ifdef __MAIN__
int main(){
	zetscript::ZetScript zs;
	try{
		test_arithmetic_vars(&zs);
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}

	return 0;
}
#endif
