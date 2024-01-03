#include "zetscript.h"

int main(){

	zetscript::String empty_string;
	zetscript::String non_empty_string="Hello World";
	
	printf("empty_string.empty()  => '%s' \n"
	,empty_string.isEmpty()?"true":"false");

	printf("non_empty_string.empty() => '%s' \n"
	,non_empty_string.isEmpty()?"true":"false");

	return 0;
}