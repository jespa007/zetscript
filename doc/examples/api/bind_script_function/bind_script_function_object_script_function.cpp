#include "zetscript.h"

using zetscript::zs_float;

int main()
{
	zetscript::ScriptEngine se;

    se.compileAndRun(
 		"class A{\n"
        "    print(){\n"
        "       Console::outln(\"Call from instanced object\")"
        "   }\n"
        "}\n"
        "var a=new A();\n"
 	);

    auto a_print=se.bindScriptFunction<void(void)>("a.print");

    a_print();

 	return 0;
}

