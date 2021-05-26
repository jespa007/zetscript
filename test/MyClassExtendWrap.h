#pragma once

MyClassExtend *MyClassExtendWrap_new(){
	return new MyClassExtend;
}

void MyClassExtendWrap_delete(MyClassExtend *_this){
	delete _this;
}
