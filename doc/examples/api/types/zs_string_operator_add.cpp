#include "zetscript.h"
using zetscript::zs_string;
int main(){
	zs_string string1="Hello",string2="World";
	zs_string string=string1 + " "+ string2 + "!!";

	printf("%s\n",string.c_str());
	
	return 0;
}