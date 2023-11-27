#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector1,vector2;

	vector1.append(1);
	vector1.append(2);
	vector1.append(3);
	vector1.append(4);

	vector2=vector1;

	printf("vector2 contents  : [");

	for(int i=0; i < vector2.size(); i++){
		if(i>0){
			printf(",");
		}
		printf("%i",vector2.get(i));
	}

	printf("]\n");

	return 0;
}