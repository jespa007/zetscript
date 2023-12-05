#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	for(int i=0; i < 10; i++){
		vector.append(i);
	}

	vector.clear();

	printf("vector.length() => %i\n",vector.length());
	
	return 0;
}