#include "zetscript.h"
int main(){
	zetscript::zs_string string;

	string="Hello world";

	printf("%s\n",string.c_str());

	return 0;
}