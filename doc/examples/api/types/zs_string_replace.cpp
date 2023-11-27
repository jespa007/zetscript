#include "zetscript.h"
using zetscript::String;
int main(){
	String string="this is a test string.";
	String str2="n example";

	string.replace(9,5,str2);     

	printf("string.replace(9,5,str2) => '%s'\n",string.toConstChar());

	return 0;
}