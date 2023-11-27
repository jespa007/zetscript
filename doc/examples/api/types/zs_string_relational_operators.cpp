#include "zetscript.h"
using zetscript::String;
int main(){
	String string="Hello World";
	String string2="!!Hello World!!";

	if(string == "Hello World"){
		printf("string == 'Hello World'\n");
	}

	if(string2 != "Hello World"){
		printf("string2 != 'Hello World'\n");
	}

	return 0;
}