/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"


zetscript::zs_string to_string(bool _b){
	return _b?"true":"false";
}

zetscript::zs_string to_string(zetscript::zs_int _i){
	return zetscript::zs_strutils::zs_int_to_str(_i);
}

zetscript::zs_string to_string(zetscript::zs_float _f){
	return zetscript::zs_strutils::zs_float_to_str(_f);
}

zetscript::zs_string to_string(const zetscript::zs_string & _s){
	return _s;
}

// Usable AlmostEqual function
bool float_values_are_almost_the_same(zetscript::zs_float A, zetscript::zs_float B, int maxUlps=8)
{
	return (fabs(A - B) <= FLT_EPSILON *2* std::max(fabs(A), fabs(B)));
}


#define INLINE_OPERATION(val1,op,val2) {zetscript::zs_strutils::format(str_format,to_string(val1).c_str(),ZS_STR(op),to_string(val2).c_str()), (val1) op (val2)}

//------------------------------------------------------------------------------------------------------------------------------------
//
// INTEGER OPERATIONS
//
#define TEST_ARITHMETIC_INT_EXPR(_zs,expr) test_arithmetic_int_expr(_zs,expr,ZS_STR(expr))
void test_arithmetic_int_expr(zetscript::ZetScript *_zs, zetscript::zs_int expr,const char *str_expr)
{ \
	try{\
		zetscript::StackElement stk=_zs->eval(zetscript::zs_string("return ")+str_expr);\
		if(stk.properties & zetscript::STK_PROPERTY_ZS_INT){\
			if((zetscript::zs_int)stk.value != expr){ \
				throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected %i but it was %i!\n",str_expr,(int)expr,int(((zetscript::zs_int)stk.value))).c_str()); \
			} \
		}else{\
			throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected int but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str()).c_str()); \
		} \
	}catch(std::exception & ex){\
		throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' : %s!\n",str_expr,ex.what()).c_str()); \
	}\
}

#define TEST_INT_EXPR(_zs,str_expr, expected_value) test_int_expr(_zs,str_expr, expected_value)
void test_int_expr(zetscript::ZetScript *_zs, const char *str_expr, zetscript::zs_int expected_value) {
	try{\
		zetscript::StackElement stk=_zs->eval(str_expr);\
		if(stk.properties & zetscript::STK_PROPERTY_ZS_INT){\
			if((zetscript::zs_int)stk.value  != (expected_value)){ \
				throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected %i but it was %i!\n",str_expr,(int)expected_value,(int)((zetscript::zs_int)stk.value)).c_str()); \
			} \
		}else{\
			throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected int but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str()).c_str()); \
		} \
	}catch(std::exception & ex){\
		throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' : %s!\n", str_expr,ex.what()).c_str()); \
	}\
}

void _complete_test_arithmetic_integer_op(zetscript::ZetScript *_zs,zetscript::zs_int val1, zetscript::zs_int val2, const char *str_format){
	struct _test_arithmetic_integer_op_data {
		zetscript::zs_string str; zetscript::zs_int val;\
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
		,INLINE_OPERATION(val1, >> , (zetscript::zs_int)1)
		,INLINE_OPERATION(val1, << , (zetscript::zs_int)1)
		,INLINE_OPERATION(val1, ^, val2)
		, { "",0 }
	};

	// process
	_test_arithmetic_integer_op_data *it_iod = test_arithmetic_integer_op_data;
	while (!it_iod->str.empty()) {
		try {
			zetscript::StackElement stk=_zs->eval(it_iod->str); \
			if (stk.properties & zetscript::STK_PROPERTY_ZS_INT) {
				if ((zetscript::zs_int)stk.value != (it_iod->val)) {
					throw std::runtime_error(zetscript::zs_strutils::format( "error test '%s' expected %i but it was %i!\n", it_iod->str.c_str(), (int)it_iod->val,(int)((zetscript::zs_int)stk.value)).c_str());
				}
			}else{
				throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected int but it was '%s'!\n",it_iod->str.c_str(),stk_to_str(_zs,&stk).c_str()).c_str()); \
			}
		}
		catch (std::exception & ex) {
			throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' : %s!\n", it_iod->str.c_str(), ex.what()).c_str());
		}
		it_iod++;
	}
}
#define INLINE_OPERATION_ASSIGN(val1,op,val2) {zetscript::zs_strutils::format(str_format,ZS_STR(op),to_string(val2).c_str()), (val1) op (val2)}
void _complete_test_arithmetic_integer_op_assign(zetscript::ZetScript *_zs, zetscript::zs_int *ref_val1, zetscript::zs_int val2, const char *str_format){
	struct _test_arithmetic_integer_op_data {
		zetscript::zs_string str; zetscript::zs_int val;\
	}test_arithmetic_integer_op_data[] = {
		// assign right +
		INLINE_OPERATION_ASSIGN(*ref_val1,=,val2)
		,INLINE_OPERATION_ASSIGN(*ref_val1,+=,val2*10)
		,INLINE_OPERATION_ASSIGN(*ref_val1,-=,val2*5)
		,INLINE_OPERATION_ASSIGN(*ref_val1,*=,val2*2)
		,INLINE_OPERATION_ASSIGN(*ref_val1,/=,val2)
		,INLINE_OPERATION_ASSIGN(*ref_val1,%=,val2*100)
		,INLINE_OPERATION_ASSIGN(*ref_val1,^=,((zetscript::zs_int)(0x1100)))
		,INLINE_OPERATION_ASSIGN(*ref_val1,&=,((zetscript::zs_int)(0xffff)))
		,INLINE_OPERATION_ASSIGN(*ref_val1,<<=,((zetscript::zs_int)(2)))
		,INLINE_OPERATION_ASSIGN(*ref_val1,>>=,((zetscript::zs_int)(1)))
		// assign right -
		,INLINE_OPERATION_ASSIGN(*ref_val1,=,-val2)
		,INLINE_OPERATION_ASSIGN(*ref_val1,+=,-val2*10)
		,INLINE_OPERATION_ASSIGN(*ref_val1,-=,-val2*5)
		,INLINE_OPERATION_ASSIGN(*ref_val1,*=,-val2*2)
		,INLINE_OPERATION_ASSIGN(*ref_val1,/=,-val2)
		,INLINE_OPERATION_ASSIGN(*ref_val1,%=,-val2*100)
		,INLINE_OPERATION_ASSIGN(*ref_val1,^=,((zetscript::zs_int)(0x1100)))
		,INLINE_OPERATION_ASSIGN(*ref_val1,&=,((zetscript::zs_int)(0xffff)))
		,INLINE_OPERATION_ASSIGN(*ref_val1,<<=,((zetscript::zs_int)(2)))
		,INLINE_OPERATION_ASSIGN(*ref_val1,>>=,((zetscript::zs_int)(1)))
		, { "",0 }
	};

	// process
	_test_arithmetic_integer_op_data *it_iod = test_arithmetic_integer_op_data;
	while (!it_iod->str.empty()) {
		try {
			zetscript::StackElement stk=_zs->eval(it_iod->str); \
			if (stk.properties & zetscript::STK_PROPERTY_ZS_INT) {
				if ((zetscript::zs_int)stk.value != (it_iod->val)) {
					throw std::runtime_error(zetscript::zs_strutils::format( "error test '%s' expected %i but it was %i!\n", it_iod->str.c_str(), (int)it_iod->val,(int)((zetscript::zs_int)stk.value)).c_str());
				}
			}else{
				throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected int but it was '%s'!\n",it_iod->str.c_str(),stk_to_str(_zs,&stk).c_str()).c_str()); \
			}
		}
		catch (std::exception & ex) {
			throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' : %s!\n", it_iod->str.c_str(), ex.what()).c_str());
		}
		it_iod++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
//
// FLOAT OPERATIONS
//
#define TEST_FLOAT_EXPR(_zs,str_expr, expected_value) test_float_expr(_zs,str_expr, expected_value)
void test_float_expr(zetscript::ZetScript *_zs,const char  *str_expr, zetscript::zs_float expected_value) {
	try{
		zetscript::StackElement stk=_zs->eval(str_expr);
		if(stk.properties & zetscript::STK_PROPERTY_ZS_FLOAT){
			zetscript::zs_float result;
			ZS_FLOAT_COPY(&result,&stk.value);
			if(result  != (expected_value)){
				throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected %f but it was %f!\n",str_expr,expected_value,result).c_str());
			}
		}else{
			throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected float but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str()).c_str());
		}
	}catch(std::exception & ex){
		throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' : %s!\n",str_expr,ex.what()).c_str());
	}
}

#define TEST_ARITHMETIC_FLOAT_EXPR(_zs,val1) test_float_expression(_zs,(zetscript::zs_float)(val1),ZS_STR(val1)";")
void test_float_expression(zetscript::ZetScript *_zs,zetscript::zs_float expr, const char *str_expr) {

	try {
		zetscript::StackElement stk=_zs->eval(zetscript::zs_string("return ")+str_expr);
		if(stk.properties & zetscript::STK_PROPERTY_ZS_FLOAT){
			zetscript::zs_float result;
			ZS_FLOAT_COPY(&result,&stk.value);
			if (!float_values_are_almost_the_same(result, expr)) {
				double error = fabs(fabs(result) - fabs(expr));
				if (error > 0.001) { /* Only error if the difference is more than expected */
					throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected %f but it was %f!\n", str_expr, expr, result).c_str());
				}else {
					fprintf(stderr, "warning: test '%s' expected %f but it was %f (it has some precision error)!\n", str_expr, expr, result);
				}
			}
		}else{
			throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected float but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str()).c_str());
		}
	}
	catch (std::exception & ex) {
		throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' : %s!\n", str_expr, ex.what()).c_str());
	}
}


#define INLINE_FLOAT_MOD_OPERATION(val1,val2) {zetscript::zs_strutils::format(str_format,to_string(val1).c_str(),"%",to_string(val2).c_str()), (zetscript::zs_float)fmod(val1,val2)}
#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_zs,val1,val2) _complete_test_arithmetic_float_op(_zs,val1, val2,"return %s%s%s")

void _complete_test_arithmetic_float_op(zetscript::ZetScript *_zs,zetscript::zs_float val1, zetscript::zs_float val2, const char *str_format) {
	struct _test_arithmetic_float_op_data {
		zetscript::zs_string str; zetscript::zs_float val;
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
		zetscript::zs_string str; zetscript::zs_float val;
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
			zetscript::StackElement stk = _zs->eval(it_af->str);
			if(stk.properties & zetscript::STK_PROPERTY_ZS_FLOAT){
				zetscript::zs_float result;
				ZS_FLOAT_COPY(&result,&stk.value);

				if (!float_values_are_almost_the_same(result, it_af->val)) {
					throw std::runtime_error(zetscript::zs_strutils::format( "error test '%s' expected %f but it was %f!\n", it_af->str.c_str(), it_af->val, result).c_str());
				}
			}else{
				throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected float but it was '%s'!\n",it_af->str.c_str(),stk_to_str(_zs,&stk).c_str()).c_str());
			}
		}
		catch (std::exception & ex) {
			throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' : %s!\n", it_af->str.c_str(), ex.what()).c_str());
		}
		it_af++;
	}

	_test_arithmetic_float_mod_op_data *it_afm = test_arithmetic_float_mod_op_data;

	// process second part
	while (!it_afm->str.empty()) {
		try {
			zetscript::StackElement stk = _zs->eval(it_afm->str);
			if(stk.properties & zetscript::STK_PROPERTY_ZS_FLOAT){
				zetscript::zs_float result;
				ZS_FLOAT_COPY(&result,&stk.value);
				if (!float_values_are_almost_the_same(result, it_afm->val)) {
					throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected %f but it was %f!\n", it_afm->str.c_str(), it_afm->val, result).c_str()); \
				}
			}else{
				throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected float but it was '%s'!\n",it_afm->str.c_str(),stk_to_str(_zs,&stk).c_str()).c_str());
			}
		}
		catch (std::exception & ex) {
			throw std::runtime_error(zetscript::zs_strutils::format( "error test '%s' : %s!\n", it_afm->str.c_str(), ex.what()).c_str());
		}
		it_afm++;
	}
}



//------------------------------------------------------------------------------------------------------------------------------------
//
// BOOL OPERATIONS
//
#define TEST_BOOL_EXPR(_zs,str_expr, expected_value) test_bool_expr(_zs,str_expr, expected_value)
void test_bool_expr(zetscript::ZetScript *_zs,const char *str_expr, bool expected_value){
	try{
		zetscript::StackElement stk = _zs->eval(str_expr);
		if(stk.properties & zetscript::STK_PROPERTY_BOOL){
			if((bool)stk.value  != (expected_value)){
				throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected %i but it was %i!\n", str_expr,expected_value,(bool)stk.value).c_str());
			}
		}else{
			throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected bool but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str()).c_str());
		}
	}catch(std::exception & ex){
		throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' : %s!\n", str_expr,ex.what()).c_str());
	}
}

#define TEST_ARITHMETIC_BOOL_EXPR(_zs,val1) test_bool_expression(_zs,val1,ZS_STR(val1))
void test_bool_expression(zetscript::ZetScript *_zs,bool expr, const char *str_expr){

	try{
		zetscript::StackElement stk = _zs->eval(zetscript::zs_string("return ")+str_expr);
		if(stk.properties & zetscript::STK_PROPERTY_BOOL){
			if((bool)stk.value  != (expr)){
				throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected %s but it was %s!\n", str_expr,(expr)?"true":"false",(bool)stk.value?"true":"false").c_str());
			}
		}else{
			throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected bool but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str()).c_str());
		}
	}catch(std::exception & ex){
		throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' : %s!\n", str_expr,ex.what()).c_str());
	}
}



//------------------------------------------------------------------------------------------------------------------------------------
//
// STRING OPERATIONS
//

#define TEST_STRING_EXPR(str_expr, expected_value) test_string_expr(expected_value,str_expr)
void test_string_expr(zetscript::ZetScript *_zs,const char * expected_value, const char *str_expr){


	try{
		zetscript::StackElement stk = _zs->eval(zetscript::zs_string("return ")+str_expr);
		if(STK_IS_SCRIPT_OBJECT_STRING(&stk)){
			zetscript::ScriptObjectString *so=(zetscript::ScriptObjectString *)stk.value;
			if (so->toString() != zetscript::zs_string(expected_value)) {
				throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected '%s' but it was '%s'!\n", str_expr, expected_value, so->toString().c_str()).c_str());
			}
		}else{
			throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected 'ScriptObjectString' but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str()).c_str());
		}
	}catch(std::exception & ex){
		throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' : %s!\n", str_expr,ex.what()).c_str());
	}
}

#define TEST_ARITHMETIC_STRING_OP(_zs,val1,op, val2) test_arithmetic_string_op(_zs,zetscript::zs_string(val1) op val2,ZS_STR(val1)ZS_STR(op)val2)
void test_arithmetic_string_op(zetscript::ZetScript *_zs,const zetscript::zs_string & expected_value, const char * str_expr){


	try{\
		zetscript::StackElement stk = _zs->eval(zetscript::zs_string("return ")+str_expr,__FILE__,__LINE__);
		if(STK_IS_SCRIPT_OBJECT_STRING(&stk)){
			zetscript::ScriptObjectString *so=(zetscript::ScriptObjectString *)stk.value;
			if (so->toString() != zetscript::zs_string(expected_value)) {
				fprintf(stderr,"error test '%s' expected %s but it was %s!\n",str_expr, expected_value.c_str(), so->toString().c_str()); \
				exit(-1); \
			} \
			// destroy lifetime object when is not needed
			_zs->unrefLifetimeObject(so);
		}else{
			throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' expected 'ScriptObjectString' but it was '%s'!\n",str_expr,stk_to_str(_zs,&stk).c_str()).c_str());

		}
	}catch(std::exception & ex){\
		throw std::runtime_error(zetscript::zs_strutils::format("error test '%s' : %s!\n", str_expr,ex.what()).c_str()); \
	}\
}

