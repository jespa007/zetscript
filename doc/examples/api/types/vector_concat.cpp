#include "zetscript.h"

int main(){
	zetscript::Vector<int> vector1;
	zetscript::Vector<int> vector2;
	zetscript::Vector<int> vector3;

	for(int i=0; i < 2; i++){
		vector1.push(i);
	}

	for(int i=2; i < 4; i++){
		vector2.push(i);
	}
	vector3.concat(vector1);
	vector3.concat(vector2);

	printf("vector3 contents : [");

	 for(int i=0; i < vector3.length(); i++){
		if(i>0){
			printf(",");
		}
		printf("%i",vector3.get(i));
	}
	
	printf("]\n");

	return 0;
}