#include "zetscript.h"

int main(){

	zetscript::String s;
	
	printf("%s\n",zetscript::String::format("Characters: %c %c", 'a', 65).toConstChar());
   	printf("%s\n",zetscript::String::format("Decimals: %d %ld", 1977, 650000L).toConstChar());
   	printf("%s\n",zetscript::String::format("Preceding with blanks: %10d", 1977).toConstChar());
   	printf("%s\n",zetscript::String::format("Preceding with zeros: %010d", 1977).toConstChar());
   	printf("%s\n",zetscript::String::format("Some different radices: %d %x %o %#x %#o", 100, 100, 100, 100, 100).toConstChar());
   	printf("%s\n",zetscript::String::format("floats: %4.2f %+.0e %E", 3.1416, 3.1416, 3.1416).toConstChar());
   	printf("%s\n",zetscript::String::format("Width trick: %*d", 5, 10).toConstChar());
   	printf("%s\n",zetscript::String::format("%s", "A string").toConstChar());
	printf("%s\n",zetscript::String::format("%10s", "A limited string").toConstChar());

	return 0;
}