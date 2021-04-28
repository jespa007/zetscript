/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#include "Float.h"

class Float;
Float *FloatWrap_New();
void FloatWrap_Delete(Float *);
void FloatWrap_Set(Float *_this,zs_float *_n);
void FloatWrap_Set(Float *_this, Float * _n);
Float * FloatWrap_Add(Float *n1, Float *n2);
Float * FloatWrap_Add(Float *n1, zs_float *n2);
Float * FloatWrap_Add(zs_float *n1, Float *n2);
Float * FloatWrap_Sub(Float *n1, Float *n2);
Float * FloatWrap_Sub(Float *n1, zs_float *n2);
Float * FloatWrap_Sub(zs_float *n1, Float *n2);
Float * FloatWrap_Neg(Float *n1);
Float * FloatWrap_Div(Float *n1, Float *n2);
Float * FloatWrap_Div(Float *n1, zs_float *n2);
Float * FloatWrap_Div(zs_float *n1, Float *n2);
Float * FloatWrap_Mod(Float *n1, Float *n2);
Float * FloatWrap_Mod(Float *n1, zs_float *n2);
Float * FloatWrap_Mod(zs_float *n1, Float *n2);
Float * FloatWrap_Mul(Float *n1, Float *n2);
Float * FloatWrap_Mul(Float *n1, zs_float *n2);
Float * FloatWrap_Mul(zs_float *n1, Float *n2);
zs_float FloatWrap_ToFloat(Float *_this);
