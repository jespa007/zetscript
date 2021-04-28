#include "FloatWrap.h"

Float *FloatWrap_New(){
	return new Float();
}

void FloatWrap_Delete(Float *_this){
	delete _this;
}

void FloatWrap_Set(Float *_this,zs_float *_n){
	_this->n=*_n;
}

void FloatWrap_Set(Float *_this, Float * _n){
	_this->n=_n->n;
}

Float * FloatWrap_Add(Float *n1, Float *n2){
	return new Float(n1->n + n2->n);
}

Float * FloatWrap_Add(Float *n1, zs_float *n2){
	return new Float(n1->n + *n2);
}

Float * FloatWrap_Add(zs_float *n1, Float *n2){
	return new Float(*n1 + n2->n);
}

Float * FloatWrap_Sub(Float *n1, Float *n2){
	return new Float(n1->n - n2->n);
}

Float * FloatWrap_Sub(Float *n1, zs_float *n2){
	return new Float(n1->n - *n2);
}

Float * FloatWrap_Sub(zs_float *n1, Float *n2){
	return new Float(*n1 - n2->n);
}

Float * FloatWrap_Neg(Float *n1){
	return new Float(-n1->n);
}

Float * FloatWrap_Div(Float *n1, Float *n2){
	return new Float(n1->n / n2->n);
}

Float * FloatWrap_Div(Float *n1, zs_float *n2){
	return new Float(n1->n / *n2);
}

Float * FloatWrap_Div(zs_float *n1, Float *n2){
	return new Float(*n1 / n2->n);
}

Float * FloatWrap_Mod(Float *n1, Float *n2){
	return new Float(fmod(n1->n ,n2->n));
}

Float * FloatWrap_Mod(Float *n1, zs_float *n2){
	return new Float(fmod(n1->n, *n2));
}

Float * FloatWrap_Mod(zs_float *n1, Float *n2){
	return new Float(fmod(*n1 ,n2->n));
}

Float * FloatWrap_Mul(Float *n1, Float *n2){
	return new Float(n1->n * n2->n);
}

Float * FloatWrap_Mul(Float *n1, zs_float *n2){
	return new Float(n1->n * *n2);
}

Float * FloatWrap_Mul(zs_float *n1, Float *n2){
	return new Float(*n1 * n2->n);
}

zs_float FloatWrap_ToFloat(Float *_this){
	 return _this->n;
}
