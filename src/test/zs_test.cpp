/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

using namespace zetscript;

#include "FloatWrap.cpp"
#include "IntegerWrap.cpp"


std::function<void ()> * test_2nd_script_call = NULL;
ZetScript *zs = NULL;

std::string to_string(bool _b){
	return _b?"true":"false";
}

std::string to_string(zs_int _i){
	return zs_strutils::zs_int_to_str(_i);
}

std::string to_string(zs_float _f){
	return zs_strutils::zs_float_to_str(_f);
}

std::string to_string(const std::string & _s){
	return _s;
}




void test_function_1st_c_call(){
	 printf("C Function 1st call from script\n");

	 if(test_2nd_script_call != NULL){
		 (*test_2nd_script_call)();
	 }
}

// Usable AlmostEqual function
bool float_values_are_almost_the_same(zs_float A, zs_float B, int maxUlps=8)
{
	return (fabs(A - B) <= FLT_EPSILON *2* std::max(fabs(A), fabs(B)));
}


#define INLINE_OPERATION(val1,op,val2) {zs_strutils::format(str_format,to_string(val1).c_str(),ZS_STR(op),to_string(val2).c_str()), (val1) op (val2)}

//------------------------------------------------------------------------------------------------------------------------------------
//
// INTEGER OPERATIONS
//
#define TEST_ARITHMETIC_INT_EXPR(expr) test_arithmetic_int_expr(expr,ZS_STR(expr))
void test_arithmetic_int_expr(zs_int expr,const char *str_expr)
{ \
	try{\
		StackElement stk=zs->eval(std::string("return ")+str_expr);\
		if(stk.properties & STK_PROPERTY_ZS_INT){\
			if((zs_int)stk.value != expr){ \
				fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",str_expr,expr,(zs_int)stk.value); \
				exit(-1); \
			} \
		}else{\
			fprintf(stderr,"error test \"%s\" expected int but it was '%s'!\n",str_expr,stk.typeOf()); \
			exit(-1); \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n",str_expr,ex.what()); \
		exit(-1); \
	}\
}

#define TEST_INT_EXPR(str_expr, expected_value)
void test_int_expr(const char *str_expr, zs_int expected_value) {
	try{\
		StackElement stk=zs->eval(str_expr);\
		if(stk.properties & STK_PROPERTY_ZS_INT){\
			if((zs_int)stk.value  != (expected_value)){ \
				fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",str_expr,expected_value,(zs_int)stk.value); \
				exit(-1); \
			} \
		}else{\
			fprintf(stderr,"error test \"%s\" expected int but it was '%s'!\n",str_expr,stk.typeOf()); \
			exit(-1); \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n", str_expr,ex.what()); \
		exit(-1); \
	}\
}

#define COMPLETE_TEST_ARITHMETIC_INTEGER_OP(val1,val2) test_arithmetic_integer_op(val1, val2, "return %s%s%s")
#define COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(val1,val2)  test_arithmetic_integer_op(val1,val2,"it1=("\
				"(i1=new Integer("\
				"%s" \
				"))"\
				"%s" \
				"(i2=new Integer("\
				"%s" \
				"))"\
				");\n" \
				"it2=it1.toInt();\n" \
				/*"delete it1;\n" \
				 "delete i1;\n"*/ \
				"delete i2;\n" \
				"return it2;\n" \
	)
void test_arithmetic_integer_op(zs_int val1, zs_int val2, const char *str_format){
	struct _test_arithmetic_integer_op_data {
		std::string str; zs_int val;\
	}test_arithmetic_integer_op_data[] = {
		INLINE_OPERATION(val1,+,val2)
		,INLINE_OPERATION(val1,+,-val2)
		,INLINE_OPERATION(-val1,+,val2)
		,INLINE_OPERATION(-val1,+,-val2)
		,INLINE_OPERATION(val1,-,val2)
		,INLINE_OPERATION(-val1,-,val2)

		,INLINE_OPERATION(val1,*,val2)
		,INLINE_OPERATION(val1,*,-val2)
		,INLINE_OPERATION(-val1,*,val2)
		,INLINE_OPERATION(-val1,*,-val2)
		
		,INLINE_OPERATION(val1, / ,val2)
		,INLINE_OPERATION(val1, / ,-val2)
		,INLINE_OPERATION(-val1, / ,val2)
		,INLINE_OPERATION(-val1, / ,-val2)
		
		,INLINE_OPERATION(val1,%,val2)
		,INLINE_OPERATION(val1,%,-val2)
		,INLINE_OPERATION(-val1,%,val2)
		,INLINE_OPERATION(-val1,%,-val2)
		,INLINE_OPERATION(val1, &, val2)
		,INLINE_OPERATION(val1, | , val2)
		,INLINE_OPERATION(val1, >> , (zs_int)1)
		,INLINE_OPERATION(val1, << , (zs_int)1)
		,INLINE_OPERATION(val1, ^, val2)
		, { "",0 }
	};

	// process
	_test_arithmetic_integer_op_data *it_iod = test_arithmetic_integer_op_data;
	while (!it_iod->str.empty()) {
		try {
			StackElement stk=zs->eval(it_iod->str); \
			if (stk.properties & STK_PROPERTY_ZS_INT) {
				if ((zs_int)stk.value != (it_iod->val)) {
					fprintf(stderr, "error test \"%s\" expected %i but it was %i!\n", it_iod->str.c_str(), it_iod->val,(zs_int)stk.value);
					exit(-1);
				}
			}else{
				fprintf(stderr,"error test \"%s\" expected int but it was '%s'!\n",it_iod->str.c_str(),stk.typeOf()); \
				exit(-1); \
			}
		}
		catch (std::exception & ex) {
			fprintf(stderr, "error test \"%s\" : %s!\n", it_iod->str.c_str(), ex.what());
			exit(-1);
		}
		it_iod++;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------
//
// FLOAT OPERATIONS
//
#define TEST_FLOAT_EXPR(str_expr, expected_value)
void test_float_expr(const char  *str_expr, zs_float expected_value) {
	try{
		StackElement stk=zs->eval(str_expr);
		if(stk.properties & STK_PROPERTY_ZS_FLOAT){
			zs_float result;
			ZS_FLOAT_COPY(&result,&stk.value);
			if(result  != (expected_value)){
				fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",str_expr,expected_value,result);
				exit(-1);
			}
		}else{
			fprintf(stderr,"error test \"%s\" expected float but it was '%s'!\n",str_expr,stk.typeOf());
			exit(-1);
		}
	}catch(std::exception & ex){
		fprintf(stderr,"error test \"%s\" : %s!\n",str_expr,ex.what());
		exit(-1);
	}
}

#define TEST_ARITHMETIC_FLOAT_EXPR(val1) test_float_expression((zs_float)(val1),ZS_STR(val1)";")
void test_float_expression(zs_float expr, const char *str_expr) {

	try {
		StackElement stk=zs->eval(std::string("return ")+str_expr);
		if(stk.properties & STK_PROPERTY_ZS_FLOAT){
			zs_float result;
			ZS_FLOAT_COPY(&result,&stk.value);
			if (!float_values_are_almost_the_same(result, expr)) {
				double error = fabs(fabs(result) - fabs(expr));
					if (error > 0.001) { /* Only error if the difference is more than expected */
						fprintf(stderr, "error test \"%s\" expected %f but it was %f!\n", str_expr, expr, result);
							exit(-1);
						}
				else {
					fprintf(stderr, "warning: test \"%s\" expected %f but it was %f (it has some precision error)!\n", str_expr, expr, result);
				
				}
			}
		}else{
			fprintf(stderr,"error test \"%s\" expected float but it was '%s'!\n",str_expr,stk.typeOf());
			exit(-1);
		}
	}
	catch (std::exception & ex) {
		fprintf(stderr, "error test \"%s\" : %s!\n", str_expr, ex.what());
		exit(-1);
	}
}

#define INLINE_FLOAT_MOD_OPERATION(val1,val2) {zs_strutils::format(str_format,to_string(val1).c_str(),"%",to_string(val2).c_str()), (zs_float)fmod(val1,val2)}
#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP(val1,val2) test_arithmetic_float_op(val1, val2,"return %s%s%s")
#define COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(val1,val2) test_arithmetic_float_op(val1, val2,"nt1=("\
				"(n1=new Float("\
				"%s" \
				"))"\
				"%s" \
				"(n2=new Float("\
				"%s" \
				"))"\
				");nt2=nt1.toFloat();delete n1;delete n2;delete nt1;return nt2;")
void test_arithmetic_float_op(zs_float val1, zs_float val2, const char *str_format) {
	struct _test_arithmetic_float_op_data {
		std::string str; zs_float val;
	}test_arithmetic_float_op_data[] = {
		INLINE_OPERATION(val1,+,val2)
		,INLINE_OPERATION(val1,+,-val2)
		,INLINE_OPERATION(-val1,+,val2)
		,INLINE_OPERATION(-val1,+,-val2)

		,INLINE_OPERATION(val1,-,val2)
		,INLINE_OPERATION(-val1,-,val2)


		,INLINE_OPERATION(val1,*,val2)
		,INLINE_OPERATION(val1,*,-val2)
		,INLINE_OPERATION(-val1,*,val2)
		,INLINE_OPERATION(-val1,*,-val2)

		,INLINE_OPERATION(val1, / ,val2)
		,INLINE_OPERATION(val1, / ,-val2)
		,INLINE_OPERATION(-val1, / ,val2)
		,INLINE_OPERATION(-val1, / ,-val2)
		, { "",0 }
	};

	struct _test_arithmetic_float_mod_op_data {
		std::string str; zs_float val;
	}test_arithmetic_float_mod_op_data[] = {
		INLINE_FLOAT_MOD_OPERATION(val1,val2)
		,INLINE_FLOAT_MOD_OPERATION(val1,-val2)
		,INLINE_FLOAT_MOD_OPERATION(-val1,val2)
		,INLINE_FLOAT_MOD_OPERATION(-val1,-val2)
		, { "",0 }
	};

	// process first part
	_test_arithmetic_float_op_data *it_af = test_arithmetic_float_op_data;
	while (!it_af->str.empty()) {
		try {
			StackElement stk = zs->eval(it_af->str);
			if(stk.properties & STK_PROPERTY_ZS_FLOAT){
				zs_float result;
				ZS_FLOAT_COPY(&result,&stk.value);
					
				if (!float_values_are_almost_the_same(result, it_af->val)) {

						fprintf(stderr, "error test \"%s\" expected %f but it was %f!\n", it_af->str.c_str(), it_af->val, result);
						exit(-1);
				}
			}else{
				fprintf(stderr,"error test \"%s\" expected float but it was '%s'!\n",it_af->str.c_str(),stk.typeOf());
				exit(-1);
			}
		}
		catch (std::exception & ex) {
				fprintf(stderr, "error test \"%s\" : %s!\n", it_af->str.c_str(), ex.what());
				exit(-1); 
		}
		it_af++;
	}

	_test_arithmetic_float_mod_op_data *it_afm = test_arithmetic_float_mod_op_data;

	// process second part
	while (!it_afm->str.empty()) {
		try {
			StackElement stk = zs->eval(it_afm->str);
			if(stk.properties & STK_PROPERTY_ZS_FLOAT){
				zs_float result;
				ZS_FLOAT_COPY(&result,&stk.value);
				if (!float_values_are_almost_the_same(result, it_afm->val)) {
					fprintf(stderr, "error test \"%s\" expected %f but it was %f!\n", it_afm->str.c_str(), it_afm->val, result); \
					exit(-1);
				}
			}else{
				fprintf(stderr,"error test \"%s\" expected float but it was '%s'!\n",it_afm->str,stk.typeOf());
				exit(-1);
			}
		}
		catch (std::exception & ex) {
			fprintf(stderr, "error test \"%s\" : %s!\n", it_afm->str.c_str(), ex.what());
			exit(-1);
		}
		it_afm++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
//
// BOOL OPERATIONS
//
#define TEST_BOOL_EXPR(str_expr, expected_value) test_bool_expr(str_expr, expected_value)
void test_bool_expr(const char *str_expr, bool expected_value){ 
	try{
		StackElement stk = zs->eval(std::string("return ")+str_expr);
		if(stk.properties & STK_PROPERTY_BOOL){
			if((bool)stk.value  != (expected_value)){
				fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n", str_expr,expected_value,(bool)stk.value);
				exit(-1);
			}
		}else{
			fprintf(stderr,"error test \"%s\" expected bool but it was '%s'!\n",str_expr,stk.typeOf());
			exit(-1);
		}
	}catch(std::exception & ex){
		fprintf(stderr,"error test \"%s\" : %s!\n", str_expr,ex.what());
		exit(-1);
	}
}

#define TEST_ARITHMETIC_BOOL_EXPR(val1) test_bool_expression(val1,ZS_STR(val1))
void test_bool_expression(bool expr, const char *str_expr){

	try{
		StackElement stk = zs->eval(std::string("return ")+str_expr);
		if(stk.properties & STK_PROPERTY_BOOL){
			if((bool)stk.value  != (expr)){
				fprintf(stderr,"error test \"%s\" expected %s but it was %s!\n", str_expr,(expr)?"true":"false",(bool)stk.value?"true":"false");
				exit(-1);
			}
		}else{
			fprintf(stderr,"error test \"%s\" expected bool but it was '%s'!\n",str_expr,stk.typeOf());
			exit(-1);

		}
	}catch(std::exception & ex){
		fprintf(stderr,"error test \"%s\" : %s!\n", str_expr,ex.what());
		exit(-1); 
	}
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

//------------------------------------------------------------------------------------------------------------------------------------
//
// STRING OPERATIONS
//

#define TEST_STRING_EXPR(str_expr, expected_value) test_string_expr(expected_value,str_expr)
void test_string_expr(const char * expected_value, const char *str_expr){
	
	
	try{
		StackElement stk = zs->eval(std::string("return ")+str_expr);
		if(STK_IS_SCRIPT_OBJECT_STRING(&stk)){
			ScriptObjectString *so=(ScriptObjectString *)stk.value;
			if (so->toString() != std::string(expected_value)) {
				fprintf(stderr, "error test \"%s\" expected \"%s\" but it was \"%s\"!\n", str_expr, expected_value, so->toString().c_str());
				exit(-1);
			}
		}else{
			fprintf(stderr,"error test \"%s\" expected 'ScriptObjectString' but it was '%s'!\n",str_expr,stk.typeOf());
			exit(-1);
		}
	}catch(std::exception & ex){
		fprintf(stderr,"error test \"%s\" : %s!\n", str_expr,ex.what());
		exit(-1);
	}
}

#define TEST_ARITHMETIC_STRING_OP(val1,op, val2) test_arithmetic_string_op(std::string(val1) op val2,ZS_STR(val1)ZS_STR(op)val2)
void test_arithmetic_string_op(const std::string & expected_value, const char * str_expr){


	try{\
		StackElement stk = ZS_EVAL(zs,std::string("return ")+str_expr);
		if(STK_IS_SCRIPT_OBJECT_STRING(&stk)){
			ScriptObjectString *so=(ScriptObjectString *)stk.value;
			if (so->toString() != std::string(expected_value)) {
				fprintf(stderr,"error test \"%s\" expected %s but it was %s!\n",str_expr, expected_value.c_str(), so->toString().c_str()); \
				exit(-1); \
			} \
			// destroy lifetime object when is not needed
			zs->unrefLifetimeObject(so);
		}else{
			fprintf(stderr,"error test \"%s\" expected 'ScriptObjectString' but it was '%s'!\n",str_expr,stk.typeOf());
			exit(-1);
		}
	}catch(std::exception & ex){\
		fprintf(stderr,"error test \"%s\" : %s!\n", str_expr,ex.what()); \
		exit(-1); \
	}\
}

int main(int argc, char * argv[]) {

	int n_test=0;

	zs=new ZetScript();

	zs->registerClass<Float>("Float",FloatWrap_new,FloatWrap_delete);

	// constructor
	zs->registerConstructor<Float>(static_cast<void (*)(Float *,zs_float *)>(&FloatWrap_set));
	zs->registerConstructor<Float>(static_cast<void (*)(Float *,Float *)>(&FloatWrap_set));

	zs->registerMemberFunction<Float>("_set",static_cast<void (*)(Float *,zs_float *)>(&FloatWrap_set));
	zs->registerMemberFunction<Float>("_set",static_cast<void (*)(Float *,Float *)>(&FloatWrap_set));


	zs->registerMemberFunction<Float>("toFloat",&FloatWrap_toFloat);
	//zs->registerMemberVariable<Float>("n",&Float::n);


	zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap_add));
	zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap_add));
	zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap_add));

	zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap_sub));
	zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap_sub));
	zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap_sub));


	zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap_mul));
	zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap_mul));
	zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap_mul));

	zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap_div));
	zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap_div));
	zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap_div));

	zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap_mod));
	zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap_mod));
	zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap_mod));

	zs->registerMemberFunction<Float>("_neg",static_cast<Float * (*)(Float *)>(&FloatWrap_neg));


	zs->registerClass<Integer>("Integer",IntegerWrap_new,IntegerWrap_delete);
	zs->registerConstructor<Integer>(static_cast<void (*)(Integer *,zs_int )>(&IntegerWrap_set));
	zs->registerConstructor<Integer>(static_cast<void (*)(Integer *,Integer *)>(&IntegerWrap_set));
	zs->registerMemberFunction<Integer>("toInt",&IntegerWrap_toInt);

	zs->registerMemberFunction<Integer>("_set",static_cast<void (*)(Integer *, zs_int)>(&IntegerWrap_set));
	zs->registerMemberFunction<Integer>("_set",static_cast<void (*)(Integer *,Integer *)>(&IntegerWrap_set));


	zs->registerMemberFunctionStatic<Integer>("_add",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_add));
	zs->registerMemberFunctionStatic<Integer>("_add",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_add));
	zs->registerMemberFunctionStatic<Integer>("_add",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_add));

	zs->registerMemberFunctionStatic<Integer>("_sub",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_sub));
	zs->registerMemberFunctionStatic<Integer>("_sub",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_sub));
	zs->registerMemberFunctionStatic<Integer>("_sub",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_sub));


	zs->registerMemberFunctionStatic<Integer>("_mul",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_mul));
	zs->registerMemberFunctionStatic<Integer>("_mul",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_mul));
	zs->registerMemberFunctionStatic<Integer>("_mul",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_mul));

	zs->registerMemberFunctionStatic<Integer>("_div",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_div));
	zs->registerMemberFunctionStatic<Integer>("_div",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_div));
	zs->registerMemberFunctionStatic<Integer>("_div",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_div));

	zs->registerMemberFunctionStatic<Integer>("_mod",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_mod));
	zs->registerMemberFunctionStatic<Integer>("_mod",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_mod));
	zs->registerMemberFunctionStatic<Integer>("_mod",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_mod));


	zs->registerMemberFunctionStatic<Integer>("_shr",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_shr));
	zs->registerMemberFunctionStatic<Integer>("_shr",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_shr));
	zs->registerMemberFunctionStatic<Integer>("_shr",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_shr));


	zs->registerMemberFunctionStatic<Integer>("_shl",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_shl));
	zs->registerMemberFunctionStatic<Integer>("_shl",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_shl));
	zs->registerMemberFunctionStatic<Integer>("_shl",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_shl));


	zs->registerMemberFunctionStatic<Integer>("_and",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_and));
	zs->registerMemberFunctionStatic<Integer>("_and",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_and));
	zs->registerMemberFunctionStatic<Integer>("_and",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_and));

	zs->registerMemberFunctionStatic<Integer>("_or",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_or));
	zs->registerMemberFunctionStatic<Integer>("_or",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_or));
	zs->registerMemberFunctionStatic<Integer>("_or",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_or));

	zs->registerMemberFunctionStatic<Integer>("_xor",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_xor));
	zs->registerMemberFunctionStatic<Integer>("_xor",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_xor));
	zs->registerMemberFunctionStatic<Integer>("_xor",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_xor));

	zs->registerMemberFunction<Integer>("_neg",static_cast<Integer * (*)(Integer *)>(&IntegerWrap_neg));


	//----------------------
	// TEST (erase on exit)
    /*zs->eval("var i1,i2,it1,it2,n1,n2,nt1,nt2");
	zs->eval("it1=("\
				"(i1=new Integer("\
				"4" \
				"))"\
				"+" \
				"(i2=new Integer("\
				"4" \
				"))"\
			");it2=it1.toInt();delete it1;delete i1;delete i2;return it2;");

	//COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(4,2); // op1==op2

	zs->eval("it1=((i1=new Integer(4))+(i2=new Integer(-4)));it2=it1.toInt();delete it1;delete i1;delete i2;return it2;");
	printf("%i. testing class Integer arithmetic operations...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(4,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(4,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(5,4); // op1 > op2
	return 0;*/

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


	zs->eval("var i");

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
	TEST_FLOAT_EXPR("i=2.0;",2.0f);
	TEST_FLOAT_EXPR("i++;i;",3.0f);
	TEST_FLOAT_EXPR("--i;i;",2.0f);

	TEST_BOOL_EXPR("i=true;",true);
	TEST_BOOL_EXPR("i=!i;",false);
	TEST_BOOL_EXPR("i==i;",true);
	TEST_BOOL_EXPR("i!=i;",false);
	TEST_BOOL_EXPR("i=!i;",true);


	printf("%i. test if-else ...\n",++n_test);
	TEST_INT_EXPR("i=0;if(i==0){i=10;}else{i=11;}i;",10);
	TEST_INT_EXPR("if(i==0){i=10;}else{i=11;}i;",11);

	zs->eval("var i1,i2,it1,it2,n1,n2,nt1,nt2");

	printf("%i. testing class Integer arithmetic operations...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(4,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(4,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(5,4); // op1 > op2

	printf("%i. testing class Float arithmetic operations ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(4.0,4.0); // op1==op2
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(4.0,5.0); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(5.0,4.0); // op1 > op2
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(5.0,2.0e2); // op1 > op2


	printf("%i. test consisten script-c-script calls ...\n",++n_test);
	zs->registerFunction("test_function_1st_c_call",test_function_1st_c_call);
	// test calling script-c-script-c

	zs->eval("function test_1st_script_call(){\n"
				"Console::outln (\"Hello from script\");\n"
				"test_function_1st_c_call();\n"
			"}\n"
			"function test_2nd_script_call(){\n"
				"Console::outln(\"2nd call script\");\n"
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
