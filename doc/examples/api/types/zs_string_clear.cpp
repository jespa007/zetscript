#include "zetscript.h"
int main(){
	zetscript::zs_string string;

	string="Hello world";
	string.clear();

	printf("String result : %s",string.c_str());

	return 0;
}