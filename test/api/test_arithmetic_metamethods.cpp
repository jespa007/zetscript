/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "test_arithmetic_common.h"
#include "NumberZs.h"


#define COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CC(_se,val1,val2)  _complete_test_arithmetic_integer_op(_se,val1,val2,"it1=("\
				"(new Number("\
				"%s" \
				"))"\
				"%s" \
				"(new Number("\
				"%s" \
				"))"\
				");\n" \
				"it2=it1.toInteger();\n" \
				"return it2;\n" \
	)

#define COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CI(_se,val1,val2)  _complete_test_arithmetic_integer_op(_se,val1,val2,"it1=("\
				"(new Number("\
				"%s" \
				"))"\
				"%s" \
				"(%s)"\
				");\n" \
				"it2=it1.toInteger();\n" \
				"return it2;\n" \
	)

#define COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_IC(_se,val1,val2)  _complete_test_arithmetic_integer_op(_se,val1,val2,"it1=("\
				"("\
				"%s" \
				")"\
				"%s" \
				"(new Number("\
				"%s" \
				"))"\
				");\n" \
				"it2=it1.toInteger();\n" \
				"return it2;\n" \
	)

// Float operations
#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CC(_se,val1,val2)  _complete_test_arithmetic_float_op(_se,(zetscript::zs_float)val1,(zetscript::zs_float)val2,"it1=("\
				"(new Number("\
				"%s" \
				"))"\
				"%s" \
				"(new Number("\
				"%s" \
				"))"\
				");\n" \
				"it2=it1.toFloat();\n" \
				"return it2;\n" \
	)

#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CF(_se,val1,val2)  _complete_test_arithmetic_float_op(_se,(zetscript::zs_float)val1,(zetscript::zs_float)val2,"it1=("\
				"(new Number("\
				"%s" \
				"))"\
				"%s" \
				"("\
				"%s" \
				")"\
				");\n" \
				"it2=it1.toFloat();\n" \
				"return it2;\n" \
	)

#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_FC(_se,val1,val2)  _complete_test_arithmetic_float_op(_se,(zetscript::zs_float)val1,(zetscript::zs_float)val2,"it1=("\
				"("\
				"%s" \
				")"\
				"%s" \
				"(new Number("\
				"%s" \
				"))"\
				");\n" \
				"it2=it1.toFloat();\n" \
				"return it2;\n" \
	)



void test_arithmetic_metamethods(zetscript::ScriptEngine *_se,bool _show_print=true) {

	int n_test=0;
	//zetscript::zs_int result_assign=10;
	//zetscript::zs_float result_float_assign=20;

	// register wraps
	NumberZs_register(_se);

	_se->compileAndRun("var i,i1,i2,it1,it2,n1,n2,nt1,nt2;");

	// testing metamethods...
	if(_show_print) printf("%i. testing arithmetic operations int cc...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CC(_se,4,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CC(_se,4,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CC(_se,5,4); // op1 > op2

	if(_show_print) printf("%i. testing arithmetic operations int ci...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CI(_se,4,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CI(_se,4,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CI(_se,5,4); // op1 > op2

	if(_show_print) printf("%i. testing arithmetic operations int ic...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_IC(_se,4,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_IC(_se,4,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_IC(_se,5,4); // op1 > op2


	if(_show_print) printf("%i. testing arithmetic operations float cc ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CC(_se,4.2,4.2); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CC(_se,4.2,5.2); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CC(_se,5.2,4.2); // op1 > op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CC(_se,5.2,2.0e2); // op1 > op2

	if(_show_print) printf("%i. testing arithmetic operations float cf ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CF(_se,4.2,4.2); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CF(_se,4.2,5.2); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CF(_se,5.2,4.2); // op1 > op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CF(_se,5.2,2.0e2); // op1 > op2

	if(_show_print) printf("%i. testing arithmetic operations float fc ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_FC(_se,4.2,4.2); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_FC(_se,4.2,5.2); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_FC(_se,5.2,4.2); // op1 > op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_FC(_se,5.2,2.0e2); // op1 > op2

	if(_show_print) printf("%i. testing arithmetic operations integer assign ...\n",++n_test);
	_se->compileAndRun("var result_assign=new Number();");
	_complete_test_arithmetic_integer_op_assign(_se,5,10,"result_assign%s%s;return result_assign.toInteger()");

	if(_show_print) printf("%i. testing arithmetic operations float assign ...\n",++n_test);
	_complete_test_arithmetic_float_op_assign(_se,33,10,"result_assign%s%s;return result_assign.toFloat()");

	if(_show_print) printf("%i. testing arithmetic operations property ...\n",++n_test);
	_complete_test_arithmetic_integer_op_assign(_se,10,5,"result_assign.value%s%s;return result_assign.toInteger()");

	if(_show_print) printf("%i. testing arithmetic self operations ...\n",++n_test);
	_se->compileAndRun("result_assign=10;");
	_complete_test_arithmetic_self_op(_se,10,"%sresult_assign%s;return result_assign.toInteger()");

	if(_show_print) printf("%i. testing arithmetic self operations property ...\n",++n_test);
	_se->compileAndRun("var source=new Number(10);");
	_se->compileAndRun("result_assign=100;");
	_complete_test_arithmetic_self_property_op(_se,10,"result_assign=%ssource.value%s;return result_assign.toInteger()");


	// self operations

	// self operations property


}

void test_arithmetic_metamethods_no_print(zetscript::ScriptEngine *_se) {
	test_arithmetic_metamethods(_se,false);
}


#ifdef __MAIN__
int main(){
	zetscript::ScriptEngine se;
	try{
		test_arithmetic_metamethods(&zs);
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}

	return 0;
}
#endif
