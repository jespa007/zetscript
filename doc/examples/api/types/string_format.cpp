#include "zetscript.h"
using zetscript::String;
int main(){

	String s;
	
	printf("%s\n",String::format("Characters: %c %c", 'a', 65).toConstChar());
   	printf("%s\n",String::format("Decimals: %d %ld", 1977, 650000L).toConstChar());
   	printf("%s\n",String::format("Preceding with blanks: %10d", 1977).toConstChar());
   	printf("%s\n",String::format("Preceding with zeros: %010d", 1977).toConstChar());
   	printf("%s\n",String::format("Some different radices: %d %x %o %#x %#o", 100, 100, 100, 100, 100).toConstChar());
   	printf("%s\n",String::format("floats: %4.2f %+.0e %E", 3.1416, 3.1416, 3.1416).toConstChar());
   	printf("%s\n",String::format("Width trick: %*d", 5, 10).toConstChar());
   	printf("%s\n",String::format("%s", "A string").toConstChar());
	printf("%s\n",String::format("%10s", "A limited string").toConstChar());

	return 0;
}