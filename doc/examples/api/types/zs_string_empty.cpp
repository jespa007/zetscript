#include "zetscript.h"

using zetscript::zs_string;

int main(){

	zs_string empty_string;
	zs_string non_empty_string="Hello World";
	
	printf("empty_string.empty()  => '%s' \n"
	,empty_string.empty()?"true":"false");

	printf("non_empty_string.empty() => '%s' \n"
	,non_empty_string.empty()?"true":"false");

	return 0;
}