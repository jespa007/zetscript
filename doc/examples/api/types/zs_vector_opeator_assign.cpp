#include "zetscript.h"
using zetscript::zs_vector;
int main(){
	zs_vector<int> vector1,vector2;

	vector1.push_back(1);
	vector1.push_back(2);
	vector1.push_back(3);
	vector1.push_back(4);

	vector2=vector1;


	 printf("vector2 contents  : [");

	 for(int i=0; i < vector2.size(); i++){
		if(i>0){
			printf(",");
		}

		printf("%i",vector2.get(i));

	 }

	 printf("]\n");

}