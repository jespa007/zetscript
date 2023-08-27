#include "zetscript.h"

int main(){
	zetscript::ZetScript zs;

   // Evaluates ZetScript function 'concat' that prints the result of the operation '+' from two arguments
   zs.eval(
 		"function printConcat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);
   // It binds 'concat' as '(void *)(const char *, const char *)'
   auto printConcat=zs.bindScriptFunction<void(const char * _value1, const char * _value2)>("printConcat");

   // Calls binded ZetScript function with parameters
   printConcat("Hello","World");
 	return 0;
}