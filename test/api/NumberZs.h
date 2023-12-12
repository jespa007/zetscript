/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"
#include "Number.h"

using zetscript::ScriptEngine;
using zetscript::zs_float;
using zetscript::zs_int;
using zetscript::String;
using zetscript::ClassScriptObject;


Number *NumberZs_new(ScriptEngine *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new Number();
}

void NumberZs_delete(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}

//-------------------------------------------------------------------------------------
// CONSTRUCTORS
void NumberZs_constructor(ScriptEngine *_zs,Number *_this, zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=*_n;
}

void NumberZs_constructor(ScriptEngine *_zs,Number *_this,Number *_ci2){
	ZS_UNUSUED_PARAM(_zs);
	 _this->value = _ci2->value;
}
// CONSTRUCTORS
//-------------------------------------------------------------------------------------
// MEMBER FUNCTIONS
zs_int NumberZs_toInteger(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}

zs_float NumberZs_toFloat(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}

// MEMBER FUNCTIONS
//-------------------------------------------------------------------------------------
// STATIC MEMBER FUNCTIONS

zs_float NumberZs_pow(ScriptEngine *_zs,zs_float *_base, zs_float * _power){
	ZS_UNUSUED_PARAM(_zs);
	return (*_base) * (*_power);
}
// STATIC MEMBER FUNCTIONS
//-------------------------------------------------------------------------------------
// MEMBER PROPERTY METAMETHODS

String NumberZs_tostring(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	char output[100];
	sprintf(output,"%0.2f",_this->value);
	return output;
}

void NumberZs_set(ScriptEngine *_zs,Number *_this, zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=*_n;
}

void NumberZs_set(ScriptEngine *_zs,Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	 _this->value = _n->value;
}


void NumberZs_addassign(ScriptEngine *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=*_n;
}

void NumberZs_addassign(ScriptEngine *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=_n->value;
}

void NumberZs_subassign(ScriptEngine *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=*_n;
}

void NumberZs_subassign(ScriptEngine *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=_n->value;
}

void NumberZs_mulassign(ScriptEngine *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=*_n;
}

void NumberZs_mulassign(ScriptEngine *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=_n->value;
}

void NumberZs_divassign(ScriptEngine *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=*_n;
}

void NumberZs_divassign(ScriptEngine *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=_n->value;
}

void NumberZs_modassign(ScriptEngine *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,*_n);
}

void NumberZs_modassign(ScriptEngine *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,_n->value);
}

void NumberZs_andassign(ScriptEngine *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) & ((zs_int)*_n);
}

void NumberZs_andassign(ScriptEngine *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) & ((zs_int)_n->value);
}

void NumberZs_orassign(ScriptEngine *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) | ((zs_int)*_n);
}

void NumberZs_orassign(ScriptEngine *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) | ((zs_int)_n->value);
}

void NumberZs_xorassign(ScriptEngine *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) ^ ((zs_int)*_n);
}

void NumberZs_xorassign(ScriptEngine *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) ^ ((zs_int)_n->value);
}

void NumberZs_shrassign(ScriptEngine *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) >> ((zs_int)*_n);
}

void NumberZs_shrassign(ScriptEngine *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) >> ((zs_int)_n->value);
}

void NumberZs_shlassign(ScriptEngine *_zs, Number *_this,zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) << ((zs_int)*_n);
}

void NumberZs_shlassign(ScriptEngine *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=((zs_int)_this->value) << ((zs_int)_n->value);
}

bool NumberZs_not(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value == 0;
}

ClassScriptObject * NumberZs_neg(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(-_this->value));
}

ClassScriptObject *  NumberZs_postinc(ScriptEngine *_zs,Number *_this){
	return _zs->newClassScriptObject(new Number(_this->value++));
}

ClassScriptObject *  NumberZs_postdec(ScriptEngine *_zs,Number *_this){
	return  _zs->newClassScriptObject(new Number(_this->value--));
}

ClassScriptObject *   NumberZs_preinc(ScriptEngine *_zs,Number *_this){
	return _zs->newClassScriptObject(new Number(++_this->value));
}

ClassScriptObject *   NumberZs_predec(ScriptEngine *_zs,Number *_this){
	return _zs->newClassScriptObject(new Number(--_this->value));
}

// MEMBER PROPERTY METAMETHODS
//-------------------------------------------------------------------------------------
// STATIC MEMBER FUNCTION METAMETHODS

// _equ (a.k.a '==')
bool  NumberZs_equ(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value ==_n2->value;
}

bool  NumberZs_equ(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value == *_n2;
}

bool NumberZs_equ(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 == _n2->value;
}

// _neq (a.k.a '!=')
bool  NumberZs_nequ(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value !=_n2->value;
}

bool  NumberZs_nequ(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value != *_n2;
}

bool NumberZs_nequ(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 != _n2->value;
}

// _lt (a.k.a '<')
bool  NumberZs_lt(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value <_n2->value;
}

bool  NumberZs_lt(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value < *_n2;
}

bool NumberZs_lt(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 < _n2->value;
}

// _lte (a.k.a '<=')
bool  NumberZs_lte(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value <=_n2->value;
}

bool  NumberZs_lte(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value <= *_n2;
}

bool NumberZs_lte(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 <= _n2->value;
}

// _gt (a.k.a '>')
bool  NumberZs_gt(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value >_n2->value;
}

bool  NumberZs_gt(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value > *_n2;
}

bool NumberZs_gt(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 > _n2->value;
}
	
// _gte (a.k.a '>=')
bool  NumberZs_gte(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value >=_n2->value;
}

bool  NumberZs_gte(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _n1->value >= *_n2;
}

bool NumberZs_gte(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return *_n1 >= _n2->value;
}

// _add (a.k.a '+')
ClassScriptObject * NumberZs_add(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value +_n2->value));
}

ClassScriptObject * NumberZs_add(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value + *_n2));
}

ClassScriptObject * NumberZs_add(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(*_n1 + _n2->value));
}

// _sub (a.k.a '-')
ClassScriptObject * NumberZs_sub(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value - _n2->value));
}

ClassScriptObject * NumberZs_sub(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value - *_n2));
}

ClassScriptObject * NumberZs_sub(ScriptEngine *_zs,zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(*_n1 - _n2->value));
}

// _mul (a.k.a '*')
ClassScriptObject * NumberZs_mul(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value * _n2->value));
}

ClassScriptObject * NumberZs_mul(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value * (*_n2)));
}

ClassScriptObject * NumberZs_mul(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(*_n1 * _n2->value));
}

// _div (a.k.a '/')
ClassScriptObject * NumberZs_div(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value / _n2->value));
}

ClassScriptObject * NumberZs_div(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(_n1->value / *_n2));
}

ClassScriptObject * NumberZs_div(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(*_n1 / _n2->value));
}

// _mod (a.k.a '%')
ClassScriptObject * NumberZs_mod(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(fmod(_n1->value,_n2->value)));
}

ClassScriptObject * NumberZs_mod(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(fmod(_n1->value, *_n2)));
}

ClassScriptObject * NumberZs_mod(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number(fmod(*_n1, _n2->value)));
}

// _and (a.k.a '&')
ClassScriptObject * NumberZs_and(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value & (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_and(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value & (zs_int)*_n2));
}

ClassScriptObject * NumberZs_and(ScriptEngine *_zs,zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)*_n1 & (zs_int)_n2->value));
}

// _or (a.k.a '|')
ClassScriptObject * NumberZs_or(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value | (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_or(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value | (zs_int)*_n2));
}

ClassScriptObject * NumberZs_or(ScriptEngine *_zs,zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)*_n1 | (zs_int)_n2->value));
}

// _xor (a.k.a '^')
ClassScriptObject * NumberZs_xor(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value ^ (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_xor(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value ^ (zs_int)*_n2));
}

ClassScriptObject * NumberZs_xor(ScriptEngine *_zs,zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)*_n1 ^ (zs_int)_n2->value));
}

// _shr (a.k.a '>>')
ClassScriptObject * NumberZs_shr(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value >> (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_shr(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value >> (zs_int)*_n2));
}

ClassScriptObject * NumberZs_shr(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)*_n1 >> (zs_int)_n2->value));
}

// _shl (a.k.a '<<')
ClassScriptObject * NumberZs_shl(ScriptEngine *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value << (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_shl(ScriptEngine *_zs,Number *_n1, zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)_n1->value << (zs_int)*_n2));
}

ClassScriptObject * NumberZs_shl(ScriptEngine *_zs,zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return _zs->newClassScriptObject(new Number((zs_int)*_n1 << (zs_int)_n2->value));
}



//-------------------------------------------------------------------------------------
// MEMBER PROPERTY METAMETHODS

// get
zs_float NumberZs_value_get(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}

// set
void NumberZs_value_set(ScriptEngine *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=*_value;
}

// _addassign (a.k.a '+=')
void NumberZs_value_addassign(ScriptEngine *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value+=*_value;
}


// _subassign (a.k.a '-=')
void NumberZs_value_subassign(ScriptEngine *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value-=*_value;
}

// _mulassign (a.k.a '*=')
void NumberZs_value_mulassign(ScriptEngine *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value*=*_value;
}

// _divassign (a.k.a '/=')
void NumberZs_value_divassign(ScriptEngine *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value/=*_value;
}

// _modassign (a.k.a '%=')
void NumberZs_value_modassign(ScriptEngine *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=fmod(_this->value,*_value);
}

// _andassign (a.k.a '&=')
void NumberZs_value_andassign(ScriptEngine *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)&(int)(*_value);
}

// _orassign (a.k.a '|=')
void NumberZs_value_orassign(ScriptEngine *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)|(int)(*_value);
}

// _xorassign (a.k.a '^=')
void NumberZs_value_xorassign(ScriptEngine *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)^(int)(*_value);
}

// _shrassign (a.k.a '>>=')
void NumberZs_value_shrassign(ScriptEngine *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)>>(int)(*_value);
}

// _shlassign (a.k.a '<<=')
void NumberZs_value_shlassign(ScriptEngine *_zs,Number *_this,zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->value=(int)(_this->value)<<(int)(*_value);
}

// _postinc (a.k.a 'a++')
zs_float  NumberZs_value_postinc(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value++;
}

// _postdec (a.k.a 'a--')
zs_float  NumberZs_value_postdec(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value--;
}

// _preinc (a.k.a '++a')
zs_float  NumberZs_value_preinc(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return ++_this->value;
}

// _predec (a.k.a '--a')
zs_float  NumberZs_value_predec(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return --_this->value;
}


// MEMBER PROPERTY METAMETHODS
//-------------------------------------------------------------------------------------
// CONST MEMBER PROPERTY

// get
zs_float NumberZs_MAX_VALUE_get(ScriptEngine *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->value;
}
// CONST MEMBER PROPERTY
//------------------------------------------------------------------------------------------



void NumberZs_register(ScriptEngine *_zs){

	_zs->registerScriptType<Number>("Number",NumberZs_new,NumberZs_delete);

	//-------------------------------------------------------------------------------------
	// CONSTRUCTORS
	_zs->registerConstructor<Number>(static_cast<void (*)(ScriptEngine *_zs,Number *_this,zs_float *_value)>(&NumberZs_constructor));
	_zs->registerConstructor<Number>(static_cast<void (*)(ScriptEngine *_zs,Number *_this,Number *_value)>(&NumberZs_constructor));
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
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(ScriptEngine *_zs,Number *, zs_float *)>(&NumberZs_set));
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(ScriptEngine *_zs,Number *,Number *)>(&NumberZs_set));

	// _addassign  (a.k.a '+=')
	_zs->registerMemberFunction<Number>("_addassign",static_cast<void (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_addassign));
	_zs->registerMemberFunction<Number>("_addassign",static_cast<void (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_addassign));

	// _subassign  (a.k.a '-=')
	_zs->registerMemberFunction<Number>("_subassign",static_cast<void (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_subassign));
	_zs->registerMemberFunction<Number>("_subassign",static_cast<void (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_subassign));

	// _mulassign  (a.k.a '*=')
	_zs->registerMemberFunction<Number>("_mulassign",static_cast<void (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_mulassign));
	_zs->registerMemberFunction<Number>("_mulassign",static_cast<void (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_mulassign));

	// _divassign  (a.k.a '/=')
	_zs->registerMemberFunction<Number>("_divassign",static_cast<void (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_divassign));
	_zs->registerMemberFunction<Number>("_divassign",static_cast<void (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_divassign));


	// _modassign  (a.k.a '%=')
	_zs->registerMemberFunction<Number>("_modassign",static_cast<void (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_modassign));
	_zs->registerMemberFunction<Number>("_modassign",static_cast<void (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_modassign));

	// _andassign  (a.k.a '&=')
	_zs->registerMemberFunction<Number>("_andassign",static_cast<void (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_andassign));
	_zs->registerMemberFunction<Number>("_andassign",static_cast<void (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_andassign));

	// _orassign   (a.k.a '|=')
	_zs->registerMemberFunction<Number>("_orassign",static_cast<void (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_orassign));
	_zs->registerMemberFunction<Number>("_orassign",static_cast<void (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_orassign));

	// _xorassign  (a.k.a '^=')
	_zs->registerMemberFunction<Number>("_xorassign",static_cast<void (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_xorassign));
	_zs->registerMemberFunction<Number>("_xorassign",static_cast<void (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_xorassign));

	// _shrassign  (a.k.a '>>=')
	_zs->registerMemberFunction<Number>("_shrassign",static_cast<void (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_shrassign));
	_zs->registerMemberFunction<Number>("_shrassign",static_cast<void (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_shrassign));

	// _shlassign  (a.k.a '<<=')
	_zs->registerMemberFunction<Number>("_shlassign",static_cast<void (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_shlassign));
	_zs->registerMemberFunction<Number>("_shlassign",static_cast<void (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_shlassign));

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
	_zs->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_equ));
	_zs->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_equ));
	_zs->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_equ));

	// _nequ  (a.k.a '!=')
	_zs->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_nequ));
	_zs->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_nequ));
	_zs->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_nequ));

	// _lt  (a.k.a '<')
	_zs->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_lt));
	_zs->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_lt));
	_zs->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_lt));

	// _lte  (a.k.a '<=')
	_zs->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_lte));
	_zs->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_lte));
	_zs->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_lte));

	// _gt  (a.k.a '>')
	_zs->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_gt));
	_zs->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_gt));
	_zs->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_gt));

	// _gte  (a.k.a '>=')
	_zs->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_gte));
	_zs->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_gte));
	_zs->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_gte));

	// _add  (a.k.a '+')
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_add));

	// _sub  (a.k.a '-')
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_sub));

	// _mul  (a.k.a '*')
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_mul));

	// _div  (a.k.a '/')
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_div));

	// _mod  (a.k.a '%')
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_mod));

	// _shr  (a.k.a '>>')
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,zs_float * )>(&NumberZs_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_shr));

	// _shl  (a.k.a '<<')
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_shl));

	// _and  (a.k.a '&')
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_and));

	// _or  (a.k.a '|')
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_or));

	// _xor  (a.k.a '^')
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,zs_float *,Number * )>(&NumberZs_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,zs_float *)>(&NumberZs_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<ClassScriptObject * (*)(ScriptEngine *_zs,Number *,Number * )>(&NumberZs_xor));


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


