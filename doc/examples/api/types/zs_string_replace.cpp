#include "zetscript.h"
using zetscript::zs_string;
int main(){
	zs_string string="this is a test string.";
	zs_string str2="n example";

	string.replace(9,5,str2);     

	printf("string.replace(9,5,str2) => '%s'\n",string.c_str());

	return 0;
}