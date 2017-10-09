#include "CZetScript.h"

using namespace zetscript;

class MyClass{
public:
	int data1;

	MyClass(){
		this->data1=5;
	}

	void function1(int arg){
		this->data1 = arg;
		printf("int argument is %i\n",this->data1);
	}
};

class MyClassExtend:public MyClass{
public:
	float data2;

	void function2(float arg){
		this->data2 = arg;
		printf("Float argument is %.02f\n",this->data2);
	}
};

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	register_C_Class<MyClass>("MyClass"); // register MyClass with name MyClass in script side.
	register_C_Class<MyClassExtend>("MyClassExtend"); // register MyClassExtend with name MyClassExtend in script side.

	register_C_VariableMember("data1",&MyClass::data1); // register data1 named data1 in script side as variable member.
	register_C_FunctionMember("function1",&MyClass::function1); // register function1 named function1 in script side as function member.

	register_C_VariableMember("data2",&MyClassExtend::data2); // register data2 named data1 in script side as variable member.
	register_C_FunctionMember("function2",&MyClassExtend::function2); // register function2 named function2 in script side as function member.


	class_C_baseof<MyClassExtend,MyClass>(); // once all vars and functions are registered, tell that MyClassExtend is base of MyClass

	zs->eval(
		"class ScriptMyClassExtend:MyClassExtend{"
		"	var data3;"
		"	function function1(arg){"
		"		var i=this.data1;"
		"		super(this.data1+arg);"
		"		print(\"result => \"+i+\"+\"+arg+\"=\"+this.data1);"
	    "   }"
		"};"
		"var myclass = new ScriptMyClassExtend();" // instances MyClassExtend
		"myclass.function1(12);" // it prints "Int argument is 12"
		"myclass.function2(0.5);" // it prints "Float argument is 0.5"
		"print(\"data1:\"+myclass.data1);" // it prints "data1:12"
		"print(\"data2:\"+myclass.data2);" // it prints "data2:0.5"
		"delete myclass;" // delete script var with c pointers attached inside.
	);

	return 0;
}
