#pragma once


MyClass *MyClassWrap_new(){
	return new MyClass;
}

//--------------------------------------------------------
// GET/SET for data1 (read & write)
void MyClassWrap_data1_set(MyClass *_this, zs_int v){
	_this->data1=v;
}

int MyClassWrap_data1_get(MyClass *_this){
	return _this->data1;
}
//--------------------------------------------------------
// GET for data2 (only read)
int MyClassWrap_data2_get(MyClass *_this){
	return _this->data2;
}
//--------------------------------------------------------
// SET for data3 (only write)
void MyClassWrap_data3_set(MyClass *_this, zs_int v){
	_this->data3=v;
}

void MyClassWrap_delete(MyClass *_this){
	delete _this;
}
