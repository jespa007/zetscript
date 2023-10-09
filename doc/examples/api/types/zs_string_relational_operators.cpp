#include "zetscript.h"
using zetscript::zs_string;
int main(){
	zetscript::zs_string string="Hello Word";
	zetscript::zs_string string2="!!Hello Word!!";

	if(string == "Hello World"){
		printf("string == 'Hello World'\n");
	}

	if(string != "Hello World"){
		printf("string == 'Hello World'\n");
	}

	return 0;
}