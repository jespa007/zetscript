#include "zetscript.h"
using zetscript::Vector;
int main(){
	Vector<int> vector1;
	Vector<int> vector2;
	Vector<int> vector3;

	for(int i=0; i < 2; i++){
		vector1.append(i);
	}

	for(int i=2; i < 4; i++){
		vector2.append(i);
	}
	vector3.concat(vector1);
	vector3.concat(vector2);

	printf("vector3 contents : [");

	 for(int i=0; i < vector3.size(); i++){
		if(i>0){
			printf(",");
		}
		printf("%i",vector3.get(i));
	}
	
	printf("]\n");

	return 0;
}