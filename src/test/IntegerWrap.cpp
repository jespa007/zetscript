/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
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


void IntegerWrap_register(ZetScript *_zs){

	_zs->registerClass<Integer>("Integer",IntegerWrap_new,IntegerWrap_delete);
	_zs->registerConstructor<Integer>(static_cast<void (*)(Integer *,zs_int )>(&IntegerWrap_set));
	_zs->registerConstructor<Integer>(static_cast<void (*)(Integer *,Integer *)>(&IntegerWrap_set));
	_zs->registerMemberFunction<Integer>("toInt",&IntegerWrap_toInt);

	_zs->registerMemberFunction<Integer>("_set",static_cast<void (*)(Integer *, zs_int)>(&IntegerWrap_set));
	_zs->registerMemberFunction<Integer>("_set",static_cast<void (*)(Integer *,Integer *)>(&IntegerWrap_set));


	_zs->registerMemberFunctionStatic<Integer>("_add",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_add));
	_zs->registerMemberFunctionStatic<Integer>("_add",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_add));
	_zs->registerMemberFunctionStatic<Integer>("_add",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_add));

	_zs->registerMemberFunctionStatic<Integer>("_sub",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_sub));
	_zs->registerMemberFunctionStatic<Integer>("_sub",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_sub));
	_zs->registerMemberFunctionStatic<Integer>("_sub",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_sub));


	_zs->registerMemberFunctionStatic<Integer>("_mul",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_mul));
	_zs->registerMemberFunctionStatic<Integer>("_mul",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_mul));
	_zs->registerMemberFunctionStatic<Integer>("_mul",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_mul));

	_zs->registerMemberFunctionStatic<Integer>("_div",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_div));
	_zs->registerMemberFunctionStatic<Integer>("_div",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_div));
	_zs->registerMemberFunctionStatic<Integer>("_div",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_div));

	_zs->registerMemberFunctionStatic<Integer>("_mod",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_mod));
	_zs->registerMemberFunctionStatic<Integer>("_mod",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_mod));
	_zs->registerMemberFunctionStatic<Integer>("_mod",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_mod));


	_zs->registerMemberFunctionStatic<Integer>("_shr",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_shr));
	_zs->registerMemberFunctionStatic<Integer>("_shr",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_shr));
	_zs->registerMemberFunctionStatic<Integer>("_shr",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_shr));


	_zs->registerMemberFunctionStatic<Integer>("_shl",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_shl));
	_zs->registerMemberFunctionStatic<Integer>("_shl",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_shl));
	_zs->registerMemberFunctionStatic<Integer>("_shl",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_shl));


	_zs->registerMemberFunctionStatic<Integer>("_and",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_and));
	_zs->registerMemberFunctionStatic<Integer>("_and",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_and));
	_zs->registerMemberFunctionStatic<Integer>("_and",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_and));

	_zs->registerMemberFunctionStatic<Integer>("_or",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_or));
	_zs->registerMemberFunctionStatic<Integer>("_or",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_or));
	_zs->registerMemberFunctionStatic<Integer>("_or",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_or));

	_zs->registerMemberFunctionStatic<Integer>("_xor",static_cast<Integer * (*)(zs_int,Integer * )>(&IntegerWrap_xor));
	_zs->registerMemberFunctionStatic<Integer>("_xor",static_cast<Integer * (*)(Integer *,zs_int)>(&IntegerWrap_xor));
	_zs->registerMemberFunctionStatic<Integer>("_xor",static_cast<Integer * (*)(Integer *,Integer * )>(&IntegerWrap_xor));

	_zs->registerMemberFunction<Integer>("_neg",static_cast<Integer * (*)(Integer *)>(&IntegerWrap_neg));
}
