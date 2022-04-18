#pragma once

MyClassExtend *MyClassExtendWrap_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new MyClassExtend;
}

zetscript::zs_int MyClassExtendWrap_get_data4(zetscript::ZetScript *_zs, MyClassExtend *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->data4;
}

void MyClassExtendWrap_set_data4(zetscript::ZetScript *_zs, MyClassExtend *_this, zetscript::zs_int v){
	ZS_UNUSUED_PARAM(_zs);
	_this->data4=v;
}

void MyClassExtendWrap_function2(zetscript::ZetScript *_zs, MyClassExtend *_this, zetscript::zs_float *f){
	ZS_UNUSUED_PARAM(_zs);
	_this->function2(*f);
}

void MyClassExtendWrap_delete(zetscript::ZetScript *_zs, MyClassExtend *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}





