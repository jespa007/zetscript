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
		n=_n;
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
		return new CNumber(fmod(n1->n , n2));
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

	void set(int _n){
		n=_n;
	}

	static int _add(CInteger *n1, CInteger *n2){
		return n1->n + n2->n;
	}

	static int _add(CInteger *n1, float n2){
		return (int)(n1->n + n2);
	}

	static int _add(CInteger *n1, int n2){
		return n1->n + n2;
	}

	static int _add(int n1, CInteger * n2){
		return n1 + n2->n;
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

};

// Usable AlmostEqual function
bool FloatValuesAreAlmostTheSame(float A, float B, int maxUlps=4)
{
    // Make sure maxUlps is non-negative and small enough that the
    // default NAN won't compare as equal to anything.
    assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
    int aInt = *(int*)&A;
    // Make aInt lexicographically ordered as a twos-complement int
    if (aInt < 0)
        aInt = 0x80000000 - aInt;
    // Make bInt lexicographically ordered as a twos-complement int
    int bInt = *(int*)&B;
    if (bInt < 0)
        bInt = 0x80000000 - bInt;
    int intDiff = abs(aInt - bInt);
    if (intDiff <= maxUlps)
        return true;
    return false;
}



#define TEST_ARITHMETIC_INT_OP(val1, op, val2) \
{ \
	int aux_value=0; \
	string str= STR(val1) \
				STR(op) \
				STR(val2) \
				";"; \
	if((aux_value=CZetScript::eval<int>(str)) != (val1 op val2)){ \
		fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",str.c_str(),val1 op val2,aux_value); \
		exit(-1); \
	} \
}

#define TEST_ARITHMETIC_FLOAT_OP(val1, op, val2) \
{ \
	float expr=(float)(val1 op val2);\
	float aux_value=0; \
	string str= STR(val1) \
				STR(op) \
				STR(val2) \
				";"; \
	if(!FloatValuesAreAlmostTheSame(aux_value=CZetScript::eval<float>(str),expr)){ \
		fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",str.c_str(),expr,aux_value); \
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
	if((aux_value=CZetScript::eval<string>(str))!=expected_str.c_str()){ \
		fprintf(stderr,"error test \"%s\" expected %s but it was %s!\n",str.c_str(),expected_str.c_str(),aux_value.c_str()); \
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
	if(!FloatValuesAreAlmostTheSame(aux_value=CZetScript::eval<float>(str)  , fmod(val1,val2))){ \
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
		exit(-1); \
	} \
}

#define TEST_ARITHMETIC_INT_EXPR(expr) \
{ \
	int aux_value=0; \
	string str_expr= STR(expr)";"; \
	\
	if((aux_value=CZetScript::eval<int>(str_expr))  != (expr)){ \
		fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",str_expr.c_str(),expr,aux_value); \
		exit(-1); \
	} \
}

#define TEST_INT_EXPR(expr, expected_value) \
{ \
	int aux_value=0; \
	\
	if((aux_value=CZetScript::eval<int>(expr))  != (expected_value)){ \
		fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",expr,expected_value,aux_value); \
		exit(-1); \
	} \
}

#define TEST_BOOL_EXPR(expr, expected_value) \
{ \
	bool aux_value=false; \
	\
	if((aux_value=CZetScript::eval<bool>(expr))  != (expected_value)){ \
		fprintf(stderr,"error test \"%s\" expected %i but it was %i!\n",expr,expected_value,aux_value); \
		exit(-1); \
	} \
}

#define TEST_STRING_EXPR(expr, expected_value) \
{ \
	string aux_value=""; \
	\
	if((aux_value=CZetScript::eval<string>(expr))  != (expected_value)){ \
		fprintf(stderr,"error test \"%s\" expected \"%s\" but it was \"%s\"!\n",expr,expected_value,aux_value.c_str()); \
		exit(-1); \
	} \
}

#define TEST_NUMBER_EXPR(expr, expected_value) \
{ \
	float aux_value=0.0f; \
	\
	if((aux_value=CZetScript::eval<float>(expr))  != (expected_value)){ \
		fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",expr,expected_value,aux_value); \
		exit(-1); \
	} \
}

#define TEST_ARITHMETIC_FLOAT_EXPR(expr) \
{ \
	float aux_value=0; \
	string str_expr= STR(expr)";"; \
	\
	if(!FloatValuesAreAlmostTheSame(aux_value=CZetScript::eval<float>(str_expr)  , (expr))){ \
		double error = fabs(fabs(aux_value)-fabs(expr));\
		if(error>0.001){ /* Only error if the difference is more than expected */\
			fprintf(stderr,"error test \"%s\" expected %f but it was %f!\n",str_expr.c_str(),expr,aux_value); \
			exit(-1); \
		}else{\
			fprintf(stderr,"warning: test \"%s\" expected %f but it was %f (it has some precision error)!\n",str_expr.c_str(),expr,aux_value); \
		}\
	} \
}

#define TEST_ARITHMETIC_BOOL_EXPR(val1) \
{ \
	bool aux_value=false; \
	string str= STR(val1) \
				";"; \
	if((aux_value=CZetScript::eval<bool>(str)) != (val1)){ \
		fprintf(stderr,"error test \"%s\" expected %s but it was %s!\n",str.c_str(),(val1)?"true":"false",aux_value?"true":"false"); \
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


const char * my_fun(const char *str){
	printf("prova %s",str);

	return str;
}

void print2(string * str){
	printf("%s\n",str->c_str());
	fflush(stdout);
}


int main(int argc, char * argv[]) {

	int n_test=0;

	/*float ff=2.1234456f;

	auto fun_ptr=(void *)( new std::function<float (intptr_t)>(std::bind((float (*)(intptr_t))return_float, std::placeholders::_1)));

	//auto fun=std::function<intptr_t (intptr_t)>(return_float);
	//void * fun_ptr=(void *)&fun;

	int int_bits=*(int*)&ff;



	float result=(*((std::function<float (intptr_t)> *)fun_ptr))(
			int_bits
	);

	printf("result:%f\n",result);

	exit(-1);*/

	//TEST_NUMBER_EXPR("var i=2.0;i=-i;i;",-2.0);
	//TEST_BOOL_EXPR("var i=2.0;i=true;i=!i;i;",false);
	//return 0;
	//TEST_ARITHMETIC_BOOL_EXPR(!(!(!false)));//  || !(true && !false));
	//TEST_ARITHMETIC_BOOL_EXPR(!(!false));//  || !(true && !false));
	//return 0;

	CZetScript::getInstance();


	//register_C_Function("my_fun",my_fun);


	//CZetScript::getInstance()->eval("var s=\"caca\"; if(s instanceof gg){print(\"true\");}else{print(\"false\");}");

	//exit(-1);
	//TEST_INT_EXPR("var s={i:3,b:true,n:2.0,s:\"is_a_string\",o:new CInteger(5)};s.size();",3);


	//printf("\nnumber is %f",((2.0f+2.0f*(5.0f-6.1f))*10.0f));
	//TEST_BOOL_EXPR("!false && !false || false;",true);
	//return 0;

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

	//exit(-1);

	// test accessing script functions...
	/*if(CZetScript::getInstance()->eval("class MyClass{\n"
			"\n"
			"function fun1(){"
			" print(\"Hello from fun!\");"
			"}"
			"};\n;"
			"var myclass=new MyClass();\n"
			)){



		auto fun=CZetScript::getInstance()->bind_function("myclass.fun1");

		if(fun){
			(*fun)(NO_PARAMS);
		}
	}*/
	//TEST_ARITHMETIC_INT_OP(10,-,*10);

	//CZetScript::eval<int>("10-*10");
	//int i= 0+ +1;
	if(!register_C_Class<CNumber>("CNumber")) return false;
	if(!register_C_FunctionMember<CNumber>("CNumber",static_cast<void (CNumber::*)(int )>(&CNumber::set))) return false;
	if(!register_C_FunctionMember<CNumber>("CNumber",static_cast<void (CNumber::*)(float )>(&CNumber::set))) return false;
	if(!register_C_VariableMember(CNumber,n)) return false;
	/*if(!CScriptClass::register_C_StaticFunctionMemberInt<CNumber>("_add",static_cast<CNumber * (*)(CNumber *,int )>(&CNumber::_add))) return false;
	if(!CScriptClass::register_C_StaticFunctionMemberInt<CNumber>("_add",static_cast<CNumber * (*)(CNumber *,float *)>(&CNumber::_add))) return false;
	if(!CScriptClass::register_C_StaticFunctionMemberInt<CNumber>("_add",static_cast<CNumber * (*)(CNumber *,CNumber  *)>(&CNumber::_add))) return false;*/

	if(!register_C_Class<CInteger>("CInteger")) return false;
	if(!register_C_FunctionMember<CInteger>("CInteger",&CInteger::set)) return false;
	if(!register_C_VariableMember(CInteger,n)) return false;



	TEST_INT_EXPR("var v=[new CInteger(15)];v.size();",1); // <-- crash if no constructor defined new CInteger(x)!
	TEST_INT_EXPR("v[0].n;",15);// <-- error !!!
	//TEST_NUMBER_EXPR("v[5].n;",10.0f);

	// test adding ...

	printf("%i. testing struct var ...\n",++n_test);

	TEST_INT_EXPR("var s={o:new CInteger(10)};s.size();",1);

	exit(-1);
	//TEST_BOOL_EXPR("s.b;",true);
	//TEST_NUMBER_EXPR("s.n;",2.0);
	//TEST_STRING_EXPR("s.s;","is_a_string");
	/*TEST_INT_EXPR("var s={i:3,b:true,n:2.0,s:\"is_a_string\",v:[0,1,2],n2:new CNumber(5.0)};s.size();",6);
	TEST_INT_EXPR("s.i;",3);
	TEST_BOOL_EXPR("s.b;",true);
	TEST_NUMBER_EXPR("s.n;",2.0);
	TEST_STRING_EXPR("s.s;","is_a_string");*/
	//TEST_NUMBER_EXPR("var n2=new CNumber(5);n2.n;",2.0);
	//exit(-1);


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
	TEST_ARITHMETIC_BOOL_EXPR(!false && !false || false);
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


	printf("%i. testing vector var ...\n",++n_test);

	TEST_INT_EXPR("var v=[3,true,2.0,\"is_a_string\", new CInteger(5), new CNumber(10.0)];v.size();",6); // <-- crash if no constructor defined new CInteger(x)!
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




	// test if-else

/*








	printf("2. testing arithmetic hexa...\n");



	//------------------------------------------
	//
	// TEST FLOAT OPS
	//


	printf("3. testing arithmetic float ...\n");

*/

	printf("All tests passed OK!");

	CZetScript::destroy();



	return 0;
}
