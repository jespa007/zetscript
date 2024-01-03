#include "zetscript.h"

int main(){
	zetscript::String string="Hello Wd";

	string.insert(7,"orl");
	printf("string.insert(7,\"orl\") => '%s'\n",string.toConstChar());

	return 0;
}