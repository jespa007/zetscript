#include "zetscript.h"

int main(){
	zetscript::Vector<int> vector;

	for(int i=0; i < 10; i++){
		vector.push(i);
	}

	vector.clear();

	printf("vector.length() => %i\n",vector.length());
	
	return 0;
}