#include "zetscript.h"
int main(){
	zetscript::String string;

	string="Hello world";

	printf("%s\n",string.toConstChar());

	return 0;
}