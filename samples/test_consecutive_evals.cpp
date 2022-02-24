
#include "zetscript.h"


void test_consistency_function_override(
	zetscript::ZetScript *_zs
){
	_zs->eval(
		"class A{\n"
		"	A(){\n"
		//		should call A::function1 of the instance of var is A
		//		should call B::function1 of the instance of var is B
		"       this.function1();\n"
		"	}\n"
		"	function1(){\n"
		"       Console::outln(\"from A \");\n"
		"	}\n"
		"};\n"
	);

	_zs->eval(
		"class B extends A{\n"
		"	B(){\n"
		"		super();\n"
		"	}\n"
		"	function1(){\n"
		"       Console::outln(\"from B \");\n"
		"	}\n"
		"};\n"
	);

	// should print
	// from A
	// from B
	_zs->eval(
			"new B()"
	);

}


int main(){

	// instances ZetScript
	zetscript::ZetScript zs;

	try{

		test_consistency_function_override(&zs);

	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
	}

}
