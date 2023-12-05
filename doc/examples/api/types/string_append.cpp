#include "zetscript.h"
int main(){
	zetscript::String string;
	
	string="Hello";
	string.append(" World");
	string.append('!');

	printf("%s\n",string.toConstChar());
	
	return 0;
}