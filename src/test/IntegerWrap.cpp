
#include "IntegerWrap.h"

Integer *IntegerWrap_new(){
	return new Integer();

}

void IntegerWrap_set(Integer *_this, zs_int _n){
	_this->n=_n;
}

void IntegerWrap_set(Integer *_this,Integer *_ci2){
	 _this->n = _ci2->n;
}

Integer * IntegerWrap_add(Integer *n1, Integer *n2){
	return new Integer(n1->n + n2->n);
}

Integer * IntegerWrap_add(Integer *n1, zs_float n2){
	return new Integer((int)(n1->n + n2));
}

Integer * IntegerWrap_add(Integer *n1, zs_int n2){
	return new Integer(n1->n + n2);
}

Integer * IntegerWrap_add(zs_int n1, Integer * n2){
	return new Integer(n1 + n2->n);
}

Integer * IntegerWrap_sub(Integer *n1, Integer *n2){
	return new Integer(n1->n - n2->n);
}

Integer * IntegerWrap_sub(Integer *n1, zs_float n2){
	return new Integer((int)(n1->n - n2));
}

Integer * IntegerWrap_sub(Integer *n1, zs_int n2){
	return new Integer(n1->n - n2);
}

Integer * IntegerWrap_sub(zs_int n1, Integer * n2){
	return new Integer(n1 - n2->n);
}

Integer * IntegerWrap_div(Integer *n1, Integer *n2){
	return new Integer(n1->n / n2->n);
}

Integer * IntegerWrap_div(Integer *n1, zs_int n2){
	return new Integer(n1->n / n2);
}

Integer * IntegerWrap_div(zs_int n1, Integer *n2){
	return new Integer(n1 / n2->n);
}

Integer * IntegerWrap_neg(Integer *n1){
	return new Integer(-n1->n);
}

Integer * IntegerWrap_mod(Integer *n1, Integer *n2){
	return new Integer(n1->n % n2->n);
}

Integer * IntegerWrap_mod(Integer *n1, zs_int n2){
	return new Integer(fmod(n1->n, n2));
}

Integer * IntegerWrap_mod(zs_int n1, Integer *n2){
	return new Integer(n1 % n2->n);
}

Integer * IntegerWrap_mul(Integer *n1, Integer *n2){
	return new Integer(n1->n * n2->n);
}

Integer * IntegerWrap_mul(Integer *n1, zs_int n2){
	return new Integer(n1->n * n2);
}

Integer * IntegerWrap_mul(zs_int n1, Integer * n2){
	return new Integer(n1 * n2->n);
}

Integer * IntegerWrap_shl(Integer *n1, Integer *n2){
	return new Integer(n1->n << n2->n);
}

Integer * IntegerWrap_shl(Integer *n1, zs_int n2){
	return new Integer(n1->n << n2);
}

Integer * IntegerWrap_shl(zs_int n1, Integer *n2){
	return new Integer(n1 << n2->n);
}

Integer * IntegerWrap_and(Integer *n1, Integer *n2){
	return new Integer(n1->n & n2->n);
}

Integer * IntegerWrap_and(Integer *n1, zs_int n2){
	return new Integer(n1->n & n2);
}

Integer * IntegerWrap_and(zs_int n1, Integer * n2){
	return new Integer(n1 & n2->n);
}

Integer * IntegerWrap_or(Integer *n1, Integer *n2){
	return new Integer(n1->n | n2->n);
}

Integer * IntegerWrap_or(Integer *n1, zs_int n2){
	return new Integer(n1->n | n2);
}

Integer * IntegerWrap_or(zs_int n1, Integer * n2){
	return new Integer(n1 | n2->n);
}

Integer * IntegerWrap_xor(Integer *n1, Integer *n2){
	return new Integer(n1->n ^ n2->n);
}

Integer * IntegerWrap_xor(Integer *n1, zs_int n2){
	return new Integer(n1->n ^ n2);
}

Integer * IntegerWrap_xor(zs_int n1, Integer * n2){
	return new Integer(n1 ^ n2->n);
}

Integer * IntegerWrap_shr(Integer *n1, Integer *n2){
	return new Integer(n1->n >> n2->n);
}

Integer * IntegerWrap_shr(Integer *n1, zs_int n2){
	return new Integer(n1->n >> n2);
}

Integer * IntegerWrap_shr(zs_int n1, Integer *n2){
	return new Integer(n1 >> n2->n);
}

zs_int IntegerWrap_toInt(Integer *_this){
	return _this->n;
}

void IntegerWrap_delete(Integer *_this){
	delete _this;
}
