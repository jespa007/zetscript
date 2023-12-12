#include "zetscript.h"

using zetscript::zs_int;
using zetscript::ScriptEngine;

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

zs_int MyCppType_function1(ScriptEngine *_script_engine, MyCppType *_this, zs_int _arg){
	return _this->function1(_arg);
}

MyCppTypeExtend * MyCppTypeExtend_new(ScriptEngine *_script_engine){
	return new MyCppTypeExtend();
}

void MyCppTypeExtend_constructor(ScriptEngine *_script_engine, MyCppTypeExtend *_this, zs_int _data1){
	_this->data1=_data1;
	_this->data2=_data1*10;
}

zs_int MyCppTypeExtend_function2(ScriptEngine *_script_engine, MyCppTypeExtend *_this, zs_int _arg){
	return _this->function1(_arg);
}

void MyCppTypeExtend_delete(ScriptEngine *_script_engine, MyCppTypeExtend *_this){
	delete _this;
}

int main(){
	ScriptEngine script_engine; // instance zetscript

	// Register MyCppType as MyCppType in script side
	script_engine.registerScriptType<MyCppType>("MyCppType");

	// Register MyCppType::function1
 	script_engine.registerMemberFunction<MyCppType>("function1",MyCppType_function1);

	// Register MyCppTypeExtend as MyCppTypeExtend in script side as instantiable
	script_engine.registerScriptType<MyCppTypeExtend>("MyCppTypeExtend",MyCppTypeExtend_new,MyCppTypeExtend_delete);

	// Tells MyCppTypeExtends extends from MyCppType
	script_engine.extends< MyCppTypeExtend,MyCppType >();

	// Register MyCppTypeExtend::function2
	script_engine.registerMemberFunction<MyCppTypeExtend>("function2",MyCppTypeExtend_function2);

	// eval print
	script_engine.eval(
		"class ScriptMyCppTypeExtend extends MyCppTypeExtend{\n"
		"	function1(_arg1){\n"
		"		Console::outln(\"script argument : {0} \",_arg1);\n"
		"		super(_arg1); // calls cpp MyCppType::function1\n"
		"	}\n"
		"};\n"
		"var object=new ScriptMyCppTypeExtend(10);\n"
		"object.function1(5);\n"
	);
	return 0;
}