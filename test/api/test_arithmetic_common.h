/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

#pragma once

#define TEST_CONSTANT_BOOL_EXPRESSION(_se,str_expr, expected_value) 	test_constant_bool_expression(_se,str_expr, expected_value)
#define TEST_CONSTANT_INT_EXPRESSION(_se,str_expr, expected_value) 		test_constant_int_expression(_se,str_expr, expected_value)
#define TEST_CONSTANT_FLOAT_EXPRESSION(_se,str_expr, expected_value) 	test_constant_float_expression(_se,str_expr, expected_value)
#define TEST_CONSTANT_STRING_EXPRESSION(str_expr, expected_value) 		test_constant_string_expression(expected_value,str_expr)

#define TEST_ARITHMETIC_INT_EXPRESSION(_se,expr) 						test_arithmetic_int_expression(_se,expr,ZS_STR(expr))
#define TEST_ARITHMETIC_FLOAT_EXPRESSION(_se,val1,print_warnings) 		test_arithmetic_float_expression(_se,(zetscript::zs_float)(val1),ZS_STR(val1)";",print_warnings)
#define TEST_ARITHMETIC_BOOL_EXPRESSION(_se,val1) 						test_arithmetic_bool_expression(_se,val1,ZS_STR(val1))
#define TEST_ARITHMETIC_ASSIGNRING_EXPRESSION(_se,val1,op, val2)			test_arithmetitoConstCharing_expression(_se,zetscript::String(val1) op val2,ZS_STR(val1)ZS_STR(op)val2)
#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_se,val1,val2) 				_complete_test_arithmetic_float_op(_se,val1, val2,"return %s%s%s")

zetscript::String toString(bool _b);
zetscript::String toString(zetscript::zs_int _i);
zetscript::String toString(zetscript::zs_float _f);
zetscript::String toString(const zetscript::String & _s);

// Usable AlmostEqual function
bool float_values_are_almost_the_same(zetscript::zs_float A, zetscript::zs_float B);

//------------------------------------------------------------------------------------------------------------------------------------
//
// INTEGER OPERATIONS
//
void test_arithmetic_int_expression(
	zetscript::ScriptEngine *_se
	, zetscript::zs_int expr
	,const char *str_expr
);

void test_constant_int_expression(zetscript::ScriptEngine *_se, const char *str_expr, zetscript::zs_int expected_value);
void _complete_test_arithmetic_integer_op(
	zetscript::ScriptEngine *_se
	,zetscript::zs_int val1
	, zetscript::zs_int val2
	, const char *str_format
);

void _complete_test_arithmetic_integer_op_assign(
	zetscript::ScriptEngine *_se
	, zetscript::zs_int _val1
	, zetscript::zs_int _val2
	, const char *_str_format
);

void _complete_test_arithmetic_self_op(
		zetscript::ScriptEngine *_se
		, zetscript::zs_int val1
		, const char *str_format
);

void _complete_test_arithmetic_self_property_op(
		zetscript::ScriptEngine *_se
		, zetscript::zs_int _start_value
		, const char *str_format
);

//------------------------------------------------------------------------------------------------------------------------------------
//
// FLOAT OPERATIONS
//
void test_constant_float_expression(
	zetscript::ScriptEngine *_se
	,const char  *str_expr
	, zetscript::zs_float expected_value
);

void test_arithmetic_float_expression(
	zetscript::ScriptEngine *_se
	,zetscript::zs_float expr
	, const char *str_expr
	, bool print_warnings
);

void _complete_test_arithmetic_float_op(
	zetscript::ScriptEngine *_se
	,zetscript::zs_float val1
	, zetscript::zs_float val2
	, const char *str_format
);

void _complete_test_arithmetic_float_op_assign(
	zetscript::ScriptEngine *_se
	, zetscript::zs_float _val1
	, zetscript::zs_float _val2
	, const char *_str_format
);

//------------------------------------------------------------------------------------------------------------------------------------
//
// BOOL OPERATIONS
//
void test_constant_bool_expression(zetscript::ScriptEngine *_se,const char *str_expr, bool expected_value);
void test_arithmetic_bool_expression(zetscript::ScriptEngine *_se,bool expr, const char *str_expr);


//------------------------------------------------------------------------------------------------------------------------------------
//
// STRING OPERATIONS
//

void test_constant_string_expression(zetscript::ScriptEngine *_se,const char * expected_value, const char *str_expr);
void test_arithmetitoConstCharing_expression(zetscript::ScriptEngine *_se,const zetscript::String & expected_value, const char * str_expr);

