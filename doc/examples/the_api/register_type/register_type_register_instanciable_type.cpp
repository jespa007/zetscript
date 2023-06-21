#include "zetscript.h"

using zetscript::ZetScript;

typedef struct{
	int x;
	int y;
}MyType;

//-----
// Function to be registered to create new instance of MyType
MyType *MyTypeZs_new(ZetScript *_zs){
	return new MyType();
}

// Function to be registered to delete instance of MyType
void MyTypeZs_delete(ZetScript *_zs, MyType * _this){
	delete _this;
}
//-----


int main(){

	ZetScript zs;

	// registers MyType exposed as "MyType"
	zs.registerType<MyType>("MyType",MyTypeZs_new,MyTypeZs_new);//,MyType_constructorZs,MyType_destructorZs);

	// eval script that creates object of type "MyType"
	zs.eval("var my_type=new MyType();")



};


