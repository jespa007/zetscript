/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "NumberWrap.h"

Number *NumberWrap_new(zetscript::ZetScript *_zs){
	return new Number();

}

void NumberWrap_set(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_int _n){
	_this->n=_n;
}

void NumberWrap_set(zetscript::ZetScript *_zs,Number *_this,Number *_ci2){
	 _this->n = _ci2->n;
}

Number * NumberWrap_add(zetscript::ZetScript *_zs,Number *n1, Number *n2){
	return new Number(n1->n + n2->n);
}

Number * NumberWrap_add(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_float n2){
	return new Number((int)(n1->n + n2));
}

Number * NumberWrap_add(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_int n2){
	return new Number(n1->n + n2);
}

Number * NumberWrap_add(zetscript::ZetScript *_zs,zetscript::zs_int n1, Number * n2){
	return new Number(n1 + n2->n);
}

Number * NumberWrap_sub(zetscript::ZetScript *_zs,Number *n1, Number *n2){
	return new Number(n1->n - n2->n);
}

Number * NumberWrap_sub(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_float n2){
	return new Number((int)(n1->n - n2));
}

Number * NumberWrap_sub(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_int n2){
	return new Number(n1->n - n2);
}

Number * NumberWrap_sub(zetscript::ZetScript *_zs,zetscript::zs_int n1, Number * n2){
	return new Number(n1 - n2->n);
}

Number * NumberWrap_div(zetscript::ZetScript *_zs,Number *n1, Number *n2){
	return new Number(n1->n / n2->n);
}

Number * NumberWrap_div(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_int n2){
	return new Number(n1->n / n2);
}

Number * NumberWrap_div(zetscript::ZetScript *_zs,zetscript::zs_int n1, Number *n2){
	return new Number(n1 / n2->n);
}

Number * NumberWrap_neg(zetscript::ZetScript *_zs,Number *n1){
	return new Number(-n1->n);
}

Number * NumberWrap_mod(zetscript::ZetScript *_zs,Number *n1, Number *n2){
	return new Number(fmod(n1->n,n2->n));
}

Number * NumberWrap_mod(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_int n2){
	return new Number(fmod(n1->n, n2));
}

Number * NumberWrap_mod(zetscript::ZetScript *_zs,zetscript::zs_int n1, Number *n2){
	return new Number(fmod(n1, n2->n));
}

Number * NumberWrap_mul(zetscript::ZetScript *_zs,Number *n1, Number *n2){
	return new Number(n1->n * n2->n);
}

Number * NumberWrap_mul(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_int n2){
	return new Number(n1->n * n2);
}

Number * NumberWrap_mul(zetscript::ZetScript *_zs,zetscript::zs_int n1, Number * n2){
	return new Number(n1 * n2->n);
}

Number * NumberWrap_shl(zetscript::ZetScript *_zs,Number *n1, Number *n2){
	return new Number((zetscript::zs_int)n1->n << (zetscript::zs_int)n2->n);
}

Number * NumberWrap_shl(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_int n2){
	return new Number((zetscript::zs_int)n1->n << n2);
}

Number * NumberWrap_shl(zetscript::ZetScript *_zs,zetscript::zs_int n1, Number *n2){
	return new Number(n1 << (zetscript::zs_int)n2->n);
}

Number * NumberWrap_and(zetscript::ZetScript *_zs,Number *n1, Number *n2){
	return new Number((zetscript::zs_int)n1->n & (zetscript::zs_int)n2->n);
}

Number * NumberWrap_and(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_int n2){
	return new Number((zetscript::zs_int)n1->n & n2);
}

Number * NumberWrap_and(zetscript::ZetScript *_zs,zetscript::zs_int n1, Number * n2){
	return new Number(n1 & (zetscript::zs_int)n2->n);
}

Number * NumberWrap_or(zetscript::ZetScript *_zs,Number *n1, Number *n2){
	return new Number((zetscript::zs_int)n1->n | (zetscript::zs_int)n2->n);
}

Number * NumberWrap_or(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_int n2){
	return new Number((zetscript::zs_int)n1->n | n2);
}

Number * NumberWrap_or(zetscript::ZetScript *_zs,zetscript::zs_int n1, Number * n2){
	return new Number(n1 | (zetscript::zs_int)n2->n);
}

Number * NumberWrap_xor(zetscript::ZetScript *_zs,Number *n1, Number *n2){
	return new Number((zetscript::zs_int)n1->n ^ (zetscript::zs_int)n2->n);
}

Number * NumberWrap_xor(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_int n2){
	return new Number((zetscript::zs_int)n1->n ^ (zetscript::zs_int)n2);
}

Number * NumberWrap_xor(zetscript::ZetScript *_zs,zetscript::zs_int n1, Number * n2){
	return new Number(n1 ^ (zetscript::zs_int)n2->n);
}

Number * NumberWrap_shr(zetscript::ZetScript *_zs,Number *n1, Number *n2){
	return new Number((zetscript::zs_int)n1->n >> (zetscript::zs_int)n2->n);
}

Number * NumberWrap_shr(zetscript::ZetScript *_zs,Number *n1, zetscript::zs_int n2){
	return new Number((zetscript::zs_int)n1->n >> n2);
}

Number * NumberWrap_shr(zetscript::ZetScript *_zs,zetscript::zs_int n1, Number *n2){
	return new Number(n1 >> (zetscript::zs_int)n2->n);
}

zetscript::zs_int NumberWrap_toInt(zetscript::ZetScript *_zs,Number *_this){
	return _this->n;
}

void NumberWrap_delete(zetscript::ZetScript *_zs,Number *_this){
	delete _this;
}


void NumberWrap_register(zetscript::ZetScript *_zs){

	_zs->registerClass<Number>("Number",NumberWrap_new,NumberWrap_delete);
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_int )>(&NumberWrap_set));
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberWrap_set));
	_zs->registerMemberFunction<Number>("toInt",&NumberWrap_toInt);

	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *, zetscript::zs_int)>(&NumberWrap_set));
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberWrap_set));


	_zs->registerMemberFunctionStatic<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_int,Number * )>(&NumberWrap_add));
	_zs->registerMemberFunctionStatic<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_int)>(&NumberWrap_add));
	_zs->registerMemberFunctionStatic<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_add));

	_zs->registerMemberFunctionStatic<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_int,Number * )>(&NumberWrap_sub));
	_zs->registerMemberFunctionStatic<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_int)>(&NumberWrap_sub));
	_zs->registerMemberFunctionStatic<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_sub));


	_zs->registerMemberFunctionStatic<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_int,Number * )>(&NumberWrap_mul));
	_zs->registerMemberFunctionStatic<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_int)>(&NumberWrap_mul));
	_zs->registerMemberFunctionStatic<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mul));

	_zs->registerMemberFunctionStatic<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_int,Number * )>(&NumberWrap_div));
	_zs->registerMemberFunctionStatic<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_int)>(&NumberWrap_div));
	_zs->registerMemberFunctionStatic<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_div));

	_zs->registerMemberFunctionStatic<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_int,Number * )>(&NumberWrap_mod));
	_zs->registerMemberFunctionStatic<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_int)>(&NumberWrap_mod));
	_zs->registerMemberFunctionStatic<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mod));


	_zs->registerMemberFunctionStatic<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_int,Number * )>(&NumberWrap_shr));
	_zs->registerMemberFunctionStatic<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_int)>(&NumberWrap_shr));
	_zs->registerMemberFunctionStatic<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shr));


	_zs->registerMemberFunctionStatic<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_int,Number * )>(&NumberWrap_shl));
	_zs->registerMemberFunctionStatic<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_int)>(&NumberWrap_shl));
	_zs->registerMemberFunctionStatic<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shl));


	_zs->registerMemberFunctionStatic<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_int,Number * )>(&NumberWrap_and));
	_zs->registerMemberFunctionStatic<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_int)>(&NumberWrap_and));
	_zs->registerMemberFunctionStatic<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_and));

	_zs->registerMemberFunctionStatic<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_int,Number * )>(&NumberWrap_or));
	_zs->registerMemberFunctionStatic<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_int)>(&NumberWrap_or));
	_zs->registerMemberFunctionStatic<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_or));

	_zs->registerMemberFunctionStatic<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_int,Number * )>(&NumberWrap_xor));
	_zs->registerMemberFunctionStatic<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_int)>(&NumberWrap_xor));
	_zs->registerMemberFunctionStatic<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_xor));

	_zs->registerMemberFunction<Number>("_neg",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *)>(&NumberWrap_neg));
}
