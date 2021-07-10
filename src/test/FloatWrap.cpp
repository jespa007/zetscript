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
