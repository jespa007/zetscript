#include "zetscript.h"

using zetscript::String;

int main(){
	String string="Helilo World";

	string.erase(3);
	printf("string.erase(3) => '%s'\n",string.toConstChar());
	
	return 0;
}