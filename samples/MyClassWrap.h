#pragma once


MyClass *MyClassWrap_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new MyClass;
}

//--------------------------------------------------------
// GET/SET for data1 (read & write)
void MyClassWrap_set_data1(zetscript::ZetScript *_zs,MyClass *_this, zetscript::zs_int v){
	ZS_UNUSUED_PARAM(_zs);
	_this->data1=v;
}

zetscript::zs_int MyClassWrap_get_data1(zetscript::ZetScript *_zs,MyClass *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->data1;
}
//--------------------------------------------------------
// GET for data2 (only read)
zetscript::zs_int MyClassWrap_get_data2(zetscript::ZetScript *_zs,MyClass *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->data2;
}
//--------------------------------------------------------
// SET for data3 (only write)
void MyClassWrap_set_data3(zetscript::ZetScript *_zs,MyClass *_this, zetscript::zs_int v){
	ZS_UNUSUED_PARAM(_zs);
	_this->data3=v;
}

void MyClassWrap_function0(zetscript::ZetScript *_zs,MyClass *_this){
	ZS_UNUSUED_PARAM(_zs);
	_this->function0();
}

// register function1 named function1 in script side as function member.
void MyClassWrap_function1(zetscript::ZetScript *_zs,MyClass *_this, zetscript::zs_int v){
	ZS_UNUSUED_PARAM(_zs);
	_this->function1(v);
}

void MyClassWrap_delete(zetscript::ZetScript *_zs,MyClass *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}
