#include "zetscript.h"

int main(){
	zetscript::Vector<int> vector;

	vector.push(1);
	vector.push(2);
	vector.push(3);

	printf("vector.length() => %i",vector.length());
	
	return 0;
}