#include "FloatWrap.h"

Float *FloatWrap_new(){
	return new Float();
}

void FloatWrap_delete(Float *_this){
	delete _this;
}

void FloatWrap_set(Float *_this,zs_float *_n){
	_this->n=*_n;
}

void FloatWrap_set(Float *_this, Float * _n){
	_this->n=_n->n;
}

Float * FloatWrap_add(Float *n1, Float *n2){
	return new Float(n1->n + n2->n);
}

Float * FloatWrap_add(Float *n1, zs_float *n2){
	return new Float(n1->n + *n2);
}

Float * FloatWrap_add(zs_float *n1, Float *n2){
	return new Float(*n1 + n2->n);
}

Float * FloatWrap_sub(Float *n1, Float *n2){
	return new Float(n1->n - n2->n);
}

Float * FloatWrap_sub(Float *n1, zs_float *n2){
	return new Float(n1->n - *n2);
}

Float * FloatWrap_sub(zs_float *n1, Float *n2){
	return new Float(*n1 - n2->n);
}

Float * FloatWrap_neg(Float *n1){
	return new Float(-n1->n);
}

Float * FloatWrap_div(Float *n1, Float *n2){
	return new Float(n1->n / n2->n);
}

Float * FloatWrap_div(Float *n1, zs_float *n2){
	return new Float(n1->n / *n2);
}

Float * FloatWrap_div(zs_float *n1, Float *n2){
	return new Float(*n1 / n2->n);
}

Float * FloatWrap_mod(Float *n1, Float *n2){
	return new Float(fmod(n1->n ,n2->n));
}

Float * FloatWrap_mod(Float *n1, zs_float *n2){
	return new Float(fmod(n1->n, *n2));
}

Float * FloatWrap_mod(zs_float *n1, Float *n2){
	return new Float(fmod(*n1 ,n2->n));
}

Float * FloatWrap_mul(Float *n1, Float *n2){
	return new Float(n1->n * n2->n);
}

Float * FloatWrap_mul(Float *n1, zs_float *n2){
	return new Float(n1->n * *n2);
}

Float * FloatWrap_mul(zs_float *n1, Float *n2){
	return new Float(*n1 * n2->n);
}

zs_float FloatWrap_toFloat(Float *_this){
	 return _this->n;
}


void FloatWrap_register(ZetScript *_zs){
	_zs->registerClass<Float>("Float",FloatWrap_new,FloatWrap_delete);

	// constructor
	_zs->registerConstructor<Float>(static_cast<void (*)(Float *,zs_float *)>(&FloatWrap_set));
	_zs->registerConstructor<Float>(static_cast<void (*)(Float *,Float *)>(&FloatWrap_set));

	_zs->registerMemberFunction<Float>("_set",static_cast<void (*)(Float *,zs_float *)>(&FloatWrap_set));
	_zs->registerMemberFunction<Float>("_set",static_cast<void (*)(Float *,Float *)>(&FloatWrap_set));


	_zs->registerMemberFunction<Float>("toFloat",&FloatWrap_toFloat);
	//_zs->registerMemberVariable<Float>("n",&Float::n);


	_zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap_add));
	_zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap_add));
	_zs->registerMemberFunctionStatic<Float>("_add",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap_add));

	_zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap_sub));
	_zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap_sub));
	_zs->registerMemberFunctionStatic<Float>("_sub",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap_sub));


	_zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap_mul));
	_zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap_mul));
	_zs->registerMemberFunctionStatic<Float>("_mul",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap_mul));

	_zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap_div));
	_zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap_div));
	_zs->registerMemberFunctionStatic<Float>("_div",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap_div));

	_zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(zs_float *,Float * )>(&FloatWrap_mod));
	_zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(Float *,zs_float *)>(&FloatWrap_mod));
	_zs->registerMemberFunctionStatic<Float>("_mod",static_cast<Float * (*)(Float *,Float * )>(&FloatWrap_mod));

	_zs->registerMemberFunction<Float>("_neg",static_cast<Float * (*)(Float *)>(&FloatWrap_neg));
}
