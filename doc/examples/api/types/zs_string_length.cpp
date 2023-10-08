#include "zetscript.h"
int main(){
	zetscript::zs_string string="Hello World";

	printf("the length of '%s' is %i",string.c_str(),string.length());
	
	return 0;
}