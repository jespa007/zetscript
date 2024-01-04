#include "zetscript.h"

int main(){
	zetscript::String string="Hello World";

	printf("string.toConstChar() => '%s'\n",string.toConstChar());
	
	return 0;
}