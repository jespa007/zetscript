/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#include "Float.h"

class Float;
Float *FloatWrap_new();
void FloatWrap_delete(Float *);
void FloatWrap_set(Float *_this,zs_float *_n);
void FloatWrap_set(Float *_this, Float * _n);
Float * FloatWrap_add(Float *n1, Float *n2);
Float * FloatWrap_add(Float *n1, zs_float *n2);
Float * FloatWrap_add(zs_float *n1, Float *n2);
Float * FloatWrap_sub(Float *n1, Float *n2);
Float * FloatWrap_sub(Float *n1, zs_float *n2);
Float * FloatWrap_sub(zs_float *n1, Float *n2);
Float * FloatWrap_neg(Float *n1);
Float * FloatWrap_div(Float *n1, Float *n2);
Float * FloatWrap_div(Float *n1, zs_float *n2);
Float * FloatWrap_div(zs_float *n1, Float *n2);
Float * FloatWrap_mod(Float *n1, Float *n2);
Float * FloatWrap_mod(Float *n1, zs_float *n2);
Float * FloatWrap_mod(zs_float *n1, Float *n2);
Float * FloatWrap_mul(Float *n1, Float *n2);
Float * FloatWrap_mul(Float *n1, zs_float *n2);
Float * FloatWrap_mul(zs_float *n1, Float *n2);
zs_float FloatWrap_toFloat(Float *_this);
