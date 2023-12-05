#include "zetscript.h"

using zetscript::String;

int main(){
	String string="Hello Wd";

	string.insert(7,"orl");
	printf("string.insert(7,\"orl\") => '%s'\n",string.toConstChar());

	return 0;
}