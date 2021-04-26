
#include "IntegerWrap.h"


Integer *IntegerWrap::_new(){
	return new Integer();

}
void IntegerWrap::_delete(Integer *_integer){
	delete _integer;
}


 void IntegerWrap::_set(Integer *_this, zs_int _n){
	_this->n=_n;
}


 void IntegerWrap::_set(Integer *_this,Integer *_ci2){
	 _this->n = _ci2->n;
}


 Integer * IntegerWrap::_add(Integer *n1, Integer *n2){
	return new Integer(n1->n + n2->n);
}

 Integer * IntegerWrap::_add(Integer *n1, zs_float n2){
	return new Integer((int)(n1->n + n2));
}

 Integer * IntegerWrap::_add(Integer *n1, zs_int n2){
	return new Integer(n1->n + n2);
}

 Integer * IntegerWrap::_add(zs_int n1, Integer * n2){
	return new Integer(n1 + n2->n);
}


 Integer * IntegerWrap::_sub(Integer *n1, Integer *n2){
	return new Integer(n1->n - n2->n);
}

 Integer * IntegerWrap::_sub(Integer *n1, zs_float n2){
	return new Integer((int)(n1->n - n2));
}

 Integer * IntegerWrap::_sub(Integer *n1, zs_int n2){
	return new Integer(n1->n - n2);
}

 Integer * IntegerWrap::_sub(zs_int n1, Integer * n2){
	return new Integer(n1 - n2->n);
}

 Integer * IntegerWrap::_div(Integer *n1, Integer *n2){
	return new Integer(n1->n / n2->n);
}

 Integer * IntegerWrap::_div(Integer *n1, zs_int n2){
	return new Integer(n1->n / n2);
}

 Integer * IntegerWrap::_div(zs_int n1, Integer *n2){
	return new Integer(n1 / n2->n);
}



 Integer * IntegerWrap::_neg(Integer *n1){
	return new Integer(-n1->n);
}


 Integer * IntegerWrap::_mod(Integer *n1, Integer *n2){
	return new Integer(n1->n % n2->n);
}

 Integer * IntegerWrap::_mod(Integer *n1, zs_int n2){
	return new Integer(fmod(n1->n, n2));
}

 Integer * IntegerWrap::_mod(zs_int n1, Integer *n2){
	return new Integer(n1 % n2->n);
}

 Integer * IntegerWrap::_mul(Integer *n1, Integer *n2){
	return new Integer(n1->n * n2->n);
}

 Integer * IntegerWrap::_mul(Integer *n1, zs_int n2){
	return new Integer(n1->n * n2);
}

 Integer * IntegerWrap::_mul(zs_int n1, Integer * n2){
	return new Integer(n1 * n2->n);
}

 Integer * IntegerWrap::_shl(Integer *n1, Integer *n2){
	return new Integer(n1->n << n2->n);
}

 Integer * IntegerWrap::_shl(Integer *n1, zs_int n2){
	return new Integer(n1->n << n2);
}

 Integer * IntegerWrap::_shl(zs_int n1, Integer *n2){
	return new Integer(n1 << n2->n);
}

 Integer * IntegerWrap::_and(Integer *n1, Integer *n2){
	return new Integer(n1->n & n2->n);
}

 Integer * IntegerWrap::_and(Integer *n1, zs_int n2){
	return new Integer(n1->n & n2);
}

 Integer * IntegerWrap::_and(zs_int n1, Integer * n2){
	return new Integer(n1 & n2->n);
}

 Integer * IntegerWrap::_or(Integer *n1, Integer *n2){
	return new Integer(n1->n | n2->n);
}

 Integer * IntegerWrap::_or(Integer *n1, zs_int n2){
	return new Integer(n1->n | n2);
}

 Integer * IntegerWrap::_or(zs_int n1, Integer * n2){
	return new Integer(n1 | n2->n);
}


 Integer * IntegerWrap::_xor(Integer *n1, Integer *n2){
	return new Integer(n1->n ^ n2->n);
}

 Integer * IntegerWrap::_xor(Integer *n1, zs_int n2){
	return new Integer(n1->n ^ n2);
}

 Integer * IntegerWrap::_xor(zs_int n1, Integer * n2){
	return new Integer(n1 ^ n2->n);
}


 Integer * IntegerWrap::_shr(Integer *n1, Integer *n2){
	return new Integer(n1->n >> n2->n);
}

 Integer * IntegerWrap::_shr(Integer *n1, zs_int n2){
	return new Integer(n1->n >> n2);
}

 Integer * IntegerWrap::_shr(zs_int n1, Integer *n2){
	return new Integer(n1 >> n2->n);
}


 zs_int IntegerWrap::toInt(Integer *_this){
	return _this->n;
}

