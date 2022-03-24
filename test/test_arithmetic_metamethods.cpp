/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "test_arithmetic_common.h"
#include "NumberWrap.cpp"


#define COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CC(_zs,val1,val2)  _complete_test_arithmetic_integer_op(_zs,val1,val2,"it1=("\
				"(new Number("\
				"%s" \
				"))"\
				"%s" \
				"(new Number("\
				"%s" \
				"))"\
				");\n" \
				"it2=it1.toInt();\n" \
				"return it2;\n" \
	)

#define COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CI(_zs,val1,val2)  _complete_test_arithmetic_integer_op(_zs,val1,val2,"it1=("\
				"(new Number("\
				"%s" \
				"))"\
				"%s" \
				"(%s)"\
				");\n" \
				"it2=it1.toInt();\n" \
				"return it2;\n" \
	)

#define COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_IC(_zs,val1,val2)  _complete_test_arithmetic_integer_op(_zs,val1,val2,"it1=("\
				"("\
				"%s" \
				")"\
				"%s" \
				"(new Number("\
				"%s" \
				"))"\
				");\n" \
				"it2=it1.toInt();\n" \
				"return it2;\n" \
	)

// Float operations
#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CC(_zs,val1,val2)  _complete_test_arithmetic_float_op(_zs,val1,val2,"it1=("\
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

#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CF(_zs,val1,val2)  _complete_test_arithmetic_float_op(_zs,val1,val2,"it1=("\
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

#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_FC(_zs,val1,val2)  _complete_test_arithmetic_float_op(_zs,val1,val2,"it1=("\
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



int main(int argc, char * argv[]) {


	int n_test=0;
	zetscript::zs_int result_assign=10;

	zetscript::ZetScript zs;

	try{

		// register wraps

		NumberWrap_register(&zs);

		zs.eval("var i,i1,i2,it1,it2,n1,n2,nt1,nt2;");

		// testing metamethods...
		printf("%i. testing arithmetic operations int cc...\n",++n_test);
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CC(&zs,4,4); // op1==op2
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CC(&zs,4,5); // op1 < op2
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CC(&zs,5,4); // op1 > op2

		printf("%i. testing arithmetic operations int ci...\n",++n_test);
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CI(&zs,4,4); // op1==op2
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CI(&zs,4,5); // op1 < op2
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_CI(&zs,5,4); // op1 > op2

		printf("%i. testing arithmetic operations int ic...\n",++n_test);
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_IC(&zs,4,4); // op1==op2
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_IC(&zs,4,5); // op1 < op2
		COMPLETE_TEST_ARITHMETIC_INTEGER_OP_WITH_CLASS_NUMBER_IC(&zs,5,4); // op1 > op2


		printf("%i. testing arithmetic operations float cc ...\n",++n_test);
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CC(&zs,4.2,4.2); // op1==op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CC(&zs,4.2,5.2); // op1 < op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CC(&zs,5.2,4.2); // op1 > op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CC(&zs,5.2,2.0e2); // op1 > op2

		printf("%i. testing arithmetic operations float cf ...\n",++n_test);
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CF(&zs,4.2,4.2); // op1==op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CF(&zs,4.2,5.2); // op1 < op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CF(&zs,5.2,4.2); // op1 > op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_CF(&zs,5.2,2.0e2); // op1 > op2

		printf("%i. testing arithmetic operations float fc ...\n",++n_test);
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_FC(&zs,4.2,4.2); // op1==op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_FC(&zs,4.2,5.2); // op1 < op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_FC(&zs,5.2,4.2); // op1 > op2
		COMPLETE_TEST_ARITHMETIC_FLOAT_OP_WITH_CLASS_NUMBER_FC(&zs,5.2,2.0e2); // op1 > op2

		printf("%i. testing arithmetic operations assign ...\n",++n_test);
		zs.eval("var result_assign=new Number();");
		_complete_test_arithmetic_integer_op_assign(&zs,&result_assign,10,"result_assign%s%s;return result_assign.toInt()");


	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}
}
