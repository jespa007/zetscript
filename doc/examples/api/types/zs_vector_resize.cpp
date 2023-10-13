#include "zetscript.h"
using zetscript::zs_vector;
int main(){
	zs_vector<int> vector;

	vector.resize(4);

	for(int i=0; i < vector.size(); i++){
		vector.set(i,i);
	}

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