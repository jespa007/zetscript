#pragma once

MyClassExtend *MyClassExtendWrap_new(){
	return new MyClassExtend;
}

zs_int MyClassExtendWrap_get_data4(MyClassExtend *_this){
	return _this->data4;
}

void MyClassExtendWrap_set_data4(MyClassExtend *_this, zs_int v){
	_this->data4=v;
}

void MyClassExtendWrap_function2(MyClassExtend *_this, zs_float *f){
	_this->function2(*f);
}

void MyClassExtendWrap_delete(MyClassExtend *_this){
	delete _this;
}





