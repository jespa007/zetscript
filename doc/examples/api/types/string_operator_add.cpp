#include "zetscript.h"

int main(){
	zetscript::String string1="Hello",string2="World";
	zetscript::String string=string1 + " "+ string2 + "!!";

	printf("%s\n",string.toConstChar());
	
	return 0;
}