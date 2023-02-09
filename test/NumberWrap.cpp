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

	_zs->bindType<Number>("Number",NumberWrap_new,NumberWrap_delete);
	_zs->bindConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_set));
	_zs->bindConstructor<Number>(static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberWrap_set));
	_zs->bindMemberFunction<Number>("toInt",&NumberWrap_toInt);
	_zs->bindMemberFunction<Number>("toFloat",&NumberWrap_toFloat);

	_zs->bindMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *, zetscript::zs_float *)>(&NumberWrap_set));
	_zs->bindMemberFunction<Number>("_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number *)>(&NumberWrap_set));

	_zs->bindMemberPropertySetter<Number>("num",NumberWrap_num_setter);
	_zs->bindMemberPropertyGetter<Number>("num",NumberWrap_num_getter);
	_zs->bindMemberPropertyNeg<Number>("num",NumberWrap_num_neg);
	_zs->bindMemberPropertyBwc<Number>("num",NumberWrap_num_bwc);
	_zs->bindMemberPropertyAddSetter<Number>("num",NumberWrap_num_addset);
	_zs->bindMemberPropertySubSetter<Number>("num",NumberWrap_num_subset);
	_zs->bindMemberPropertyMulSetter<Number>("num",NumberWrap_num_mulset);
	_zs->bindMemberPropertyDivSetter<Number>("num",NumberWrap_num_divset);
	_zs->bindMemberPropertyModSetter<Number>("num",NumberWrap_num_modset);
	_zs->bindMemberPropertyXorSetter<Number>("num",NumberWrap_num_xorset);
	_zs->bindMemberPropertyOrSetter<Number>("num",NumberWrap_num_orset);
	_zs->bindMemberPropertyAndSetter<Number>("num",NumberWrap_numandset);
	_zs->bindMemberPropertyShrSetter<Number>("num",NumberWrap_num_shrset);
	_zs->bindMemberPropertyShlSetter<Number>("num",NumberWrap_num_shlset);

	_zs->bindMemberPropertyPostInc<Number>("num",NumberWrap_num_postinc);
	_zs->bindMemberPropertyPostDec<Number>("num",NumberWrap_num_postdec);

	_zs->bindMemberPropertyPreInc<Number>("num",NumberWrap_num_preinc);
	_zs->bindMemberPropertyPreDec<Number>("num",NumberWrap_num_predec);


	_zs->bindStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_add));
	_zs->bindStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_add));
	_zs->bindStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_add));

	// add set
	_zs->bindMemberFunction<Number>("_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_addset));
	_zs->bindMemberFunction<Number>("_addset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_addset));


	_zs->bindStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_sub));
	_zs->bindStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_sub));
	_zs->bindStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_sub));

	// sub set
	_zs->bindMemberFunction<Number>("_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_subset));
	_zs->bindMemberFunction<Number>("_subset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_subset));

	_zs->bindStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_mul));
	_zs->bindStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mul));
	_zs->bindStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mul));

	// mul set
	_zs->bindMemberFunction<Number>("_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mulset));
	_zs->bindMemberFunction<Number>("_mulset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mulset));


	_zs->bindStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_div));
	_zs->bindStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_div));
	_zs->bindStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_div));

	// div set
	_zs->bindMemberFunction<Number>("_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_divset));
	_zs->bindMemberFunction<Number>("_divset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_divset));


	_zs->bindStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_mod));
	_zs->bindStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mod));
	_zs->bindStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mod));

	// mod set
	_zs->bindMemberFunction<Number>("_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_modset));
	_zs->bindMemberFunction<Number>("_modset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_modset));


	_zs->bindStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_shr));
	_zs->bindStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float * )>(&NumberWrap_shr));
	_zs->bindStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shr));

	// shr set
	_zs->bindMemberFunction<Number>("_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_shrset));
	_zs->bindMemberFunction<Number>("_shrset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shrset));


	_zs->bindStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_shl));
	_zs->bindStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_shl));
	_zs->bindStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shl));


	// shl set
	_zs->bindMemberFunction<Number>("_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_shlset));
	_zs->bindMemberFunction<Number>("_shlset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shlset));

	_zs->bindStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_and));
	_zs->bindStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_and));
	_zs->bindStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_and));

	// and set
	_zs->bindMemberFunction<Number>("_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrapandset));
	_zs->bindMemberFunction<Number>("_andset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrapandset));


	_zs->bindStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_or));
	_zs->bindStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_or));
	_zs->bindStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_or));

	// or set
	_zs->bindMemberFunction<Number>("_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_orset));
	_zs->bindMemberFunction<Number>("_orset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_orset));


	_zs->bindStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_xor));
	_zs->bindStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_xor));
	_zs->bindStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_xor));

	// xor set
	_zs->bindMemberFunction<Number>("_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_xorset));
	_zs->bindMemberFunction<Number>("_xorset",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_xorset));


	_zs->bindMemberFunction<Number>("_neg",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *)>(&NumberWrap_neg));

	_zs->bindMemberFunction<Number>("_postinc",NumberWrap_postinc);
	_zs->bindMemberFunction<Number>("_postdec",NumberWrap_postdec);

	_zs->bindMemberFunction<Number>("_preinc",NumberWrap_preinc);
	_zs->bindMemberFunction<Number>("_predec",NumberWrap_predec);
}


