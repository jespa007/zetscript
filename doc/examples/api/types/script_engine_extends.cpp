#include "zetscript.h"

using zetscript::zs_int;
using zetscript::ScriptEngine;

class MyCppType{
};

class MyCppTypeExtend:public MyCppType{
};


MyCppTypeExtend * MyCppTypeExtend_new(ScriptEngine *_script_engine){
	return new MyCppTypeExtend();
}

void MyCppTypeExtend_delete(ScriptEngine *_script_engine, MyCppTypeExtend *_this){
	delete _this;
}

int main(){
	ScriptEngine script_engine; // instance zetscript

	// Register MyCppType as MyCppType in script side
	script_engine.registerScriptType<MyCppType>("MyCppType");

	// Register MyCppTypeExtend as MyCppTypeExtend in script side as instantiable
	script_engine.registerScriptType<MyCppTypeExtend>("MyCppTypeExtend",MyCppTypeExtend_new,MyCppTypeExtend_delete);

	// Tells MyCppTypeExtends extends from MyCppType
	script_engine.extends< MyCppTypeExtend,MyCppType >();

	// Print typeof 'script_my_cpp_type_extend'
	script_engine.compileAndRun(
		"var script_my_cpp_type_extend=new ScriptMyCppTypeExtend();\n"
		"Console::outln(\"tyoeof(object) => \"+typeof(script_my_cpp_type_extend));\n"
	);
	return 0;
}