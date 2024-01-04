#include "zetscript.h"

int main(){
	zetscript::String string="Helilo World";

	string.erase(3);
	printf("string.erase(3) => '%s'\n",string.toConstChar());
	
	return 0;
}