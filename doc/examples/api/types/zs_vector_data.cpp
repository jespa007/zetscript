#include "zetscript.h"

using zetscript::zs_vector;

int main(){
	zs_vector<int> vector;

	for(int i=0; i < 10; i++){
		vector.push_back(i);
	}


	int *ptr_data=vector.data();

	 printf("contents from ptr_data : [");

	 for(int i=0; i < vector.size(); i++){
		if(i>0){
			printf(",");
		}

		printf("%i",*ptr_data++);

	 }

	 printf("]\n");



	
	return 0;
}