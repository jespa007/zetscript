#include "zetscript.h"

class MyClass{
public:
	int data1;


	int function1(int arg){
 		printf("c++ argument is %i\n",this->data1+arg);
 		return this->data1 = arg;
 	}
};

// global access
MyClass *my_class=NULL;

// function to set MyClass as pointer instance
void setMyClassPtr(MyClass *_my_class_ptr){
	my_class=_my_class_ptr;
}


//---- BINDINGS
MyClass * MyClassPtrWrap_getMyClassPtr(zetscript::ZetScript *_zs){
	return my_class;
}

zetscript::zs_int MyClassPtrWrap_function1(zetscript::ZetScript *_zs, MyClass *_this, zetscript::zs_int _arg){
	return _this->function1(_arg);
}

//---- BINDINGS


int main(){

	zetscript::ZetScript zs;

	MyClass my_class;
	setMyClassPtr(&my_class);


 	//register MyClass as static (i.e no instantiable) in script side.
 	zs.bindType<MyClass>("MyClass");

 	zs.bindFunction("getMyClass",MyClassPtrWrap_getMyClassPtr);

 	zs.bindMemberFunction<MyClass>("function1",MyClassPtrWrap_function1);

	// It throws an error that MyClass is not instanciable because is static
 	zs.eval(
 		"var myclass= getMyClass();\n"
 		"myclass.function1(5);\n"

 	);

 	return 0;
 }
