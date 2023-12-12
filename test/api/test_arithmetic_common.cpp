
#include "test_arithmetic_common.h"

#define INLINE_OPERATION(val1,op,val2) {\
	zetscript::String::format(\
			str_format\
			,toString(val1).toConstChar()\
			,ZS_STR(op)\
			,toString(val2).toConstChar()\
	)\
	, (val1) op (val2)\
}


zetscript::String toString(
		bool _b
){
	return _b?"true":"false";
}

zetscript::String toString(
		zetscript::zs_int _i
){
	return zetscript::Integer::toString(_i);
}

zetscript::String toString(
		zetscript::zs_float _f
){
	return zetscript::Float::toString(_f);
}

zetscript::String toString(
		const zetscript::String & _s
){
	return _s;
}

// Usable AlmostEqual function
bool float_values_are_almost_the_same(
		zetscript::zs_float A
		, zetscript::zs_float B
){
	zetscript::zs_float a=fabs(A - B);
	zetscript::zs_float b=FLT_EPSILON *2* ZS_MAX(fabs(A), fabs(B));
	return a <= b;//(fabs(A - B) <= FLT_EPSILON *2* ZS_MAX(fabs(A), fabs(B)));
}

//------------------------------------------------------------------------------------------------------------------------------------
//
// INTEGER OPERATIONS
//
void test_arithmetic_int_expression(
		zetscript::ScriptEngine *_script_engine
		, zetscript::zs_int expr
		,const char *str_expr
){ \
	try{\
		zetscript::StackElement stk=_script_engine->eval(zetscript::String("return Integer::parse(")+str_expr+")");\
		if(stk.properties & zetscript::STACK_ELEMENT_PROPERTY_INT){\
			if((zetscript::zs_int)stk.value != expr){ \
				throw std::runtime_error(zetscript::String::format("error test '%s' expected %i but it was %i!\n",str_expr,(int)expr,int(((zetscript::zs_int)stk.value))).toConstChar()); \
			} \
		}else{\
			throw std::runtime_error(
				zetscript::String::format(
					"error test '%s' expected int but it was '%s'!\n"
					,str_expr
					,_script_engine->stackElementToString(&stk).toConstChar()
				).toConstChar()
			); \
		} \
	}catch(std::exception & ex){\
		throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n",str_expr,ex.what()).toConstChar()); \
	}\
}

void test_constant_int_expression(
		zetscript::ScriptEngine *_script_engine
		, const char *str_expr
		, zetscript::zs_int expected_value
) {
	try{\
		zetscript::StackElement stk=_script_engine->eval(str_expr);\
		if(stk.properties & zetscript::STACK_ELEMENT_PROPERTY_INT){\
			if((zetscript::zs_int)stk.value  != (expected_value)){ \
				throw std::runtime_error(zetscript::String::format("error test '%s' expected %i but it was %i!\n",str_expr,(int)expected_value,(int)((zetscript::zs_int)stk.value)).toConstChar()); \
			} \
		}else{\
			throw std::runtime_error(
					zetscript::String::format(
							"error test '%s' expected int but it was '%s'!\n"
							,str_expr
							,_script_engine->stackElementToString(&stk).toConstChar()).toConstChar()
					); \
		} \
	}catch(std::exception & ex){\
		throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", str_expr,ex.what()).toConstChar()); \
	}\
}

#define INLINE_OPERATION_ASSIGN(val1,op,val2) {\
		zetscript::String::format(\
			str_format\
			,ZS_STR(op)\
			,toString(val2).toConstChar()\
		)\
		, (val1) op (val2)\
	}

#define INLINE_SELF_OPERATION_RIGHT(val1,op) {\
	zetscript::String::format(\
		str_format\
		,""\
		,ZS_STR(op)\
	)\
	, (val1) op\
}

#define INLINE_SELF_OPERATION_LEFT(op, val1) {\
	zetscript::String::format(\
		str_format\
		,ZS_STR(op)\
		,""\
	)\
	, op (val1)\
}\

#define INLINE_SELF_OPERATION_BOTH(op1,val1,op2) {\
	zetscript::String::format(\
		str_format\
		,ZS_STR(op1)\
		,ZS_STR(op2)\
	)\
	, op1 (val1) op2\
}\

void _complete_test_arithmetic_integer_op(
		zetscript::ScriptEngine *_script_engine
		,zetscript::zs_int val1
		, zetscript::zs_int val2
		, const char *str_format
){
	struct _test_arithmetic_integer_op_data {
		zetscript::String str;
		zetscript::zs_int val;
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
	while (!it_iod->str.isEmpty()) {
		try {
			zetscript::StackElement stk=_script_engine->eval(it_iod->str); \
			if (_script_engine->stackElementTo<zetscript::zs_int>(stk) != (it_iod->val)) {
				throw std::runtime_error(zetscript::String::format( "error test '%s' expected %i but it was %i!\n", it_iod->str.toConstChar(), (int)it_iod->val,(int)((zetscript::zs_int)stk.value)).toConstChar());
			}
		}
		catch (std::exception & ex) {
			throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", it_iod->str.toConstChar(), ex.what()).toConstChar());
		}
		it_iod++;
	}
}

void _complete_test_arithmetic_integer_op_assign(
		zetscript::ScriptEngine *_script_engine
		, zetscript::zs_int _val1
		, zetscript::zs_int _val2
		, const char *str_format
){
	zetscript::zs_int ref_val1=_val1;
	zetscript::zs_int val2=_val2;
	struct _test_arithmetic_integer_op_data {
		zetscript::String str; zetscript::zs_int val;\
	}test_arithmetic_integer_op_data[] = {
		// assign right +
		INLINE_OPERATION_ASSIGN(ref_val1,=,val2)
		,INLINE_OPERATION_ASSIGN(ref_val1,+=,val2*10)
		,INLINE_OPERATION_ASSIGN(ref_val1,-=,val2*5)
		,INLINE_OPERATION_ASSIGN(ref_val1,*=,val2*2)
		,INLINE_OPERATION_ASSIGN(ref_val1,/=,val2)
		,INLINE_OPERATION_ASSIGN(ref_val1,%=,val2*100)
		,INLINE_OPERATION_ASSIGN(ref_val1,^=,((zetscript::zs_int)(0x1100)))
		,INLINE_OPERATION_ASSIGN(ref_val1,&=,((zetscript::zs_int)(0xffff)))
		,INLINE_OPERATION_ASSIGN(ref_val1,<<=,((zetscript::zs_int)(2)))
		,INLINE_OPERATION_ASSIGN(ref_val1,>>=,((zetscript::zs_int)(1)))
		// assign right -
		,INLINE_OPERATION_ASSIGN(ref_val1,=,-val2)
		,INLINE_OPERATION_ASSIGN(ref_val1,+=,-val2*10)
		,INLINE_OPERATION_ASSIGN(ref_val1,-=,-val2*5)
		,INLINE_OPERATION_ASSIGN(ref_val1,*=,-val2*2)
		,INLINE_OPERATION_ASSIGN(ref_val1,/=,-val2)
		,INLINE_OPERATION_ASSIGN(ref_val1,%=,-val2*100)
		,INLINE_OPERATION_ASSIGN(ref_val1,^=,((zetscript::zs_int)(0x1100)))
		,INLINE_OPERATION_ASSIGN(ref_val1,&=,((zetscript::zs_int)(0xffff)))
		,INLINE_OPERATION_ASSIGN(ref_val1,<<=,((zetscript::zs_int)(2)))
		,INLINE_OPERATION_ASSIGN(ref_val1,>>=,((zetscript::zs_int)(1)))
		, { "",0 }
	};

	// process
	_test_arithmetic_integer_op_data *it_iod = test_arithmetic_integer_op_data;
	while (!it_iod->str.isEmpty()) {
		try {
			zetscript::StackElement stk=_script_engine->eval(it_iod->str); \
			if (_script_engine->stackElementTo<zetscript::zs_int>(stk) != (it_iod->val)) {
				throw std::runtime_error(zetscript::String::format( "error test '%s' expected %i but it was %i!\n", it_iod->str.toConstChar(), (int)it_iod->val,(int)((zetscript::zs_int)stk.value)).toConstChar());
			}
		}
		catch (std::exception & ex) {
			throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", it_iod->str.toConstChar(), ex.what()).toConstChar());
		}
		it_iod++;
	}
}

#define INLINE_FLOAD_MOD_OPERATION_ASSIGN(val1,val2) {\
	zetscript::String::format(\
		str_format\
		,toString(val1).toConstChar()\
		,"%="\
		,toString(val2).toConstChar()\
	)\
	, val1=(zetscript::zs_float)fmod(val1,val2)\
}

void _complete_test_arithmetic_self_op(
		zetscript::ScriptEngine *_script_engine
		, zetscript::zs_int _start_value
		, const char *str_format
){
	//zetscript::zs_int result=0;
	//zetscript::zs_int ref_val1=_start_value;
	// postinc
	struct _test_arithmetic_integer_op_data {
		zetscript::String str;
		zetscript::zs_int val;
	}test_arithmetic_integer_op_data[] = {
		{zetscript::String::format(str_format,"","++"),_start_value+1}
		,{zetscript::String::format(str_format,"","--"),_start_value}
		,{zetscript::String::format(str_format,"++",""),_start_value+1}
		,{zetscript::String::format(str_format,"--",""),_start_value}
		,{ "",0 }
	};

	// process
	_test_arithmetic_integer_op_data *it_iod = test_arithmetic_integer_op_data;
	while (!it_iod->str.isEmpty()) {
		try {
			zetscript::StackElement stk=_script_engine->eval(it_iod->str); \

			if (_script_engine->stackElementTo<zetscript::zs_int>(stk) != (it_iod->val)) {
				throw std::runtime_error(zetscript::String::format( "error test '%s' expected %i but it was %i!\n", it_iod->str.toConstChar(), (int)it_iod->val,(int)((zetscript::zs_int)stk.value)).toConstChar());
			}

		}catch (std::exception & ex) {
			throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", it_iod->str.toConstChar(), ex.what()).toConstChar());
		}
		it_iod++;
	}
}


void _complete_test_arithmetic_self_property_op(
		zetscript::ScriptEngine *_script_engine
		, zetscript::zs_int _start_value
		, const char *str_format
){
	//zetscript::zs_int result=0;
	zetscript::zs_int ref_val1=_start_value;
	// postinc
	struct _test_arithmetic_integer_op_data {
		zetscript::String str;
		zetscript::zs_int val;
	}test_arithmetic_integer_op_data[] = {
		INLINE_SELF_OPERATION_RIGHT(ref_val1,++)
		,INLINE_SELF_OPERATION_RIGHT(ref_val1,--)
		,INLINE_SELF_OPERATION_LEFT(++,ref_val1)
		,INLINE_SELF_OPERATION_LEFT(--,ref_val1)
		,INLINE_SELF_OPERATION_BOTH(-,ref_val1,++)
		,INLINE_SELF_OPERATION_BOTH(-,ref_val1,--)
		, { "",0 }
	};


	// process
	_test_arithmetic_integer_op_data *it_iod = test_arithmetic_integer_op_data;
	while (!it_iod->str.isEmpty()) {
		try {
			zetscript::StackElement stk=_script_engine->eval(it_iod->str); \

			if (_script_engine->stackElementTo<zetscript::zs_int>(stk) != it_iod->val) {
				throw std::runtime_error(zetscript::String::format( "error test '%s' expected %i but it was %i!\n", it_iod->str.toConstChar(), (int)it_iod->val,(int)((zetscript::zs_int)stk.value)).toConstChar());
			}
		}
		catch (std::exception & ex) {
			throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", it_iod->str.toConstChar(), ex.what()).toConstChar());
		}
		it_iod++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
//
// FLOAT OPERATIONS
//
void test_constant_float_expression(
		zetscript::ScriptEngine *_script_engine
		,const char  *str_expr
		, zetscript::zs_float expected_value
) {
	try{
		zetscript::StackElement stk=_script_engine->eval(str_expr);
		zetscript::zs_float result=_script_engine->stackElementTo<zetscript::zs_float>(stk);
		if(result  != expected_value){
			throw std::runtime_error(zetscript::String::format("error test '%s' expected %f but it was %f!\n",str_expr,expected_value,result).toConstChar());
		}

	}catch(std::exception & ex){
		throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n",str_expr,ex.what()).toConstChar());
	}
}

void test_arithmetic_float_expression(
		zetscript::ScriptEngine *_script_engine
		,zetscript::zs_float expr
		, const char *str_expr
		, bool print_warnings
) {

	try {
		zetscript::StackElement stk=_script_engine->eval(zetscript::String("return ")+str_expr);
		zetscript::zs_float result = _script_engine->stackElementTo<zetscript::zs_float>(stk);
		if (!float_values_are_almost_the_same(result, expr)) {
			double error = fabs(fabs(result) - fabs(expr));
			if (error > 0.001) { /* Only error if the difference is more than expected */
				throw std::runtime_error(zetscript::String::format("error test '%s' expected %f but it was %f!\n", str_expr, expr, result).toConstChar());
			}else if(print_warnings) {
				fprintf(stderr, "warning: test '%s' expected %f but it was %f (it has some precision error)!\n", str_expr, expr, result);
			}
		}

	}
	catch (std::exception & ex) {
		throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", str_expr, ex.what()).toConstChar());
	}
}

#define INLINE_FLOAT_MOD_OPERATION(val1,val2) {zetscript::String::format(str_format,toString(val1).toConstChar(),"%",toString(val2).toConstChar()), (zetscript::zs_float)fmod(val1,val2)}
void _complete_test_arithmetic_float_op(
		zetscript::ScriptEngine *_script_engine
		,zetscript::zs_float val1
		, zetscript::zs_float val2
		, const char *str_format
) {
	struct _test_arithmetic_float_op_data {
		zetscript::String str;
		zetscript::zs_float val;
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
		zetscript::String str; zetscript::zs_float val;
	}test_arithmetic_float_mod_op_data[] = {
		INLINE_FLOAT_MOD_OPERATION(val1,val2)
		,INLINE_FLOAT_MOD_OPERATION(val1,-val2)
		,INLINE_FLOAT_MOD_OPERATION(-val1,val2)
		,INLINE_FLOAT_MOD_OPERATION(-val1,-val2)
		, { "",0 }
	};

	// process first part
	_test_arithmetic_float_op_data *it_af = test_arithmetic_float_op_data;
	while (!it_af->str.isEmpty()) {
		try {
			zetscript::StackElement stk = _script_engine->eval(it_af->str);
			zetscript::zs_float result=_script_engine->stackElementTo<zetscript::zs_float>(stk);

			if (!float_values_are_almost_the_same(result, it_af->val)) {
				throw std::runtime_error(zetscript::String::format( "error test '%s' expected %f but it was %f!\n", it_af->str.toConstChar(), it_af->val, result).toConstChar());
			}

		}
		catch (std::exception & ex) {
			throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", it_af->str.toConstChar(), ex.what()).toConstChar());
		}
		it_af++;
	}

	_test_arithmetic_float_mod_op_data *it_afm = test_arithmetic_float_mod_op_data;

	// process second part
	while (!it_afm->str.isEmpty()) {
		try {
			zetscript::StackElement stk = _script_engine->eval(it_afm->str);

			zetscript::zs_float result=_script_engine->stackElementTo<zetscript::zs_float>(stk);
			if (!float_values_are_almost_the_same(result, it_afm->val)) {
				throw std::runtime_error(zetscript::String::format("error test '%s' expected %f but it was %f!\n", it_afm->str.toConstChar(), it_afm->val, result).toConstChar()); \
			}
		}
		catch (std::exception & ex) {
			throw std::runtime_error(zetscript::String::format( "error test '%s' : %s!\n", it_afm->str.toConstChar(), ex.what()).toConstChar());
		}
		it_afm++;
	}
}

void _complete_test_arithmetic_float_op_assign(
		zetscript::ScriptEngine *_script_engine
		, zetscript::zs_float _val1
		, zetscript::zs_float _val2

		, const char *str_format
){
	zetscript::zs_float ref_val1=_val1;
	zetscript::zs_float val2=_val2;

	struct _test_arithmetic_float_op_data {
		zetscript::String str;
		zetscript::zs_float val;
	}test_arithmetic_float_op_data[] = {
		INLINE_OPERATION_ASSIGN(ref_val1,=,val2)
		,INLINE_OPERATION_ASSIGN(ref_val1,+=,val2*10)
		,INLINE_OPERATION_ASSIGN(ref_val1,-=,val2*5)
		,INLINE_OPERATION_ASSIGN(ref_val1,*=,val2*2)
		,INLINE_OPERATION_ASSIGN(ref_val1,/=,val2)
		// assign right -
		,INLINE_OPERATION_ASSIGN(ref_val1,=,-val2)
		,INLINE_OPERATION_ASSIGN(ref_val1,+=,-val2*10)
		,INLINE_OPERATION_ASSIGN(ref_val1,-=,-val2*5)
		,INLINE_OPERATION_ASSIGN(ref_val1,*=,-val2*2)
		,INLINE_OPERATION_ASSIGN(ref_val1,/=,-val2)
		, { "",0 }
	};

	struct _test_arithmetic_float_mod_op_data {
		zetscript::String str; zetscript::zs_float val;
	}test_arithmetic_float_mod_op_data[] = {
		INLINE_FLOAD_MOD_OPERATION_ASSIGN(ref_val1,val2*100)
		,INLINE_FLOAD_MOD_OPERATION_ASSIGN(ref_val1,-val2*100)
		, { "",0 }
	};

	// process
	_test_arithmetic_float_op_data *it_iod = test_arithmetic_float_op_data;
	while (!it_iod->str.isEmpty()) {
		try {
			zetscript::StackElement stk=_script_engine->eval(it_iod->str); \
			zetscript::zs_float result=_script_engine->stackElementTo<zetscript::zs_float>(stk);
			if (float_values_are_almost_the_same(result,it_iod->val)==false) {
				throw std::runtime_error(zetscript::String::format(
					"error test '%s' expected '%.02f' but it was '%.02f'!\n"
					, it_iod->str.toConstChar()
					, it_iod->val
					,ZS_READ_INTPTR_FLOAT(stk.value)).toConstChar()
				);
			}

		}
		catch (std::exception & ex) {
			throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", it_iod->str.toConstChar(), ex.what()).toConstChar());
		}
		it_iod++;
	}
}


//------------------------------------------------------------------------------------------------------------------------------------
//
// BOOL OPERATIONS
//
void test_constant_bool_expression(zetscript::ScriptEngine *_script_engine,const char *str_expr, bool expected_value){
	try{
		zetscript::StackElement stk = _script_engine->eval(str_expr);
		if(stk.properties & zetscript::STACK_ELEMENT_PROPERTY_BOOL){
			if((bool)stk.value  != (expected_value)){
				throw std::runtime_error(zetscript::String::format("error test '%s' expected %s but it was %s!\n", str_expr,expected_value?"true":"false",(bool)stk.value?"true":"false").toConstChar());
			}
		}else{
			throw std::runtime_error(
					zetscript::String::format(
							"error test '%s' expected bool type but it was '%s'!\n"
							,str_expr
							,_script_engine->stackElementToString(&stk).toConstChar()
					).toConstChar()
			);
		}
	}catch(std::exception & ex){
		throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", str_expr,ex.what()).toConstChar());
	}
}

void test_arithmetic_bool_expression(zetscript::ScriptEngine *_script_engine,bool expr, const char *str_expr){

	try{
		zetscript::StackElement stk = _script_engine->eval(zetscript::String("return ")+str_expr);
		if(stk.properties & zetscript::STACK_ELEMENT_PROPERTY_BOOL){
			if((bool)stk.value  != (expr)){
				throw std::runtime_error(zetscript::String::format("error test '%s' expected %s but it was %s!\n", str_expr,(expr)?"true":"false",(bool)stk.value?"true":"false").toConstChar());
			}
		}else{
			throw std::runtime_error(
					zetscript::String::format(
							"error test '%s' expected bool but it was '%s'!\n"
							,str_expr
							,_script_engine->stackElementToString(&stk).toConstChar()
					).toConstChar()
			);
		}
	}catch(std::exception & ex){
		throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", str_expr,ex.what()).toConstChar());
	}
}



//------------------------------------------------------------------------------------------------------------------------------------
//
// STRING OPERATIONS
//

void test_constant_string_expression(zetscript::ScriptEngine *_script_engine,const char * expected_value, const char *str_expr){


	try{
		zetscript::StackElement stk = _script_engine->eval(zetscript::String("return ")+str_expr);
		if(STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
			zetscript::StringScriptObject *so=(zetscript::StringScriptObject *)stk.value;
			if (so->toString() != zetscript::String(expected_value)) {
				throw std::runtime_error(zetscript::String::format("error test '%s' expected '%s' but it was '%s'!\n", str_expr, expected_value, so->toString().toConstChar()).toConstChar());
			}
		}else{
			throw std::runtime_error(
					zetscript::String::format(
							"error test '%s' expected 'StringScriptObject' but it was '%s'!\n"
							,str_expr
							,_script_engine->stackElementToString(&stk).toConstChar()
					).toConstChar()
			);
		}
	}catch(std::exception & ex){
		throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", str_expr,ex.what()).toConstChar());
	}
}

void test_arithmetitoConstCharing_expression(zetscript::ScriptEngine *_script_engine,const zetscript::String & expected_value, const char * str_expr){


	try{\
		zetscript::StackElement stk = _script_engine->eval(zetscript::String("return ")+str_expr,__FILE__,__LINE__);
		if(STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
			zetscript::StringScriptObject *so=(zetscript::StringScriptObject *)stk.value;
			if (so->toString() != zetscript::String(expected_value)) {
				fprintf(stderr,"error test '%s' expected %s but it was %s!\n",str_expr, expected_value.toConstChar(), so->toString().toConstChar()); \
				exit(-1); \
			} \
			// destroy lifetime object when is not needed
			_script_engine->unrefLifetimeObject(so);
		}else{
			throw std::runtime_error(
					zetscript::String::format(
							"error test '%s' expected 'StringScriptObject' but it was '%s'!\n"
							,str_expr,_script_engine->stackElementToString(&stk).toConstChar()
					).toConstChar()
			);

		}
	}catch(std::exception & ex){\
		throw std::runtime_error(zetscript::String::format("error test '%s' : %s!\n", str_expr,ex.what()).toConstChar()); \
	}\
}

