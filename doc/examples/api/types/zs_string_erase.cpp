#include "zetscript.h"

using zetscript::zs_string;

int main(){
	zs_string string="Helilo World";

	string.erase(3);
	printf("string.erase(3) => '%s'\n",string.c_str());
	
	return 0;
}