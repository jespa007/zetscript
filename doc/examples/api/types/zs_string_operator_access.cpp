#include "zetscript.h"
int main(){
	zetscript::zs_string string="Hello World";

	printf("String contents : ");
	for(int c=0; c < string.length();c++){
		printf("%c",string[c]);
	}

	printf("\n");




	
	return 0;
}