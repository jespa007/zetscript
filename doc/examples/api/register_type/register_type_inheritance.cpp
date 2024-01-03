#include "zetscript.h"

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

zetscript::zs_int MyCppType_function1(
	zetscript::ScriptEngine *_script_engine
	, MyCppType *_this
	,zetscript::zs_int _arg
){
	return _this->function1(_arg);
}

MyCppTypeExtend * MyCppTypeExtend_new(
	zetscript::ScriptEngine *_script_engine
){
	return new MyCppTypeExtend();
}

void MyCppTypeExtend_constructor(
	zetscript::ScriptEngine *_script_engine
	,MyCppTypeExtend *_this
	,zetscript::zs_int _data1){
	_this->data1=_data1;
	_this->data2=_data1*10;
}

zetscript::zs_int MyCppTypeExtend_function2(
	zetscript::ScriptEngine *_script_engine
	,MyCppTypeExtend *_this
	,zetscript::zs_int _arg){
	return _this->function1(_arg);
}

void MyCppTypeExtend_delete(
	zetscript::ScriptEngine *_script_engine
	,MyCppTypeExtend *_this
){
	delete _this;
}

int main(){
	zetscript::ScriptEngine script_engine; // instance zetscript

	// Register MyCppType as MyCppType in script side
	script_engine.registerType<MyCppType>("MyCppType");

	// Register MyCppType::function1
 	script_engine.registerMemberFunction<MyCppType>("function1",MyCppType_function1);

	// Register MyCppTypeExtend as MyCppTypeExtend in script side as instantiable
	script_engine.registerType<MyCppTypeExtend>("MyCppTypeExtend",MyCppTypeExtend_new,MyCppTypeExtend_delete);

	// Tells MyCppTypeExtends extends from MyCppType
	script_engine.extends< MyCppTypeExtend,MyCppType >();

	// Register MyCppTypeExtend::function2
	script_engine.registerMemberFunction<MyCppTypeExtend>("function2",MyCppTypeExtend_function2);

	// Instance object as ScriptMyCppTypeExtend calls object.function1
	script_engine.compileAndRun(
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