/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"
#include "Number.h"

using zetscript::ZetScript;
using zetscript::zs_float;
using zetscript::zs_int;
using zetscript::zs_string;
using zetscript::ClassScriptObject;


Number *NumberZs_new(ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new Number();
}

void NumberZs_delete(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}

//-------------------------------------------------------------------------------------
// CONSTRUCTORS
void NumberZs_constructor(ZetScript *_zs,Number *_this, zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=*_n;
}

void NumberZs_constructor(ZetScript *_zs,Number *_this,Number *_ci2){
	ZS_UNUSUED_PARAM(_zs);
	 _this->value = _ci2->value;
}
// CONSTRUCTORS
//-------------------------------------------------------------------------------------
// MEMBER FUNCTIONS
zs_int NumberZs_toInteger(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}

zs_float NumberZs_toFloat(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}

// MEMBER FUNCTIONS
//-------------------------------------------------------------------------------------
// STATIC MEMBER FUNCTIONS

zs_float NumberZs_pow(ZetScript *_zs,zs_float *_base, zs_float * _power){
	ZS_UNUSUED_PARAM(_zs);
	return (*_base) * (*_power);
}
// STATIC MEMBER FUNCTIONS
//-------------------------------------------------------------------------------------
// MEMBER PROPERTY METAMETHODS

zs_string NumberZs_tostring(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	char output[100];
	sprintf(output,"%0.2f",_this->value);
	return output;
}

void NumberZs_set(ZetScript *_zs,Number *_this, zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=*_n;
}

void NumberZs_set(ZetScript *_zs,Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	 _this->value = _n->value;
}


void NumberZs_addset(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=*_n;
}

void NumberZs_addset(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=_n->value;
}

void NumberZs_subset(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=*_n;
}

void NumberZs_subset(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=_n->value;
}

void NumberZs_mulset(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=*_n;
}

void NumberZs_mulset(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=_n->value;
}

void NumberZs_divset(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=*_n;
}

void NumberZs_divset(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=_n->value;
}

void NumberZs_modset(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,*_n);
}

void NumberZs_modset(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,_n->value);
}

void NumberZs_andset(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) & ((zs_int)*_n);
}

void NumberZs_andset(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) & ((zs_int)_n->value);
}

void NumberZs_orset(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) | ((zs_int)*_n);
}

void NumberZs_orset(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) | ((zs_int)_n->value);
}

void NumberZs_xorset(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) ^ ((zs_int)*_n);
}

void NumberZs_xorset(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) ^ ((zs_int)_n->value);
}

void NumberZs_shrset(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) >> ((zs_int)*_n);
}

void NumberZs_shrset(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) >> ((zs_int)_n->value);
}

void NumberZs_shlset(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) << ((zs_int)*_n);
}

void NumberZs_shlset(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) << ((zs_int)_n->value);
}

bool NumberZs_not(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value == 0;
}

ClassScriptObject * NumberZs_neg(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(-_this->value));
}

ClassScriptObject *  NumberZs_postinc(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	Number *number_before_increment=new Number(_this->value);
	_this->value++;
	return _zs->newClassScriptObject(number_before_increment);
}

ClassScriptObject *  NumberZs_postdec(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	Number *number_before_increment=new Number(_this->value);
	_this->value--;
	return _zs->newClassScriptObject(number_before_increment);
}

void  NumberZs_preinc(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->value;
}

void  NumberZs_predec(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	--_this->value;
}

// MEMBER PROPERTY METAMETHODS
//-------------------------------------------------------------------------------------
// STATIC MEMBER FUNCTION METAMETHODS

// _equ (a.k.a '==')
bool  NumberZs_equ(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value ==_n2->value;
}

bool  NumberZs_equ(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value == *_n2;
}

bool NumberZs_equ(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 == _n2->value;
}

// _neq (a.k.a '!=')
bool  NumberZs_nequ(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value !=_n2->value;
}

bool  NumberZs_nequ(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value != *_n2;
}

bool NumberZs_nequ(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 != _n2->value;
}

// _lt (a.k.a '<')
bool  NumberZs_lt(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value <_n2->value;
}

bool  NumberZs_lt(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value < *_n2;
}

bool NumberZs_lt(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 < _n2->value;
}

// _lte (a.k.a '<=')
bool  NumberZs_lte(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value <=_n2->value;
}

bool  NumberZs_lte(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value <= *_n2;
}

bool NumberZs_lte(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 <= _n2->value;
}

// _gt (a.k.a '>')
bool  NumberZs_gt(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value >_n2->value;
}

bool  NumberZs_gt(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value > *_n2;
}

bool NumberZs_gt(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 > _n2->value;
}
	
// _gte (a.k.a '>=')
bool  NumberZs_gte(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value >=_n2->value;
}

bool  NumberZs_gte(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value >= *_n2;
}

bool NumberZs_gte(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 >= _n2->value;
}

// _add (a.k.a '+')
ClassScriptObject * NumberZs_add(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value +_n2->value));
}

ClassScriptObject * NumberZs_add(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value + *_n2));
}

ClassScriptObject * NumberZs_add(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(*_n1 + _n2->value));
}

// _sub (a.k.a '-')
ClassScriptObject * NumberZs_sub(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value - _n2->value));
}

ClassScriptObject * NumberZs_sub(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value - *_n2));
}

ClassScriptObject * NumberZs_sub(ZetScript *_zs,zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(*_n1 - _n2->value));
}

// _mul (a.k.a '*')
ClassScriptObject * NumberZs_mul(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value * _n2->value));
}

ClassScriptObject * NumberZs_mul(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value * (*_n2)));
}

ClassScriptObject * NumberZs_mul(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(*_n1 * _n2->value));
}

// _div (a.k.a '/')
ClassScriptObject * NumberZs_div(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value / _n2->value));
}

ClassScriptObject * NumberZs_div(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value / *_n2));
}

ClassScriptObject * NumberZs_div(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(*_n1 / _n2->value));
}

// _mod (a.k.a '%')
ClassScriptObject * NumberZs_mod(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(fmod(_n1->value,_n2->value)));
}

ClassScriptObject * NumberZs_mod(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(fmod(_n1->value, *_n2)));
}

ClassScriptObject * NumberZs_mod(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(fmod(*_n1, _n2->value)));
}

// _and (a.k.a '&')
ClassScriptObject * NumberZs_and(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value & (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_and(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value & (zs_int)*_n2));
}

ClassScriptObject * NumberZs_and(ZetScript *_zs,zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)*_n1 & (zs_int)_n2->value));
}

// _or (a.k.a '|')
ClassScriptObject * NumberZs_or(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value | (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_or(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value | (zs_int)*_n2));
}

ClassScriptObject * NumberZs_or(ZetScript *_zs,zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)*_n1 | (zs_int)_n2->value));
}

// _xor (a.k.a '^')
ClassScriptObject * NumberZs_xor(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value ^ (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_xor(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value ^ (zs_int)*_n2));
}

ClassScriptObject * NumberZs_xor(ZetScript *_zs,zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)*_n1 ^ (zs_int)_n2->value));
}

// _shr (a.k.a '>>')
ClassScriptObject * NumberZs_shr(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value >> (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_shr(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value >> (zs_int)*_n2));
}

ClassScriptObject * NumberZs_shr(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)*_n1 >> (zs_int)_n2->value));
}

// _shl (a.k.a '<<')
ClassScriptObject * NumberZs_shl(ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value << (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_shl(ZetScript *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value << (zs_int)*_n2));
}

ClassScriptObject * NumberZs_shl(ZetScript *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)*_n1 << (zs_int)_n2->value));
}



//-------------------------------------------------------------------------------------
// MEMBER PROPERTY METAMETHODS

// getter
zs_float NumberZs_value_getter(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}

// setter
void NumberZs_value_setter(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=*_value;
}

// _addset (a.k.a '+=')
void NumberZs_value_addset(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=*_value;
}

void NumberZs_value_addset(ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=_value->value;
}

// _subset (a.k.a '-=')
void NumberZs_value_subset(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=*_value;
}

void NumberZs_value_subset(ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=_value->value;
}

// _mulset (a.k.a '*=')
void NumberZs_value_mulset(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=*_value;
}

void NumberZs_value_mulset(ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=_value->value;
}


// _divset (a.k.a '/=')
void NumberZs_value_divset(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=*_value;
}

void NumberZs_value_divset(ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=_value->value;
}

// _modset (a.k.a '%=')
void NumberZs_value_modset(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,*_value);
}

void NumberZs_value_modset(ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,_value->value);
}

// _andset (a.k.a '&=')
void NumberZs_value_andset(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)&(int)(*_value);
}

void NumberZs_value_andset(ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)&(int)(_value->value);
}

// _orset (a.k.a '|=')
void NumberZs_value_orset(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)|(int)(*_value);
}

void NumberZs_value_orset(ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)|(int)(_value->value);
}

// _xorset (a.k.a '^=')
void NumberZs_value_xorset(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)^(int)(*_value);
}

void NumberZs_value_xorset(ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)^(int)(_value->value);
}

// _shrset (a.k.a '>>=')
void NumberZs_value_shrset(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)>>(int)(*_value);
}

void NumberZs_value_shrset(ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)>>(int)(_value->value);
}

// _shlset (a.k.a '<<=')
void NumberZs_value_shlset(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)<<(int)(*_value);
}

void NumberZs_value_shlset(ZetScript *_zs,Number *_this,Number *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)<<(int)(_value->value);
}

// _neg (a.k.a '-a')
zs_float NumberZs_value_neg(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return -_this->value;
}

// _bwc (a.k.a '~a')
zs_float NumberZs_value_bwc(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return ~((zs_int)(_this->value));
}

// _postinc (a.k.a 'a++')
zs_float  NumberZs_value_postinc(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	zs_float number_before=_this->value;
	_this->value++;
	return number_before;
}

// _postdec (a.k.a 'a--')
zs_float  NumberZs_value_postdec(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	zs_float number_before=_this->value;
	_this->value--;
	return number_before;
}

// _preinc (a.k.a '++a')
void  NumberZs_value_preinc(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->value;
}

// _predec (a.k.a '--a')
void  NumberZs_value_predec(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	--_this->value;
}


// MEMBER PROPERTY METAMETHODS
//-------------------------------------------------------------------------------------
// CONST MEMBER PROPERTY

// getter
zs_float NumberZs_MAX_VALUE_getter(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}
// CONST MEMBER PROPERTY
//------------------------------------------------------------------------------------------



void NumberZs_register(ZetScript *_zs){

	_zs->registerType<Number>("Number",NumberZs_new,NumberZs_delete);

	//-------------------------------------------------------------------------------------
	// CONSTRUCTORS
	_zs->registerConstructor<Number>(static_cast<void (*)(ZetScript *_zs,Number *_this,zs_float *_value)>(&NumberZs_constructor));
	_zs->registerConstructor<Number>(static_cast<void (*)(ZetScript *_zs,Number *_this,Number *_value)>(&NumberZs_constructor));
	// CONSTRUCTORS
	//-------------------------------------------------------------------------------------
	// STATIC FUNCTIONS
	_zs->registerStaticMemberFunction<Number>("pow",&NumberZs_pow);
	// STATIC FUNCTIONS
	//-------------------------------------------------------------------------------------
	// MEMBER FUNCTIONS
	_zs->registerMemberFunction<Number>("toInteger",&NumberZs_toInteger);
	_zs->registerMemberFunction<Number>("toFloat",&NumberZs_toFloat);
	// MEMBER FUNCTIONS
	//-------------------------------------------------------------------------------------
	// MEMBER FUNCTION METAMETHODS

	// tostring
	_zs->registerMemberFunction<Number>("_tostring",&NumberZs_tostring);

	// setter
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(ZetScript *_zs,Number *, zs_float *)>(&NumberZs_set));
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(ZetScript *_zs,Number *,Number *)>(&NumberZs_set));

	// _addset  (a.k.a '+=')
	_zs->registerMemberFunction<Number>("_addset",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_addset));
	_zs->registerMemberFunction<Number>("_addset",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_addset));

	// _subset  (a.k.a '-=')
	_zs->registerMemberFunction<Number>("_subset",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_subset));
	_zs->registerMemberFunction<Number>("_subset",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_subset));

	// _mulset  (a.k.a '*=')
	_zs->registerMemberFunction<Number>("_mulset",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_mulset));
	_zs->registerMemberFunction<Number>("_mulset",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_mulset));

	// _divset  (a.k.a '/=')
	_zs->registerMemberFunction<Number>("_divset",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_divset));
	_zs->registerMemberFunction<Number>("_divset",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_divset));


	// _modset  (a.k.a '%=')
	_zs->registerMemberFunction<Number>("_modset",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_modset));
	_zs->registerMemberFunction<Number>("_modset",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_modset));

	// _andset  (a.k.a '&=')
	_zs->registerMemberFunction<Number>("_andset",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_andset));
	_zs->registerMemberFunction<Number>("_andset",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_andset));

	// _orset   (a.k.a '|=')
	_zs->registerMemberFunction<Number>("_orset",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_orset));
	_zs->registerMemberFunction<Number>("_orset",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_orset));

	// _xorset  (a.k.a '^=')
	_zs->registerMemberFunction<Number>("_xorset",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_xorset));
	_zs->registerMemberFunction<Number>("_xorset",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_xorset));

	// _shrset  (a.k.a '>>=')
	_zs->registerMemberFunction<Number>("_shrset",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_shrset));
	_zs->registerMemberFunction<Number>("_shrset",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_shrset));

	// _shlset  (a.k.a '<<=')
	_zs->registerMemberFunction<Number>("_shlset",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_shlset));
	_zs->registerMemberFunction<Number>("_shlset",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_shlset));

	// _neg (a.k.a '-a')
	_zs->registerMemberFunction<Number>("_neg",&NumberZs_neg);

	// _not (a.k.a '!a')
	_zs->registerMemberFunction<Number>("_not",&NumberZs_not);

	// _postinc (a.k.a 'a++')
	_zs->registerMemberFunction<Number>("_postinc",NumberZs_postinc);

	// _postdec (a.k.a 'a--')
	_zs->registerMemberFunction<Number>("_postdec",NumberZs_postdec);

	// _preinc (a.k.a '++a')
	_zs->registerMemberFunction<Number>("_preinc",NumberZs_preinc);

	// _predec (a.k.a '--a')
	_zs->registerMemberFunction<Number>("_predec",NumberZs_predec);


	// MEMBER FUNCTION METAMETHODS
	//-------------------------------------------------------------------------------------
	// STATIC MEMBER FUNCTION METAMETHODS

	// _equ   (a.k.a '==')
	_zs->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_equ));
	_zs->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_equ));
	_zs->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_equ));

	// _nequ  (a.k.a '!=')
	_zs->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_nequ));
	_zs->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_nequ));
	_zs->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_nequ));

	// _lt  (a.k.a '<')
	_zs->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_lt));
	_zs->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_lt));
	_zs->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_lt));

	// _lte  (a.k.a '<=')
	_zs->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_lte));
	_zs->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_lte));
	_zs->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_lte));

	// _gt  (a.k.a '>')
	_zs->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_gt));
	_zs->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_gt));
	_zs->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_gt));

	// _gte  (a.k.a '>=')
	_zs->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_gte));
	_zs->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_gte));
	_zs->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_gte));

	// _add  (a.k.a '+')
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<ClassScriptObject * (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_add));

	// _sub  (a.k.a '-')
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<ClassScriptObject * (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_sub));

	// _mul  (a.k.a '*')
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<ClassScriptObject * (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_mul));

	// _div  (a.k.a '/')
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<ClassScriptObject * (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_div));

	// _mod  (a.k.a '%')
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<ClassScriptObject * (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_mod));

	// _shr  (a.k.a '>>')
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<ClassScriptObject * (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,zs_float * )>(&NumberZs_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_shr));

	// _shl  (a.k.a '<<')
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<ClassScriptObject * (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_shl));

	// _and  (a.k.a '&')
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<ClassScriptObject * (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_and));

	// _or  (a.k.a '|')
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<ClassScriptObject * (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_or));

	// _xor  (a.k.a '^')
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<ClassScriptObject * (*)(ZetScript *_zs,zs_float *,Number * )>(&NumberZs_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<ClassScriptObject * (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_xor));


	// STATIC MEMBER FUNCTIONS METAMETHODS
	//-------------------------------------------------------------------------------------
	// MEMBER PROPERTY METAMETHODS

	// _setter
	_zs->registerMemberPropertyMetamethod<Number>("value","_set",NumberZs_value_setter);
	
	// _getter
	_zs->registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_value_getter);
	
	// _neg (a.k.a '-a')
	_zs->registerMemberPropertyMetamethod<Number>("value","_neg",NumberZs_value_neg);
	
	// _bwc (a.k.a '~a')
	_zs->registerMemberPropertyMetamethod<Number>("value","_bwc",NumberZs_value_bwc);
	
	// _addset (a.k.a '+=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_addset",static_cast<void (*)(ZetScript *_zs,Number *_this, zs_float *_value)>(&NumberZs_value_addset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_addset",static_cast<void (*)(ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_addset));
	
	// _subset (a.k.a '-=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_subset",static_cast<void (*)(ZetScript *_zs,Number *_this, zs_float *_value)>(&NumberZs_value_subset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_subset",static_cast<void (*)(ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_subset));
	
	// _mulset (a.k.a '*=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_mulset",static_cast<void (*)(ZetScript *_zs,Number *_this, zs_float *_value)>(&NumberZs_value_mulset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_mulset",static_cast<void (*)(ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_mulset));
	
	// _divset (a.k.a '/=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_divset",static_cast<void (*)(ZetScript *_zs,Number *_this, zs_float *_value)>(&NumberZs_value_divset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_divset",static_cast<void (*)(ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_divset));
	
	// _modset (a.k.a '%=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_modset",static_cast<void (*)(ZetScript *_zs,Number *_this, zs_float *_value)>(&NumberZs_value_modset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_modset",static_cast<void (*)(ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_modset));
	
	// _xorset (a.k.a '^=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_xorset",static_cast<void (*)(ZetScript *_zs,Number *_this, zs_float *_value)>(&NumberZs_value_xorset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_xorset",static_cast<void (*)(ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_xorset));
	
	// _orset  (a.k.a '|=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_orset",static_cast<void (*)(ZetScript *_zs,Number *_this, zs_float *_value)>(&NumberZs_value_orset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_orset",static_cast<void (*)(ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_orset));
	
	// _andset  (a.k.a '&=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_andset",static_cast<void (*)(ZetScript *_zs,Number *_this, zs_float *_value)>(&NumberZs_value_andset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_andset",static_cast<void (*)(ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_andset));

	// _shrset  (a.k.a '>>=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_shrset",static_cast<void (*)(ZetScript *_zs,Number *_this, zs_float *_value)>(&NumberZs_value_shrset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_shrset",static_cast<void (*)(ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_shrset));

	// _shlset  (a.k.a '<<=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_shlset",static_cast<void (*)(ZetScript *_zs,Number *_this, zs_float *_value)>(&NumberZs_value_shlset));
	_zs->registerMemberPropertyMetamethod<Number>("value","_shlset",static_cast<void (*)(ZetScript *_zs,Number *_this, Number *_value)>(&NumberZs_value_shlset));

	// _postinc  (a.k.a 'a++')
	_zs->registerMemberPropertyMetamethod<Number>("value","_postinc",NumberZs_value_postinc);

	// _postdec  (a.k.a 'a--')
	_zs->registerMemberPropertyMetamethod<Number>("value","_postdec",NumberZs_value_postdec);

	// _preinc  (a.k.a '++a')
	_zs->registerMemberPropertyMetamethod<Number>("value","_preinc",NumberZs_value_preinc);

	// _predec  (a.k.a '--a')
	_zs->registerMemberPropertyMetamethod<Number>("value","_predec",NumberZs_value_predec);

	// MEMBER PROPERTY METAMETHODS
	//-------------------------------------------------------------------------------------
	// CONST MEMBER PROPERTIES
	_zs->registerConstMemberProperty<Number>("MAX_VALUE",NumberZs_MAX_VALUE_getter);

	// CONST MEMBER PROPERTIES
	//-------------------------------------------------------------------------------------


}


