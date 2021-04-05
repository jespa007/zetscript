#include "FloatBind.h"



 void FloatBind::_set(Float *_this,zs_float *_n){
	_this->n=*_n;
}


 void FloatBind::_set(Float *_this, Float * _n){
	_this->n=_n->n;
}


 Float * FloatBind::_add(Float *n1, Float *n2){
	return new Float(n1->n + n2->n);
}

 Float * FloatBind::_add(Float *n1, zs_float *n2){
	return new Float(n1->n + *n2);
}

 Float * FloatBind::_add(zs_float *n1, Float *n2){
	return new Float(*n1 + n2->n);
}

 Float * FloatBind::_sub(Float *n1, Float *n2){
	return new Float(n1->n - n2->n);
}

 Float * FloatBind::_sub(Float *n1, zs_float *n2){
	return new Float(n1->n - *n2);
}

 Float * FloatBind::_sub(zs_float *n1, Float *n2){
	return new Float(*n1 - n2->n);
}

 Float * FloatBind::_neg(Float *n1){
	return new Float(-n1->n);
}

 Float * FloatBind::_div(Float *n1, Float *n2){
	return new Float(n1->n / n2->n);
}

 Float * FloatBind::_div(Float *n1, zs_float *n2){
	return new Float(n1->n / *n2);
}

 Float * FloatBind::_div(zs_float *n1, Float *n2){
	return new Float(*n1 / n2->n);
}

 Float * FloatBind::_mod(Float *n1, Float *n2){
	return new Float(fmod(n1->n ,n2->n));
}

 Float * FloatBind::_mod(Float *n1, zs_float *n2){
	return new Float(fmod(n1->n, *n2));
}

 Float * FloatBind::_mod(zs_float *n1, Float *n2){
	return new Float(fmod(*n1 ,n2->n));
}

 Float * FloatBind::_mul(Float *n1, Float *n2){
	return new Float(n1->n * n2->n);
}

 Float * FloatBind::_mul(Float *n1, zs_float *n2){
	return new Float(n1->n * *n2);
}

 Float * FloatBind::_mul(zs_float *n1, Float *n2){
	return new Float(*n1 * n2->n);
}

 zs_float FloatBind::toFloat(Float *_this){
	 return _this->n;

 }
