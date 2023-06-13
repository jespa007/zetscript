#include "zetscript.h"

using zetscript::ZetScript;

typedef struct{
	int x;
	int y;
}MyType;

//-----
// MyType ZetScript constructor
MyType *MyType_constructorZs(ZetScript *_zs){
	return new MyType();
}

// MyType ZetScript destructor
void MyType_destructorZs(ZetScript *_zs, MyType * _this){
	delete _this;
}
//-----


int main(){

	ZetScript zs;

	// registers MyType exposed as "MyType"
	zs.registerType<MyType>("MyType",MyType_constructorZs,MyType_destructorZs);

	// eval script that creates object of type "MyType"
	zs.eval("var my_type=new MyType();")



};


