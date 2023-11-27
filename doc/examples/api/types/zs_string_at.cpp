#include "zetscript.h"
using zetscript::String;
int main(){
	String string="Hello World";

	for(int c=0; c < string.length();c++){
		printf("%c",string.at(c));
	}

	printf("\n");
	
	return 0;
}