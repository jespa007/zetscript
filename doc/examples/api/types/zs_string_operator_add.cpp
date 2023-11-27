#include "zetscript.h"
using zetscript::String;
int main(){
	String string1="Hello",string2="World";
	String string=string1 + " "+ string2 + "!!";

	printf("%s\n",string.toConstChar());
	
	return 0;
}