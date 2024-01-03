#include "zetscript.h"

int main(){
	zetscript::ScriptEngine script_engine;

    // Evaluates ZetScript function 'concat' that prints the result of the operation '+' from two arguments
    script_engine.compileAndRun(
 		"function printConcat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);

  // It binds 'printConcat' as '(void *)(String *, String *)'
  auto printConcat=script_engine.bindScriptFunction<
    void(
    zetscript::String * _value1, zetscript::String * _value2
  )>("printConcat");

  // Prepare param values
  zetscript::String value1="Hello";
  zetscript::String value2="World";

  // Calls ZetScript function by value
  printConcat(&value1,&value2);
 	return 0;
}

