/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

using namespace zetscript;

#include "FloatWrap.cpp"
#include "IntegerWrap.cpp"


std::function<void ()> * test_2nd_script_call = NULL;


zs_string to_string(bool _b){
	return _b?"true":"false";
}

zs_string to_string(zs_int _i){
	return zs_strutils::zs_int_to_str(_i);
}

zs_string to_string(zs_float _f){
	return zs_strutils::zs_float_to_str(_f);
}

zs_string to_string(const zs_string & _s){
	return _s;
}




void test_function_1st_c_call(ZetScript *_zs){
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
#define TEST_ARITHMETIC_INT_EXPR(_zs,expr) test_arithmetic_int_expr(_zs,expr,ZS_STR(expr))
void test_arithmetic_int_expr(ZetScript *_zs, zs_int expr,const char *str_expr)
{ \
	try{\
		StackElement stk=_zs->eval(zs_string("return ")+str_expr);\
		if(stk.properties & STK_PROPERTY_ZS_INT){\
			if((zs_int)stk.value != expr){ \
				fprintf(stderr,"error test '%s' expected %i but it was %i!\n",str_expr,(int)expr,int(((zs_int)stk.value))); \
				exit(-1); \
			} \
		}else{\
			fprintf(stderr,"error test '%s' expected int but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str()); \
			exit(-1); \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test '%s' : %s!\n",str_expr,ex.what()); \
		exit(-1); \
	}\
}

#define TEST_INT_EXPR(_zs,str_expr, expected_value) test_int_expr(_zs,str_expr, expected_value)
void test_int_expr(ZetScript *_zs, const char *str_expr, zs_int expected_value) {
	try{\
		StackElement stk=_zs->eval(str_expr);\
		if(stk.properties & STK_PROPERTY_ZS_INT){\
			if((zs_int)stk.value  != (expected_value)){ \
				fprintf(stderr,"error test '%s' expected %i but it was %i!\n",str_expr,(int)expected_value,(int)((zs_int)stk.value)); \
				exit(-1); \
			} \
		}else{\
			fprintf(stderr,"error test '%s' expected int but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str()); \
			exit(-1); \
		} \
	}catch(std::exception & ex){\
		fprintf(stderr,"error test '%s' : %s!\n", str_expr,ex.what()); \
		exit(-1); \
	}\
}

#define COMPLETE_TEST_ARITHMETIC_INTEGER_OP(_zs,val1,val2) test_arithmetic_integer_op(_zs,val1, val2, "return %s%s%s")
#define COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(_zs,val1,val2)  test_arithmetic_integer_op(_zs,val1,val2,"it1=("\
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
void test_arithmetic_integer_op(ZetScript *_zs,zs_int val1, zs_int val2, const char *str_format){
	struct _test_arithmetic_integer_op_data {
		zs_string str; zs_int val;\
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
			StackElement stk=_zs->eval(it_iod->str); \
			if (stk.properties & STK_PROPERTY_ZS_INT) {
				if ((zs_int)stk.value != (it_iod->val)) {
					fprintf(stderr, "error test '%s' expected %i but it was %i!\n", it_iod->str.c_str(), (int)it_iod->val,(int)((zs_int)stk.value));
					exit(-1);
				}
			}else{
				fprintf(stderr,"error test '%s' expected int but it was '%s'!\n",it_iod->str.c_str(),stk_to_str(_zs,&stk).c_str()); \
				exit(-1); \
			}
		}
		catch (std::exception & ex) {
			fprintf(stderr, "error test '%s' : %s!\n", it_iod->str.c_str(), ex.what());
			exit(-1);
		}
		it_iod++;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------
//
// FLOAT OPERATIONS
//
#define TEST_FLOAT_EXPR(_zs,str_expr, expected_value) test_float_expr(_zs,str_expr, expected_value)
void test_float_expr(ZetScript *_zs,const char  *str_expr, zs_float expected_value) {
	try{
		StackElement stk=_zs->eval(str_expr);
		if(stk.properties & STK_PROPERTY_ZS_FLOAT){
			zs_float result;
			ZS_FLOAT_COPY(&result,&stk.value);
			if(result  != (expected_value)){
				fprintf(stderr,"error test '%s' expected %f but it was %f!\n",str_expr,expected_value,result);
				exit(-1);
			}
		}else{
			fprintf(stderr,"error test '%s' expected float but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str());
			exit(-1);
		}
	}catch(std::exception & ex){
		fprintf(stderr,"error test '%s' : %s!\n",str_expr,ex.what());
		exit(-1);
	}
}

#define TEST_ARITHMETIC_FLOAT_EXPR(_zs,val1) test_float_expression(_zs,(zs_float)(val1),ZS_STR(val1)";")
void test_float_expression(ZetScript *_zs,zs_float expr, const char *str_expr) {

	try {
		StackElement stk=_zs->eval(zs_string("return ")+str_expr);
		if(stk.properties & STK_PROPERTY_ZS_FLOAT){
			zs_float result;
			ZS_FLOAT_COPY(&result,&stk.value);
			if (!float_values_are_almost_the_same(result, expr)) {
				double error = fabs(fabs(result) - fabs(expr));
					if (error > 0.001) { /* Only error if the difference is more than expected */
						fprintf(stderr, "error test '%s' expected %f but it was %f!\n", str_expr, expr, result);
							exit(-1);
						}
				else {
					fprintf(stderr, "warning: test '%s' expected %f but it was %f (it has some precision error)!\n", str_expr, expr, result);
				
				}
			}
		}else{
			fprintf(stderr,"error test '%s' expected float but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str());
			exit(-1);
		}
	}
	catch (std::exception & ex) {
		fprintf(stderr, "error test '%s' : %s!\n", str_expr, ex.what());
		exit(-1);
	}
}

#define INLINE_FLOAT_MOD_OPERATION(val1,val2) {zs_strutils::format(str_format,to_string(val1).c_str(),"%",to_string(val2).c_str()), (zs_float)fmod(val1,val2)}
#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_zs,val1,val2) test_arithmetic_float_op(_zs,val1, val2,"return %s%s%s")
#define COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(_zs,val1,val2) test_arithmetic_float_op(_zs,val1, val2,"nt1=("\
				"(n1=new Float("\
				"%s" \
				"))"\
				"%s" \
				"(n2=new Float("\
				"%s" \
				"))"\
				");nt2=nt1.toFloat();delete n1;delete n2;delete nt1;return nt2;")
void test_arithmetic_float_op(ZetScript *_zs,zs_float val1, zs_float val2, const char *str_format) {
	struct _test_arithmetic_float_op_data {
		zs_string str; zs_float val;
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
		zs_string str; zs_float val;
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
			StackElement stk = _zs->eval(it_af->str);
			if(stk.properties & STK_PROPERTY_ZS_FLOAT){
				zs_float result;
				ZS_FLOAT_COPY(&result,&stk.value);
					
				if (!float_values_are_almost_the_same(result, it_af->val)) {

						fprintf(stderr, "error test '%s' expected %f but it was %f!\n", it_af->str.c_str(), it_af->val, result);
						exit(-1);
				}
			}else{
				fprintf(stderr,"error test '%s' expected float but it was '%s'!\n",it_af->str.c_str(),stk_to_str(_zs,&stk).c_str());
				exit(-1);
			}
		}
		catch (std::exception & ex) {
				fprintf(stderr, "error test '%s' : %s!\n", it_af->str.c_str(), ex.what());
				exit(-1); 
		}
		it_af++;
	}

	_test_arithmetic_float_mod_op_data *it_afm = test_arithmetic_float_mod_op_data;

	// process second part
	while (!it_afm->str.empty()) {
		try {
			StackElement stk = _zs->eval(it_afm->str);
			if(stk.properties & STK_PROPERTY_ZS_FLOAT){
				zs_float result;
				ZS_FLOAT_COPY(&result,&stk.value);
				if (!float_values_are_almost_the_same(result, it_afm->val)) {
					fprintf(stderr, "error test '%s' expected %f but it was %f!\n", it_afm->str.c_str(), it_afm->val, result); \
					exit(-1);
				}
			}else{
				fprintf(stderr,"error test '%s' expected float but it was '%s'!\n",it_afm->str.c_str(),stk_to_str(_zs,&stk).c_str());
				exit(-1);
			}
		}
		catch (std::exception & ex) {
			fprintf(stderr, "error test '%s' : %s!\n", it_afm->str.c_str(), ex.what());
			exit(-1);
		}
		it_afm++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
//
// BOOL OPERATIONS
//
#define TEST_BOOL_EXPR(_zs,str_expr, expected_value) test_bool_expr(_zs,str_expr, expected_value)
void test_bool_expr(ZetScript *_zs,const char *str_expr, bool expected_value){
	try{
		StackElement stk = _zs->eval(str_expr);
		if(stk.properties & STK_PROPERTY_BOOL){
			if((bool)stk.value  != (expected_value)){
				fprintf(stderr,"error test '%s' expected %i but it was %i!\n", str_expr,expected_value,(bool)stk.value);
				exit(-1);
			}
		}else{
			fprintf(stderr,"error test '%s' expected bool but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str());
			exit(-1);
		}
	}catch(std::exception & ex){
		fprintf(stderr,"error test '%s' : %s!\n", str_expr,ex.what());
		exit(-1);
	}
}

#define TEST_ARITHMETIC_BOOL_EXPR(_zs,val1) test_bool_expression(_zs,val1,ZS_STR(val1))
void test_bool_expression(ZetScript *_zs,bool expr, const char *str_expr){

	try{
		StackElement stk = _zs->eval(zs_string("return ")+str_expr);
		if(stk.properties & STK_PROPERTY_BOOL){
			if((bool)stk.value  != (expr)){
				fprintf(stderr,"error test '%s' expected %s but it was %s!\n", str_expr,(expr)?"true":"false",(bool)stk.value?"true":"false");
				exit(-1);
			}
		}else{
			fprintf(stderr,"error test '%s' expected bool but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str());
			exit(-1);

		}
	}catch(std::exception & ex){
		fprintf(stderr,"error test '%s' : %s!\n", str_expr,ex.what());
		exit(-1); 
	}
}


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

//------------------------------------------------------------------------------------------------------------------------------------
//
// STRING OPERATIONS
//

#define TEST_STRING_EXPR(str_expr, expected_value) test_string_expr(expected_value,str_expr)
void test_string_expr(ZetScript *_zs,const char * expected_value, const char *str_expr){
	
	
	try{
		StackElement stk = _zs->eval(zs_string("return ")+str_expr);
		if(STK_IS_SCRIPT_OBJECT_STRING(&stk)){
			ScriptObjectString *so=(ScriptObjectString *)stk.value;
			if (so->toString() != zs_string(expected_value)) {
				fprintf(stderr, "error test '%s' expected '%s' but it was '%s'!\n", str_expr, expected_value, so->toString().c_str());
				exit(-1);
			}
		}else{
			fprintf(stderr,"error test '%s' expected 'ScriptObjectString' but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str());
			exit(-1);
		}
	}catch(std::exception & ex){
		fprintf(stderr,"error test '%s' : %s!\n", str_expr,ex.what());
		exit(-1);
	}
}

#define TEST_ARITHMETIC_STRING_OP(_zs,val1,op, val2) test_arithmetic_string_op(_zs,zs_string(val1) op val2,ZS_STR(val1)ZS_STR(op)val2)
void test_arithmetic_string_op(ZetScript *_zs,const zs_string & expected_value, const char * str_expr){


	try{\
		StackElement stk = _zs->eval(zs_string("return ")+str_expr,__FILE__,__LINE__);
		if(STK_IS_SCRIPT_OBJECT_STRING(&stk)){
			ScriptObjectString *so=(ScriptObjectString *)stk.value;
			if (so->toString() != zs_string(expected_value)) {
				fprintf(stderr,"error test '%s' expected %s but it was %s!\n",str_expr, expected_value.c_str(), so->toString().c_str()); \
				exit(-1); \
			} \
			// destroy lifetime object when is not needed
			_zs->unrefLifetimeObject(so);
		}else{
			fprintf(stderr,"error test '%s' expected 'ScriptObjectString' but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str());
			exit(-1);
		}
	}catch(std::exception & ex){\
		fprintf(stderr,"error test '%s' : %s!\n", str_expr,ex.what()); \
		exit(-1); \
	}\
}

int main(int argc, char * argv[]) {

	int n_test=0;

	ZetScript zs;

	// register wraps
	FloatWrap_register(&zs);
	IntegerWrap_register(&zs);

	// unsinged
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

	//zs.eval("var i1='s',i2='s\",it1=\"s\",it2=\"s\",n1=\"s\",n2=\"s\",nt1=\"s\",nt2=\"s\"");
	// testing metamethods...
	printf("%i. testing class Integer arithmetic operations...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(&zs,4,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(&zs,4,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_CLASS_INTEGER_OP(&zs,5,4); // op1 > op2

	printf("%i. testing class Float arithmetic operations ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(&zs,4.0,4.0); // op1==op2
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(&zs,4.0,5.0); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(&zs,5.0,4.0); // op1 > op2
	COMPLETE_TEST_ARITHMETIC_CLASS_FLOAT_OP(&zs,5.0,2.0e2); // op1 > op2


	printf("%i. test consisten script-c-script calls ...\n",++n_test);
	zs.registerFunction("test_function_1st_c_call",test_function_1st_c_call);
	// test calling script-c-script-c

	zs.eval("function test_1st_script_call(){\n"
				"Console::outln (\"Hello from script\");\n"
				"test_function_1st_c_call();\n"
			"}\n"
			"function test_2nd_script_call(){\n"
				"Console::outln(\"2nd call script\");\n"
			"}");

	auto  test_1st_script_call=zs.bindScriptFunction<void ()>("test_1st_script_call");
	test_2nd_script_call=new std::function<void()>(zs.bindScriptFunction<void ()>("test_2nd_script_call"));

	if(test_1st_script_call){
		test_1st_script_call();
	}

	delete test_2nd_script_call;

	// test files

/*
	// test all external tests...
	const char *test_files[]={
		//"samples/assert_error.zs"
		"samples/assign.zs"
		,"samples/class_property.zs"
		,"samples/class_inheritance_call.zs"
		,"samples/class_metamethod.zs"
		,"samples/const.zs"
		,"samples/datetime.zs"
		,"samples/eval.zs"
		,"samples/for_in_object.zs"
		,"samples/for_in_string.zs"
		,"samples/for_in_vector.zs"
		,"samples/function_arg_by_ref.zs"
		,"samples/function_arg_default.zs"
		,"samples/function.zs"
		,"samples/if_else.zs"
		,"samples/import.zs"
		,"samples/in.zs"
		,"samples/instanceof.zs"
		,"samples/iterator_object.zs"
		,"samples/iterator_string.zs"
		,"samples/iterator_vector.zs"
		,"samples/json.zs"
		,"samples/loops_break_continue.zs"
		,"samples/loops.zs"
		,"samples/object.zs"
		,"samples/return.zs"
		,"samples/string.zs"
		,"samples/switch.zs"
		,"samples/ternary.zs"
		,"samples/typeof.zs"
		,"samples/vector.zs"
		,"samples/zs_int.zs"
		,0
	};

	printf("======================================\n");
	printf("Testing all zetscript samples\n");

	char **it=(char **)test_files;
	int total=sizeof(test_files)/sizeof(char **);
	int n=1;

	while(*it!=0){
		// clear all vars in order to no have conflict with previous evaluations
		zs.clear();
		printf("Evaluating %i/%i:'%s'\n",n++,total,*it);
		zs.evalFile(*it);
		it++;
	}*/

	printf("All tests passed OK!\n");



	return 0;
}
