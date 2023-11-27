#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	vector.append(10);
	vector.append(100);
	vector.append(1000);

	printf("vector.get(1) => %i\n",vector.get(1));
	
	return 0;
}