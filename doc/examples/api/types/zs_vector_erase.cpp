#include "zetscript.h"
using zetscript::zs_vector;
int main(){
	zs_vector<int> vector;

	for(int i=0; i < 10; i++){
		vector.push_back(i);
	}


	vector.erase(5);
	

	 printf("vector contents  : [");

	 for(int i=0; i < vector.size(); i++){
		if(i>0){
			printf(",");
		}

		printf("%i",vector.get(i));

	 }

	 printf("]\n");
}