#include "zetscript.h"

using zetscript::zs_string;

int main(){
	zs_string string="Hello Wd";

	string.insert(7,"orl");
	printf("string.insert(7,\"orl\") => '%s'\n",string.c_str());

	return 0;
}