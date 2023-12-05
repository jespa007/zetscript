#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	vector.push(1);
	vector.push(2);
	vector.push(3);

	printf("vector.length() => %i",vector.length());
	
	return 0;
}