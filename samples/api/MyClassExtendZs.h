#pragma once

MyClassExtend *MyClassExtendZs_new(zetscript::ScriptEngine *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new MyClassExtend;
}

zetscript::zs_int MyClassExtendZs_get_data4(zetscript::ScriptEngine *_zs, MyClassExtend *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->data4;
}

void MyClassExtendZs_set_data4(zetscript::ScriptEngine *_zs, MyClassExtend *_this, zetscript::zs_int v){
	ZS_UNUSUED_PARAM(_zs);
	_this->data4=v;
}

void MyClassExtendZs_function2(zetscript::ScriptEngine *_zs, MyClassExtend *_this, zetscript::zs_float *f){
	ZS_UNUSUED_PARAM(_zs);
	_this->function2(*f);
}

void MyClassExtendZs_delete(zetscript::ScriptEngine *_zs, MyClassExtend *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}





