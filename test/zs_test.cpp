/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

using namespace zetscript;

class CNumber{
public:

	float n;
	CNumber(){
		n=0;
	}

	CNumber(float _n){
		n=_n;
	}

	void set(float _n){
		n=_n;
	}

	void set(int _n){
		n=(float)_n;
	}

	static CNumber * _add(CNumber *n1, CNumber *n2){
		return new CNumber(n1->n + n2->n);
	}

	static CNumber * _add(CNumber *n1, float *n2){
		return new CNumber(n1->n + *n2);
	}

	static CNumber * _add(CNumber *n1, int n2){
		return new CNumber(n1->n + n2);
	}

	static CNumber * _div(CNumber *n1, CNumber *n2){
		return new CNumber(n1->n / n2->n);
	}

	static CNumber * _div(CNumber *n1, float *n2){
		return new CNumber(n1->n / *n2);
	}

	static CNumber * _div(CNumber *n1, int n2){
		return new CNumber(n1->n / n2);
	}

	static CNumber * _mod(CNumber *n1, CNumber *n2){
		return new CNumber(fmod(n1->n ,n2->n));
	}

	static CNumber * _mod(CNumber *n1, float *n2){
		return new CNumber(fmod(n1->n, *n2));
	}

	static CNumber * _mod(CNumber *n1, int n2){
		return new CNumber((float)fmod(n1->n , n2));
	}

	static CNumber * _mul(CNumber *n1, CNumber *n2){
		return new CNumber(n1->n * n2->n);
	}

	static CNumber * _mul(CNumber *n1, float *n2){
		return new CNumber(n1->n * *n2);
	}

	static CNumber * _mul(CNumber *n1, int n2){
		return new CNumber(n1->n * n2);
	}

};

class CInteger{
public:

	int n;
	CInteger(){
		n=0;
	}

	CInteger(int _n){
		n=_n;
	}

	void ScriptConstructor(int _n){
		n=_n;
	}

	void _set(int i){
		this->n = i;
	}

	void _set(CInteger *_ci2){
		this->n = _ci2->n;
	}


	static CInteger * _add(CInteger *n1, CInteger *n2){
		return new CInteger(n1->n + n2->n);
	}

	static CInteger * _add(CInteger *n1, float n2){
		return new CInteger((int)(n1->n + n2));
	}

	static CInteger * _add(CInteger *n1, int n2){
		return new CInteger(n1->n + n2);
	}

	static CInteger * _add(int n1, CInteger * n2){
		return new CInteger(n1 + n2->n);
	}

	static CInteger * _div(CInteger *n1, CInteger *n2){
		return new CInteger(n1->n / n2->n);
	}

	static CInteger * _div(CInteger *n1, float *n2){
		return new CInteger(n1->n / *n2);
	}

	static CInteger * _div(CInteger *n1, int n2){
		return new CInteger(n1->n / n2);
	}

	static CInteger * _mod(CInteger *n1, CInteger *n2){
		return new CInteger(n1->n % n2->n);
	}

	static CInteger * _mod(CInteger *n1, float *n2){
		return new CInteger(fmod(n1->n, *n2));
	}

	static CInteger * _mod(CInteger *n1, int n2){
		return new CInteger(n1->n % n2);
	}

	static CInteger * _mul(CInteger *n1, CInteger *n2){
		return new CInteger(n1->n * n2->n);
	}

	static CInteger * _mul(CInteger *n1, float *n2){
		return new CInteger(n1->n * *n2);
	}

	static CInteger * _mul(CInteger *n1, int n2){
		return new CInteger(n1->n * n2);
	}

	static CInteger * _shl(CInteger *n1, CInteger *n2){
		return new CInteger(n1->n << n2->n);
	}

	static CInteger * _shl(CInteger *n1, int n2){
		return new CInteger(n1->n << n2);
	}

	static CInteger * _shr(CInteger *n1, CInteger *n2){
		return new CInteger(n1->n >> n2->n);
	}

	static CInteger * _shr(CInteger *n1, int n2){
		return new CInteger(n1->n >> n2);
	}

	~CInteger(){
	}

};


std::function<void ()> * test_2nd_script_call = NULL;


void test_function_1st_c_call(){
	 printf("C Function 1st call from script\n");

	 if(test_2nd_script_call != NULL){
		 (*test_2nd_script_call)();
	 }
}


class CNumber2:public CNumber {

};

class CNumber3 :public CNumber {

};




// Usable AlmostEqual function
bool FloatValuesAreAlmostTheSame(float A, float B, int maxUlps=8)
{
	 return (fabs(A - B) <= FLT_EPSILON *2* std::max(fabs(A), fabs(B)));
    // Make sure maxUlps is non-negative and small enough that the
    // default NAN won't compare as equal to anything.
    /*assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
    //intptr_t aInt = *(int*)&A;
    int aInt;
    memcpy(&aInt,&A,sizeof(int));
    // Make aInt lexicographically ordered as a twos-complement int
    if ((int)aInt < 0)
        aInt = (int)(0x80000000 - aInt);
    // Make bInt lexicographically ordered as a twos-complement int
    int bInt;// = *(int*)&B;
    memcpy(&bInt,&B,sizeof(int));
    if (bInt < 0)
        bInt = 0x80000000 - bInt;
    int intDiff = abs(aInt - bInt);
    if (intDiff <= maxUlps)
        return true;
    return false;*/
}



#define TEST_ARITHMETIC_INT_OP(val1, op, val2) \
{ \
	int aux_value=0; \
	string str= STR(val1) \
				STR(op) \
				STR(val2) \
				";"; \
	if((aux_value=CZetScript::eval_int(str)) != (val1 op val2)){ \
		fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",str.c_str(),val1 op val2,aux_value); \
		fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
		exit(-1); \
	} \
}

#define TEST_ARITHMETIC_FLOAT_OP(val1, op, val2) \
{ \
	float expr=((float)(val1) op (float)(val2));\
	float aux_value=0; \
	string str= STR(val1) \
				STR(op) \
				STR(val2) \
				";"; \
	if(!FloatValuesAreAlmostTheSame(aux_value=CZetScript::eval_float(str),expr)){ \
		fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",str.c_str(),expr,aux_value); \
		fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
		exit(-1); \
	} \
}

#define TEST_ARITHMETIC_STRING_OP(val1,op, val2) \
{ \
	string aux_value=""; \
	string str= STR(val1) \
				STR(op) \
				val2\
				";"; \
	string expected_str = string(val1) op val2;\
	if((aux_value=CZetScript::eval_string(str))!=expected_str.c_str()){ \
		fprintf(stderr,"error test \"%s\" expected %s but it was %s!\n",str.c_str(),expected_str.c_str(),aux_value.c_str()); \
		fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
		exit(-1); \
	} \
}

#define TEST_ARITHMETIC_FLOAT_MOD(val1, val2) \
{ \
	float aux_value=0; \
	string str= STR(val1) \
				"%" \
				STR(val2) \
				";"; \
	if(!FloatValuesAreAlmostTheSame(aux_value=CZetScript::eval_float(str)  , fmod(val1,val2))){ \
		fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",str.c_str(),fmod(val1,val2),aux_value); \
		exit(-1); \
	} \
}


#define COMPLETE_TEST_ARITHMETIC_INT_OP(val1,val2) \
		TEST_ARITHMETIC_INT_OP(val1,+,val2); \
		TEST_ARITHMETIC_INT_OP(val1,+,-val2); \
		TEST_ARITHMETIC_INT_OP(-val1,+,val2); \
		TEST_ARITHMETIC_INT_OP(-val1,+,-val2); \
		\
		TEST_ARITHMETIC_INT_OP(val1,-,val2); \
		TEST_ARITHMETIC_INT_OP(val1,-,-val2); \
		TEST_ARITHMETIC_INT_OP(-val1,-,val2); \
		TEST_ARITHMETIC_INT_OP(-val1,-,-val2); \
		\
		TEST_ARITHMETIC_INT_OP(val1,*,val2); \
		TEST_ARITHMETIC_INT_OP(val1,*,-val2); \
		TEST_ARITHMETIC_INT_OP(-val1,*,val2); \
		TEST_ARITHMETIC_INT_OP(-val1,*,-val2); \
		\
		TEST_ARITHMETIC_INT_OP(val1,/,val2); \
		TEST_ARITHMETIC_INT_OP(val1,/,-val2); \
		TEST_ARITHMETIC_INT_OP(-val1,/,val2); \
		TEST_ARITHMETIC_INT_OP(-val1,/,-val2); \
		\
		TEST_ARITHMETIC_INT_OP(val1,%,val2); \
		TEST_ARITHMETIC_INT_OP(val1,%,-val2); \
		TEST_ARITHMETIC_INT_OP(-val1,%,val2); \
		TEST_ARITHMETIC_INT_OP(-val1,%,-val2);

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

#define COMPLETE_TEST_ARITHMETIC_BINARY_OP(val1, val2) \
	TEST_ARITHMETIC_INT_OP(val1,&,val2); \
	TEST_ARITHMETIC_INT_OP(val1,|,val2); \
	TEST_ARITHMETIC_INT_OP(val1,>>,1); \
	TEST_ARITHMETIC_INT_OP(val1,<<,1); \
	TEST_ARITHMETIC_INT_OP(val1,^,val2);


#define TEST_ARITHMETIC_OP(val1, op, val2) \
{ \
	int aux_value=0; \
	string str= STR(val1) \
				STR(op) \
				STR(val2) \
				";"; \
	if((aux_value=CZetScript::eval<f>(str)) != (val1 op val2)){ \
		fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",str.c_str(),val1 op val2,aux_value); \
		fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
		exit(-1); \
	} \
}

#define TEST_ARITHMETIC_INT_EXPR(expr) \
{ \
	int aux_value=0; \
	string str_expr= STR(expr)";"; \
	\
	if((aux_value=CZetScript::eval_int(str_expr))  != (expr)){ \
		fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",str_expr.c_str(),expr,aux_value); \
		fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
		exit(-1); \
	} \
}

#define TEST_INT_EXPR(expr, expected_value) \
{ \
	int aux_value=0; \
	\
	if((aux_value=CZetScript::eval_int(expr))  != (expected_value)){ \
		fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",expr,expected_value,aux_value); \
		fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
		exit(-1); \
	} \
}

#define TEST_BOOL_EXPR(expr, expected_value) \
{ \
	bool aux_value=false; \
	\
	if((aux_value=CZetScript::eval_bool(expr))  != (expected_value)){ \
		fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",expr,expected_value,aux_value); \
		fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
		exit(-1); \
	} \
}

#define TEST_STRING_EXPR(expr, expected_value) \
{ \
	string aux_value=""; \
	\
	if((aux_value=CZetScript::eval_string(expr))  != (expected_value)){ \
		fprintf(stderr,"error test \"%s\" expected \"%s\" but it was \"%s\"!\n",expr,expected_value,aux_value.c_str()); \
		fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
		exit(-1); \
	} \
}

#define TEST_NUMBER_EXPR(expr, expected_value) \
{ \
	float aux_value=0.0f; \
	\
	if((aux_value=CZetScript::eval_float(expr))  != (expected_value)){ \
		fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",expr,expected_value,aux_value); \
		fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
		exit(-1); \
	} \
}

#define TEST_ARITHMETIC_FLOAT_EXPR(expr) \
{ \
	float aux_value=0; \
	string str_expr= STR(expr)";"; \
	\
	if(!FloatValuesAreAlmostTheSame(aux_value=CZetScript::eval_float(str_expr)  , (expr))){ \
		double error = fabs(fabs(aux_value)-fabs(expr));\
		if(error>0.001){ /* Only error if the difference is more than expected */\
			fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",str_expr.c_str(),expr,aux_value); \
			fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
			exit(-1); \
		}else{\
			fprintf(stderr,"warning: test \"%s\" expected %f but it was %f (it has some precision error)!\n",str_expr.c_str(),expr,aux_value); \
			fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
		}\
	} \
}

#define TEST_ARITHMETIC_BOOL_EXPR(val1) \
{ \
	bool aux_value=false; \
	string str= STR(val1) \
				";"; \
	if((aux_value=CZetScript::eval_bool(str)) != (val1)){ \
		fprintf(stderr,"error test \"%s\" expected %s but it was %s!\n",str.c_str(),(val1)?"true":"false",aux_value?"true":"false"); \
		fprintf(stderr,"%s",ZS_GET_ERROR_MSG());\
		exit(-1); \
	} \
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

	CZetScript::getInstance();

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

	//CZetScript::getInstance()->eval("if(undefined){print(\"true\");}else{print(\"\");} }var prova=[]; prova.add(0); prova.size();");

	//TEST_NUMBER_EXPR("4.0*4;",16.0);
	//exit(-1);
	//int i= 0+ +1;
	if(!register_C_Class<CNumber>("CNumber")) return false;

	if (!register_C_Class<CNumber2>("CNumber2")) return false;
	if (!register_C_Class<CNumber3>("CNumber3")) return false;

	if(!class_C_baseof<CNumber, CNumber>()) return false;
	//if(!class_C_baseof<CNumber2, CNumber>()) return false;

	if(!register_C_FunctionMember("CNumber",static_cast<void (CNumber::*)(int )>(&CNumber::set))) return false;
	if(!register_C_FunctionMember("CNumber",static_cast<void (CNumber::*)(float )>(&CNumber::set))) return false;
	if(!register_C_VariableMember("n",&CNumber::n)) return false;

	if(!register_C_Class<CInteger>("CInteger")) return false;
	if(!register_C_FunctionMember("CInteger",&CInteger::ScriptConstructor)) return false;
	if(!register_C_VariableMember("n",&CInteger::n)) return false;

	if(!register_C_StaticFunctionMember<CInteger>("_add",static_cast<CInteger * (*)(int,CInteger * )>(&CInteger::_add))) return false;
	if(!register_C_StaticFunctionMember<CInteger>("_add",static_cast<CInteger * (*)(CInteger *,int)>(&CInteger::_add))) return false;
	if(!register_C_FunctionMember("_set",static_cast<void (CInteger::*)(int)>(&CInteger::_set))) return false;
	if(!register_C_FunctionMember("_set",static_cast<void (CInteger::*)(CInteger *)>(&CInteger::_set))) return false;




	// unsinged
	printf("%i. testing arithmetic ints...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_INT_OP(4,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_INT_OP(4,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_INT_OP(5,4); // op1 > op2


	printf("%i. testing arithmetic hexa (int)...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_INT_OP(0x4,0x4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_INT_OP(0x4,0x5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_INT_OP(0x5,0x4); // op1 > op2

	printf("%i. testing arithmetic float ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4.0,4.0); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4.0,5.0); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(5.0,4.0); // op1 > op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(5.0,2.0e2); // op1 > op2

	printf("%i. testing arithmetic float vs int ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4.0,4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4.0,5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(5.0,4); // op1 > op2

	printf("%i. testing arithmetic int vs float ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4,4.0); // op1==op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(4,5.0); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_FLOAT_OP(5,4.0); // op1 > op2

	printf("%i. testing binary op ...\n",++n_test);
	COMPLETE_TEST_ARITHMETIC_BINARY_OP(0x4,0x4); // op1==op2
	COMPLETE_TEST_ARITHMETIC_BINARY_OP(0x4,0x5); // op1 < op2
	COMPLETE_TEST_ARITHMETIC_BINARY_OP(0x5,0x4); // op1 > op2


	printf("%i. testing string...\n",++n_test);
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
	printf("%i. testing compare op ...\n",++n_test);
	COMPLETE_TEST_COMPARE_OP(10,10);
	COMPLETE_TEST_COMPARE_OP(15,10);
	COMPLETE_TEST_COMPARE_OP(10,15);

	// test logic and/or ...
	printf("%i. testing logic op ...\n",++n_test);
	COMPLETE_TEST_LOGIC_OP(10,10);
	COMPLETE_TEST_LOGIC_OP(15,10);
	COMPLETE_TEST_LOGIC_OP(10,15);

	// some basics tests
	TEST_ARITHMETIC_BOOL_EXPR(!false && !(false || false));
	TEST_ARITHMETIC_BOOL_EXPR(!(true && !false) || false);
	TEST_ARITHMETIC_BOOL_EXPR((true && !false) || !false);

	// test declare var int/bool/string/number
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


	/*printf("%i. testing vector var ...\n",++n_test);

	TEST_INT_EXPR("var v=[3,true,2.0,\"is_a_string\"];v.size();",4); // <-- crash if no constructor defined new CInteger(x)!
	TEST_INT_EXPR("v[0];",3);
	TEST_BOOL_EXPR("v[1];",true);
	TEST_NUMBER_EXPR("v[2];",2.0);
	TEST_STRING_EXPR("v[3];","is_a_string");
	//TEST_INT_EXPR("v[4].n;",5);// <-- error !!!
	//TEST_NUMBER_EXPR("v[5].n;",10.0f);

	// test adding ...

	printf("%i. testing struct var ...\n",++n_test);

	TEST_INT_EXPR("var s={i:3,b:true,n:2.0,s:\"is_a_string\",o:new CInteger(5)};s.size();",5);
	TEST_BOOL_EXPR("s.b;",true);
	TEST_NUMBER_EXPR("s.n;",2.0);
	TEST_STRING_EXPR("s.s;","is_a_string");
//	TEST_BOOL_EXPR("s.o.instanceof(MyObject);",true);

	printf("%i. testing metamethod integer ...\n",++n_test);
	TEST_INT_EXPR("var mt=new CInteger(5);mt=0+1+2+mt+4+5;mt.n;",17);*/

	printf("%i. test if-else ...\n",++n_test);
	TEST_INT_EXPR("i=0;if(i==0){i=10;}else{i=11;}i;",10);
	TEST_INT_EXPR("if(i==0){i=10;}else{i=11;}i;",11);

	printf("%i. test consisten script-c-script calls ...\n",++n_test);
	// test calling script-c-script-c
	register_C_Function("test_function_1st_c_call",test_function_1st_c_call);
	CZetScript::getInstance()->eval("function test_1st_script_call(){ print (\"Hello from script\");test_function_1st_c_call();}\nfunction test_2nd_script_call(){print(\"2nd call script\");}");

	std::function<void ()> * test_1st_script_call=bind_function<void ()>("test_1st_script_call");
	test_2nd_script_call=bind_function<void ()>("test_2nd_script_call");

	if(test_1st_script_call){
		(*test_1st_script_call)();
	}


	printf("All tests passed OK!");

	CZetScript::destroy();



	return 0;
}
