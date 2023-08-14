#include "zetscript.h"

using zetscript::zs_int;
using zetscript::ZetScript;

class MyCppType{
	public:
	int data1;
	int function1(int arg){
		printf("MyCppType::function1 - Argument is %i\n",this->data1+arg);
		return this->data1 + arg;
	}
};


class MyCppTypeExtend:public MyCppType{
public:
	float data2;
	int function2(float arg){
		printf("MyCppTypeExtend::function2 - Float argument is %.02f\n",this->data2 + arg);
		return this->data2 + arg;
	}
};


//---- BINDINGS
MyCppType * MyCppType_new(ZetScript *_zs){
	return new MyCppType();
}

void MyCppType_constructor(ZetScript *_zs, MyCppType *_this, zs_int _data1){
	_this->data1=_data1;
}

zs_int MyCppType_function1(ZetScript *_zs, MyCppType *_this, zs_int _arg){
	return _this->function1(_arg);
}

void MyCppType_delete(ZetScript *_zs, MyCppType *_this){
	delete _this;
}

zs_int MyCppTypeExtend_function2(ZetScript *_zs, MyCppTypeExtend *_this, zs_int _arg){
	return _this->function1(_arg);
}


//---- BINDINGS

int main(){
	ZetScript zs; // instance zetscript

	// Register MyCppType as MyCppType in script side
	zs.registerType<MyCppType>("MyCppType");//,MyCppType_new,MyCppType_delete);

	// Register MyCppType::constructor
	//zs.registerConstructor<MyCppType>(MyCppType_constructor);

	// Register MyCppType::function1
 	zs.registerMemberFunction<MyCppType>("function1",MyCppType_function1);

	// Register MyCppType as MyCppType in script side
	zs.registerType<MyCppTypeExtend>("MyCppTypeExtend");

	// Tells MyCppTypeExtends extends from MyCppType
	zs.extends< MyCppTypeExtend,MyCppType >();

	// Register MyCppTypeExtend::function2
	zs.registerMemberFunction<MyCppTypeExtend>("function2",MyCppTypeExtend_function2);

	// eval print
	zs.eval(
		"class ScriptMyCppTypeExtend extends MyCppTypeExtend{\n"
		"	function1(_arg1){\n"
		"		Console::outln(\"script argument is \"+_arg1);\n"
		"		super(this.data1+arg1); // calls cpp MyCppType::function1\n"
		"	}\n"
		"};\n"
		"var object=new ScriptMyCppTypeExtend();\n"
		"object.function1(5);\n"
	);

	return 0;
}
