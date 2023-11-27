#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	vector.append(1);
	vector.append(2);
	vector.append(3);

	printf("vector.size() => %i",vector.size());
	
	return 0;
}