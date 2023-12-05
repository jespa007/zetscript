#include "zetscript.h"
using zetscript::String;
int main(){
	String string="Hello World";

	printf("string.substring(0) => '%s'\n",string.substr(0).toConstChar());
	printf("string.substring(3) => '%s'\n",string.substr(3).toConstChar());
	printf("string.substring(2,3) => '%s'\n",string.substr(2,3).toConstChar());
	
	return 0;
}