#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	for(int i=0; i < 5; i++){
		vector.append(i);
	}

	vector.erase(2);

	printf("vector contents  : [");
	for(int i=0; i < vector.size(); i++){
		if(i>0){
			printf(",");
		}
		printf("%i",vector.get(i));
	}
	
	printf("]\n");

	return 0;
}