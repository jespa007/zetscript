#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	for(int i=0; i < 10; i++){
		vector.append(i);
	}

	vector.clear();

	printf("vector.size() => %i\n",vector.size());
	
	return 0;
}