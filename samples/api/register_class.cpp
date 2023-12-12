/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#include "MyClass.h"
#include "MyClassZs.h"
#include "MyClassExtend.h"
#include "MyClassExtendZs.h"


int main(){

	zetscript::ScriptEngine zs;

	// register MyClass with name MyClass in script side.
	zs.registerScriptType<MyClass>("MyClass",MyClassZs_new,MyClassZs_delete);

	 // register MyClassExtend with name MyClassExtend in script side.
	zs.registerScriptType<MyClassExtend>("MyClassExtend",MyClassExtendZs_new,MyClassExtendZs_delete);

	// register data1 named data1 in script side as variable member and read/write.
	zs.registerMemberPropertyMetamethod<MyClass>("data1","_set",&MyClassZs_set_data1);
	zs.registerMemberPropertyMetamethod<MyClass>("data1","_get",&MyClassZs_get_data1);

	// register data2 named data1 in script side as variable member (only read).
	zs.registerMemberPropertyMetamethod<MyClass>("data2","_get",&MyClassZs_get_data2);

	// register data1 named data1 in script side as variable member (only write).
	zs.registerMemberPropertyMetamethod<MyClass>("data3","_set",&MyClassZs_set_data3);

	// register function0 named function1 in script side as function member.
	zs.registerMemberFunction<MyClass>("function0",&MyClassZs_function0);

	// register function1 named function1 in script side as function member.
	zs.registerMemberFunction<MyClass>("function1",&MyClassZs_function1);


	// register data2 named data1 in script side as variable member.
	zs.registerMemberPropertyMetamethod<MyClassExtend>("data4","_set",&MyClassExtendZs_set_data4);
	zs.registerMemberPropertyMetamethod<MyClassExtend>("data4","_get",&MyClassExtendZs_get_data4);

	// register function2 named function2 in script side as function member.
	zs.registerMemberFunction<MyClassExtend>("function2",&MyClassExtendZs_function2);

	// once all vars and functions are registered, tell that MyClassExtend is base of MyClass
	zs.extends<MyClassExtend,MyClass>();


	zs.compileAndRun(
		"class ScriptMyClassExtend extends MyClassExtend{\n"
		"	var data5;\n"
		"	function function0(){\n"
		"		super();\n"
		"   }\n"
		"	function function1(arg){\n"
		"		var i=this.data1;\n"
		"		super(this.data1+arg);\n"
		"		Console::outln(\"result => \"+i+\"+\"+arg+\"=\"+this.data1);\n"
		"   }\n"
		"};\n"
		"class ScriptMyClassExtend2 extends ScriptMyClassExtend{\n"
		"	var data6;\n"
		"	function function5(arg){\n"
		"		var i=this.data1;\n"
		"   }\n"
		"};\n"
		"var myclass = new ScriptMyClassExtend2();\n" // instances MyClassExtend
		"myclass.function0();\n" // it prints "function0"
		"myclass.function1(12);\n" // it prints "Int argument is 12"
		"myclass.function2(0.5);\n" // it prints "Float argument is 0.5"
		"Console::outln(\"data1:\"+myclass.data1);\n" // it prints "data1:12"
		"Console::outln(\"data2:\"+myclass.data2);\n" // it prints "data2:0.5"
	);

	return 0;
}
