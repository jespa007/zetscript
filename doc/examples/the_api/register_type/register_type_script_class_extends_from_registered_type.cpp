#include "zetscript.h"

class MyClass{
	public:
	int data1;
	int function1(int arg){
		printf("c++ argument is %i\n",this->data1+arg);
		return this->data1 + arg;
	}
};


class MyClassExtend:public MyClass{
public:
	float data2;
	int function2(float arg){
		printf("Float argument is %.02f\n",this->data2 + arg);
		return this->data2 + arg;
	}
};


//---- BINDINGS
MyClass * MyClassPtr_new(zetscript::ZetScript *_zs){
	return new MyClass();
}

void MyClassPtr_constructor(zetscript::ZetScript *_zs, MyClass *_this, zetscript::zs_int _data1){
	_this->data1=_data1;
}

zetscript::zs_int MyClassPtr_function1(zetscript::ZetScript *_zs, MyClass *_this, zetscript::zs_int _arg){
	return _this->function1(_arg);
}

void MyClassPtr_delete(zetscript::ZetScript *_zs, MyClass *_this){
	delete _this;
}


//---- BINDINGS

int main(){
	zetscript::ZetScript zs; // instance zetscript

	// register MyClass as MyClass in script side
	zs.registerType<MyClass>("MyClass",MyClassPtr_new,MyClassPtr_delete);

	// register MyClass as MyClass in script side
	zs.registerType<MyClassExtend>("MyClassExtend");

	// tells MyClassExtends extends from MyClass
	zs.extends< MyClassExtend,MyClass >();

	// register MyClass::constructor
	zs.registerConstructor<MyClass>(MyClassPtr_constructor);

	//reg MyClass:: function1
 	zs.registerMemberFunction<MyClass>("function1",MyClassPtr_function1);

	// eval print
	zs.eval(
		"class ScriptMyClassExtend extends MyClassExtend{\n"
		"	function1(_arg1){\n"
		"		Console::outln(\"script argument is \"+_arg1);\n"
		"		super(this.data1+arg1); // calls function1 c++\n"
		"	}\n"
		"};\n"
		"var myclass=new ScriptMyClassExtend(10);\n"
		"myclass.function1(5);\n"
	);


	return 0;
}
