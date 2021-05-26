#pragma once

class MyClassExtend:public MyClass{
public:
	zs_float data2;

	void function2(zs_float *arg){
		this->data2 = *arg;
		printf("Float argument is %.02f\n",this->data2);
	}
};
