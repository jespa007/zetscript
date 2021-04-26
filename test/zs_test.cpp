/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

using namespace zetscript;

#include "FloatWrap.cpp"
#include "IntegerWrap.cpp"



std::function<void ()> * test_2nd_script_call = NULL;


void test_function_1st_c_call(){
	 printf("C Function 1st call from script\n");

	 if(test_2nd_script_call != NULL){
		 (*test_2nd_script_call)();
	 }
}


// Usable AlmostEqual function
bool floatValuesAreAlmostTheSame(zs_float A, zs_float B, int maxUlps=8)
{
	 return (fabs(A - B) <= FLT_EPSILON *2* std::max(fabs(A), fabs(B)));
}



#define TEST_ARITHMETIC_INTEGER_OP(val1, op, val2) \
{ \
	zs_int *aux_value=NULL; \
	std::string str= ZS_STR(val1) \
				ZS_STR(op) \
				" "\
				ZS_STR(val2) \
				";"; \
	try{\
		aux_value=zs->evalIntValue(str); \
		if(aux_value!=NULL){ \
			if(*aux_value != (val1 op val2)){ \
				fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",str.c_str(),val1 op val2,(int)*aux_value); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",str.c_str(),ex.what()); \
		exit(-1); \
	}\
}

#define TEST_ARITHMETIC_CLASS_INTEGER_OP(val1, op, val2) \
{ \
	zs_int *aux_value=NULL; \
	std::string str= "it1=("\
				"(i1=new Integer("\
				ZS_STR(val1) \
				"))"\
				ZS_STR(op) \
				"(i2=new Integer("\
				ZS_STR(val2) \
				"))"\
				");it2=it1.toInt();delete it1;delete i1;delete i2;it2;"; \
	try{\
		aux_value=zs->evalIntValue(str); \
		if(aux_value!=NULL){ \
			if( *aux_value != (val1 op val2)){ \
				fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",str.c_str(),val1 op val2,(int)*aux_value); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
			fprintf(stderr,"error test \"%s\" : %s!\n",str.c_str(),ex.what()); \
			exit(-1); \
	}\
}

#define TEST_ARITHMETIC_FLOAT_OP(val1, op, val2) \
{ \
	zs_float expr=((zs_float)(val1) op (zs_float)(val2));\
	zs_float * aux_value=NULL; \
	std::string str= ZS_STR(val1) \
				ZS_STR(op) \
				" "\
				ZS_STR(val2) \
				";"; \
	try{\
		aux_value=zs->evalFloatValue(str); \
		if(aux_value!=NULL){ \
			if(!floatValuesAreAlmostTheSame(*aux_value,expr)){ \
				fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",str.c_str(),expr,*aux_value); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",str.c_str(),ex.what()); \
		exit(-1); \
	}\
}


#define TEST_ARITHMETIC_FLOAT_MOD(val1, val2) \
{ \
	zs_float *aux_value=NULL; \
	std::string str= ZS_STR(val1) \
				"%" \
				ZS_STR(val2) \
				";"; \
	try{\
		aux_value=zs->evalFloatValue(str); \
		if(aux_value!=NULL){ \
			if(!floatValuesAreAlmostTheSame( *aux_value , fmod(val1,val2))){ \
				fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",str.c_str(),fmod(val1,val2),*aux_value); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",str.c_str(),ex.what()); \
		exit(-1); \
	}\
}


#define TEST_ARITHMETIC_CLASS_FLOAT_OP(val1, op, val2) \
{ \
	zs_float expr=((zs_float)(val1) op (zs_float)(val2));\
	zs_float *aux_value=NULL; \
	std::string str= "nt1=("\
				"(n1=new Float("\
				ZS_STR(val1) \
				"))"\
				ZS_STR(op) \
				"(n2=new Float("\
				ZS_STR(val2) \
				"))"\
				");nt2=nt1.toFloat();delete n1;delete n2;delete nt1;nt2;"; \
	try{\
		aux_value=zs->evalFloatValue(str); \
		if(aux_value!=NULL){ \
			if(!floatValuesAreAlmostTheSame(*aux_value,expr)){ \
				fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",str.c_str(),expr,*aux_value); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",str.c_str(),ex.what()); \
		exit(-1); \
	}\
}

#define TEST_ARITHMETIC_CLASS_FLOAT_MOD(val1, val2) \
{ \
	zs_float *aux_value=NULL; \
	std::string str= "nt1=("\
				"(n1=new Float("\
				ZS_STR(val1) \
				"))"\
				"%" \
				"(n2=new Float("\
				ZS_STR(val2) \
				"))"\
				");nt2=nt1.toFloat();delete n1; delete n2;delete nt1;nt2;"; \
	try{\
		aux_value=zs->evalFloatValue(str);\
		if(aux_value!=NULL){ \
			if(!floatValuesAreAlmostTheSame(*aux_value  , fmod(val1,val2))){ \
				fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",str.c_str(),fmod(val1,val2),*aux_value); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",str.c_str(),ex.what()); \
		exit(-1); \
	}\
}


#define TEST_ARITHMETIC_STRING_OP(val1,op, val2) \
{ \
	std::string * aux_value=NULL; \
	std::string str= ZS_STR(val1) \
				ZS_STR(op) \
				" "\
				val2\
				";"; \
	std::string expected_str = std::string(val1) op val2;\
	try{\
		aux_value=zs->evalStringValue(str);\
		if(aux_value!=NULL){ \
			if(*aux_value!=expected_str){ \
				fprintf(stderr,"error test \"%s\" expected %s but it was %s!\n",str.c_str(),expected_str.c_str(),aux_value->c_str()); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",str.c_str(),ex.what()); \
		exit(-1); \
	}\
}



#define COMPLETE_TEST_ARITHMETIC_INTEGER_OP(val1,val2) \
		TEST_ARITHMETIC_INTEGER_OP(val1,+,val2); \
		TEST_ARITHMETIC_INTEGER_OP(val1,+,-val2); \
		TEST_ARITHMETIC_INTEGER_OP(-val1,+,val2); \
		TEST_ARITHMETIC_INTEGER_OP(-val1,+,-val2); \
		\
		TEST_ARITHMETIC_INTEGER_OP(val1,-,val2); \
		TEST_ARITHMETIC_INTEGER_OP(val1,-,-val2); \
		TEST_ARITHMETIC_INTEGER_OP(-val1,-,val2); \
		TEST_ARITHMETIC_INTEGER_OP(-val1,-,-val2); \
		\
		TEST_ARITHMETIC_INTEGER_OP(val1,*,val2); \
		TEST_ARITHMETIC_INTEGER_OP(val1,*,-val2); \
		TEST_ARITHMETIC_INTEGER_OP(-val1,*,val2); \
		TEST_ARITHMETIC_INTEGER_OP(-val1,*,-val2); \
		\
		TEST_ARITHMETIC_INTEGER_OP(val1,/,val2); \
		TEST_ARITHMETIC_INTEGER_OP(val1,/,-val2); \
		TEST_ARITHMETIC_INTEGER_OP(-val1,/,val2); \
		TEST_ARITHMETIC_INTEGER_OP(-val1,/,-val2); \
		\
		TEST_ARITHMETIC_INTEGER_OP(val1,%,val2); \
		TEST_ARITHMETIC_INTEGER_OP(val1,%,-val2); \
		TEST_ARITHMETIC_INTEGER_OP(-val1,%,val2); \
		TEST_ARITHMETIC_INTEGER_OP(-val1,%,-val2);

#define COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,val2) \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,+,val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,+,-val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(-val1,+,val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(-val1,+,-val2); \
		\
		TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,-,val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,-,-val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(-val1,-,val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(-val1,-,-val2); \
		\
		TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,*,val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,*,-val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(-val1,*,val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(-val1,*,-val2); \
		\
		TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,/,val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,/,-val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(-val1,/,val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(-val1,/,-val2); \
		\
		TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,%,val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,%,-val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(-val1,%,val2); \
		TEST_ARITHMETIC_CLASS_INTEGER_OP(-val1,%,-val2);


#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP(val1,val2) \
		TEST_ARITHMETIC_FLOAT_OP(val1,+,val2); \
		TEST_ARITHMETIC_FLOAT_OP(val1,+,-val2); \
		TEST_ARITHMETIC_FLOAT_OP(-val1,+,val2); \
		TEST_ARITHMETIC_FLOAT_OP(-val1,+,-val2); \
		\
		TEST_ARITHMETIC_FLOAT_OP(val1,-,val2); \
		TEST_ARITHMETIC_FLOAT_OP(val1,-,-val2); \
		TEST_ARITHMETIC_FLOAT_OP(-val1,-,val2); \
		TEST_ARITHMETIC_FLOAT_OP(-val1,-,-val2); \
		\
		TEST_ARITHMETIC_FLOAT_OP(val1,*,val2); \
		TEST_ARITHMETIC_FLOAT_OP(val1,*,-val2); \
		TEST_ARITHMETIC_FLOAT_OP(-val1,*,val2); \
		TEST_ARITHMETIC_FLOAT_OP(-val1,*,-val2); \
		\
		TEST_ARITHMETIC_FLOAT_OP(val1,/,val2); \
		TEST_ARITHMETIC_FLOAT_OP(val1,/,-val2); \
		TEST_ARITHMETIC_FLOAT_OP(-val1,/,val2); \
		TEST_ARITHMETIC_FLOAT_OP(-val1,/,-val2); \
		\
		TEST_ARITHMETIC_FLOAT_MOD(val1,val2); \
		TEST_ARITHMETIC_FLOAT_MOD(val1,-val2); \
		TEST_ARITHMETIC_FLOAT_MOD(-val1,val2); \
		TEST_ARITHMETIC_FLOAT_MOD(-val1,-val2);

#define COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(val1,val2) \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(val1,+,val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(val1,+,-val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(-val1,+,val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(-val1,+,-val2); \
		\
		TEST_ARITHMETIC_CLASS_FLOAT_OP(val1,-,val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(val1,-,-val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(-val1,-,val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(-val1,-,-val2); \
		\
		TEST_ARITHMETIC_CLASS_FLOAT_OP(val1,*,val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(val1,*,-val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(-val1,*,val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(-val1,*,-val2); \
		\
		TEST_ARITHMETIC_CLASS_FLOAT_OP(val1,/,val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(val1,/,-val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(-val1,/,val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_OP(-val1,/,-val2); \
		\
		TEST_ARITHMETIC_CLASS_FLOAT_MOD(val1,val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_MOD(val1,-val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_MOD(-val1,val2); \
		TEST_ARITHMETIC_CLASS_FLOAT_MOD(-val1,-val2);

#define COMPLETE_TEST_ARITHMETIC_BINARY_OP(val1, val2) \
	TEST_ARITHMETIC_INTEGER_OP(val1,&,val2); \
	TEST_ARITHMETIC_INTEGER_OP(val1,|,val2); \
	TEST_ARITHMETIC_INTEGER_OP(val1,>>,1); \
	TEST_ARITHMETIC_INTEGER_OP(val1,<<,1); \
	TEST_ARITHMETIC_INTEGER_OP(val1,^,val2);


#define COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_BINARY_OP(val1, val2) \
	TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,&,val2); \
	TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,|,val2); \
	TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,>>,1); \
	TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,<<,1); \
	TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,^,val2);


#define TEST_ARITHMETIC_OP(val1, op, val2) \
{ \
	zs_int aux_value=0; \
	std::string str= ZS_STR(val1) \
				ZS_STR(op) \
				" "\
				ZS_STR(val2) \
				";"; \
	try{\
		if((aux_value=ZetScript::eval<f>(str)) != (val1 op val2)){ \
			fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",str.c_str(),val1 op val2,aux_value); \
			exit(-1); \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",str.c_str(),ex.what()); \
		exit(-1); \
	}\
}

#define TEST_ARITHMETIC_INT_EXPR(expr) \
{ \
	zs_int * aux_value=NULL; \
	std::string str_expr= ZS_STR(expr)";"; \
	\
	try{\
		aux_value=zs->evalIntValue(str_expr);\
		if(aux_value != NULL){\
			if(*aux_value != (expr)){ \
				fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",str_expr.c_str(),expr,(int)*aux_value); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",str_expr.c_str(),ex.what()); \
		exit(-1); \
	}\
}

#define TEST_INT_EXPR(expr, expected_value) \
{ \
	zs_int *aux_value=NULL; \
	\
	try{\
		aux_value=zs->evalIntValue(expr);\
		if(aux_value != NULL){\
			if(*aux_value  != (expected_value)){ \
				fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",expr,expected_value,(int)*aux_value); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",expr,ex.what()); \
		exit(-1); \
	}\
}


#define TEST_FLOAT_EXPR(expr, expected_value) \
{ \
	zs_float * aux_value=NULL; \
	\
	aux_value=zs->evalFloatValue(expr); \
	try{\
		if(aux_value != NULL){ \
			if(*aux_value  != (expected_value)){ \
				fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",expr,expected_value,*aux_value); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",expr,ex.what()); \
		exit(-1); \
	}\
}

#define TEST_BOOL_EXPR(expr, expected_value) \
{ \
	bool *aux_value=NULL; \
	\
	try{\
		aux_value=zs->evalBoolValue(expr);\
		if(aux_value != NULL){ \
			if(*aux_value  != (expected_value)){ \
				fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",expr,expected_value,*aux_value); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",expr,ex.what()); \
		exit(-1); \
	}\
}

#define TEST_STRING_EXPR(expr, expected_value) \
{ \
	std::string aux_value=""; \
	\
	try{\
		if((aux_value=ZetScript::evalStringValue(expr))  != (expected_value)){ \
			fprintf(stderr,"error test \"%s\" expected \"%s\" but it was \"%s\"!\n",expr,expected_value,aux_value.c_str()); \
			exit(-1); \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",expr,ex.what()); \
		exit(-1); \
	}\
}

#define TEST_NUMBER_EXPR(expr, expected_value) \
{ \
	zs_float *aux_value=NULL; \
	\
	try{\
		aux_value=zs->evalFloatValue(expr);\
		if(aux_value != NULL){ \
			if(*aux_value  != (expected_value)){ \
				fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",expr,expected_value,*aux_value); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",expr,ex.what()); \
		exit(-1); \
	}\
}

#define TEST_ARITHMETIC_FLOAT_EXPR(expr) \
{ \
	zs_float *aux_value=NULL; \
	std::string str_expr= ZS_STR(expr)";"; \
	\
	try{\
		aux_value=zs->evalFloatValue(str_expr); \
		if(aux_value != NULL){ \
			if(!floatValuesAreAlmostTheSame(*aux_value  , (zs_float)(expr))){ \
				double error = fabs(fabs(*aux_value)-fabs(expr));\
				if(error>0.001){ /* Only error if the difference is more than expected */\
					fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",str_expr.c_str(),expr,*aux_value); \
					exit(-1); \
				}else{\
					fprintf(stderr,"warning: test \"%s\" expected %f but it was %f (it has some precision error)!\n",str_expr.c_str(),expr,*aux_value); \
				}\
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",str_expr.c_str(),ex.what()); \
		exit(-1); \
	}\
}

#define TEST_ARITHMETIC_BOOL_EXPR(val1) \
{ \
	bool *aux_value=NULL; \
	std::string str= ZS_STR(val1) \
				";"; \
	try{\
		aux_value=zs->evalBoolValue(str);\
		if(aux_value != NULL) { \
			if(*aux_value != (val1)){ \
				fprintf(stderr,"error test \"%s\" expected %s but it was %s!\n",str.c_str(),(val1)?"true":"false",*aux_value?"true":"false"); \
				exit(-1); \
			} \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",str.c_str(),ex.what()); \
		exit(-1); \
	}\
}


#define COMPLETE_TEST_COMPARE_OP(val1,val2) \
		TEST_ARITHMETIC_BOOL_EXPR(val1<val2); \
		TEST_ARITHMETIC_BOOL_EXPR(val1*10<-val2/10); \
		TEST_ARITHMETIC_BOOL_EXPR(-(val1+10)<(val2-8)); \
		TEST_ARITHMETIC_BOOL_EXPR((-val1-100)<(-val2+100)); \
		\
		TEST_ARITHMETIC_BOOL_EXPR((val1+10)>(val2-80)); \
		TEST_ARITHMETIC_BOOL_EXPR(val1*70>(-val2+300)); \
		TEST_ARITHMETIC_BOOL_EXPR(-val1*60>val2*90); \
		TEST_ARITHMETIC_BOOL_EXPR(-val1*90>-val2*60); \
		\
		TEST_ARITHMETIC_BOOL_EXPR((val1+10)<=10*val2); \
		TEST_ARITHMETIC_BOOL_EXPR(val1*10<=-80/val2); \
		TEST_ARITHMETIC_BOOL_EXPR(-val1/10<=val2*70); \
		TEST_ARITHMETIC_BOOL_EXPR((-val1-10)<=-val2*10); \
		\
		TEST_ARITHMETIC_BOOL_EXPR(val1*70>=val2); \
		TEST_ARITHMETIC_BOOL_EXPR(-val1/10>=-(val2+90)); \
		TEST_ARITHMETIC_BOOL_EXPR((-val1+10)>=val2*10/10); \
		TEST_ARITHMETIC_BOOL_EXPR((-val1-val1)>=-(val2-val2-10)); \

#define COMPLETE_TEST_LOGIC_OP(val1,val2) \
		TEST_ARITHMETIC_BOOL_EXPR(!(val1>0)&&(val2>0)); \
		TEST_ARITHMETIC_BOOL_EXPR((val1<0)&&(val2<0)); \
		TEST_ARITHMETIC_BOOL_EXPR((val1>=0)&&!(val2>=0)); \
		TEST_ARITHMETIC_BOOL_EXPR((val1<=0)&&(val2<=0)); \
		TEST_ARITHMETIC_BOOL_EXPR((val1<=0)&&!(false)); \
		TEST_ARITHMETIC_BOOL_EXPR((val1<=0)&&(true)); \
		TEST_ARITHMETIC_BOOL_EXPR((val1>0)||!(val2>0)); \
		TEST_ARITHMETIC_BOOL_EXPR((val1<0)||(val2<0)); \
		TEST_ARITHMETIC_BOOL_EXPR((val1>=0)||(val2>=0)); \
		TEST_ARITHMETIC_BOOL_EXPR(!(val1<=0)||(val2<=0)); \
		TEST_ARITHMETIC_BOOL_EXPR((val1<=0)||(false)); \
		TEST_ARITHMETIC_BOOL_EXPR((val1<=0)||(true));

int main(int argc, char * argv[]) {

	int n_test=0;

	ZetScript *zs=new ZetScript();

/*	"test_arithmetic_operations.zs",
	"test_binary_operations.zs",
	"test_class.zs",
	"test_for.zs",
	"test_function.zs",
	"test_if_else.zs",
	"test_logic_operations.zs",
	"test_metamethod.zs",
	"test_recursion.zs",
	"test_struct.zs",
	"test_switch.zs",
	"test_ternari.zs",
	"test_vector.zs",
	"test_while.zs"*/

	//------------------------------------------
	//
	// TEST INT OPS
	//

	//ZetScript::getInstance()->eval("if(null){print(\"true\");}else{print(\"\");} }var prova=[]; prova.add(0); prova.size();");

	//TEST_NUMBER_EXPR("4.0*4;",16.0);
	//exit(-1);
	//int i= 0+ +1;
	zs->registerClass<Float>("Float",FloatWrap::_new,FloatWrap::_delete);

	// constructor
	zs->registerConstructor<Float>(static_cast<void (*)(Float *,zs_float *)>(&FloatWrap::_set));
	zs->registerConstructor<Float>(static_cast<void (*)(Float *,Float *)>(&FloatWrap::_set));

	zs->registerMemberFunction<Float>("toFloat",&FloatWrap::toFloat);
	//zs->registerMemberVariable<Float>("n",&Float::n);


	zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap::_add));
	zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap::_add));
	zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap::_add));

	zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap::_sub));
	zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap::_sub));
	zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap::_sub));


	zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap::_mul));
	zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap::_mul));
	zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap::_mul));

	zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap::_div));
	zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap::_div));
	zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap::_div));

	zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap::_mod));
	zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap::_mod));
	zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap::_mod));

	zs->registerMemberFunctionStatic<Float>("_neg",static_cast<Float * (*)(Float *)>(&FloatWrap::_neg));

	zs->registerMemberFunction<Float>("_set",static_cast<void (*)(Float *,zs_float *)>(&FloatWrap::_set));
	zs->registerMemberFunction<Float>("_set",static_cast<void (*)(Float *,Float *)>(&FloatWrap::_set));


	zs->registerClass<Integer>("Integer",IntegerWrap::_new,IntegerWrap::_delete);
	zs->registerConstructor<Integer>(static_cast<void (*)(Integer *,zs_int )>(&IntegerWrap::_set));
	zs->registerConstructor<Integer>(static_cast<void (*)(Integer *,Integer *)>(&IntegerWrap::_set));
	zs->registerMemberFunction<Integer>("toInt",&IntegerWrap::toInt);
//	zs->registerMemberVariable<Integer>("n",&Integer::n);


	zs->registerMemberFunctionStatic<Integer>("_add",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap::_add));
	zs->registerMemberFunctionStatic<Integer>("_add",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap::_add));
	zs->registerMemberFunctionStatic<Integer>("_add",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap::_add));

	zs->registerMemberFunctionStatic<Integer>("_sub",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap::_sub));
	zs->registerMemberFunctionStatic<Integer>("_sub",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap::_sub));
	zs->registerMemberFunctionStatic<Integer>("_sub",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap::_sub));


	zs->registerMemberFunctionStatic<Integer>("_mul",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap::_mul));
	zs->registerMemberFunctionStatic<Integer>("_mul",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap::_mul));
	zs->registerMemberFunctionStatic<Integer>("_mul",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap::_mul));

	zs->registerMemberFunctionStatic<Integer>("_div",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap::_div));
	zs->registerMemberFunctionStatic<Integer>("_div",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap::_div));
	zs->registerMemberFunctionStatic<Integer>("_div",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap::_div));

	zs->registerMemberFunctionStatic<Integer>("_mod",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap::_mod));
	zs->registerMemberFunctionStatic<Integer>("_mod",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap::_mod));
	zs->registerMemberFunctionStatic<Integer>("_mod",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap::_mod));


	zs->registerMemberFunctionStatic<Integer>("_shr",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap::_shr));
	zs->registerMemberFunctionStatic<Integer>("_shr",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap::_shr));
	zs->registerMemberFunctionStatic<Integer>("_shr",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap::_shr));


	zs->registerMemberFunctionStatic<Integer>("_shl",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap::_shl));
	zs->registerMemberFunctionStatic<Integer>("_shl",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap::_shl));
	zs->registerMemberFunctionStatic<Integer>("_shl",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap::_shl));


	zs->registerMemberFunctionStatic<Integer>("_and",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap::_and));
	zs->registerMemberFunctionStatic<Integer>("_and",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap::_and));
	zs->registerMemberFunctionStatic<Integer>("_and",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap::_and));

	zs->registerMemberFunctionStatic<Integer>("_or",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap::_or));
	zs->registerMemberFunctionStatic<Integer>("_or",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap::_or));
	zs->registerMemberFunctionStatic<Integer>("_or",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap::_or));

	zs->registerMemberFunctionStatic<Integer>("_xor",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap::_xor));
	zs->registerMemberFunctionStatic<Integer>("_xor",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap::_xor));
	zs->registerMemberFunctionStatic<Integer>("_xor",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap::_xor));


	zs->registerMemberFunctionStatic<Integer>("_neg",static_cast<Integer * (*)(Integer *)>(&IntegerWrap::_neg));

	zs->registerMemberFunction<Integer>("_set",static_cast<void (*)(Integer *, zs_int)>(&IntegerWrap::_set));
	zs->registerMemberFunction<Integer>("_set",static_cast<void (*)(Integer *,Integer *)>(&IntegerWrap::_set));


	// unsinged
	printf("%i. testing arithmetic operations integer...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(4,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(4,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(5,4); // op1 > op2


	printf("%i. testing arithmetic operations hexa (int)...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(0x4,0x4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(0x4,0x5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_INTEGER_OP(0x5,0x4); // op1 > op2

	printf("%i. testing arithmetic operations float ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4.0,4.0); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4.0,5.0); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(5.0,4.0); // op1 > op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(5.0,2.0e2); // op1 > op2

	printf("%i. testing arithmetic operations float vs int ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4.0,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4.0,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(5.0,4); // op1 > op2

	printf("%i. testing arithmetic operations int vs float ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4,4.0); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4,5.0); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(5,4.0); // op1 > op2

	printf("%i. testing binary operations ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_BINARY_OP(0x4,0x4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_BINARY_OP(0x4,0x5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_BINARY_OP(0x5,0x4); // op1 > op2


	printf("%i. testing std::string...\n",++n_test);
	TEST_ARITHMETIC_STRING_OP("test_",+,"100"); // concatenate int
	TEST_ARITHMETIC_STRING_OP("test_",+,"-100"); // concatenate -int
	TEST_ARITHMETIC_STRING_OP("test_",+,"100.000000"); // concatenate float
	TEST_ARITHMETIC_STRING_OP("test_",+,"-100.000000"); // concatenate float
	TEST_ARITHMETIC_STRING_OP("test_",+,"true"); // concatenate bool
	TEST_ARITHMETIC_STRING_OP("test_",+,"false"); // concatenate bool

	printf("%i. testing parenthesis operations ...\n",++n_test);

	TEST_ARITHMETIC_INT_EXPR((((2+2*(5-9))*1000))/100);
	TEST_ARITHMETIC_FLOAT_EXPR((((2.0+2.0*(5.0-6.1))*1000.0))/100.0);

	// test bool compare ...
	printf("%i. testing compare operations ...\n",++n_test);
	COMPLETE_TEST_COMPARE_OP(10,10);
	COMPLETE_TEST_COMPARE_OP(15,10);
	COMPLETE_TEST_COMPARE_OP(10,15);

	// test logic and/or ...
	printf("%i. testing logic operations ...\n",++n_test);
	COMPLETE_TEST_LOGIC_OP(10,10);
	COMPLETE_TEST_LOGIC_OP(15,10);
	COMPLETE_TEST_LOGIC_OP(10,15);

	// some basics tests
	TEST_ARITHMETIC_BOOL_EXPR(!false && !(false || false));
	TEST_ARITHMETIC_BOOL_EXPR(!(true && !false) || false);
	TEST_ARITHMETIC_BOOL_EXPR((true && !false) || !false);

	// test declare var int/bool/std::string/number
	printf("%i. testing primitive var\n",++n_test);


	TEST_INT_EXPR("var i=1;",1);
	TEST_INT_EXPR("i++;i;",2);
	TEST_INT_EXPR("++i;i;",3);
	TEST_INT_EXPR("i--;i;",2);
	TEST_INT_EXPR("--i;i;",1);


	TEST_INT_EXPR("i=10;i*=10;",100);
	TEST_INT_EXPR("i/=10;",10);
	TEST_INT_EXPR("i+=10;",20);
	TEST_INT_EXPR("i-=5;",15);
	TEST_INT_EXPR("i%=10;",5);

	// test reassign and float
	TEST_NUMBER_EXPR("i=2.0;",2.0f);
	TEST_NUMBER_EXPR("i++;i;",3.0f);
	TEST_NUMBER_EXPR("--i;i;",2.0f);

	TEST_BOOL_EXPR("i=true;",true);
	TEST_BOOL_EXPR("i=!i;",false);
	TEST_BOOL_EXPR("i==i;",true);
	TEST_BOOL_EXPR("i!=i;",false);
	//TEST_BOOL_EXPR("i=!i;",true);


	/*printf("%i. testing std::vector var ...\n",++n_test);

	TEST_INT_EXPR("var v=[3,true,2.0,\"is_a_string\"];v.size();",4); // <-- crash if no constructor defined new Integer(x)!
	TEST_INT_EXPR("v[0];",3);
	TEST_BOOL_EXPR("v[1];",true);
	TEST_NUMBER_EXPR("v[2];",2.0);
	TEST_STRING_EXPR("v[3];","is_a_string");
	//TEST_INT_EXPR("v[4].n;",5);// <-- error !!!
	//TEST_NUMBER_EXPR("v[5].n;",10.0f);

	// test adding ...

	printf("%i. testing struct var ...\n",++n_test);

	TEST_INT_EXPR("var s={i:3,b:true,n:2.0,s:\"is_a_string\",o:new Integer(5)};s.size();",5);
	TEST_BOOL_EXPR("s.b;",true);
	TEST_NUMBER_EXPR("s.n;",2.0);
	TEST_STRING_EXPR("s.s;","is_a_string");
//	TEST_BOOL_EXPR("s.o.instanceof(MyObject);",true);

	printf("%i. testing metamethod integer ...\n",++n_test);
	TEST_INT_EXPR("var mt=new Integer(5);mt=0+1+2+mt+4+5;mt.n;",17);*/

	printf("%i. test if-else ...\n",++n_test);
	TEST_INT_EXPR("i=0;if(i==0){i=10;}else{i=11;}i;",10);
	TEST_INT_EXPR("if(i==0){i=10;}else{i=11;}i;",11);

	zs->eval("var i1,i2,it1,it2,n1,n2,nt1,nt2;");

	printf("%i. testing class Integer arithmetic operations...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(4,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(4,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(5,4); // op1 > op2

	printf("%i. testing class Float arithmetic operations ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(4.0,4.0); // op1==op2
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(4.0,5.0); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(5.0,4.0); // op1 > op2
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(5.0,2.0e2); // op1 > op2

	printf("%i. testing class Integer binary operations ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_BINARY_OP(0x4,0x4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_BINARY_OP(0x4,0x5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_BINARY_OP(0x5,0x4); // op1 > op2


	printf("%i. test consisten script-c-script calls ...\n",++n_test);
	// test calling script-c-script-c
	zs->registerFunction("test_function_1st_c_call",test_function_1st_c_call);
	zs->eval("function test_1st_script_call(){\n"
				"Console::println (\"Hello from script\");\n"
				"test_function_1st_c_call();\n"
			"}\n"
			"function test_2nd_script_call(){\n"
				"System::print(\"2nd call script\");\n"
			"}");

	std::function<void ()> * test_1st_script_call=zs->bindScriptFunction<void ()>("test_1st_script_call");
	test_2nd_script_call=zs->bindScriptFunction<void ()>("test_2nd_script_call");

	if(test_1st_script_call){
		(*test_1st_script_call)();
	}

	delete test_1st_script_call;
	delete test_2nd_script_call;


	printf("All tests passed OK!\n");

	delete zs;

#ifdef __MEMMANAGER__
	MEMMGR_print_status();
#endif

	return 0;
}
