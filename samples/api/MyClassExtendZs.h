#pragma once

MyClassExtend *MyClassExtendZs_new(zetscript::ScriptEngine *_script_engine){
	ZS_UNUSUED_PARAM(_script_engine);
	return new MyClassExtend;
}

zetscript::zs_int MyClassExtendZs_get_data4(zetscript::ScriptEngine *_script_engine, MyClassExtend *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return _this->data4;
}

void MyClassExtendZs_set_data4(zetscript::ScriptEngine *_script_engine, MyClassExtend *_this, zetscript::zs_int v){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->data4=v;
}

void MyClassExtendZs_function2(zetscript::ScriptEngine *_script_engine, MyClassExtend *_this, zetscript::zs_float *f){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->function2(*f);
}

void MyClassExtendZs_delete(zetscript::ScriptEngine *_script_engine, MyClassExtend *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	delete _this;
}





