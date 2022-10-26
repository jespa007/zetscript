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

void NumberWrap_num_setter(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=*_value;
}

void  NumberWrap_num_post_inc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num++;
}

void  NumberWrap_num_post_dec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num--;
}

void  NumberWrap_num_pre_inc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->num;
}

void  NumberWrap_num_pre_dec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	--_this->num;
}

void NumberWrap_num_add_set(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num+=*_value;
}

void NumberWrap_num_sub_set(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num-=*_value;
}

void NumberWrap_num_mul_set(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num*=*_value;
}

void NumberWrap_num_div_set(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num/=*_value;
}

void NumberWrap_num_mod_set(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=fmod(_this->num,*_value);
}

void NumberWrap_num_xor_set(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)^(int)(*_value);
}

void NumberWrap_num_or_set(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)|(int)(*_value);
}

void NumberWrap_num_and_set(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)&(int)(*_value);
}

void NumberWrap_num_shl_set(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=(int)(_this->num)<<(int)(*_value);
}

void NumberWrap_num_shr_set(zetscript::ZetScript *_zs,Number *_this,zetscript::zs_float *_value){
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

void NumberWrap_add_set(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num+=*_n;
}

void NumberWrap_add_set(zetscript::ZetScript *_zs, Number *_this,Number *_n){
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

void NumberWrap_sub_set(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num-=*_n;
}

void NumberWrap_sub_set(zetscript::ZetScript *_zs, Number *_this,Number *_n){
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

void NumberWrap_div_set(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num/=*_n;
}

void NumberWrap_div_set(zetscript::ZetScript *_zs, Number *_this,Number *_n){
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

void NumberWrap_mod_set(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=fmod(_this->num,*_n);
}

void NumberWrap_mod_set(zetscript::ZetScript *_zs, Number *_this,Number *_n){
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

void NumberWrap_mul_set(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num*=*_n;
}

void NumberWrap_mul_set(zetscript::ZetScript *_zs, Number *_this,Number *_n){
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

void NumberWrap_shl_set(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) << ((zetscript::zs_int)*_n);
}

void NumberWrap_shl_set(zetscript::ZetScript *_zs, Number *_this,Number *_n){
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

void NumberWrap_and_set(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) & ((zetscript::zs_int)*_n);
}

void NumberWrap_and_set(zetscript::ZetScript *_zs, Number *_this,Number *_n){
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

void NumberWrap_or_set(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) | ((zetscript::zs_int)*_n);
}

void NumberWrap_or_set(zetscript::ZetScript *_zs, Number *_this,Number *_n){
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

void NumberWrap_xor_set(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) ^ ((zetscript::zs_int)*_n);
}

void NumberWrap_xor_set(zetscript::ZetScript *_zs, Number *_this,Number *_n){
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

void NumberWrap_shr_set(zetscript::ZetScript *_zs, Number *_this,zetscript::zs_float *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) >> ((zetscript::zs_int)*_n);
}

void NumberWrap_shr_set(zetscript::ZetScript *_zs, Number *_this,Number *_n){
	ZS_UNUSUED_PARAM(_zs);
	_this->num=((zetscript::zs_int)_this->num) >> ((zetscript::zs_int)_n->num);
}


void  NumberWrap_post_inc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num++;
}

void  NumberWrap_post_dec(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->num--;
}

void  NumberWrap_pre_inc(zetscript::ZetScript *_zs,Number *_this){
	ZS_UNUSUED_PARAM(_zs);
	++_this->num;
}

void  NumberWrap_pre_dec(zetscript::ZetScript *_zs,Number *_this){
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
	_zs->bindMemberPropertyAddSetter<Number>("num",NumberWrap_num_add_set);
	_zs->bindMemberPropertySubSetter<Number>("num",NumberWrap_num_sub_set);
	_zs->bindMemberPropertyMulSetter<Number>("num",NumberWrap_num_mul_set);
	_zs->bindMemberPropertyDivSetter<Number>("num",NumberWrap_num_div_set);
	_zs->bindMemberPropertyModSetter<Number>("num",NumberWrap_num_mod_set);
	_zs->bindMemberPropertyXorSetter<Number>("num",NumberWrap_num_xor_set);
	_zs->bindMemberPropertyOrSetter<Number>("num",NumberWrap_num_or_set);
	_zs->bindMemberPropertyAndSetter<Number>("num",NumberWrap_num_and_set);
	_zs->bindMemberPropertyShrSetter<Number>("num",NumberWrap_num_shr_set);
	_zs->bindMemberPropertyShlSetter<Number>("num",NumberWrap_num_shl_set);

	_zs->bindMemberPropertyPostInc<Number>("num",NumberWrap_num_post_inc);
	_zs->bindMemberPropertyPostDec<Number>("num",NumberWrap_num_post_dec);

	_zs->bindMemberPropertyPreInc<Number>("num",NumberWrap_num_pre_inc);
	_zs->bindMemberPropertyPreDec<Number>("num",NumberWrap_num_pre_dec);


	_zs->bindStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_add));
	_zs->bindStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_add));
	_zs->bindStaticMemberFunction<Number>("_add",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_add));

	// add set
	_zs->bindMemberFunction<Number>("_add_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_add_set));
	_zs->bindMemberFunction<Number>("_add_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_add_set));


	_zs->bindStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_sub));
	_zs->bindStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_sub));
	_zs->bindStaticMemberFunction<Number>("_sub",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_sub));

	// sub set
	_zs->bindMemberFunction<Number>("_sub_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_sub_set));
	_zs->bindMemberFunction<Number>("_sub_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_sub_set));

	_zs->bindStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_mul));
	_zs->bindStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mul));
	_zs->bindStaticMemberFunction<Number>("_mul",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mul));

	// mul set
	_zs->bindMemberFunction<Number>("_mul_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mul_set));
	_zs->bindMemberFunction<Number>("_mul_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mul_set));


	_zs->bindStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_div));
	_zs->bindStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_div));
	_zs->bindStaticMemberFunction<Number>("_div",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_div));

	// div set
	_zs->bindMemberFunction<Number>("_div_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_div_set));
	_zs->bindMemberFunction<Number>("_div_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_div_set));


	_zs->bindStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_mod));
	_zs->bindStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mod));
	_zs->bindStaticMemberFunction<Number>("_mod",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mod));

	// mod set
	_zs->bindMemberFunction<Number>("_mod_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_mod_set));
	_zs->bindMemberFunction<Number>("_mod_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_mod_set));


	_zs->bindStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_shr));
	_zs->bindStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float * )>(&NumberWrap_shr));
	_zs->bindStaticMemberFunction<Number>("_shr",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shr));

	// shr set
	_zs->bindMemberFunction<Number>("_shr_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_shr_set));
	_zs->bindMemberFunction<Number>("_shr_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shr_set));


	_zs->bindStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_shl));
	_zs->bindStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_shl));
	_zs->bindStaticMemberFunction<Number>("_shl",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shl));


	// shl set
	_zs->bindMemberFunction<Number>("_shl_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_shl_set));
	_zs->bindMemberFunction<Number>("_shl_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_shl_set));

	_zs->bindStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_and));
	_zs->bindStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_and));
	_zs->bindStaticMemberFunction<Number>("_and",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_and));

	// and set
	_zs->bindMemberFunction<Number>("_and_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_and_set));
	_zs->bindMemberFunction<Number>("_and_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_and_set));


	_zs->bindStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_or));
	_zs->bindStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_or));
	_zs->bindStaticMemberFunction<Number>("_or",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_or));

	// or set
	_zs->bindMemberFunction<Number>("_or_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_or_set));
	_zs->bindMemberFunction<Number>("_or_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_or_set));


	_zs->bindStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,zetscript::zs_float *,Number * )>(&NumberWrap_xor));
	_zs->bindStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_xor));
	_zs->bindStaticMemberFunction<Number>("_xor",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_xor));

	// xor set
	_zs->bindMemberFunction<Number>("_xor_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,zetscript::zs_float *)>(&NumberWrap_xor_set));
	_zs->bindMemberFunction<Number>("_xor_set",static_cast<void (*)(zetscript::ZetScript *_zs,Number *,Number * )>(&NumberWrap_xor_set));


	_zs->bindMemberFunction<Number>("_neg",static_cast<Number * (*)(zetscript::ZetScript *_zs,Number *)>(&NumberWrap_neg));

	_zs->bindMemberFunction<Number>("_post_inc",NumberWrap_post_inc);
	_zs->bindMemberFunction<Number>("_post_dec",NumberWrap_post_dec);

	_zs->bindMemberFunction<Number>("_pre_inc",NumberWrap_pre_inc);
	_zs->bindMemberFunction<Number>("_pre_dec",NumberWrap_pre_dec);
}


