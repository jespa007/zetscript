#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector;

	vector.push(1);
	vector.push(2);
	vector.push(3);
	vector.push(4);

	vector.pop();

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