/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "NumberWrap.h"

Number *NumberWrap_new(zetscript::ZetScript *_zs){
	return new Number();

}

void NumberWrap_set(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_n){
	_this->n=*_n;
}

void NumberWrap_set(zetscript::ZetScript *_zs,Number *_this,Number *_ci2){
	 _this->n = _ci2->n;
}

Number * NumberWrap_add(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	return new Number(_n1->n +_n2->n);
}

Number * NumberWrap_add(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	return new Number(_n1->n + *_n2);
}

Number * NumberWrap_add(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	return new Number(*_n1 + _n2->n);
}

void NumberWrap_add_set(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	_this->n+=*_n;
}

void NumberWrap_add_set(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	_this->n+=_n->n;
}

Number * NumberWrap_sub(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	return new Number(_n1->n - _n2->n);
}

Number * NumberWrap_sub(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	return new Number(_n1->n - *_n2);
}

Number * NumberWrap_sub(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	return new Number(*_n1 - _n2->n);
}

Number * NumberWrap_div(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	return new Number(_n1->n / _n2->n);
}

Number * NumberWrap_div(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	return new Number(_n1->n / *_n2);
}

Number * NumberWrap_div(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	return new Number(*_n1 / _n2->n);
}

Number * NumberWrap_neg(zetscript::ZetScript *_zs,Number *_n1){
	return new Number(-_n1->n);
}

Number * NumberWrap_mod(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	return new Number(fmod(_n1->n,_n2->n));
}

Number * NumberWrap_mod(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	return new Number(fmod(_n1->n, *_n2));
}

Number * NumberWrap_mod(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	return new Number(fmod(*_n1, _n2->n));
}

Number * NumberWrap_mul(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	return new Number(_n1->n * _n2->n);
}

Number * NumberWrap_mul(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	return new Number(_n1->n * (*_n2));
}

Number * NumberWrap_mul(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	return new Number(*_n1 * _n2->n);
}

Number * NumberWrap_shl(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	return new Number((zetscript::zs_int)_n1->n << (zetscript::zs_int)_n2->n);
}

Number * NumberWrap_shl(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	return new Number((zetscript::zs_int)_n1->n << (zetscript::zs_int)*_n2);
}

Number * NumberWrap_shl(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	return new Number((zetscript::zs_int)*_n1 << (zetscript::zs_int)_n2->n);
}

Number * NumberWrap_and(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	return new Number((zetscript::zs_int)_n1->n & (zetscript::zs_int)_n2->n);
}

Number * NumberWrap_and(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	return new Number((zetscript::zs_int)_n1->n & (zetscript::zs_int)*_n2);
}

Number * NumberWrap_and(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	return new Number((zetscript::zs_int)*_n1 & (zetscript::zs_int)_n2->n);
}

Number * NumberWrap_or(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	return new Number((zetscript::zs_int)_n1->n | (zetscript::zs_int)_n2->n);
}

Number * NumberWrap_or(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	return new Number((zetscript::zs_int)_n1->n | (zetscript::zs_int)*_n2);
}

Number * NumberWrap_or(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	return new Number((zetscript::zs_int)*_n1 | (zetscript::zs_int)_n2->n);
}

Number * NumberWrap_xor(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	return new Number((zetscript::zs_int)_n1->n ^ (zetscript::zs_int)_n2->n);
}

Number * NumberWrap_xor(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	return new Number((zetscript::zs_int)_n1->n ^ (zetscript::zs_int)*_n2);
}

Number * NumberWrap_xor(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	return new Number((zetscript::zs_int)*_n1 ^ (zetscript::zs_int)_n2->n);
}

Number * NumberWrap_shr(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	return new Number((zetscript::zs_int)_n1->n >> (zetscript::zs_int)_n2->n);
}

Number * NumberWrap_shr(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	return new Number((zetscript::zs_int)_n1->n >> (zetscript::zs_int)*_n2);
}

Number * NumberWrap_shr(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	return new Number((zetscript::zs_int)*_n1 >> (zetscript::zs_int)_n2->n);
}

zetscript::zs_int NumberWrap_toInt(zetscript::ZetScript *_zs,Number *_this){
	return _this->n;
}

zetscript::zs_float NumberWrap_toFloat(zetscript::ZetScript *_zs,Number *_this){
	return _this->n;
}

void NumberWrap_delete(zetscript::ZetScript *_zs,Number *_this){
	delete _this;
}


void NumberWrap_register(zetscript::ZetScript *_zs){

	_zs->registerClass<Number>("Number",NumberWrap_new,NumberWrap_delete);
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_set));
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberWrap_set));
	_zs->registerMemberFunction<Number>("toInt",&NumberWrap_toInt);
	_zs->registerMemberFunction<Number>("toFloat",&NumberWrap_toFloat);

	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *, zetscript::zs_float *)>(&NumberWrap_set));
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberWrap_set));


	_zs->registerMemberFunctionStatic<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_add));
	_zs->registerMemberFunctionStatic<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_add));
	_zs->registerMemberFunctionStatic<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_add));

	// add set
	_zs->registerMemberFunction<Number>("_add_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_add_set));
	_zs->registerMemberFunction<Number>("_add_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_add_set));


	_zs->registerMemberFunctionStatic<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_sub));
	_zs->registerMemberFunctionStatic<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_sub));
	_zs->registerMemberFunctionStatic<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_sub));


	_zs->registerMemberFunctionStatic<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_mul));
	_zs->registerMemberFunctionStatic<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mul));
	_zs->registerMemberFunctionStatic<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mul));

	_zs->registerMemberFunctionStatic<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_div));
	_zs->registerMemberFunctionStatic<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_div));
	_zs->registerMemberFunctionStatic<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_div));

	_zs->registerMemberFunctionStatic<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_mod));
	_zs->registerMemberFunctionStatic<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mod));
	_zs->registerMemberFunctionStatic<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mod));


	_zs->registerMemberFunctionStatic<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_shr));
	_zs->registerMemberFunctionStatic<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float * )>(&NumberWrap_shr));
	_zs->registerMemberFunctionStatic<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shr));


	_zs->registerMemberFunctionStatic<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_shl));
	_zs->registerMemberFunctionStatic<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_shl));
	_zs->registerMemberFunctionStatic<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shl));


	_zs->registerMemberFunctionStatic<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_and));
	_zs->registerMemberFunctionStatic<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_and));
	_zs->registerMemberFunctionStatic<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_and));

	_zs->registerMemberFunctionStatic<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_or));
	_zs->registerMemberFunctionStatic<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_or));
	_zs->registerMemberFunctionStatic<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_or));

	_zs->registerMemberFunctionStatic<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_xor));
	_zs->registerMemberFunctionStatic<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_xor));
	_zs->registerMemberFunctionStatic<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_xor));

	_zs->registerMemberFunction<Number>("_neg",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *)>(&NumberWrap_neg));
}
