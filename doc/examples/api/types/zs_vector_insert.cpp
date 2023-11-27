#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	vector.append(1);
	vector.append(2);
	vector.append(4);

	// insert integer 3 at position 2
	vector.insert(2,3);

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