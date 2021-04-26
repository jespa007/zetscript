#include "FloatWrap.h"


Float *FloatWrap::_new(){
	return new Float();

}
void FloatWrap::_delete(Float *_float){
	delete _float;
}

 void FloatWrap::_set(Float *_this,zs_float *_n){
	_this->n=*_n;
}


 void FloatWrap::_set(Float *_this, Float * _n){
	_this->n=_n->n;
}


 Float * FloatWrap::_add(Float *n1, Float *n2){
	return new Float(n1->n + n2->n);
}

 Float * FloatWrap::_add(Float *n1, zs_float *n2){
	return new Float(n1->n + *n2);
}

 Float * FloatWrap::_add(zs_float *n1, Float *n2){
	return new Float(*n1 + n2->n);
}

 Float * FloatWrap::_sub(Float *n1, Float *n2){
	return new Float(n1->n - n2->n);
}

 Float * FloatWrap::_sub(Float *n1, zs_float *n2){
	return new Float(n1->n - *n2);
}

 Float * FloatWrap::_sub(zs_float *n1, Float *n2){
	return new Float(*n1 - n2->n);
}

 Float * FloatWrap::_neg(Float *n1){
	return new Float(-n1->n);
}

 Float * FloatWrap::_div(Float *n1, Float *n2){
	return new Float(n1->n / n2->n);
}

 Float * FloatWrap::_div(Float *n1, zs_float *n2){
	return new Float(n1->n / *n2);
}

 Float * FloatWrap::_div(zs_float *n1, Float *n2){
	return new Float(*n1 / n2->n);
}

 Float * FloatWrap::_mod(Float *n1, Float *n2){
	return new Float(fmod(n1->n ,n2->n));
}

 Float * FloatWrap::_mod(Float *n1, zs_float *n2){
	return new Float(fmod(n1->n, *n2));
}

 Float * FloatWrap::_mod(zs_float *n1, Float *n2){
	return new Float(fmod(*n1 ,n2->n));
}

 Float * FloatWrap::_mul(Float *n1, Float *n2){
	return new Float(n1->n * n2->n);
}

 Float * FloatWrap::_mul(Float *n1, zs_float *n2){
	return new Float(n1->n * *n2);
}

 Float * FloatWrap::_mul(zs_float *n1, Float *n2){
	return new Float(*n1 * n2->n);
}

 zs_float FloatWrap::toFloat(Float *_this){
	 return _this->n;

 }
