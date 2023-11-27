#include "zetscript.h"
int main(){
	zetscript::String string="Hello World";

	printf("the length of '%s' is %i\n",string.toConstChar(),string.length());
	
	return 0;
}