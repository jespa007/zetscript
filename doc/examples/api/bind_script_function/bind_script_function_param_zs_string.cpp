#include "zetscript.h"

using zetscript::String;

int main(){
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'concat' that prints the result of the operation '+' from two arguments
    zs.eval(
 		"function printConcat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);

   // It binds 'printConcat' as '(void *)(String *, String *)'
   auto printConcat=zs.bindFunction<void(String * _value1, String * _value2)>("printConcat");

   // Prepare param values
   String value1="Hello";
   String value2="World";

   // Calls ZetScript function by value
   printConcat(&value1,&value2);
 	return 0;
}

