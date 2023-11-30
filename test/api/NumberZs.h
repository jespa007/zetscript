/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"
#include "Number.h"

using zetscript::ZetScript;
using zetscript::zs_float;
using zetscript::zs_int;
using zetscript::String;
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

String NumberZs_tostring(ZetScript *_zs,Number *_this){
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


void NumberZs_addassign(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=*_n;
}

void NumberZs_addassign(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=_n->value;
}

void NumberZs_subassign(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=*_n;
}

void NumberZs_subassign(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=_n->value;
}

void NumberZs_mulassign(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=*_n;
}

void NumberZs_mulassign(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=_n->value;
}

void NumberZs_divassign(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=*_n;
}

void NumberZs_divassign(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=_n->value;
}

void NumberZs_modassign(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,*_n);
}

void NumberZs_modassign(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,_n->value);
}

void NumberZs_andassign(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) & ((zs_int)*_n);
}

void NumberZs_andassign(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) & ((zs_int)_n->value);
}

void NumberZs_orassign(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) | ((zs_int)*_n);
}

void NumberZs_orassign(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) | ((zs_int)_n->value);
}

void NumberZs_xorassign(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) ^ ((zs_int)*_n);
}

void NumberZs_xorassign(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) ^ ((zs_int)_n->value);
}

void NumberZs_shrassign(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) >> ((zs_int)*_n);
}

void NumberZs_shrassign(ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) >> ((zs_int)_n->value);
}

void NumberZs_shlassign(ZetScript *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) << ((zs_int)*_n);
}

void NumberZs_shlassign(ZetScript *_zs, Number *_this,Number *_n){
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
	return _zs->newClassScriptObject(new Number(_this->value++));
}

ClassScriptObject *  NumberZs_postdec(ZetScript *_zs,Number *_this){
	return  _zs->newClassScriptObject(new Number(_this->value--));
}

ClassScriptObject *   NumberZs_preinc(ZetScript *_zs,Number *_this){
	return _zs->newClassScriptObject(new Number(++_this->value));
}

ClassScriptObject *   NumberZs_predec(ZetScript *_zs,Number *_this){
	return _zs->newClassScriptObject(new Number(--_this->value));
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

// get
zs_float NumberZs_value_get(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}

// set
void NumberZs_value_set(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=*_value;
}

// _addassign (a.k.a '+=')
void NumberZs_value_addassign(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=*_value;
}


// _subassign (a.k.a '-=')
void NumberZs_value_subassign(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=*_value;
}

// _mulassign (a.k.a '*=')
void NumberZs_value_mulassign(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=*_value;
}

// _divassign (a.k.a '/=')
void NumberZs_value_divassign(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=*_value;
}

// _modassign (a.k.a '%=')
void NumberZs_value_modassign(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,*_value);
}

// _andassign (a.k.a '&=')
void NumberZs_value_andassign(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)&(int)(*_value);
}

// _orassign (a.k.a '|=')
void NumberZs_value_orassign(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)|(int)(*_value);
}

// _xorassign (a.k.a '^=')
void NumberZs_value_xorassign(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)^(int)(*_value);
}

// _shrassign (a.k.a '>>=')
void NumberZs_value_shrassign(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)>>(int)(*_value);
}

// _shlassign (a.k.a '<<=')
void NumberZs_value_shlassign(ZetScript *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)<<(int)(*_value);
}

// _postinc (a.k.a 'a++')
zs_float  NumberZs_value_postinc(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value++;
}

// _postdec (a.k.a 'a--')
zs_float  NumberZs_value_postdec(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value--;
}

// _preinc (a.k.a '++a')
zs_float  NumberZs_value_preinc(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return ++_this->value;
}

// _predec (a.k.a '--a')
zs_float  NumberZs_value_predec(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return --_this->value;
}


// MEMBER PROPERTY METAMETHODS
//-------------------------------------------------------------------------------------
// CONST MEMBER PROPERTY

// get
zs_float NumberZs_MAX_VALUE_get(ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}
// CONST MEMBER PROPERTY
//------------------------------------------------------------------------------------------



void NumberZs_register(ZetScript *_zs){

	_zs->registerScriptType<Number>("Number",NumberZs_new,NumberZs_delete);

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

	// set
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(ZetScript *_zs,Number *, zs_float *)>(&NumberZs_set));
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(ZetScript *_zs,Number *,Number *)>(&NumberZs_set));

	// _addassign  (a.k.a '+=')
	_zs->registerMemberFunction<Number>("_addassign",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_addassign));
	_zs->registerMemberFunction<Number>("_addassign",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_addassign));

	// _subassign  (a.k.a '-=')
	_zs->registerMemberFunction<Number>("_subassign",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_subassign));
	_zs->registerMemberFunction<Number>("_subassign",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_subassign));

	// _mulassign  (a.k.a '*=')
	_zs->registerMemberFunction<Number>("_mulassign",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_mulassign));
	_zs->registerMemberFunction<Number>("_mulassign",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_mulassign));

	// _divassign  (a.k.a '/=')
	_zs->registerMemberFunction<Number>("_divassign",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_divassign));
	_zs->registerMemberFunction<Number>("_divassign",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_divassign));


	// _modassign  (a.k.a '%=')
	_zs->registerMemberFunction<Number>("_modassign",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_modassign));
	_zs->registerMemberFunction<Number>("_modassign",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_modassign));

	// _andassign  (a.k.a '&=')
	_zs->registerMemberFunction<Number>("_andassign",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_andassign));
	_zs->registerMemberFunction<Number>("_andassign",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_andassign));

	// _orassign   (a.k.a '|=')
	_zs->registerMemberFunction<Number>("_orassign",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_orassign));
	_zs->registerMemberFunction<Number>("_orassign",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_orassign));

	// _xorassign  (a.k.a '^=')
	_zs->registerMemberFunction<Number>("_xorassign",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_xorassign));
	_zs->registerMemberFunction<Number>("_xorassign",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_xorassign));

	// _shrassign  (a.k.a '>>=')
	_zs->registerMemberFunction<Number>("_shrassign",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_shrassign));
	_zs->registerMemberFunction<Number>("_shrassign",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_shrassign));

	// _shlassign  (a.k.a '<<=')
	_zs->registerMemberFunction<Number>("_shlassign",static_cast<void (*)(ZetScript *_zs,Number *,zs_float *)>(&NumberZs_shlassign));
	_zs->registerMemberFunction<Number>("_shlassign",static_cast<void (*)(ZetScript *_zs,Number *,Number * )>(&NumberZs_shlassign));

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

	// _set
	_zs->registerMemberPropertyMetamethod<Number>("value","_set",NumberZs_value_set);
	
	// _get
	_zs->registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_value_get);
	
	// _addassign (a.k.a '+=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_addassign",NumberZs_value_addassign);
	
	// _subassign (a.k.a '-=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_subassign",NumberZs_value_subassign);

	// _mulassign (a.k.a '*=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_mulassign",NumberZs_value_mulassign);
	
	// _divassign (a.k.a '/=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_divassign",NumberZs_value_divassign);
	
	// _modassign (a.k.a '%=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_modassign",NumberZs_value_modassign);
	
	// _andassign  (a.k.a '&=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_andassign",NumberZs_value_andassign);

	// _orassign  (a.k.a '|=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_orassign",NumberZs_value_orassign);

	// _xorassign (a.k.a '^=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_xorassign",NumberZs_value_xorassign);

	// _shrassign  (a.k.a '>>=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_shrassign",NumberZs_value_shrassign);

	// _shlassign  (a.k.a '<<=')
	_zs->registerMemberPropertyMetamethod<Number>("value","_shlassign",NumberZs_value_shlassign);

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
	_zs->registerConstMemberProperty<Number>("MAX_VALUE",NumberZs_MAX_VALUE_get);

	// CONST MEMBER PROPERTIES
	//-------------------------------------------------------------------------------------


}


