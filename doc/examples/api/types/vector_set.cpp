#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	vector.push(1);
	vector.push(100);
	vector.push(3);

	vector.set(1,2); // vector = [1,2,3]
	
	return 0;
}