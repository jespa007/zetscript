#include "zetscript.h"

int main()
{
	zetscript::ScriptEngine script_engine;

    script_engine.compile(
 		"function add(_a,_b){\n"
        "    return _a+_b;\n"
        "}\n"
 	);

    auto add=script_engine.bindScriptFunction<
      zetscript::String(
         zetscript::String *
         ,zetscript::String *
      )>("add");
      
    zetscript::String s1="Hello";
    zetscript::String s2="World";

    printf("result : %s\n", add(&s1,&s2).toConstChar());

 	return 0;
}

