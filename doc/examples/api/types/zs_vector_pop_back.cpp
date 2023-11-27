#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	vector.append(1);
	vector.append(2);
	vector.append(3);
	vector.append(4);

	vector.pop();

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