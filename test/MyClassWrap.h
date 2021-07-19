#pragma once


MyClass *MyClassWrap_new(){
	return new MyClass;
}

//--------------------------------------------------------
// GET/SET for data1 (read & write)
void MyClassWrap_set_data1(MyClass *_this, zs_int v){
	_this->data1=v;
}

zs_int MyClassWrap_get_data1(MyClass *_this){
	return _this->data1;
}
//--------------------------------------------------------
// GET for data2 (only read)
zs_int MyClassWrap_get_data2(MyClass *_this){
	return _this->data2;
}
//--------------------------------------------------------
// SET for data3 (only write)
void MyClassWrap_set_data3(MyClass *_this, zs_int v){
	_this->data3=v;
}

void MyClassWrap_function0(MyClass *_this){
	_this->function0();
}

// register function1 named function1 in script side as function member.
void MyClassWrap_function1(MyClass *_this, zs_int v){
	_this->function1(v);
}

void MyClassWrap_delete(MyClass *_this){
	delete _this;
}
