#include "zetscript.h"

int main(){
	zetscript::Vector<int> vector;

	vector.resize(4);

	for(int i=0; i < vector.length(); i++){
		vector.set(i,i);
	}

	printf("vector contents  : [");

	 for(int i=0; i < vector.length(); i++){
		if(i>0){
			printf(",");
		}
		printf("%i",vector.get(i));
	}

	printf("]\n");
	
	return 0;
}