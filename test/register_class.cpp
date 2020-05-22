/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

using namespace zetscript;

class MyClass{
public:
	int data1;

	MyClass(){
		this->data1=5;
	}

	void function0(){
		printf("function0\n");
	}

	void function1(int arg){
		this->data1 = arg;
		printf("int argument is %i\n",this->data1);
	}

};

class MyClassExtend:public MyClass{
public:
	float data2;

	void function2(float *arg){
		this->data2 = *arg;
		printf("Float argument is %.02f\n",this->data2);
	}
};

int main(){

	CZetScript *zs=new CZetScript();

	zs->register_C_Class<MyClass>("MyClass"); // register MyClass with name MyClass in script side.
	zs->register_C_Class<MyClassExtend>("MyClassExtend"); // register MyClassExtend with name MyClassExtend in script side.

	zs->register_C_VariableMember<MyClass>("data1",&MyClass::data1); // register data1 named data1 in script side as variable member.
	zs->register_C_FunctionMember<MyClass>("function0",&MyClass::function0); // register function0 named function1 in script side as function member.
	zs->register_C_FunctionMember<MyClass>("function1",&MyClass::function1); // register function1 named function1 in script side as function member.


	zs->register_C_VariableMember<MyClassExtend>("data2",&MyClassExtend::data2); // register data2 named data1 in script side as variable member.
	zs->register_C_FunctionMember<MyClassExtend>("function2",&MyClassExtend::function2); // register function2 named function2 in script side as function member.


	zs->class_C_BaseOf<MyClassExtend,MyClass>(); // once all vars and functions are registered, tell that MyClassExtend is base of MyClass

	zs->evalString(
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
	);

	delete zs;

#ifdef __MEMMANAGER__
  MEM_ViewStatus();
#endif

	return 0;
}
