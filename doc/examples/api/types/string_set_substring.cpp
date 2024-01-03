#include "zetscript.h"

int main(){
	zetscript::String string="this is a test string.";
	zetscript::String str2="n example";

	string.setSubstring(9,5,str2);     

	printf("string.replace(9,5,str2) => '%s'\n",string.toConstChar());

	return 0;
}