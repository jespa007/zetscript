#include "zetscript.h"

int main(){
	zetscript::Vector<int> vector1,vector2;

	vector1.push(1);
	vector1.push(2);
	vector1.push(3);
	vector1.push(4);

	vector2=vector1;

	printf("vector2 contents  : [");

	for(int i=0; i < vector2.length(); i++){
		if(i>0){
			printf(",");
		}
		printf("%i",vector2.get(i));
	}

	printf("]\n");

	return 0;
}