#include "zetscript.h"
using zetscript::zs_string;
int main(){
	zs_string string="Hello World";

	for(int c=0; c < string.length();c++){
		printf("%c",string.at(c));
	}

	printf("\n");
	
	return 0;
}