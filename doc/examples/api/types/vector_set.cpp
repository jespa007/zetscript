#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	vector.append(1);
	vector.append(100);
	vector.append(3);

	vector.set(1,2); // vector = [1,2,3]
	
	return 0;
}