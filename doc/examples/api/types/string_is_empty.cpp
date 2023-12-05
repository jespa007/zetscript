#include "zetscript.h"

using zetscript::String;

int main(){

	String empty_string;
	String non_empty_string="Hello World";
	
	printf("empty_string.empty()  => '%s' \n"
	,empty_string.isEmpty()?"true":"false");

	printf("non_empty_string.empty() => '%s' \n"
	,non_empty_string.isEmpty()?"true":"false");

	return 0;
}