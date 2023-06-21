#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

typedef struct{
	int x;
	int y;
}MyType;

//-----
// Function to be registered to create new instance of MyType
MyType *MyTypeZs_new(ZetScript *_zs){
	return new MyType();
}


void MyTypeZs_constructor(ZetScript *_zs, MyType *_this, zs_int _x, zs_int _y){
	_this->x=_x;
	_this->y=_y;
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

	// register MyType constructor
	zs.registerConstructor<MyType>(MyTypeZs_constructor);

	// eval script that creates object of type "MyType" and inits x and y as  10 and 20 respectibely
	zs.eval("var my_type=new MyType(10,20);")



};


