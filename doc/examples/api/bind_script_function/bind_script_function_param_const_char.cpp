#include "zetscript.h"

int main(){
	zetscript::ScriptEngine se;

   // Evaluates ScriptEngine function 'concat' that prints the result of the operation '+' from two arguments
   se.compile(
 		"function printConcat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);
   // It binds 'concat' as '(void *)(const char *, const char *)'
   auto printConcat=se.bindScriptFunction<void(const char * _value1, const char * _value2)>("printConcat");

   // Calls binded ScriptEngine function with parameters
   printConcat("Hello","World");
 	return 0;
}