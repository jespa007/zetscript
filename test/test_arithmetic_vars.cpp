/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "test_arithmetic_common.h"



int main(int argc, char * argv[]) {

	int n_test=0;

	zetscript::ZetScript zs;

	try{

		// test declare var int/bool/zs_string/number
		printf("%i. testing primitive var\n",++n_test);

		// decalre vars
		zs.eval("var i,i1,i2,it1,it2,n1,n2,nt1,nt2;");

		TEST_INT_EXPR(&zs,"i=1;return i;",1);
		TEST_INT_EXPR(&zs,"i++;return i;",2);
		TEST_INT_EXPR(&zs,"++i;return i;",3);
		TEST_INT_EXPR(&zs,"i--;return i;",2);
		TEST_INT_EXPR(&zs,"--i;return i;",1);


		TEST_INT_EXPR(&zs,"i=10;i*=10;return i;",100);
		TEST_INT_EXPR(&zs,"i/=10;return i;",10);
		TEST_INT_EXPR(&zs,"i+=10;return i;",20);
		TEST_INT_EXPR(&zs,"i-=5;return i;",15);
		TEST_INT_EXPR(&zs,"i%=10;return i;",5);

		// test reassign and float
		TEST_FLOAT_EXPR(&zs,"i=2.0;return i;",2.0f);
		TEST_FLOAT_EXPR(&zs,"i++;return i;",3.0f);
		TEST_FLOAT_EXPR(&zs,"--i;return i;",2.0f);

		TEST_BOOL_EXPR(&zs,"i=true;return i;",true);
		TEST_BOOL_EXPR(&zs,"i=!i;return i;",false);
		TEST_BOOL_EXPR(&zs,"return i==i;",true);
		TEST_BOOL_EXPR(&zs,"return i!=i;",false);
		TEST_BOOL_EXPR(&zs,"i=!i;return i;",true);


		printf("%i. test if-else ...\n",++n_test);
		TEST_INT_EXPR(&zs,"i=0;if(i==0){i=10;}else{i=11;}return i;",10);
		TEST_INT_EXPR(&zs,"if(i==0){i=10;}else{i=11;}return i;",11);
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}

}
