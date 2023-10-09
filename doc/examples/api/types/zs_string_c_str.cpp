#include "zetscript.h"
using zetscript::zs_string;
int main(){
	zs_string string="Hello World";

	printf("string.c_str() => '%s'\n",string.c_str());
	
	return 0;
}