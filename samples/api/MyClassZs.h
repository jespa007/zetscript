#pragma once


MyClass *MyClassZs_new(zetscript::ScriptEngine *_script_engine){
	ZS_UNUSUED_PARAM(_script_engine);
	return new MyClass;
}

//--------------------------------------------------------
// GET/SET for data1 (read & write)
void MyClassZs_set_data1(zetscript::ScriptEngine *_script_engine,MyClass *_this, zetscript::zs_int v){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->data1=v;
}

zetscript::zs_int MyClassZs_get_data1(zetscript::ScriptEngine *_script_engine,MyClass *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return _this->data1;
}
//--------------------------------------------------------
// GET for data2 (only read)
zetscript::zs_int MyClassZs_get_data2(zetscript::ScriptEngine *_script_engine,MyClass *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return _this->data2;
}
//--------------------------------------------------------
// SET for data3 (only write)
void MyClassZs_set_data3(zetscript::ScriptEngine *_script_engine,MyClass *_this, zetscript::zs_int v){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->data3=v;
}

void MyClassZs_function0(zetscript::ScriptEngine *_script_engine,MyClass *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->function0();
}

// register function1 named function1 in script side as function member.
void MyClassZs_function1(zetscript::ScriptEngine *_script_engine,MyClass *_this, zetscript::zs_int v){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->function1(v);
}

void MyClassZs_delete(zetscript::ScriptEngine *_script_engine,MyClass *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	delete _this;
}
