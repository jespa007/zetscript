/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "NumberWrap.h"

Number *Number_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new Number();
}

void Number_set(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=*_n;
}

void Number_set(zetscript::ZetScript *_zs,Number *_this,Number *_ci2){
	ZS_UNUSUED_PARAM(_zs);
	 _this->num = _ci2->num;
}

//--------------------------
// PROPERTY

zetscript::zs_float Number_num_getter(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->num;
}

zetscript::zs_float Number_num_neg(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return -_this->num;
}

zetscript::zs_float Number_num_bwc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return ~((zetscript::zs_int)(_this->num));
}

void Number_num_setter(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=*_value;
}

void  Number_num_postinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num++;
}

void  Number_num_postdec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num--;
}

void  Number_num_preinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->num;
}

void  Number_num_predec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	--_this->num;
}

void Number_num_addset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num+=*_value;
}

void Number_num_subset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num-=*_value;
}

void Number_num_mulset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num*=*_value;
}

void Number_num_divset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num/=*_value;
}

void Number_num_modset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=fmod(_this->num,*_value);
}

void Number_num_xorset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)^(int)(*_value);
}

void Number_num_orset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)|(int)(*_value);
}

void Number_numandset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)&(int)(*_value);
}

void Number_num_shlset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)<<(int)(*_value);
}

void Number_num_shrset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)>>(int)(*_value);
}


//--------------------------
// METAMETHODS
Number * Number_add(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num +_n2->num);
}

Number * Number_add(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num + *_n2);
}

Number * Number_add(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 + _n2->num);
}

void Number_addset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num+=*_n;
}

void Number_addset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num+=_n->num;
}

Number * Number_sub(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num - _n2->num);
}

Number * Number_sub(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num - *_n2);
}

Number * Number_sub(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 - _n2->num);
}

void Number_subset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num-=*_n;
}

void Number_subset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num-=_n->num;
}

Number * Number_div(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num / _n2->num);
}

Number * Number_div(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num / *_n2);
}

Number * Number_div(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 / _n2->num);
}

void Number_divset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num/=*_n;
}

void Number_divset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num/=_n->num;
}

Number * Number_neg(zetscript::ZetScript *_zs,Number *_n1){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(-_n1->num);
}

Number * Number_mod(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(_n1->num,_n2->num));
}

Number * Number_mod(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(_n1->num, *_n2));
}

Number * Number_mod(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(*_n1, _n2->num));
}

void Number_modset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=fmod(_this->num,*_n);
}

void Number_modset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=fmod(_this->num,_n->num);
}

Number * Number_mul(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num * _n2->num);
}

Number * Number_mul(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num * (*_n2));
}

Number * Number_mul(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 * _n2->num);
}

void Number_mulset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num*=*_n;
}

void Number_mulset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num*=_n->num;
}

Number * Number_shl(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num << (zetscript::zs_int)_n2->num);
}

Number * Number_shl(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num << (zetscript::zs_int)*_n2);
}

Number * Number_shl(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 << (zetscript::zs_int)_n2->num);
}

void Number_shlset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) << ((zetscript::zs_int)*_n);
}

void Number_shlset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) << ((zetscript::zs_int)_n->num);
}

Number * Number_and(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num & (zetscript::zs_int)_n2->num);
}

Number * Number_and(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num & (zetscript::zs_int)*_n2);
}

Number * Number_and(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 & (zetscript::zs_int)_n2->num);
}

void NumberWrapandset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) & ((zetscript::zs_int)*_n);
}

void NumberWrapandset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) & ((zetscript::zs_int)_n->num);
}

Number * Number_or(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num | (zetscript::zs_int)_n2->num);
}

Number * Number_or(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num | (zetscript::zs_int)*_n2);
}

Number * Number_or(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 | (zetscript::zs_int)_n2->num);
}

void Number_orset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) | ((zetscript::zs_int)*_n);
}

void Number_orset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) | ((zetscript::zs_int)_n->num);
}

Number * Number_xor(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num ^ (zetscript::zs_int)_n2->num);
}

Number * Number_xor(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num ^ (zetscript::zs_int)*_n2);
}

Number * Number_xor(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 ^ (zetscript::zs_int)_n2->num);
}

void Number_xorset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) ^ ((zetscript::zs_int)*_n);
}

void Number_xorset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) ^ ((zetscript::zs_int)_n->num);
}

Number * Number_shr(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num >> (zetscript::zs_int)_n2->num);
}

Number * Number_shr(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num >> (zetscript::zs_int)*_n2);
}

Number * Number_shr(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 >> (zetscript::zs_int)_n2->num);
}

void Number_shrset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) >> ((zetscript::zs_int)*_n);
}

void Number_shrset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) >> ((zetscript::zs_int)_n->num);
}


void  Number_postinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num++;
}

void  Number_postdec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num--;
}

void  Number_preinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->num;
}

void  Number_predec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	--_this->num;
}


// METAMETHODS
//--------------------------------

zetscript::zs_int Number_toInt(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->num;
}

zetscript::zs_float Number_toFloat(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->num;
}

void Number_delete(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}


void Number_register(zetscript::ZetScript *_zs){

	_zs->registerClass<Number>("Number",Number_new,Number_delete);
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_set));
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&Number_set));
	_zs->registerMemberFunction<Number>("toInt",&Number_toInt);
	_zs->registerMemberFunction<Number>("toFloat",&Number_toFloat);

	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *, zetscript::zs_float *)>(&Number_set));
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&Number_set));

	_zs->registerMemberPropertySetter<Number>("num",Number_num_setter);
	_zs->registerMemberPropertyGetter<Number>("num",Number_num_getter);
	_zs->registerMemberPropertyNeg<Number>("num",Number_num_neg);
	_zs->registerMemberPropertyBwc<Number>("num",Number_num_bwc);
	_zs->registerMemberPropertyAddSetter<Number>("num",Number_num_addset);
	_zs->registerMemberPropertySubSetter<Number>("num",Number_num_subset);
	_zs->registerMemberPropertyMulSetter<Number>("num",Number_num_mulset);
	_zs->registerMemberPropertyDivSetter<Number>("num",Number_num_divset);
	_zs->registerMemberPropertyModSetter<Number>("num",Number_num_modset);
	_zs->registerMemberPropertyXorSetter<Number>("num",Number_num_xorset);
	_zs->registerMemberPropertyOrSetter<Number>("num",Number_num_orset);
	_zs->registerMemberPropertyAndSetter<Number>("num",Number_numandset);
	_zs->registerMemberPropertyShrSetter<Number>("num",Number_num_shrset);
	_zs->registerMemberPropertyShlSetter<Number>("num",Number_num_shlset);

	_zs->registerMemberPropertyPostInc<Number>("num",Number_num_postinc);
	_zs->registerMemberPropertyPostDec<Number>("num",Number_num_postdec);

	_zs->registerMemberPropertyPreInc<Number>("num",Number_num_preinc);
	_zs->registerMemberPropertyPreDec<Number>("num",Number_num_predec);


	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&Number_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_add));

	// add set
	_zs->registerMemberFunction<Number>("_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_addset));
	_zs->registerMemberFunction<Number>("_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_addset));


	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&Number_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_sub));

	// sub set
	_zs->registerMemberFunction<Number>("_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_subset));
	_zs->registerMemberFunction<Number>("_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_subset));

	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&Number_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_mul));

	// mul set
	_zs->registerMemberFunction<Number>("_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_mulset));
	_zs->registerMemberFunction<Number>("_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_mulset));


	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&Number_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_div));

	// div set
	_zs->registerMemberFunction<Number>("_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_divset));
	_zs->registerMemberFunction<Number>("_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_divset));


	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&Number_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_mod));

	// mod set
	_zs->registerMemberFunction<Number>("_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_modset));
	_zs->registerMemberFunction<Number>("_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_modset));


	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&Number_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float * )>(&Number_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_shr));

	// shr set
	_zs->registerMemberFunction<Number>("_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_shrset));
	_zs->registerMemberFunction<Number>("_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_shrset));


	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&Number_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_shl));


	// shl set
	_zs->registerMemberFunction<Number>("_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_shlset));
	_zs->registerMemberFunction<Number>("_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_shlset));

	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&Number_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_and));

	// and set
	_zs->registerMemberFunction<Number>("_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrapandset));
	_zs->registerMemberFunction<Number>("_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrapandset));


	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&Number_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_or));

	// or set
	_zs->registerMemberFunction<Number>("_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_orset));
	_zs->registerMemberFunction<Number>("_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_orset));


	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&Number_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_xor));

	// xor set
	_zs->registerMemberFunction<Number>("_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&Number_xorset));
	_zs->registerMemberFunction<Number>("_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&Number_xorset));


	_zs->registerMemberFunction<Number>("_neg",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *)>(&Number_neg));

	_zs->registerMemberFunction<Number>("_postinc",Number_postinc);
	_zs->registerMemberFunction<Number>("_postdec",Number_postdec);

	_zs->registerMemberFunction<Number>("_preinc",Number_preinc);
	_zs->registerMemberFunction<Number>("_predec",Number_predec);
}


