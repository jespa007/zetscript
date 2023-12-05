#include "zetscript.h"

using zetscript::Vector;

int main(){
	Vector<int> vector;

	for(int i=0; i < 5; i++){
		vector.push(i);
	}

	int *ptr_data=vector.data();

	printf("contents from ptr_data : [");

	for(int i=0; i < vector.length(); i++){
		if(i>0){
			printf(",");
		}
		printf("%i",*ptr_data++);
	}

	printf("]\n");
	
	return 0;
}