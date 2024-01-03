#include "zetscript.h"

class MyCppType{
};

class MyCppTypeExtend:public MyCppType{
};


MyCppTypeExtend * MyCppTypeExtend_new(
	zetscript::ScriptEngine *_script_engine
){
	return new MyCppTypeExtend();
}

void MyCppTypeExtend_delete(
	zetscript::ScriptEngine *_script_engine
	, MyCppTypeExtend *_this
){
	delete _this;
}

int main(){
	zetscript::ScriptEngine script_engine; // instance zetscript

	// Register MyCppType as MyCppType in script side
	script_engine.registerType<MyCppType>("MyCppType");

	// Register MyCppTypeExtend as MyCppTypeExtend in script side as instantiable
	script_engine.registerType<MyCppTypeExtend>("MyCppTypeExtend",MyCppTypeExtend_new,MyCppTypeExtend_delete);

	// Tells MyCppTypeExtends extends from MyCppType
	script_engine.extends< MyCppTypeExtend,MyCppType >();

	// Print typeof 'script_my_cpp_type_extend'
	script_engine.compileAndRun(
		"var script_my_cpp_type_extend=new MyCppTypeExtend();\n"
		"Console::outln(\"tyoeof(object) => \"+typeof script_my_cpp_type_extend);\n"
	);
	return 0;
}