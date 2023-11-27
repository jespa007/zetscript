#include "zetscript.h"
using zetscript::String;
int main(){
	zetscript::String string="Hello World";

	string.clear();

	printf("string : '%s'\n",string.toConstChar());

	return 0;
}