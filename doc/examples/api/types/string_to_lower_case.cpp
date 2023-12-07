#include "zetscript.h"
using zetscript::String;
int main(){
	String s="Hello World";
	
	printf("s.toLowerCase() => '%s'",s.toLowerCase().toConstChar());
	return 0;
}