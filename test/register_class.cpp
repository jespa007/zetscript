/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

using namespace zetscript;

#include "MyClass.h"
#include "MyClassWrap.h"
#include "MyClassExtend.h"
#include "MyClassExtendWrap.h"


int main(){

	ZetScript *zs=new ZetScript();

	// register MyClass with name MyClass in script side.
	zs->registerClass<MyClass>("MyClass",MyClassWrap_new,MyClassWrap_delete);

	 // register MyClassExtend with name MyClassExtend in script side.
	zs->registerClass<MyClassExtend>("MyClassExtend",MyClassExtendWrap_new,MyClassExtendWrap_delete);

	// register data1 named data1 in script side as variable member and read/write.
	zs->registerMemberAttributeSetter<MyClass>("data1",&MyClassWrap_data1_set);
	zs->registerMemberAttributeGetter<MyClass>("data1",&MyClassWrap_data1_get);

	// register data2 named data1 in script side as variable member (only read).
	zs->registerMemberAttributeGetter<MyClass>("data2",&MyClassWrap_data2_get);

	// register data1 named data1 in script side as variable member (only write).
	zs->registerMemberAttributeSetter<MyClass>("data3",&MyClassWrap_data3_set);

	// register function0 named function1 in script side as function member.
	/*zs->registerNativeMemberFunction<MyClass>("function0",&MyClass::function0);

	// register function1 named function1 in script side as function member.
	zs->registerNativeMemberFunction<MyClass>("function1",&MyClass::function1);


	// register data2 named data1 in script side as variable member.
	zs->registerNativeMemberVariable<MyClassExtend>("data2",&MyClassExtend::data2);

	// register function2 named function2 in script side as function member.
	zs->registerNativeMemberFunction<MyClassExtend>("function2",&MyClassExtend::function2);

	// once all vars and functions are registered, tell that MyClassExtend is base of MyClass
	zs->nativeClassInheritsFrom<MyClassExtend,MyClass>();
*/

	zs->eval(
			"var myclass = new MyClass();\n"
			"Console::outln(myclass.data1);\n"
	);

	/*zs->eval(
		"class ScriptMyClassExtend:MyClassExtend{\n"
		"	var data3;\n"
		"	function function0(){\n"
		"		super();\n"
		"   }\n"
		"	function function1(arg){\n"
		"		var i=this.data1;\n"
		"		super(this.data1+arg);\n"
		"		print(\"result => \"+i+\"+\"+arg+\"=\"+this.data1);\n"
	    "   }\n"
		"};\n"
			"class ScriptMyClassExtend2:ScriptMyClassExtend{\n"
					"	var data4;\n"
					"	function function5(arg){\n"
					"		var i=this.data1;\n"
				    "   }\n"
					"};\n"
		"var myclass = new ScriptMyClassExtend2();\n" // instances MyClassExtend
		"myclass.function0();\n" // it prints "function0"
		"myclass.function1(12);\n" // it prints "Int argument is 12"
		"myclass.function2(0.5);\n" // it prints "Float argument is 0.5"
		"print(\"data1:\"+myclass.data1);\n" // it prints "data1:12"
		"print(\"data2:\"+myclass.data2);\n" // it prints "data2:0.5"
		"delete myclass;\n" // delete script var with c pointers attached inside.
	);*/

	delete zs;

#ifdef __MEMMANAGER__
	MEMMGR_print_status();
#endif

	return 0;
}
