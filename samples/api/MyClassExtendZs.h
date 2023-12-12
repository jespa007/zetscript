#pragma once

MyClassExtend *MyClassExtendZs_new(zetscript::ScriptEngine *_se){
	ZS_UNUSUED_PARAM(_se);
	return new MyClassExtend;
}

zetscript::zs_int MyClassExtendZs_get_data4(zetscript::ScriptEngine *_se, MyClassExtend *_this){
	ZS_UNUSUED_PARAM(_se);
	return _this->data4;
}

void MyClassExtendZs_set_data4(zetscript::ScriptEngine *_se, MyClassExtend *_this, zetscript::zs_int v){
	ZS_UNUSUED_PARAM(_se);
	_this->data4=v;
}

void MyClassExtendZs_function2(zetscript::ScriptEngine *_se, MyClassExtend *_this, zetscript::zs_float *f){
	ZS_UNUSUED_PARAM(_se);
	_this->function2(*f);
}

void MyClassExtendZs_delete(zetscript::ScriptEngine *_se, MyClassExtend *_this){
	ZS_UNUSUED_PARAM(_se);
	delete _this;
}





