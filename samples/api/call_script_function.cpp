/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

void test_call_script_function(){
	// instances ZetScript
	zetscript::ZetScript zs;

	zs.eval(
		// ScriptFunction 'say_hello'
		"function say_hello(){\n"
		"	Console::outln(\"hello from 'say_hello'!\")\n"
		"}\n"
		"\n"
		// ScriptType 'Test' declaration
		"class Test{\n"
		// Member function 'say_hello'
		"	say_hello(arg){\n"
		"		Console::outln(\"hello from 'Test::say_hello' with arg:\"+arg+\"!\")\n"
		"	}\n"
		"};\n"
		"\n"

		// test variable declaration and instances 'Text' type
		"var test=new Test();\n"
		""
	);

	// instance function delete_test function.
	auto  say_hello=zs.bindFunction<void ()>("say_hello");

	// it calls "say_hello" function with no parameters
	say_hello();

	// instance member function test.function1.
	auto  test_say_hello=zs.bindFunction<void (zetscript::zs_int)>("test.say_hello");

	// it calls "test.function1" member function with 10 as parameter.
	test_say_hello(10);

}
