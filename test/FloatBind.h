/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#include "Float.h"

class Float;
class FloatBind{
public:

	static Float *_new();
	static void _delete(Float *);

	static void _set(Float *_this,zs_float *_n);
	static void _set(Float *_this, Float * _n);

	static Float * _add(Float *n1, Float *n2);
	static Float * _add(Float *n1, zs_float *n2);
	static Float * _add(zs_float *n1, Float *n2);
	static Float * _sub(Float *n1, Float *n2);
	static Float * _sub(Float *n1, zs_float *n2);
	static Float * _sub(zs_float *n1, Float *n2);
	static Float * _neg(Float *n1);
	static Float * _div(Float *n1, Float *n2);
	static Float * _div(Float *n1, zs_float *n2);
	static Float * _div(zs_float *n1, Float *n2);
	static Float * _mod(Float *n1, Float *n2);
	static Float * _mod(Float *n1, zs_float *n2);
	static Float * _mod(zs_float *n1, Float *n2);
	static Float * _mul(Float *n1, Float *n2);
	static Float * _mul(Float *n1, zs_float *n2);
	static Float * _mul(zs_float *n1, Float *n2);

	static zs_float toFloat(Float *_this);
};
