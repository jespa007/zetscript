#include "zetscript.h"
using zetscript::zs_vector;
int main(){
	zs_vector<int> vector;

	for(int i=0; i < 5; i++){
		vector.push_back(i);
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