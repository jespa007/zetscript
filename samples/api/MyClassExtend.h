#pragma once

class MyClassExtend:public MyClass{
public:
	zetscript::zs_float data4;

	void function2(float arg){
		this->data4 = arg;
		printf("Float argument is %.02f\n",this->data4);
	}
};
