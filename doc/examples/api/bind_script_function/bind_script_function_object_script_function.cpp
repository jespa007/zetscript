#include "zetscript.h"

using zetscript::zs_float;

int main()
{
	zetscript::ScriptEngine script_engine;

    script_engine.eval(
 		"class A{\n"
        "    print(){\n"
        "       Console::outln(\"Call from instanced object\")"
        "   }\n"
        "}\n"
        "var a=new A();\n"
 	);

    auto a_print=script_engine.bindScriptFunction<void(void)>("a.print");

    a_print();

 	return 0;
}

