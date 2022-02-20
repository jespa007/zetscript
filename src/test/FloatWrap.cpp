/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "FloatWrap.h"

Float *FloatWrap_new(zetscript::ZetScript *_zs){
	return new Float();
}

void FloatWrap_delete(zetscript::ZetScript *_zs,Float *_this){
	delete _this;
}

void FloatWrap_set(zetscript::ZetScript *_zs,Float *_this,zs_float *_n){
	_this->n=*_n;
}

void FloatWrap_set(zetscript::ZetScript *_zs,Float *_this, Float * _n){
	_this->n=_n->n;
}

Float * FloatWrap_add(zetscript::ZetScript *_zs,Float *n1, Float *n2){
	return new Float(n1->n + n2->n);
}

Float * FloatWrap_add(zetscript::ZetScript *_zs,Float *n1, zs_float *n2){
	return new Float(n1->n + *n2);
}

Float * FloatWrap_add(zetscript::ZetScript *_zs,zs_float *n1, Float *n2){
	return new Float(*n1 + n2->n);
}

Float * FloatWrap_sub(zetscript::ZetScript *_zs,Float *n1, Float *n2){
	return new Float(n1->n - n2->n);
}

Float * FloatWrap_sub(zetscript::ZetScript *_zs,Float *n1, zs_float *n2){
	return new Float(n1->n - *n2);
}

Float * FloatWrap_sub(zetscript::ZetScript *_zs,zs_float *n1, Float *n2){
	return new Float(*n1 - n2->n);
}

Float * FloatWrap_neg(zetscript::ZetScript *_zs,Float *n1){
	return new Float(-n1->n);
}

Float * FloatWrap_div(zetscript::ZetScript *_zs,Float *n1, Float *n2){
	return new Float(n1->n / n2->n);
}

Float * FloatWrap_div(zetscript::ZetScript *_zs,Float *n1, zs_float *n2){
	return new Float(n1->n / *n2);
}

Float * FloatWrap_div(zetscript::ZetScript *_zs,zs_float *n1, Float *n2){
	return new Float(*n1 / n2->n);
}

Float * FloatWrap_mod(zetscript::ZetScript *_zs,Float *n1, Float *n2){
	return new Float(fmod(n1->n ,n2->n));
}

Float * FloatWrap_mod(zetscript::ZetScript *_zs,Float *n1, zs_float *n2){
	return new Float(fmod(n1->n, *n2));
}

Float * FloatWrap_mod(zetscript::ZetScript *_zs,zs_float *n1, Float *n2){
	return new Float(fmod(*n1 ,n2->n));
}

Float * FloatWrap_mul(zetscript::ZetScript *_zs,Float *n1, Float *n2){
	return new Float(n1->n * n2->n);
}

Float * FloatWrap_mul(zetscript::ZetScript *_zs,Float *n1, zs_float *n2){
	return new Float(n1->n * *n2);
}

Float * FloatWrap_mul(zetscript::ZetScript *_zs,zs_float *n1, Float *n2){
	return new Float(*n1 * n2->n);
}

zs_float FloatWrap_toFloat(zetscript::ZetScript *_zs,Float *_this){
	 return _this->n;
}


void FloatWrap_register(ZetScript *_zs){
	_zs->registerClass<Float>("Float",FloatWrap_new,FloatWrap_delete);

	// constructor
	_zs->registerConstructor<Float>(static_cast<void (*)(zetscript::ZetScript *_zs,Float *,zs_float *)>(&FloatWrap_set));
	_zs->registerConstructor<Float>(static_cast<void (*)(zetscript::ZetScript *_zs,Float *,Float *)>(&FloatWrap_set));

	_zs->registerMemberFunction<Float>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Float *,zs_float *)>(&FloatWrap_set));
	_zs->registerMemberFunction<Float>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Float *,Float *)>(&FloatWrap_set));


	_zs->registerMemberFunction<Float>("toFloat",&FloatWrap_toFloat);
	//_zs->registerMemberVariable<Float>("n",&Float::n);


	_zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(zetscript::ZetScript *_zs,zs_float *,Float * )>(&FloatWrap_add));
	_zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(zetscript::ZetScript *_zs,Float *,zs_float *)>(&FloatWrap_add));
	_zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(zetscript::ZetScript *_zs,Float *,Float * )>(&FloatWrap_add));

	_zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(zetscript::ZetScript *_zs,zs_float *,Float * )>(&FloatWrap_sub));
	_zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(zetscript::ZetScript *_zs,Float *,zs_float *)>(&FloatWrap_sub));
	_zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(zetscript::ZetScript *_zs,Float *,Float * )>(&FloatWrap_sub));


	_zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(zetscript::ZetScript *_zs,zs_float *,Float * )>(&FloatWrap_mul));
	_zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(zetscript::ZetScript *_zs,Float *,zs_float *)>(&FloatWrap_mul));
	_zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(zetscript::ZetScript *_zs,Float *,Float * )>(&FloatWrap_mul));

	_zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(zetscript::ZetScript *_zs,zs_float *,Float * )>(&FloatWrap_div));
	_zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(zetscript::ZetScript *_zs,Float *,zs_float *)>(&FloatWrap_div));
	_zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(zetscript::ZetScript *_zs,Float *,Float * )>(&FloatWrap_div));

	_zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(zetscript::ZetScript *_zs,zs_float *,Float * )>(&FloatWrap_mod));
	_zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(zetscript::ZetScript *_zs,Float *,zs_float *)>(&FloatWrap_mod));
	_zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(zetscript::ZetScript *_zs,Float *,Float * )>(&FloatWrap_mod));

	_zs->registerMemberFunction<Float>("_neg",static_cast<Float * (*)(zetscript::ZetScript *_zs,Float *)>(&FloatWrap_neg));
}
