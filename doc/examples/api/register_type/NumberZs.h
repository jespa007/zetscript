/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"
#include "Number.h"

Number *NumberZs_new(zetscript::ScriptEngine *_script_engine){
	ZS_UNUSUED_PARAM(_script_engine);
	return new Number();
}

void NumberZs_delete(zetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	delete _this;
}

//-------------------------------------------------------------------------------------
// CONSTRUCTORS
void NumberZs_constructor(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=*_n;
}

void NumberZs_constructor(zetscript::ScriptEngine *_script_engine,Number *_this,Number *_ci2){
	ZS_UNUSUED_PARAM(_script_engine);
	 _this->value = _ci2->value;
}
// CONSTRUCTORS
//-------------------------------------------------------------------------------------
// MEMBER FUNCTIONS
zetscript::zs_int NumberZs_toInteger(zetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return _this->value;
}

zetscript::zs_float NumberZs_toFloat(zetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return _this->value;
}

// MEMBER FUNCTIONS
//-------------------------------------------------------------------------------------
// STATIC MEMBER FUNCTIONS

zetscript::zs_float NumberZs_pow(zetscript::ScriptEngine *_script_engine,zetscript::zs_float *_base, zetscript::zs_float * _power){
	ZS_UNUSUED_PARAM(_script_engine);
	return (*_base) * (*_power);
}
// STATIC MEMBER FUNCTIONS
//-------------------------------------------------------------------------------------
// MEMBER PROPERTY METAMETHODS

zetscript::String NumberZs_tostringzetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	char output[100];
	sprintf(output,"%0.2f",_this->value);
	return output;
}

void NumberZs_set(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=*_n;
}

void NumberZs_set(zetscript::ScriptEngine *_script_engine,Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	 _this->value = _n->value;
}


void NumberZs_addassign(zetscript::ScriptEngine *_script_engine, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value+=*_n;
}

void NumberZs_addassign(zetscript::ScriptEngine *_script_engine, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value+=_n->value;
}

void NumberZs_subassign(zetscript::ScriptEngine *_script_engine, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value-=*_n;
}

void NumberZs_subassign(zetscript::ScriptEngine *_script_engine, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value-=_n->value;
}

void NumberZs_mulassign(zetscript::ScriptEngine *_script_engine, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value*=*_n;
}

void NumberZs_mulassign(zetscript::ScriptEngine *_script_engine, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value*=_n->value;
}

void NumberZs_divassign(zetscript::ScriptEngine *_script_engine, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value/=*_n;
}

void NumberZs_divassign(zetscript::ScriptEngine *_script_engine, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value/=_n->value;
}

void NumberZs_modassign(zetscript::ScriptEngine *_script_engine, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=fmod(_this->value,*_n);
}

void NumberZs_modassign(zetscript::ScriptEngine *_script_engine, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=fmod(_this->value,_n->value);
}

void NumberZs_andassign(zetscript::ScriptEngine *_script_engine, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=((zs_int)_this->value) & ((zs_int)*_n);
}

void NumberZs_andassign(zetscript::ScriptEngine *_script_engine, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=((zs_int)_this->value) & ((zs_int)_n->value);
}

void NumberZs_orassign(zetscript::ScriptEngine *_script_engine, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=((zs_int)_this->value) | ((zs_int)*_n);
}

void NumberZs_orassign(zetscript::ScriptEngine *_script_engine, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=((zs_int)_this->value) | ((zs_int)_n->value);
}

void NumberZs_xorassign(zetscript::ScriptEngine *_script_engine, Number *_this, zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=((zs_int)_this->value) ^ ((zs_int)*_n);
}

void NumberZs_xorassign(zetscript::ScriptEngine *_script_engine, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=((zs_int)_this->value) ^ ((zs_int)_n->value);
}

void NumberZs_shrassign(zetscript::ScriptEngine *_script_engine, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=((zs_int)_this->value) >> ((zs_int)*_n);
}

void NumberZs_shrassign(zetscript::ScriptEngine *_script_engine, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=((zs_int)_this->value) >> ((zs_int)_n->value);
}

void NumberZs_shlassign(zetscript::ScriptEngine *_script_engine, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=((zs_int)_this->value) << ((zs_int)*_n);
}

void NumberZs_shlassign(zetscript::ScriptEngine *_script_engine, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=((zs_int)_this->value) << ((zs_int)_n->value);
}

bool NumberZs_not(zetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return _this->value == 0;
}

ClassScriptObject * NumberZs_neg(zetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(-_this->value));
}

ClassScriptObject *  NumberZs_postinc(zetscript::ScriptEngine *_script_engine,Number *_this){
	return _script_engine->newClassScriptObject(new Number(_this->value++));
}

ClassScriptObject *  NumberZs_postdec(zetscript::ScriptEngine *_script_engine,Number *_this){
	return  _script_engine->newClassScriptObject(new Number(_this->value--));
}

ClassScriptObject *   NumberZs_preinc(zetscript::ScriptEngine *_script_engine,Number *_this){
	return _script_engine->newClassScriptObject(new Number(++_this->value));
}

ClassScriptObject *   NumberZs_predec(zetscript::ScriptEngine *_script_engine,Number *_this){
	return _script_engine->newClassScriptObject(new Number(--_this->value));
}

// MEMBER PROPERTY METAMETHODS
//-------------------------------------------------------------------------------------
// STATIC MEMBER FUNCTION METAMETHODS

// _equ (a.k.a '==')
bool  NumberZs_equ(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value ==_n2->value;
}

bool  NumberZs_equ(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value == *_n2;
}

bool NumberZs_equ(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return *_n1 == _n2->value;
}

// _neq (a.k.a '!=')
bool  NumberZs_nequ(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value !=_n2->value;
}

bool  NumberZs_nequ(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value != *_n2;
}

bool NumberZs_nequ(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return *_n1 != _n2->value;
}

// _lt (a.k.a '<')
bool  NumberZs_lt(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value <_n2->value;
}

bool  NumberZs_lt(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value < *_n2;
}

bool NumberZs_lt(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return *_n1 < _n2->value;
}

// _lte (a.k.a '<=')
bool  NumberZs_lte(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value <=_n2->value;
}

bool  NumberZs_lte(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value <= *_n2;
}

bool NumberZs_lte(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return *_n1 <= _n2->value;
}

// _gt (a.k.a '>')
bool  NumberZs_gt(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value >_n2->value;
}

bool  NumberZs_gt(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value > *_n2;
}

bool NumberZs_gt(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return *_n1 > _n2->value;
}
	
// _gte (a.k.a '>=')
bool  NumberZs_gte(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value >=_n2->value;
}

bool  NumberZs_gte(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _n1->value >= *_n2;
}

bool NumberZs_gte(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return *_n1 >= _n2->value;
}

// _add (a.k.a '+')
ClassScriptObject * NumberZs_add(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(_n1->value +_n2->value));
}

ClassScriptObject * NumberZs_add(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(_n1->value + *_n2));
}

ClassScriptObject * NumberZs_add(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(*_n1 + _n2->value));
}

// _sub (a.k.a '-')
ClassScriptObject * NumberZs_sub(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(_n1->value - _n2->value));
}

ClassScriptObject * NumberZs_sub(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(_n1->value - *_n2));
}

ClassScriptObject * NumberZs_sub(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(*_n1 - _n2->value));
}

// _mul (a.k.a '*')
ClassScriptObject * NumberZs_mul(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(_n1->value * _n2->value));
}

ClassScriptObject * NumberZs_mul(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(_n1->value * (*_n2)));
}

ClassScriptObject * NumberZs_mul(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(*_n1 * _n2->value));
}

// _div (a.k.a '/')
ClassScriptObject * NumberZs_div(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(_n1->value / _n2->value));
}

ClassScriptObject * NumberZs_div(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(_n1->value / *_n2));
}

ClassScriptObject * NumberZs_div(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(*_n1 / _n2->value));
}

// _mod (a.k.a '%')
ClassScriptObject * NumberZs_mod(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(fmod(_n1->value,_n2->value)));
}

ClassScriptObject * NumberZs_mod(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(fmod(_n1->value, *_n2)));
}

ClassScriptObject * NumberZs_mod(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number(fmod(*_n1, _n2->value)));
}

// _and (a.k.a '&')
ClassScriptObject * NumberZs_and(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)_n1->value & (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_and(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)_n1->value & (zs_int)*_n2));
}

ClassScriptObject * NumberZs_and(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)*_n1 & (zs_int)_n2->value));
}

// _or (a.k.a '|')
ClassScriptObject * NumberZs_or(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)_n1->value | (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_or(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)_n1->value | (zs_int)*_n2));
}

ClassScriptObject * NumberZs_or(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)*_n1 | (zs_int)_n2->value));
}

// _xor (a.k.a '^')
ClassScriptObject * NumberZs_xor(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)_n1->value ^ (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_xor(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)_n1->value ^ (zs_int)*_n2));
}

ClassScriptObject * NumberZs_xor(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)*_n1 ^ (zs_int)_n2->value));
}

// _shr (a.k.a '>>')
ClassScriptObject * NumberZs_shr(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)_n1->value >> (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_shr(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)_n1->value >> (zs_int)*_n2));
}

ClassScriptObject * NumberZs_shr(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)*_n1 >> (zs_int)_n2->value));
}

// _shl (a.k.a '<<')
ClassScriptObject * NumberZs_shl(zetscript::ScriptEngine *_script_engine,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)_n1->value << (zs_int)_n2->value));
}

ClassScriptObject * NumberZs_shl(zetscript::ScriptEngine *_script_engine,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)_n1->value << (zs_int)*_n2));
}

ClassScriptObject * NumberZs_shl(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_script_engine);
	return _script_engine->newClassScriptObject(new Number((zs_int)*_n1 << (zs_int)_n2->value));
	
}



//-------------------------------------------------------------------------------------
// MEMBER PROPERTY METAMETHODS

// get
zs_float NumberZs_value_get(zetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return _this->value;
}

// set
void NumberZs_value_set(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=*_value;
}

// _addassign (a.k.a '+=')
void NumberZs_value_addassign(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value+=*_value;
}


// _subassign (a.k.a '-=')
void NumberZs_value_subassign(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value-=*_value;
}

// _mulassign (a.k.a '*=')
void NumberZs_value_mulassign(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value*=*_value;
}

// _divassign (a.k.a '/=')
void NumberZs_value_divassign(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value/=*_value;
}

// _modassign (a.k.a '%=')
void NumberZs_value_modassign(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=fmod(_this->value,*_value);
}

// _andassign (a.k.a '&=')
void NumberZs_value_andassign(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=(int)(_this->value)&(int)(*_value);
}

// _orassign (a.k.a '|=')
void NumberZs_value_orassign(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=(int)(_this->value)|(int)(*_value);
}

// _xorassign (a.k.a '^=')
void NumberZs_value_xorassign(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=(int)(_this->value)^(int)(*_value);
}

// _shrassign (a.k.a '>>=')
void NumberZs_value_shrassign(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=(int)(_this->value)>>(int)(*_value);
}

// _shlassign (a.k.a '<<=')
void NumberZs_value_shlassign(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->value=(int)(_this->value)<<(int)(*_value);
}

// _postinc (a.k.a 'a++')
zs_float  NumberZs_value_postinc(zetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return _this->value++;
}

// _postdec (a.k.a 'a--')
zs_float  NumberZs_value_postdec(zetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return _this->value--;
}

// _preinc (a.k.a '++a')
zs_float  NumberZs_value_preinc(zetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return ++_this->value;
}

// _predec (a.k.a '--a')
zs_float  NumberZs_value_predec(zetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return --_this->value;
}


// MEMBER PROPERTY METAMETHODS
//-------------------------------------------------------------------------------------
// CONST MEMBER PROPERTY

// get
zs_float NumberZs_MAX_VALUE_get(zetscript::ScriptEngine *_script_engine,Number *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return FLT_MAX;
}
// CONST MEMBER PROPERTY
//------------------------------------------------------------------------------------------



void NumberZs_register(zetscript::ScriptEngine *_script_engine){

	_script_engine->registerType<Number>("Number",NumberZs_new,NumberZs_delete);

	//-------------------------------------------------------------------------------------
	// CONSTRUCTORS
	_script_engine->registerConstructor<Number>(static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *_this, zetscript::zs_float *_value)>(&NumberZs_constructor));
	_script_engine->registerConstructor<Number>(static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *_this,Number *_value)>(&NumberZs_constructor));
	// CONSTRUCTORS
	//-------------------------------------------------------------------------------------
	// STATIC FUNCTIONS
	_script_engine->registerStaticMemberFunction<Number>("pow",&NumberZs_pow);
	// STATIC FUNCTIONS
	//-------------------------------------------------------------------------------------
	// MEMBER FUNCTIONS
	_script_engine->registerMemberFunction<Number>("toInteger",&NumberZs_toInteger);
	_script_engine->registerMemberFunction<Number>("toFloat",&NumberZs_toFloat);
	// MEMBER FUNCTIONS
	//-------------------------------------------------------------------------------------
	// MEMBER FUNCTION METAMETHODS

	// tostring
	_script_engine->registerMemberFunction<Number>("_tostring",&NumberZs_tostring);

	// set
	_script_engine->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_set));
	_script_engine->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *,Number *)>(&NumberZs_set));

	// _addassign  (a.k.a '+=')
	_script_engine->registerMemberFunction<Number>("_addassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_addassign));
	_script_engine->registerMemberFunction<Number>("_addassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_addassign));

	// _subassign  (a.k.a '-=')
	_script_engine->registerMemberFunction<Number>("_subassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_subassign));
	_script_engine->registerMemberFunction<Number>("_subassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_subassign));

	// _mulassign  (a.k.a '*=')
	_script_engine->registerMemberFunction<Number>("_mulassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_mulassign));
	_script_engine->registerMemberFunction<Number>("_mulassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_mulassign));

	// _divassign  (a.k.a '/=')
	_script_engine->registerMemberFunction<Number>("_divassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_divassign));
	_script_engine->registerMemberFunction<Number>("_divassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_divassign));


	// _modassign  (a.k.a '%=')
	_script_engine->registerMemberFunction<Number>("_modassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_modassign));
	_script_engine->registerMemberFunction<Number>("_modassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_modassign));

	// _andassign  (a.k.a '&=')
	_script_engine->registerMemberFunction<Number>("_andassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_andassign));
	_script_engine->registerMemberFunction<Number>("_andassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_andassign));

	// _orassign   (a.k.a '|=')
	_script_engine->registerMemberFunction<Number>("_orassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_orassign));
	_script_engine->registerMemberFunction<Number>("_orassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_orassign));

	// _xorassign  (a.k.a '^=')
	_script_engine->registerMemberFunction<Number>("_xorassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_xorassign));
	_script_engine->registerMemberFunction<Number>("_xorassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_xorassign));

	// _shrassign  (a.k.a '>>=')
	_script_engine->registerMemberFunction<Number>("_shrassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_shrassign));
	_script_engine->registerMemberFunction<Number>("_shrassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_shrassign));

	// _shlassign  (a.k.a '<<=')
	_script_engine->registerMemberFunction<Number>("_shlassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_shlassign));
	_script_engine->registerMemberFunction<Number>("_shlassign",static_cast<void (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_shlassign));

	// _neg (a.k.a '-a')
	_script_engine->registerMemberFunction<Number>("_neg",&NumberZs_neg);

	// _not (a.k.a '!a')
	_script_engine->registerMemberFunction<Number>("_not",&NumberZs_not);

	// _postinc (a.k.a 'a++')
	_script_engine->registerMemberFunction<Number>("_postinc",NumberZs_postinc);

	// _postdec (a.k.a 'a--')
	_script_engine->registerMemberFunction<Number>("_postdec",NumberZs_postdec);

	// _preinc (a.k.a '++a')
	_script_engine->registerMemberFunction<Number>("_preinc",NumberZs_preinc);

	// _predec (a.k.a '--a')
	_script_engine->registerMemberFunction<Number>("_predec",NumberZs_predec);


	// MEMBER FUNCTION METAMETHODS
	//-------------------------------------------------------------------------------------
	// STATIC MEMBER FUNCTION METAMETHODS

	// _equ   (a.k.a '==')
	_script_engine->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_equ));
	_script_engine->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_equ));
	_script_engine->registerStaticMemberFunction<Number>("_equ",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_equ));

	// _nequ  (a.k.a '!=')
	_script_engine->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_nequ));
	_script_engine->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_nequ));
	_script_engine->registerStaticMemberFunction<Number>("_nequ",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_nequ));

	// _lt  (a.k.a '<')
	_script_engine->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_lt));
	_script_engine->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_lt));
	_script_engine->registerStaticMemberFunction<Number>("_lt",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_lt));

	// _lte  (a.k.a '<=')
	_script_engine->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_lte));
	_script_engine->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_lte));
	_script_engine->registerStaticMemberFunction<Number>("_lte",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_lte));

	// _gt  (a.k.a '>')
	_script_engine->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_gt));
	_script_engine->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_gt));
	_script_engine->registerStaticMemberFunction<Number>("_gt",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_gt));

	// _gte  (a.k.a '>=')
	_script_engine->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_gte));
	_script_engine->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_gte));
	_script_engine->registerStaticMemberFunction<Number>("_gte",static_cast<bool (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_gte));

	// _add  (a.k.a '+')
	_script_engine->registerStaticMemberFunction<Number>("_add",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_add));
	_script_engine->registerStaticMemberFunction<Number>("_add",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_add));
	_script_engine->registerStaticMemberFunction<Number>("_add",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_add));

	// _sub  (a.k.a '-')
	_script_engine->registerStaticMemberFunction<Number>("_sub",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_sub));
	_script_engine->registerStaticMemberFunction<Number>("_sub",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_sub));
	_script_engine->registerStaticMemberFunction<Number>("_sub",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_sub));

	// _mul  (a.k.a '*')
	_script_engine->registerStaticMemberFunction<Number>("_mul",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_mul));
	_script_engine->registerStaticMemberFunction<Number>("_mul",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_mul));
	_script_engine->registerStaticMemberFunction<Number>("_mul",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_mul));

	// _div  (a.k.a '/')
	_script_engine->registerStaticMemberFunction<Number>("_div",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_div));
	_script_engine->registerStaticMemberFunction<Number>("_div",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_div));
	_script_engine->registerStaticMemberFunction<Number>("_div",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_div));

	// _mod  (a.k.a '%')
	_script_engine->registerStaticMemberFunction<Number>("_mod",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_mod));
	_script_engine->registerStaticMemberFunction<Number>("_mod",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_mod));
	_script_engine->registerStaticMemberFunction<Number>("_mod",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_mod));

	// _shr  (a.k.a '>>')
	_script_engine->registerStaticMemberFunction<Number>("_shr",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_shr));
	_script_engine->registerStaticMemberFunction<Number>("_shr",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float * )>(&NumberZs_shr));
	_script_engine->registerStaticMemberFunction<Number>("_shr",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_shr));

	// _shl  (a.k.a '<<')
	_script_engine->registerStaticMemberFunction<Number>("_shl",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_shl));
	_script_engine->registerStaticMemberFunction<Number>("_shl",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_shl));
	_script_engine->registerStaticMemberFunction<Number>("_shl",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_shl));

	// _and  (a.k.a '&')
	_script_engine->registerStaticMemberFunction<Number>("_and",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_and));
	_script_engine->registerStaticMemberFunction<Number>("_and",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_and));
	_script_engine->registerStaticMemberFunction<Number>("_and",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_and));

	// _or  (a.k.a '|')
	_script_engine->registerStaticMemberFunction<Number>("_or",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_or));
	_script_engine->registerStaticMemberFunction<Number>("_or",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_or));
	_script_engine->registerStaticMemberFunction<Number>("_or",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_or));

	// _xor  (a.k.a '^')
	_script_engine->registerStaticMemberFunction<Number>("_xor",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *,Number * )>(&NumberZs_xor));
	_script_engine->registerStaticMemberFunction<Number>("_xor",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *, zetscript::zs_float *)>(&NumberZs_xor));
	_script_engine->registerStaticMemberFunction<Number>("_xor",static_cast<ClassScriptObject * (*)(zetscript::ScriptEngine *_script_engine,Number *,Number * )>(&NumberZs_xor));


	// STATIC MEMBER FUNCTIONS METAMETHODS
	//-------------------------------------------------------------------------------------
	// MEMBER PROPERTY METAMETHODS

	// _set
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_set",NumberZs_value_set);
	
	// _get
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_get",NumberZs_value_get);
	
	// _addassign (a.k.a '+=')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_addassign",NumberZs_value_addassign);
	
	// _subassign (a.k.a '-=')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_subassign",NumberZs_value_subassign);

	// _mulassign (a.k.a '*=')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_mulassign",NumberZs_value_mulassign);
	
	// _divassign (a.k.a '/=')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_divassign",NumberZs_value_divassign);
	
	// _modassign (a.k.a '%=')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_modassign",NumberZs_value_modassign);
	
	// _andassign  (a.k.a '&=')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_andassign",NumberZs_value_andassign);

	// _orassign  (a.k.a '|=')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_orassign",NumberZs_value_orassign);

	// _xorassign (a.k.a '^=')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_xorassign",NumberZs_value_xorassign);

	// _shrassign  (a.k.a '>>=')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_shrassign",NumberZs_value_shrassign);

	// _shlassign  (a.k.a '<<=')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_shlassign",NumberZs_value_shlassign);

	// _postinc  (a.k.a 'a++')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_postinc",NumberZs_value_postinc);

	// _postdec  (a.k.a 'a--')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_postdec",NumberZs_value_postdec);

	// _preinc  (a.k.a '++a')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_preinc",NumberZs_value_preinc);

	// _predec  (a.k.a '--a')
	_script_engine->registerMemberPropertyMetamethod<Number>("value","_predec",NumberZs_value_predec);

	// MEMBER PROPERTY METAMETHODS
	//-------------------------------------------------------------------------------------
	// CONST MEMBER PROPERTIES
	_script_engine->registerConstMemberProperty<Number>("MAX_VALUE",NumberZs_MAX_VALUE_get);

	// CONST MEMBER PROPERTIES
	//-------------------------------------------------------------------------------------


}


