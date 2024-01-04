#include "zetscript.h"

int main(){
	zetscript::Vector<int> vector;

	vector.push(1);
	vector.push(2);
	vector.push(4);

	// insert integer 3 at position 2
	vector.insert(2,3);

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