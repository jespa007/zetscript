#include "zetscript.h"
using zetscript::String;
int main(){
	String s="Hello World";
	
	printf("s.toUpperCase() => '%s'",s.toUpperCase().toConstChar());
	return 0;
}