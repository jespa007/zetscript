/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#include "MyClass.h"
#include "MyClassWrap.h"
#include "MyClassExtend.h"
#include "MyClassExtendWrap.h"


int main(){

	zetscript::ZetScript zs;

	// register MyClass with name MyClass in script side.
	zs.registerClass<MyClass>("MyClass",MyClassWrap_new,MyClassWrap_delete);

	 // register MyClassExtend with name MyClassExtend in script side.
	zs.registerClass<MyClassExtend>("MyClassExtend",MyClassExtendWrap_new,MyClassExtendWrap_delete);

	// register data1 named data1 in script side as variable member and read/write.
	zs.registerMemberPropertySetter<MyClass>("data1",&MyClassWrap_set_data1);
	zs.registerMemberPropertyGetter<MyClass>("data1",&MyClassWrap_get_data1);

	// register data2 named data1 in script side as variable member (only read).
	zs.registerMemberPropertyGetter<MyClass>("data2",&MyClassWrap_get_data2);

	// register data1 named data1 in script side as variable member (only write).
	zs.registerMemberPropertySetter<MyClass>("data3",&MyClassWrap_set_data3);

	// register function0 named function1 in script side as function member.
	zs.registerMemberFunction<MyClass>("function0",&MyClassWrap_function0);

	// register function1 named function1 in script side as function member.
	zs.registerMemberFunction<MyClass>("function1",&MyClassWrap_function1);


	// register data2 named data1 in script side as variable member.
	zs.registerMemberPropertySetter<MyClassExtend>("data4",&MyClassExtendWrap_set_data4);
	zs.registerMemberPropertyGetter<MyClassExtend>("data4",&MyClassExtendWrap_get_data4);

	// register function2 named function2 in script side as function member.
	zs.registerMemberFunction<MyClassExtend>("function2",&MyClassExtendWrap_function2);

	// once all vars and functions are registered, tell that MyClassExtend is base of MyClass
	zs.extends<MyClassExtend,MyClass>();


	zs.eval(
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
