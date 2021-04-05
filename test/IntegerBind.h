/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#include "Integer.h"

class Integer;
class IntegerBind{
public:

	static void _set(Integer *_this, zs_int _n);
	static void _set(Integer *_this,Integer *_ci2);
	static Integer * _add(Integer *n1, Integer *n2);
	static Integer * _add(Integer *n1, zs_float n2);
	static Integer * _add(Integer *n1, zs_int n2);
	static Integer * _add(zs_int n1, Integer * n2);
	static Integer * _sub(Integer *n1, Integer *n2);
	static Integer * _sub(Integer *n1, zs_float n2);
	static Integer * _sub(Integer *n1, zs_int n2);
	static Integer * _sub(zs_int n1, Integer * n2);
	static Integer * _div(Integer *n1, Integer *n2);
	static Integer * _div(Integer *n1, zs_int n2);
	static Integer * _div(zs_int n1, Integer *n2);
	static Integer * _neg(Integer *n1);
	static Integer * _mod(Integer *n1, Integer *n2);
	static Integer * _mod(Integer *n1, zs_int n2);
	static Integer * _mod(zs_int n1, Integer *n2);
	static Integer * _mul(Integer *n1, Integer *n2);
	static Integer * _mul(Integer *n1, zs_int n2);
	static Integer * _mul(zs_int n1, Integer * n2);
	static Integer * _shl(Integer *n1, Integer *n2);
	static Integer * _shl(Integer *n1, zs_int n2);
	static Integer * _shl(zs_int n1, Integer *n2);
	static Integer * _and(Integer *n1, Integer *n2);
	static Integer * _and(Integer *n1, zs_int n2);
	static Integer * _and(zs_int n1, Integer * n2);
	static Integer * _or(Integer *n1, Integer *n2);
	static Integer * _or(Integer *n1, zs_int n2);
	static Integer * _or(zs_int n1, Integer * n2);
	static Integer * _xor(Integer *n1, Integer *n2);
	static Integer * _xor(Integer *n1, zs_int n2);
	static Integer * _xor(zs_int n1, Integer * n2);
	static Integer * _shr(Integer *n1, Integer *n2);
	static Integer * _shr(Integer *n1, zs_int n2);
	static Integer * _shr(zs_int n1, Integer *n2);
	static zs_int toInt(Integer *_this);

};
