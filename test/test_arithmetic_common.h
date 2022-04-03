/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

#pragma once

#define TEST_CONSTANT_INT_EXPRESSION(_zs,str_expr, expected_value) 		test_constant_int_expression(_zs,str_expr, expected_value)
#define TEST_CONSTANT_FLOAT_EXPRESSION(_zs,str_expr, expected_value) 	test_constant_float_expression(_zs,str_expr, expected_value)
#define TEST_CONSTANT_STRING_EXPRESSION(str_expr, expected_value) 		test_constant_string_expression(expected_value,str_expr)

#define TEST_ARITHMETIC_INT_EXPRESSION(_zs,expr) 						test_arithmetic_int_expression(_zs,expr,ZS_STR(expr))
#define TEST_ARITHMETIC_FLOAT_EXPRESSION(_zs,val1,print_warnings) 		test_arithmetic_float_expression(_zs,(zetscript::zs_float)(val1),ZS_STR(val1)";",print_warnings)
#define TEST_ARITHMETIC_BOOL_EXPRESSION(_zs,val1) 						test_arithmetic_bool_expression(_zs,val1,ZS_STR(val1))
#define TEST_ARITHMETIC_STRING_EXPRESSION(_zs,val1,op, val2)			test_arithmetic_string_expression(_zs,zetscript::zs_string(val1) op val2,ZS_STR(val1)ZS_STR(op)val2)
#define COMPLETE_TEST_ARITHMETIC_FLOAT_OP(_zs,val1,val2) 				_complete_test_arithmetic_float_op(_zs,val1, val2,"return %s%s%s")

zetscript::zs_string to_string(bool _b);
zetscript::zs_string to_string(zetscript::zs_int _i);
zetscript::zs_string to_string(zetscript::zs_float _f);
zetscript::zs_string to_string(const zetscript::zs_string & _s);

// Usable AlmostEqual function
bool float_values_are_almost_the_same(zetscript::zs_float A, zetscript::zs_float B, int maxUlps=8);

//------------------------------------------------------------------------------------------------------------------------------------
//
// INTEGER OPERATIONS
//
void test_arithmetic_int_expression(zetscript::ZetScript *_zs, zetscript::zs_int expr,const char *str_expr);
void test_constant_int_expression(zetscript::ZetScript *_zs, const char *str_expr, zetscript::zs_int expected_value);
void _complete_test_arithmetic_integer_op(zetscript::ZetScript *_zs,zetscript::zs_int val1, zetscript::zs_int val2, const char *str_format);
void _complete_test_arithmetic_integer_op_assign(zetscript::ZetScript *_zs, zetscript::zs_int *ref_val1, zetscript::zs_int val2, const char *str_format);
//------------------------------------------------------------------------------------------------------------------------------------
//
// FLOAT OPERATIONS
//
void test_constant_float_expression(zetscript::ZetScript *_zs,const char  *str_expr, zetscript::zs_float expected_value);
void test_arithmetic_float_expression(zetscript::ZetScript *_zs,zetscript::zs_float expr, const char *str_expr, bool print_warnings);
void _complete_test_arithmetic_float_op(zetscript::ZetScript *_zs,zetscript::zs_float val1, zetscript::zs_float val2, const char *str_format);


//------------------------------------------------------------------------------------------------------------------------------------
//
// BOOL OPERATIONS
//
void test_constant_bool_expression(zetscript::ZetScript *_zs,const char *str_expr, bool expected_value);
void test_arithmetic_bool_expression(zetscript::ZetScript *_zs,bool expr, const char *str_expr);


//------------------------------------------------------------------------------------------------------------------------------------
//
// STRING OPERATIONS
//

void test_constant_string_expression(zetscript::ZetScript *_zs,const char * expected_value, const char *str_expr);
void test_arithmetic_string_expression(zetscript::ZetScript *_zs,const zetscript::zs_string & expected_value, const char * str_expr);

