/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "test_arithmetic_common.h"

#define COMPLETE_TEST_ARITHMETIC_INTEGER_OP(_script_engine,val1,val2) _complete_test_arithmetic_integer_op(_script_engine,val1, val2, "return %s%s%s")

#define COMPLETE_TEST_COMPARE_OP(zs,val1,val2) \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,val1<val2); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,val1*10<-val2/10); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,-(val1+10)<(val2-8)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(-val1-100)<(-val2+100)); \
		\
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1+10)>(val2-80)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,val1*70>(-val2+300)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,-val1*60>val2*90); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,-val1*90>-val2*60); \
		\
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1+10)<=10*val2); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,val1*10<=-80/val2); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,-val1/10<=val2*70); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(-val1-10)<=-val2*10); \
		\
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,val1*70>=val2); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,-val1/10>=-(val2+90)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(-val1+10)>=val2*10/10); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(-val1-val1)>=-(val2-val2-10)); \

#define COMPLETE_TEST_LOGIC_OP(zs,val1,val2) \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,!(val1>0)&&(val2>0)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1<0)&&(val2<0)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1>=0)&&!(val2>=0)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1<=0)&&(val2<=0)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1<=0)&&!(false)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1<=0)&&(true)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1>0)||!(val2>0)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1<0)||(val2<0)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1>=0)||(val2>=0)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,!(val1<=0)||(val2<=0)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1<=0)||(false)); \
		TEST_ARITHMETIC_BOOL_EXPRESSION(zs,(val1<=0)||(true));

void test_arithmetic_constants(zetscript::ScriptEngine *_script_engine, bool _show_print=true) {

	int n_test=0;

	if(_show_print) printf("%i. testing arithmetic operations integer...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(_script_engine,4,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(_script_engine,4,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(_script_engine,5,4); // op1 > op2


	if(_show_print) printf("%i. testing arithmetic operations hexa (int)...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(_script_engine,0x4,0x4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(_script_engine,0x4,0x5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(_script_engine,0x5,0x4); // op1 > op2

	if(_show_print) printf("%i. testing arithmetic operations float ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_script_engine,4.0,4.0); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_script_engine,4.0,5.0); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_script_engine,5.0,4.0); // op1 > op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_script_engine,5.0,2.0e2); // op1 > op2

	if(_show_print) printf("%i. testing arithmetic operations float vs int ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_script_engine,4.0,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_script_engine,4.0,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_script_engine,5.0,4); // op1 > op2

	if(_show_print) printf("%i. testing arithmetic operations int vs float ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_script_engine,4,4.0); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_script_engine,4,5.0); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_script_engine,5,4.0); // op1 > op2

	if(_show_print) printf("%i. testing String...\n",++n_test);
	TEST_ARITHMETIC_ASSIGNRING_EXPRESSION(_script_engine,"test_",+,"100"); // concatenate int
	TEST_ARITHMETIC_ASSIGNRING_EXPRESSION(_script_engine,"test_",+,"-100"); // concatenate -int
	TEST_ARITHMETIC_ASSIGNRING_EXPRESSION(_script_engine,"test_",+,"100.000000"); // concatenate float
	TEST_ARITHMETIC_ASSIGNRING_EXPRESSION(_script_engine,"test_",+,"-100.000000"); // concatenate float
	TEST_ARITHMETIC_ASSIGNRING_EXPRESSION(_script_engine,"test_",+,"true"); // concatenate bool
	TEST_ARITHMETIC_ASSIGNRING_EXPRESSION(_script_engine,"test_",+,"false"); // concatenate bool

	if(_show_print) printf("%i. testing parenthesis operations ...\n",++n_test);

	TEST_ARITHMETIC_INT_EXPRESSION(_script_engine,(((2+2*(5-9))*1000))/100);
	TEST_ARITHMETIC_FLOAT_EXPRESSION(_script_engine,(((2.0+2.0*(5.0-6.1))*1000.0))/100.0,_show_print);

	// test bool compare ...
	if(_show_print) printf("%i. testing compare operations ...\n",++n_test);
	COMPLETE_TEST_COMPARE_OP(_script_engine,10,10);
	COMPLETE_TEST_COMPARE_OP(_script_engine,15,10);
	COMPLETE_TEST_COMPARE_OP(_script_engine,10,15);

	// test logic and/or ...
	if(_show_print) printf("%i. testing logic operations ...\n",++n_test);
	COMPLETE_TEST_LOGIC_OP(_script_engine,10,10);
	COMPLETE_TEST_LOGIC_OP(_script_engine,15,10);
	COMPLETE_TEST_LOGIC_OP(_script_engine,10,15);

	// some basics tests
	TEST_ARITHMETIC_BOOL_EXPRESSION(_script_engine,!false && !(false || false));
	TEST_ARITHMETIC_BOOL_EXPRESSION(_script_engine,!(true && !false) || false);
	TEST_ARITHMETIC_BOOL_EXPRESSION(_script_engine,(true && !false) || !false);

}

void test_arithmetic_constants_no_print(zetscript::ScriptEngine *_script_engine) {
	test_arithmetic_constants(_script_engine,false);
}


#ifdef __MAIN__
int main(){
	zetscript::ScriptEngine script_engine;
	try{
		test_arithmetic_constants(&script_engine);
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}

	return 0;
}
#endif
