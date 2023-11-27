#include "zetscript.h"
using zetscript::String;
int main(){
	String string="Hello World";

	printf("string.toConstChar() => '%s'\n",string.toConstChar());
	
	return 0;
}