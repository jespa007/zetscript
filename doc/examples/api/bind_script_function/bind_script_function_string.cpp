#include "zetscript.h"

using zetscript::String;

int main()
{
	zetscript::ScriptEngine script_engine;

    script_engine.compile(
 		"function add(_a,_b){\n"
        "    return _a+_b;\n"
        "}\n"
 	);

    auto add=script_engine.bindScriptFunction<String(String *,String *)>("add");
    String s1="Hello";
    String s2="World";

    printf("result : %s\n", add(&s1,&s2).toConstChar());

 	return 0;
}

