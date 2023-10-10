#include "zetscript.h"
using zetscript::zs_string;
int main(){
	zs_string string="Hello World";

	printf("string.substring(0) => '%s'\n",string.substr(0).c_str());
	printf("string.substring(3) => '%s'\n",string.substr(3).c_str());
	printf("string.substring(2,3) => '%s'\n",string.substr(2,3).c_str());
	printf("string.substring(3,-2) => '%s'\n",string.substr(3,-2).c_str());
	
	return 0;
}