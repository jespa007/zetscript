#include "zetscript.h"
int main(){
	zetscript::zs_string string;
	
	string="Hello";
	string+=" World";
	string+='!';

	printf("%s",string.c_str());
	
	return 0;
}