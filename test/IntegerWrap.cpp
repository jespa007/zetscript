
#include "IntegerWrap.h"

Integer *IntegerWrap_New(){
	return new Integer();

}
void IntegerWrap_Delete(Integer *_this){
	delete _this;
}

void IntegerWrap_Set(Integer *_this, zs_int _n){
	_this->n=_n;
}

void IntegerWrap_Set(Integer *_this,Integer *_ci2){
	 _this->n = _ci2->n;
}

Integer * IntegerWrap_Add(Integer *n1, Integer *n2){
	return new Integer(n1->n + n2->n);
}

Integer * IntegerWrap_Add(Integer *n1, zs_float n2){
	return new Integer((int)(n1->n + n2));
}

Integer * IntegerWrap_Add(Integer *n1, zs_int n2){
	return new Integer(n1->n + n2);
}

Integer * IntegerWrap_Add(zs_int n1, Integer * n2){
	return new Integer(n1 + n2->n);
}

Integer * IntegerWrap_Sub(Integer *n1, Integer *n2){
	return new Integer(n1->n - n2->n);
}

Integer * IntegerWrap_Sub(Integer *n1, zs_float n2){
	return new Integer((int)(n1->n - n2));
}

Integer * IntegerWrap_Sub(Integer *n1, zs_int n2){
	return new Integer(n1->n - n2);
}

Integer * IntegerWrap_Sub(zs_int n1, Integer * n2){
	return new Integer(n1 - n2->n);
}

Integer * IntegerWrap_Div(Integer *n1, Integer *n2){
	return new Integer(n1->n / n2->n);
}

Integer * IntegerWrap_Div(Integer *n1, zs_int n2){
	return new Integer(n1->n / n2);
}

Integer * IntegerWrap_Div(zs_int n1, Integer *n2){
	return new Integer(n1 / n2->n);
}

Integer * IntegerWrap_Neg(Integer *n1){
	return new Integer(-n1->n);
}

Integer * IntegerWrap_Mod(Integer *n1, Integer *n2){
	return new Integer(n1->n % n2->n);
}

Integer * IntegerWrap_Mod(Integer *n1, zs_int n2){
	return new Integer(fmod(n1->n, n2));
}

Integer * IntegerWrap_Mod(zs_int n1, Integer *n2){
	return new Integer(n1 % n2->n);
}

Integer * IntegerWrap_Mul(Integer *n1, Integer *n2){
	return new Integer(n1->n * n2->n);
}

Integer * IntegerWrap_Mul(Integer *n1, zs_int n2){
	return new Integer(n1->n * n2);
}

Integer * IntegerWrap_Mul(zs_int n1, Integer * n2){
	return new Integer(n1 * n2->n);
}

Integer * IntegerWrap_Shl(Integer *n1, Integer *n2){
	return new Integer(n1->n << n2->n);
}

Integer * IntegerWrap_Shl(Integer *n1, zs_int n2){
	return new Integer(n1->n << n2);
}

Integer * IntegerWrap_Shl(zs_int n1, Integer *n2){
	return new Integer(n1 << n2->n);
}

Integer * IntegerWrap_And(Integer *n1, Integer *n2){
	return new Integer(n1->n & n2->n);
}

Integer * IntegerWrap_And(Integer *n1, zs_int n2){
	return new Integer(n1->n & n2);
}

Integer * IntegerWrap_And(zs_int n1, Integer * n2){
	return new Integer(n1 & n2->n);
}

Integer * IntegerWrap_Or(Integer *n1, Integer *n2){
	return new Integer(n1->n | n2->n);
}

Integer * IntegerWrap_Or(Integer *n1, zs_int n2){
	return new Integer(n1->n | n2);
}

Integer * IntegerWrap_Or(zs_int n1, Integer * n2){
	return new Integer(n1 | n2->n);
}

Integer * IntegerWrap_Xor(Integer *n1, Integer *n2){
	return new Integer(n1->n ^ n2->n);
}

Integer * IntegerWrap_Xor(Integer *n1, zs_int n2){
	return new Integer(n1->n ^ n2);
}

Integer * IntegerWrap_Xor(zs_int n1, Integer * n2){
	return new Integer(n1 ^ n2->n);
}

Integer * IntegerWrap_Shr(Integer *n1, Integer *n2){
	return new Integer(n1->n >> n2->n);
}

Integer * IntegerWrap_Shr(Integer *n1, zs_int n2){
	return new Integer(n1->n >> n2);
}

Integer * IntegerWrap_Shr(zs_int n1, Integer *n2){
	return new Integer(n1 >> n2->n);
}

zs_int IntegerWrap_ToInt(Integer *_this){
	return _this->n;
}

