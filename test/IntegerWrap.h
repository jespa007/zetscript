/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#include "Integer.h"

class Integer;
Integer *IntegerWrap_New();
void IntegerWrap_Delete(Integer *_this);
void IntegerWrap_Set(Integer *_this, zs_int _n);
void IntegerWrap_Set(Integer *_this,Integer *_ci2);
Integer * IntegerWrap_Add(Integer *n1, Integer *n2);
Integer * IntegerWrap_Add(Integer *n1, zs_float n2);
Integer * IntegerWrap_Add(Integer *n1, zs_int n2);
Integer * IntegerWrap_Add(zs_int n1, Integer * n2);
Integer * IntegerWrap_Sub(Integer *n1, Integer *n2);
Integer * IntegerWrap_Sub(Integer *n1, zs_float n2);
Integer * IntegerWrap_Sub(Integer *n1, zs_int n2);
Integer * IntegerWrap_Sub(zs_int n1, Integer * n2);
Integer * IntegerWrap_Div(Integer *n1, Integer *n2);
Integer * IntegerWrap_Div(Integer *n1, zs_int n2);
Integer * IntegerWrap_Div(zs_int n1, Integer *n2);
Integer * IntegerWrap_Neg(Integer *n1);
Integer * IntegerWrap_Mod(Integer *n1, Integer *n2);
Integer * IntegerWrap_Mod(Integer *n1, zs_int n2);
Integer * IntegerWrap_Mod(zs_int n1, Integer *n2);
Integer * IntegerWrap_Mul(Integer *n1, Integer *n2);
Integer * IntegerWrap_Mul(Integer *n1, zs_int n2);
Integer * IntegerWrap_Mul(zs_int n1, Integer * n2);
Integer * IntegerWrap_Shl(Integer *n1, Integer *n2);
Integer * IntegerWrap_Shl(Integer *n1, zs_int n2);
Integer * IntegerWrap_Shl(zs_int n1, Integer *n2);
Integer * IntegerWrap_And(Integer *n1, Integer *n2);
Integer * IntegerWrap_And(Integer *n1, zs_int n2);
Integer * IntegerWrap_And(zs_int n1, Integer * n2);
Integer * IntegerWrap_Or(Integer *n1, Integer *n2);
Integer * IntegerWrap_Or(Integer *n1, zs_int n2);
Integer * IntegerWrap_Or(zs_int n1, Integer * n2);
Integer * IntegerWrap_Xor(Integer *n1, Integer *n2);
Integer * IntegerWrap_Xor(Integer *n1, zs_int n2);
Integer * IntegerWrap_Xor(zs_int n1, Integer * n2);
Integer * IntegerWrap_Shr(Integer *n1, Integer *n2);
Integer * IntegerWrap_Shr(Integer *n1, zs_int n2);
Integer * IntegerWrap_Shr(zs_int n1, Integer *n2);
zs_int IntegerWrap_ToInt(Integer *_this);

