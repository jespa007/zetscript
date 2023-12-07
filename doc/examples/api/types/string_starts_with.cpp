#include "zetscript.h"

using zetscript::String;

int main(){

	String s="Hello World";
	
	printf("s.startsWith(\"Hel\")  => '%s \n"
	,s.startsWith("Hel")?"true":"false");

	printf("s.startsWith(\"orld\")  => '%s' \n"
	,s.startsWith("orld")?"true":"false");	


	return 0;
}