#pragma once

class MyClass{
public:
	int data1;
	int data2;
	int data3;

	MyClass(){
		this->data1=5;
		this->data2=10;
		this->data3=20;
	}

	void function0(){
		printf("function0\n");
	}

	void function1(int arg){
		this->data1 = arg;
		printf("int argument is %i\n",this->data1);
	}

};
