/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "NumberZs.h"

Number *NumberZs_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new Number();
}

void NumberZs_set(zetscript::ZetScript *_zs,Number *_this, zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=*_n;
}

void NumberZs_set(zetscript::ZetScript *_zs,Number *_this,Number *_ci2){
	ZS_UNUSUED_PARAM(_zs);
	 _this->num = _ci2->num;
}

//--------------------------
// PROPERTY

zetscript::zs_float NumberZs_num_getter(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->num;
}

zetscript::zs_float NumberZs_num_neg(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return -_this->num;
}

zetscript::zs_float NumberZs_num_bwc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return ~((zetscript::zs_int)(_this->num));
}

void NumberZs_num_setter(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=*_value;
}

void  NumberZs_num_postinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num++;
}

void  NumberZs_num_postdec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num--;
}

void  NumberZs_num_preinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->num;
}

void  NumberZs_num_predec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	--_this->num;
}

void NumberZs_num_addset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num+=*_value;
}

void NumberZs_num_subset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num-=*_value;
}

void NumberZs_num_mulset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num*=*_value;
}

void NumberZs_num_divset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num/=*_value;
}

void NumberZs_num_modset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=fmod(_this->num,*_value);
}

void NumberZs_num_xorset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)^(int)(*_value);
}

void NumberZs_num_orset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)|(int)(*_value);
}

void NumberZs_numandset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)&(int)(*_value);
}

void NumberZs_num_shlset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)<<(int)(*_value);
}

void NumberZs_num_shrset(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)>>(int)(*_value);
}


//--------------------------
// METAMETHODS
Number * NumberZs_add(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num +_n2->num);
}

Number * NumberZs_add(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num + *_n2);
}

Number * NumberZs_add(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 + _n2->num);
}

void NumberZs_addset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num+=*_n;
}

void NumberZs_addset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num+=_n->num;
}

Number * NumberZs_sub(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num - _n2->num);
}

Number * NumberZs_sub(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num - *_n2);
}

Number * NumberZs_sub(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 - _n2->num);
}

void NumberZs_subset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num-=*_n;
}

void NumberZs_subset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num-=_n->num;
}

Number * NumberZs_div(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num / _n2->num);
}

Number * NumberZs_div(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num / *_n2);
}

Number * NumberZs_div(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 / _n2->num);
}

void NumberZs_divset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num/=*_n;
}

void NumberZs_divset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num/=_n->num;
}

Number * NumberZs_neg(zetscript::ZetScript *_zs,Number *_n1){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(-_n1->num);
}

Number * NumberZs_mod(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(_n1->num,_n2->num));
}

Number * NumberZs_mod(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(_n1->num, *_n2));
}

Number * NumberZs_mod(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(fmod(*_n1, _n2->num));
}

void NumberZs_modset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=fmod(_this->num,*_n);
}

void NumberZs_modset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=fmod(_this->num,_n->num);
}

Number * NumberZs_mul(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num * _n2->num);
}

Number * NumberZs_mul(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(_n1->num * (*_n2));
}

Number * NumberZs_mul(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number(*_n1 * _n2->num);
}

void NumberZs_mulset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num*=*_n;
}

void NumberZs_mulset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num*=_n->num;
}

Number * NumberZs_shl(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num << (zetscript::zs_int)_n2->num);
}

Number * NumberZs_shl(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num << (zetscript::zs_int)*_n2);
}

Number * NumberZs_shl(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 << (zetscript::zs_int)_n2->num);
}

void NumberZs_shlset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) << ((zetscript::zs_int)*_n);
}

void NumberZs_shlset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) << ((zetscript::zs_int)_n->num);
}

Number * NumberZs_and(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num & (zetscript::zs_int)_n2->num);
}

Number * NumberZs_and(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num & (zetscript::zs_int)*_n2);
}

Number * NumberZs_and(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
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

Number * NumberZs_or(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num | (zetscript::zs_int)_n2->num);
}

Number * NumberZs_or(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num | (zetscript::zs_int)*_n2);
}

Number * NumberZs_or(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 | (zetscript::zs_int)_n2->num);
}

void NumberZs_orset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) | ((zetscript::zs_int)*_n);
}

void NumberZs_orset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) | ((zetscript::zs_int)_n->num);
}

Number * NumberZs_xor(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num ^ (zetscript::zs_int)_n2->num);
}

Number * NumberZs_xor(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num ^ (zetscript::zs_int)*_n2);
}

Number * NumberZs_xor(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number * _n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 ^ (zetscript::zs_int)_n2->num);
}

void NumberZs_xorset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) ^ ((zetscript::zs_int)*_n);
}

void NumberZs_xorset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) ^ ((zetscript::zs_int)_n->num);
}

Number * NumberZs_shr(zetscript::ZetScript *_zs,Number *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num >> (zetscript::zs_int)_n2->num);
}

Number * NumberZs_shr(zetscript::ZetScript *_zs,Number *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)_n1->num >> (zetscript::zs_int)*_n2);
}

Number * NumberZs_shr(zetscript::ZetScript *_zs,zetscript::zs_float *_n1, Number *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return new Number((zetscript::zs_int)*_n1 >> (zetscript::zs_int)_n2->num);
}

void NumberZs_shrset(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) >> ((zetscript::zs_int)*_n);
}

void NumberZs_shrset(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) >> ((zetscript::zs_int)_n->num);
}


void  NumberZs_postinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num++;
}

void  NumberZs_postdec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num--;
}

void  NumberZs_preinc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->num;
}

void  NumberZs_predec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	--_this->num;
}


// METAMETHODS
//--------------------------------

zetscript::zs_int NumberZs_toInt(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->num;
}

zetscript::zs_float NumberZs_toFloat(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->num;
}

void NumberZs_delete(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}


void NumberZs_register(zetscript::ZetScript *_zs){

	_zs->registerType<Number>("Number",NumberZs_new,NumberZs_delete);
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_set));
	_zs->registerConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberZs_set));
	_zs->registerMemberFunction<Number>("toInt",&NumberZs_toInt);
	_zs->registerMemberFunction<Number>("toFloat",&NumberZs_toFloat);

	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *, zetscript::zs_float *)>(&NumberZs_set));
	_zs->registerMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberZs_set));

	_zs->registerMemberPropertySetter<Number>("num",NumberZs_num_setter);
	_zs->registerMemberPropertyGetter<Number>("num",NumberZs_num_getter);
	_zs->registerMemberPropertyNeg<Number>("num",NumberZs_num_neg);
	_zs->registerMemberPropertyBwc<Number>("num",NumberZs_num_bwc);
	_zs->registerMemberPropertyAddSetter<Number>("num",NumberZs_num_addset);
	_zs->registerMemberPropertySubSetter<Number>("num",NumberZs_num_subset);
	_zs->registerMemberPropertyMulSetter<Number>("num",NumberZs_num_mulset);
	_zs->registerMemberPropertyDivSetter<Number>("num",NumberZs_num_divset);
	_zs->registerMemberPropertyModSetter<Number>("num",NumberZs_num_modset);
	_zs->registerMemberPropertyXorSetter<Number>("num",NumberZs_num_xorset);
	_zs->registerMemberPropertyOrSetter<Number>("num",NumberZs_num_orset);
	_zs->registerMemberPropertyAndSetter<Number>("num",NumberZs_numandset);
	_zs->registerMemberPropertyShrSetter<Number>("num",NumberZs_num_shrset);
	_zs->registerMemberPropertyShlSetter<Number>("num",NumberZs_num_shlset);

	_zs->registerMemberPropertyPostInc<Number>("num",NumberZs_num_postinc);
	_zs->registerMemberPropertyPostDec<Number>("num",NumberZs_num_postdec);

	_zs->registerMemberPropertyPreInc<Number>("num",NumberZs_num_preinc);
	_zs->registerMemberPropertyPreDec<Number>("num",NumberZs_num_predec);


	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_add));
	_zs->registerStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_add));

	// add set
	_zs->registerMemberFunction<Number>("_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_addset));
	_zs->registerMemberFunction<Number>("_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_addset));


	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_sub));
	_zs->registerStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_sub));

	// sub set
	_zs->registerMemberFunction<Number>("_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_subset));
	_zs->registerMemberFunction<Number>("_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_subset));

	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_mul));
	_zs->registerStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_mul));

	// mul set
	_zs->registerMemberFunction<Number>("_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_mulset));
	_zs->registerMemberFunction<Number>("_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_mulset));


	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_div));
	_zs->registerStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_div));

	// div set
	_zs->registerMemberFunction<Number>("_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_divset));
	_zs->registerMemberFunction<Number>("_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_divset));


	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_mod));
	_zs->registerStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_mod));

	// mod set
	_zs->registerMemberFunction<Number>("_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_modset));
	_zs->registerMemberFunction<Number>("_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_modset));


	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float * )>(&NumberZs_shr));
	_zs->registerStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_shr));

	// shr set
	_zs->registerMemberFunction<Number>("_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_shrset));
	_zs->registerMemberFunction<Number>("_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_shrset));


	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_shl));
	_zs->registerStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_shl));


	// shl set
	_zs->registerMemberFunction<Number>("_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_shlset));
	_zs->registerMemberFunction<Number>("_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_shlset));

	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_and));
	_zs->registerStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_and));

	// and set
	_zs->registerMemberFunction<Number>("_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrapandset));
	_zs->registerMemberFunction<Number>("_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrapandset));


	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_or));
	_zs->registerStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_or));

	// or set
	_zs->registerMemberFunction<Number>("_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_orset));
	_zs->registerMemberFunction<Number>("_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_orset));


	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberZs_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_xor));
	_zs->registerStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_xor));

	// xor set
	_zs->registerMemberFunction<Number>("_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberZs_xorset));
	_zs->registerMemberFunction<Number>("_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberZs_xorset));


	_zs->registerMemberFunction<Number>("_neg",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *)>(&NumberZs_neg));

	_zs->registerMemberFunction<Number>("_postinc",NumberZs_postinc);
	_zs->registerMemberFunction<Number>("_postdec",NumberZs_postdec);

	_zs->registerMemberFunction<Number>("_preinc",NumberZs_preinc);
	_zs->registerMemberFunction<Number>("_predec",NumberZs_predec);
}


