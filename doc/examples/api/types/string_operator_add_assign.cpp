#include "zetscript.h"
int main(){
	zetscript::String string;
	
	string="Hello";
	string+=" World";
	string+='!';

	printf("%s",string.toConstChar());
	
	return 0;
}