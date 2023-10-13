#include "zetscript.h"
using zetscript::zs_vector;
int main(){
	zs_vector<int> vector1;
	zs_vector<int> vector2;
	zs_vector<int> vector3;

	for(int i=0; i < 2; i++){
		vector1.push_back(i);
	}

	for(int i=2; i < 4; i++){
		vector2.push_back(i);
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