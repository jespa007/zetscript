#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	vector.push(10);
	vector.push(100);
	vector.push(1000);

	printf("vector.get(1) => %i\n",vector.get(1));
	
	return 0;
}