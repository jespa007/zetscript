#include "zetscript.h"

int main(){
	zetscript::String s="Hello World";
	
	printf("s.toUpperCase() => '%s'",s.toUpperCase().toConstChar());
	return 0;
}