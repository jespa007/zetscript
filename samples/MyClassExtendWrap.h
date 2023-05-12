#pragma once

MyClassExtend *MyClassExtend_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new MyClassExtend;
}

zetscript::zs_int MyClassExtend_get_data4(zetscript::ZetScript *_zs, MyClassExtend *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->data4;
}

void MyClassExtend_set_data4(zetscript::ZetScript *_zs, MyClassExtend *_this, zetscript::zs_int v){
	ZS_UNUSUED_PARAM(_zs);
	_this->data4=v;
}

void MyClassExtend_function2(zetscript::ZetScript *_zs, MyClassExtend *_this, zetscript::zs_float *f){
	ZS_UNUSUED_PARAM(_zs);
	_this->function2(*f);
}

void MyClassExtend_delete(zetscript::ZetScript *_zs, MyClassExtend *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}





