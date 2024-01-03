#include "zetscript.h"

int main(){
	zetscript::String string="Hello World";

	printf("string.substring(0) => '%s'\n",string.getSubstring(0).toConstChar());
	printf("string.substring(3) => '%s'\n",string.getSubstring(3).toConstChar());
	printf("string.substring(2,3) => '%s'\n",string.getSubstring(2,3).toConstChar());
	
	return 0;
}