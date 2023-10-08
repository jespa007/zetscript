#include "zetscript.h"
int main(){
	zetscript::zs_string string="this is a test string.";
	zetscript::zs_string str2="n example";

	string.replace(9,5,str2);     

	printf("string.replace(9,5,str2) => '%s'",string.replace(9,5,str2).c_str());

	return 0;
}