#include "zetscript.h"

int main(){
	zetscript::Vector<int> vector;

	for(int i=0; i < 5; i++){
		vector.push(i);
	}

	vector.erase(2);

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