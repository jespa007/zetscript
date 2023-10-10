#include "zetscript.h"
using zetscript::zs_string;
int main(){
	zs_string string="Hello World";
	zs_string string2="!!Hello World!!";

	if(string == "Hello World"){
		printf("string == 'Hello World'\n");
	}

	if(string2 != "Hello World"){
		printf("string2 != 'Hello World'\n");
	}

	return 0;
}