#include "zetscript.h"

int main(){

	zetscript::String s="Hello World";
	
	printf("s.endsWith(\"Hel\")  => '%s \n"
	,s.endsWith("Hel")?"true":"false");

	printf("s.endsWith(\"orld\")  => '%s' \n"
	,s.endsWith("orld")?"true":"false");	


	return 0;
}