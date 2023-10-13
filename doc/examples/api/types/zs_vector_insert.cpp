#include "zetscript.h"
using zetscript::zs_vector;
int main(){
	zs_vector<int> vector;

	vector.push_back(1);
	vector.push_back(2);
	vector.push_back(4);

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