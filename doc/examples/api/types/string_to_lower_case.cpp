#include "zetscript.h"

int main(){
	zetscript::String s="Hello World";
	
	printf("s.toLowerCase() => '%s'",s.toLowerCase().toConstChar());
	return 0;
}