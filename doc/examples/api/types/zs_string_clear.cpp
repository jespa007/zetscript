#include "zetscript.h"
int main(){
	zetscript::zs_string string="Hello World";

	string.clear();

	printf("string : '%s'\n",string.c_str());

	return 0;
}