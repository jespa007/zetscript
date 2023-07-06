/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"
#include "Number.h"

using zetscript::ZetScript;
using zetscript::zs_float;
using zetscript::zs_string;


Number *NumberZs_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new Number();
}

void NumberZs_set(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=*_n;
}

void NumberZs_set(zetscript::ZetScript *_zs,Number *_this,Number *_ci2){
	ZS_UNUSUED_PARAM(_zs);
	 _this->value = _ci2->value;
}

//--------------------------
// PROPERTY

zetscript::zs_float NumberZs_value_getter(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}

zetscript::zs_float NumberZs_value_neg(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return -_this->value;
}

zetscript::zs_float NumberZs_value_bwc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return ~((zetscript::zs_int)(_this->value));
}

void NumberZs_value_setter(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=*_value;
}

void  NumberZs_value_postinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->value++;
}

void  NumberZs_value_postdec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->value--;
}

void  NumberZs_value_preinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->value;
}

void  NumberZs_value_predec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	--_this->value;
}

void NumberZs_value_addset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=*_value;
}

void NumberZs_value_addset(zetscript::ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=_value->value;
}

void NumberZs_value_subset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=*_value;
}

void NumberZs_value_subset(zetscript::ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=_value->value;
}

void NumberZs_value_mulset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=*_value;
}

void NumberZs_value_mulset(zetscript::ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=_value->value;
}

void NumberZs_value_divset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=*_value;
}

void NumberZs_value_divset(zetscript::ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=_value->value;
}

void NumberZs_value_modset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,*_value);
}

void NumberZs_value_modset(zetscript::ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,_value->value);
}

void NumberZs_value_xorset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)^(int)(*_value);
}

void NumberZs_value_xorset(zetscript::ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)^(int)(_value->value);
}


void NumberZs_value_orset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)|(int)(*_value);
}

void NumberZs_value_orset(zetscript::ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)|(int)(_value->value);
}

void NumberZs_value_andset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)&(int)(*_value);
}

void NumberZs_value_andset(zetscript::ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)&(int)(_value->value);
}

void NumberZs_value_shlset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)<<(int)(*_value);
}

void NumberZs_value_shlset(zetscript::ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)<<(int)(_value->value);
}

void NumberZs_value_shrset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)>>(int)(*_value);
}

void NumberZs_value_shrset(zetscript::ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)>>(int)(_value->value);
}
//--------------------------
// METAMETHODS
bool  NumberZs_equ(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value ==_n2->value;
}

bool  NumberZs_equ(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value == *_n2;
}

bool NumberZs_equ(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 == _n2->value;
}

bool  NumberZs_nequ(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value !=_n2->value;
}

bool  NumberZs_nequ(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value != *_n2;
}

bool NumberZs_nequ(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 != _n2->value;
}

bool  NumberZs_lt(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value <_n2->value;
}

bool  NumberZs_lt(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value < *_n2;
}

bool NumberZs_lt(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 < _n2->value;
}

bool  NumberZs_lte(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value <=_n2->value;
}

bool  NumberZs_lte(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value <= *_n2;
}

bool NumberZs_lte(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 <= _n2->value;
}

bool  NumberZs_gt(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value >_n2->value;
}

bool  NumberZs_gt(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value > *_n2;
}

bool NumberZs_gt(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 > _n2->value;
}
	
bool  NumberZs_gte(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value >=_n2->value;
}

bool  NumberZs_gte(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value >= *_n2;
}

bool NumberZs_gte(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 >= _n2->value;
}

Number * NumberZs_add(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->value +_n2->value);
}

Number * NumberZs_add(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->value + *_n2);
}

Number * NumberZs_add(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 + _n2->value);
}

void NumberZs_addset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=*_n;
}

void NumberZs_addset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=_n->value;
}

Number * NumberZs_sub(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->value - _n2->value);
}

Number * NumberZs_sub(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->value - *_n2);
}

Number * NumberZs_sub(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 - _n2->value);
}

void NumberZs_subset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=*_n;
}

void NumberZs_subset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=_n->value;
}

Number * NumberZs_div(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->value / _n2->value);
}

Number * NumberZs_div(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->value / *_n2);
}

Number * NumberZs_div(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 / _n2->value);
}

void NumberZs_divset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=*_n;
}

void NumberZs_divset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=_n->value;
}

Number * NumberZs_neg(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(-_this->value);
}

bool NumberZs_not(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value == 0;
}

Number * NumberZs_mod(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(_n1->value,_n2->value));
}

Number * NumberZs_mod(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(_n1->value, *_n2));
}

Number * NumberZs_mod(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(*_n1, _n2->value));
}

void NumberZs_modset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,*_n);
}

void NumberZs_modset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,_n->value);
}

Number * NumberZs_mul(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->value * _n2->value);
}

Number * NumberZs_mul(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->value * (*_n2));
}

Number * NumberZs_mul(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 * _n2->value);
}

void NumberZs_mulset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=*_n;
}

void NumberZs_mulset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=_n->value;
}

Number * NumberZs_shl(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->value << (zetscript::zs_int)_n2->value);
}

Number * NumberZs_shl(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->value << (zetscript::zs_int)*_n2);
}

Number * NumberZs_shl(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 << (zetscript::zs_int)_n2->value);
}

void NumberZs_shlset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zetscript::zs_int)_this->value) << ((zetscript::zs_int)*_n);
}

void NumberZs_shlset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zetscript::zs_int)_this->value) << ((zetscript::zs_int)_n->value);
}

Number * NumberZs_and(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->value & (zetscript::zs_int)_n2->value);
}

Number * NumberZs_and(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->value & (zetscript::zs_int)*_n2);
}

Number * NumberZs_and(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 & (zetscript::zs_int)_n2->value);
}

void NumberZs_andset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zetscript::zs_int)_this->value) & ((zetscript::zs_int)*_n);
}

void NumberZs_andset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zetscript::zs_int)_this->value) & ((zetscript::zs_int)_n->value);
}

Number * NumberZs_or(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->value | (zetscript::zs_int)_n2->value);
}

Number * NumberZs_or(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->value | (zetscript::zs_int)*_n2);
}

Number * NumberZs_or(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 | (zetscript::zs_int)_n2->value);
}

void NumberZs_orset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zetscript::zs_int)_this->value) | ((zetscript::zs_int)*_n);
}

void NumberZs_orset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zetscript::zs_int)_this->value) | ((zetscript::zs_int)_n->value);
}

Number * NumberZs_xor(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->value ^ (zetscript::zs_int)_n2->value);
}

Number * NumberZs_xor(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->value ^ (zetscript::zs_int)*_n2);
}

Number * NumberZs_xor(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 ^ (zetscript::zs_int)_n2->value);
}

void NumberZs_xorset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zetscript::zs_int)_this->value) ^ ((zetscript::zs_int)*_n);
}

void NumberZs_xorset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zetscript::zs_int)_this->value) ^ ((zetscript::zs_int)_n->value);
}

Number * NumberZs_shr(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->value >> (zetscript::zs_int)_n2->value);
}

Number * NumberZs_shr(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->value >> (zetscript::zs_int)*_n2);
}

Number * NumberZs_shr(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 >> (zetscript::zs_int)_n2->value);
}

void NumberZs_shrset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zetscript::zs_int)_this->value) >> ((zetscript::zs_int)*_n);
}

void NumberZs_shrset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zetscript::zs_int)_this->value) >> ((zetscript::zs_int)_n->value);
}


void  NumberZs_postinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->value++;
}

void  NumberZs_postdec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->value--;
}

void  NumberZs_preinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->value;
}

void  NumberZs_predec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	--_this->value;
}


// METAMETHODS
//--------------------------------

zetscript::zs_int NumberZs_toInt(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}

zetscript::zs_float NumberZs_toFloat(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}

zetscript::zs_float NumberZs_pow(zetscript::ZetScript *_zs,zs_float *_n1, zs_float * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return (*_n1) * (*_n2);
}


void NumberZs_delete(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}


void NumberZs_register(zetscript::ZetScript *_zs){

	_zs->registerType<Number>("Number",NumberZs_new,NumberZs_delete);
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_set));
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberZs_set));
	_zs->registerMemberFunction<Number>("toInt",&NumberZs_toInt);
	_zs->registerMemberFunction<Number>("toFloat",&NumberZs_toFloat);

	_zs->registerStaticMemberFunction<Number>("pow",&NumberZs_pow);


	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *, zetscript::zs_float *)>(&NumberZs_set));
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberZs_set));

	_zs->registerMemberPropertyMetamethod<Number>("value","_set",NumberZs_value_setter);
	_zs->registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_value_getter);
	_zs->registerMemberPropertyMetamethod<Number>("value","_neg",NumberZs_value_neg);
	_zs->registerMemberPropertyMetamethod<Number>("value","_bwc",NumberZs_value_bwc);
	_zs->registerMemberPropertyMetamethod<Number>("value","_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_value)>(&NumberZs_value_addset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_addset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_value)>(&NumberZs_value_subset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_subset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_value)>(&NumberZs_value_mulset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_mulset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_value)>(&NumberZs_value_divset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_divset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_value)>(&NumberZs_value_modset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_modset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_value)>(&NumberZs_value_xorset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_xorset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_value)>(&NumberZs_value_orset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_orset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_value)>(&NumberZs_value_andset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_andset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_value)>(&NumberZs_value_shrset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_shrset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_value)>(&NumberZs_value_shlset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_shlset));

	_zs->registerMemberPropertyMetamethod<Number>("value","_postinc",NumberZs_value_postinc);
	_zs->registerMemberPropertyMetamethod<Number>("value","_postdec",NumberZs_value_postdec);

	_zs->registerMemberPropertyMetamethod<Number>("value","_preinc",NumberZs_value_preinc);
	_zs->registerMemberPropertyMetamethod<Number>("value","_predec",NumberZs_value_predec);

	// logic
	_zs->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_equ));
	_zs->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_equ));
	_zs->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_equ));

	_zs->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_nequ));
	_zs->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_nequ));
	_zs->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_nequ));

	_zs->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_lt));
	_zs->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_lt));
	_zs->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_lt));

	_zs->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_lte));
	_zs->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_lte));
	_zs->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_lte));

	_zs->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_gt));
	_zs->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_gt));
	_zs->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_gt));

	_zs->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_gte));
	_zs->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_gte));
	_zs->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_gte));

	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_add));

	// add set
	_zs->registerMemberFunction<Number>("_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_addset));
	_zs->registerMemberFunction<Number>("_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_addset));


	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_sub));

	// sub set
	_zs->registerMemberFunction<Number>("_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_subset));
	_zs->registerMemberFunction<Number>("_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_subset));

	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_mul));

	// mul set
	_zs->registerMemberFunction<Number>("_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_mulset));
	_zs->registerMemberFunction<Number>("_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_mulset));

	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_div));

	// div set
	_zs->registerMemberFunction<Number>("_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_divset));
	_zs->registerMemberFunction<Number>("_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_divset));

	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_mod));

	// mod set
	_zs->registerMemberFunction<Number>("_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_modset));
	_zs->registerMemberFunction<Number>("_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_modset));


	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float * )>(&NumberZs_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_shr));

	// shr set
	_zs->registerMemberFunction<Number>("_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_shrset));
	_zs->registerMemberFunction<Number>("_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_shrset));


	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_shl));


	// shl set
	_zs->registerMemberFunction<Number>("_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_shlset));
	_zs->registerMemberFunction<Number>("_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_shlset));

	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_and));

	// and set
	_zs->registerMemberFunction<Number>("_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_andset));
	_zs->registerMemberFunction<Number>("_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_andset));


	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_or));

	// or set
	_zs->registerMemberFunction<Number>("_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_orset));
	_zs->registerMemberFunction<Number>("_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_orset));


	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_xor));

	// xor set
	_zs->registerMemberFunction<Number>("_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_xorset));
	_zs->registerMemberFunction<Number>("_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_xorset));


	_zs->registerMemberFunction<Number>("_neg",&NumberZs_neg);
	_zs->registerMemberFunction<Number>("_not",&NumberZs_not);

	_zs->registerMemberFunction<Number>("_postinc",NumberZs_postinc);
	_zs->registerMemberFunction<Number>("_postdec",NumberZs_postdec);

	_zs->registerMemberFunction<Number>("_preinc",NumberZs_preinc);
	_zs->registerMemberFunction<Number>("_predec",NumberZs_predec);
}


