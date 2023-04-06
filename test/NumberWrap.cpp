/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "NumberWrap.h"

Number *NumberWrap_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new Number();
}

void NumberWrap_set(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=*_n;
}

void NumberWrap_set(zetscript::ZetScript *_zs,Number *_this,Number *_ci2){
	ZS_UNUSUED_PARAM(_zs);
	 _this->num = _ci2->num;
}

//--------------------------
// PROPERTY

zetscript::zs_float NumberWrap_num_getter(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->num;
}

zetscript::zs_float NumberWrap_num_neg(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return -_this->num;
}

zetscript::zs_float NumberWrap_num_bwc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return ~((zetscript::zs_int)(_this->num));
}

void NumberWrap_num_setter(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=*_value;
}

void  NumberWrap_num_postinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num++;
}

void  NumberWrap_num_postdec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num--;
}

void  NumberWrap_num_preinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->num;
}

void  NumberWrap_num_predec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	--_this->num;
}

void NumberWrap_num_addset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num+=*_value;
}

void NumberWrap_num_subset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num-=*_value;
}

void NumberWrap_num_mulset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num*=*_value;
}

void NumberWrap_num_divset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num/=*_value;
}

void NumberWrap_num_modset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=fmod(_this->num,*_value);
}

void NumberWrap_num_xorset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)^(int)(*_value);
}

void NumberWrap_num_orset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)|(int)(*_value);
}

void NumberWrap_numandset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)&(int)(*_value);
}

void NumberWrap_num_shlset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)<<(int)(*_value);
}

void NumberWrap_num_shrset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)>>(int)(*_value);
}


//--------------------------
// METAMETHODS
Number * NumberWrap_add(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num +_n2->num);
}

Number * NumberWrap_add(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num + *_n2);
}

Number * NumberWrap_add(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 + _n2->num);
}

void NumberWrap_addset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num+=*_n;
}

void NumberWrap_addset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num+=_n->num;
}

Number * NumberWrap_sub(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num - _n2->num);
}

Number * NumberWrap_sub(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num - *_n2);
}

Number * NumberWrap_sub(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 - _n2->num);
}

void NumberWrap_subset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num-=*_n;
}

void NumberWrap_subset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num-=_n->num;
}

Number * NumberWrap_div(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num / _n2->num);
}

Number * NumberWrap_div(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num / *_n2);
}

Number * NumberWrap_div(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 / _n2->num);
}

void NumberWrap_divset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num/=*_n;
}

void NumberWrap_divset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num/=_n->num;
}

Number * NumberWrap_neg(zetscript::ZetScript *_zs,Number *_n1){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(-_n1->num);
}

Number * NumberWrap_mod(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(_n1->num,_n2->num));
}

Number * NumberWrap_mod(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(_n1->num, *_n2));
}

Number * NumberWrap_mod(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(*_n1, _n2->num));
}

void NumberWrap_modset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=fmod(_this->num,*_n);
}

void NumberWrap_modset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=fmod(_this->num,_n->num);
}

Number * NumberWrap_mul(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num * _n2->num);
}

Number * NumberWrap_mul(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num * (*_n2));
}

Number * NumberWrap_mul(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 * _n2->num);
}

void NumberWrap_mulset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num*=*_n;
}

void NumberWrap_mulset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num*=_n->num;
}

Number * NumberWrap_shl(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num << (zetscript::zs_int)_n2->num);
}

Number * NumberWrap_shl(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num << (zetscript::zs_int)*_n2);
}

Number * NumberWrap_shl(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 << (zetscript::zs_int)_n2->num);
}

void NumberWrap_shlset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) << ((zetscript::zs_int)*_n);
}

void NumberWrap_shlset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) << ((zetscript::zs_int)_n->num);
}

Number * NumberWrap_and(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num & (zetscript::zs_int)_n2->num);
}

Number * NumberWrap_and(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num & (zetscript::zs_int)*_n2);
}

Number * NumberWrap_and(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
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

Number * NumberWrap_or(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num | (zetscript::zs_int)_n2->num);
}

Number * NumberWrap_or(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num | (zetscript::zs_int)*_n2);
}

Number * NumberWrap_or(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 | (zetscript::zs_int)_n2->num);
}

void NumberWrap_orset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) | ((zetscript::zs_int)*_n);
}

void NumberWrap_orset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) | ((zetscript::zs_int)_n->num);
}

Number * NumberWrap_xor(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num ^ (zetscript::zs_int)_n2->num);
}

Number * NumberWrap_xor(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num ^ (zetscript::zs_int)*_n2);
}

Number * NumberWrap_xor(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 ^ (zetscript::zs_int)_n2->num);
}

void NumberWrap_xorset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) ^ ((zetscript::zs_int)*_n);
}

void NumberWrap_xorset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) ^ ((zetscript::zs_int)_n->num);
}

Number * NumberWrap_shr(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num >> (zetscript::zs_int)_n2->num);
}

Number * NumberWrap_shr(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num >> (zetscript::zs_int)*_n2);
}

Number * NumberWrap_shr(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 >> (zetscript::zs_int)_n2->num);
}

void NumberWrap_shrset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) >> ((zetscript::zs_int)*_n);
}

void NumberWrap_shrset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) >> ((zetscript::zs_int)_n->num);
}


void  NumberWrap_postinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num++;
}

void  NumberWrap_postdec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num--;
}

void  NumberWrap_preinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->num;
}

void  NumberWrap_predec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	--_this->num;
}


// METAMETHODS
//--------------------------------

zetscript::zs_int NumberWrap_toInt(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->num;
}

zetscript::zs_float NumberWrap_toFloat(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->num;
}

void NumberWrap_delete(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}


void NumberWrap_register(zetscript::ZetScript *_zs){

	_zs->registerClass<Number>("Number",NumberWrap_new,NumberWrap_delete);
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_set));
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberWrap_set));
	_zs->registerMemberFunction<Number>("toInt",&NumberWrap_toInt);
	_zs->registerMemberFunction<Number>("toFloat",&NumberWrap_toFloat);

	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *, zetscript::zs_float *)>(&NumberWrap_set));
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberWrap_set));

	_zs->registerMemberPropertySetter<Number>("num",NumberWrap_num_setter);
	_zs->registerMemberPropertyGetter<Number>("num",NumberWrap_num_getter);
	_zs->registerMemberPropertyNeg<Number>("num",NumberWrap_num_neg);
	_zs->registerMemberPropertyBwc<Number>("num",NumberWrap_num_bwc);
	_zs->registerMemberPropertyAddSetter<Number>("num",NumberWrap_num_addset);
	_zs->registerMemberPropertySubSetter<Number>("num",NumberWrap_num_subset);
	_zs->registerMemberPropertyMulSetter<Number>("num",NumberWrap_num_mulset);
	_zs->registerMemberPropertyDivSetter<Number>("num",NumberWrap_num_divset);
	_zs->registerMemberPropertyModSetter<Number>("num",NumberWrap_num_modset);
	_zs->registerMemberPropertyXorSetter<Number>("num",NumberWrap_num_xorset);
	_zs->registerMemberPropertyOrSetter<Number>("num",NumberWrap_num_orset);
	_zs->registerMemberPropertyAndSetter<Number>("num",NumberWrap_numandset);
	_zs->registerMemberPropertyShrSetter<Number>("num",NumberWrap_num_shrset);
	_zs->registerMemberPropertyShlSetter<Number>("num",NumberWrap_num_shlset);

	_zs->registerMemberPropertyPostInc<Number>("num",NumberWrap_num_postinc);
	_zs->registerMemberPropertyPostDec<Number>("num",NumberWrap_num_postdec);

	_zs->registerMemberPropertyPreInc<Number>("num",NumberWrap_num_preinc);
	_zs->registerMemberPropertyPreDec<Number>("num",NumberWrap_num_predec);


	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_add));

	// add set
	_zs->registerMemberFunction<Number>("_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_addset));
	_zs->registerMemberFunction<Number>("_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_addset));


	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_sub));

	// sub set
	_zs->registerMemberFunction<Number>("_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_subset));
	_zs->registerMemberFunction<Number>("_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_subset));

	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mul));

	// mul set
	_zs->registerMemberFunction<Number>("_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mulset));
	_zs->registerMemberFunction<Number>("_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mulset));


	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_div));

	// div set
	_zs->registerMemberFunction<Number>("_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_divset));
	_zs->registerMemberFunction<Number>("_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_divset));


	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mod));

	// mod set
	_zs->registerMemberFunction<Number>("_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_modset));
	_zs->registerMemberFunction<Number>("_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_modset));


	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float * )>(&NumberWrap_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shr));

	// shr set
	_zs->registerMemberFunction<Number>("_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_shrset));
	_zs->registerMemberFunction<Number>("_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shrset));


	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shl));


	// shl set
	_zs->registerMemberFunction<Number>("_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_shlset));
	_zs->registerMemberFunction<Number>("_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shlset));

	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_and));

	// and set
	_zs->registerMemberFunction<Number>("_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrapandset));
	_zs->registerMemberFunction<Number>("_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrapandset));


	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_or));

	// or set
	_zs->registerMemberFunction<Number>("_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_orset));
	_zs->registerMemberFunction<Number>("_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_orset));


	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_xor));

	// xor set
	_zs->registerMemberFunction<Number>("_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_xorset));
	_zs->registerMemberFunction<Number>("_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_xorset));


	_zs->registerMemberFunction<Number>("_neg",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *)>(&NumberWrap_neg));

	_zs->registerMemberFunction<Number>("_postinc",NumberWrap_postinc);
	_zs->registerMemberFunction<Number>("_postdec",NumberWrap_postdec);

	_zs->registerMemberFunction<Number>("_preinc",NumberWrap_preinc);
	_zs->registerMemberFunction<Number>("_predec",NumberWrap_predec);
}


