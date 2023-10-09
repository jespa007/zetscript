#include "zetscript.h"
int main(){
	zetscript::zs_string string;
	
	string="Hello";
	string.append(" World");
	string.append('!');

	printf("%s\n",string.c_str());
	
	return 0;
}