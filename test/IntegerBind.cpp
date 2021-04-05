
#include "IntegerBind.h"


 void IntegerBind::_set(Integer *_this, zs_int _n){
	_this->n=_n;
}


 void IntegerBind::_set(Integer *_this,Integer *_ci2){
	 _this->n = _ci2->n;
}


 Integer * IntegerBind::_add(Integer *n1, Integer *n2){
	return new Integer(n1->n + n2->n);
}

 Integer * IntegerBind::_add(Integer *n1, zs_float n2){
	return new Integer((int)(n1->n + n2));
}

 Integer * IntegerBind::_add(Integer *n1, zs_int n2){
	return new Integer(n1->n + n2);
}

 Integer * IntegerBind::_add(zs_int n1, Integer * n2){
	return new Integer(n1 + n2->n);
}


 Integer * IntegerBind::_sub(Integer *n1, Integer *n2){
	return new Integer(n1->n - n2->n);
}

 Integer * IntegerBind::_sub(Integer *n1, zs_float n2){
	return new Integer((int)(n1->n - n2));
}

 Integer * IntegerBind::_sub(Integer *n1, zs_int n2){
	return new Integer(n1->n - n2);
}

 Integer * IntegerBind::_sub(zs_int n1, Integer * n2){
	return new Integer(n1 - n2->n);
}

 Integer * IntegerBind::_div(Integer *n1, Integer *n2){
	return new Integer(n1->n / n2->n);
}

 Integer * IntegerBind::_div(Integer *n1, zs_int n2){
	return new Integer(n1->n / n2);
}

 Integer * IntegerBind::_div(zs_int n1, Integer *n2){
	return new Integer(n1 / n2->n);
}



 Integer * IntegerBind::_neg(Integer *n1){
	return new Integer(-n1->n);
}


 Integer * IntegerBind::_mod(Integer *n1, Integer *n2){
	return new Integer(n1->n % n2->n);
}

 Integer * IntegerBind::_mod(Integer *n1, zs_int n2){
	return new Integer(fmod(n1->n, n2));
}

 Integer * IntegerBind::_mod(zs_int n1, Integer *n2){
	return new Integer(n1 % n2->n);
}

 Integer * IntegerBind::_mul(Integer *n1, Integer *n2){
	return new Integer(n1->n * n2->n);
}

 Integer * IntegerBind::_mul(Integer *n1, zs_int n2){
	return new Integer(n1->n * n2);
}

 Integer * IntegerBind::_mul(zs_int n1, Integer * n2){
	return new Integer(n1 * n2->n);
}

 Integer * IntegerBind::_shl(Integer *n1, Integer *n2){
	return new Integer(n1->n << n2->n);
}

 Integer * IntegerBind::_shl(Integer *n1, zs_int n2){
	return new Integer(n1->n << n2);
}

 Integer * IntegerBind::_shl(zs_int n1, Integer *n2){
	return new Integer(n1 << n2->n);
}

 Integer * IntegerBind::_and(Integer *n1, Integer *n2){
	return new Integer(n1->n & n2->n);
}

 Integer * IntegerBind::_and(Integer *n1, zs_int n2){
	return new Integer(n1->n & n2);
}

 Integer * IntegerBind::_and(zs_int n1, Integer * n2){
	return new Integer(n1 & n2->n);
}

 Integer * IntegerBind::_or(Integer *n1, Integer *n2){
	return new Integer(n1->n | n2->n);
}

 Integer * IntegerBind::_or(Integer *n1, zs_int n2){
	return new Integer(n1->n | n2);
}

 Integer * IntegerBind::_or(zs_int n1, Integer * n2){
	return new Integer(n1 | n2->n);
}


 Integer * IntegerBind::_xor(Integer *n1, Integer *n2){
	return new Integer(n1->n ^ n2->n);
}

 Integer * IntegerBind::_xor(Integer *n1, zs_int n2){
	return new Integer(n1->n ^ n2);
}

 Integer * IntegerBind::_xor(zs_int n1, Integer * n2){
	return new Integer(n1 ^ n2->n);
}


 Integer * IntegerBind::_shr(Integer *n1, Integer *n2){
	return new Integer(n1->n >> n2->n);
}

 Integer * IntegerBind::_shr(Integer *n1, zs_int n2){
	return new Integer(n1->n >> n2);
}

 Integer * IntegerBind::_shr(zs_int n1, Integer *n2){
	return new Integer(n1 >> n2->n);
}


 zs_int IntegerBind::toInt(Integer *_this){
	return _this->n;
}

