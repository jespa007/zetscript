#include "zetscript.h"

int main(){
	zetscript::String string="Hello World";
	zetscript::String string2="!!Hello World!!";

	if(string == "Hello World"){
		printf("string == 'Hello World'\n");
	}

	if(string2 != "Hello World"){
		printf("string2 != 'Hello World'\n");
	}

	return 0;
}