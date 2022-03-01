/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "test_arithmetic_common.h"


#define COMPLETE_TEST_COMPARE_OP(zs,val1,val2) \
		TEST_ARITHMETIC_BOOL_EXPR(zs,val1<val2); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,val1*10<-val2/10); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,-(val1+10)<(val2-8)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(-val1-100)<(-val2+100)); \
		\
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1+10)>(val2-80)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,val1*70>(-val2+300)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,-val1*60>val2*90); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,-val1*90>-val2*60); \
		\
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1+10)<=10*val2); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,val1*10<=-80/val2); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,-val1/10<=val2*70); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(-val1-10)<=-val2*10); \
		\
		TEST_ARITHMETIC_BOOL_EXPR(zs,val1*70>=val2); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,-val1/10>=-(val2+90)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(-val1+10)>=val2*10/10); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(-val1-val1)>=-(val2-val2-10)); \

#define COMPLETE_TEST_LOGIC_OP(zs,val1,val2) \
		TEST_ARITHMETIC_BOOL_EXPR(zs,!(val1>0)&&(val2>0)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1<0)&&(val2<0)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1>=0)&&!(val2>=0)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1<=0)&&(val2<=0)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1<=0)&&!(false)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1<=0)&&(true)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1>0)||!(val2>0)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1<0)||(val2<0)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1>=0)||(val2>=0)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,!(val1<=0)||(val2<=0)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1<=0)||(false)); \
		TEST_ARITHMETIC_BOOL_EXPR(zs,(val1<=0)||(true));

int main(int argc, char * argv[]) {

	int n_test=0;

	zetscript::ZetScript zs;

	try{

		printf("%i. testing arithmetic operations integer...\n",++n_test);
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP(&zs,4,4); // op1==op2
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP(&zs,4,5); // op1 < op2
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP(&zs,5,4); // op1 > op2


		printf("%i. testing arithmetic operations hexa (int)...\n",++n_test);
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP(&zs,0x4,0x4); // op1==op2
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP(&zs,0x4,0x5); // op1 < op2
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP(&zs,0x5,0x4); // op1 > op2

		printf("%i. testing arithmetic operations float ...\n",++n_test);
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP(&zs,4.0,4.0); // op1==op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP(&zs,4.0,5.0); // op1 < op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP(&zs,5.0,4.0); // op1 > op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP(&zs,5.0,2.0e2); // op1 > op2

		printf("%i. testing arithmetic operations float vs int ...\n",++n_test);
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP(&zs,4.0,4); // op1==op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP(&zs,4.0,5); // op1 < op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP(&zs,5.0,4); // op1 > op2

		printf("%i. testing arithmetic operations int vs float ...\n",++n_test);
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP(&zs,4,4.0); // op1==op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP(&zs,4,5.0); // op1 < op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP(&zs,5,4.0); // op1 > op2

		printf("%i. testing zs_string...\n",++n_test);
		TEST_ARITHMETIC_STRING_OP(&zs,"test_",+,"100"); // concatenate int
		TEST_ARITHMETIC_STRING_OP(&zs,"test_",+,"-100"); // concatenate -int
		TEST_ARITHMETIC_STRING_OP(&zs,"test_",+,"100.000000"); // concatenate float
		TEST_ARITHMETIC_STRING_OP(&zs,"test_",+,"-100.000000"); // concatenate float
		TEST_ARITHMETIC_STRING_OP(&zs,"test_",+,"true"); // concatenate bool
		TEST_ARITHMETIC_STRING_OP(&zs,"test_",+,"false"); // concatenate bool

		printf("%i. testing parenthesis operations ...\n",++n_test);

		TEST_ARITHMETIC_INT_EXPR(&zs,(((2+2*(5-9))*1000))/100);
		TEST_ARITHMETIC_FLOAT_EXPR(&zs,(((2.0+2.0*(5.0-6.1))*1000.0))/100.0);

		// test bool compare ...
		printf("%i. testing compare operations ...\n",++n_test);
		COMPLETE_TEST_COMPARE_OP(&zs,10,10);
		COMPLETE_TEST_COMPARE_OP(&zs,15,10);
		COMPLETE_TEST_COMPARE_OP(&zs,10,15);

		// test logic and/or ...
		printf("%i. testing logic operations ...\n",++n_test);
		COMPLETE_TEST_LOGIC_OP(&zs,10,10);
		COMPLETE_TEST_LOGIC_OP(&zs,15,10);
		COMPLETE_TEST_LOGIC_OP(&zs,10,15);

		// some basics tests
		TEST_ARITHMETIC_BOOL_EXPR(&zs,!false && !(false || false));
		TEST_ARITHMETIC_BOOL_EXPR(&zs,!(true && !false) || false);
		TEST_ARITHMETIC_BOOL_EXPR(&zs,(true && !false) || !false);
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}

}
