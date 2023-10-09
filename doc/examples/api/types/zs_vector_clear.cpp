#include "zetscript.h"
using zetscript::zs_vector;
int main(){
	zs_vector<int> vector;

	for(int i=0; i < 10; i++){
		vector.push_back(i);
	}

	vector.clear();

	printf("vector.size() => %i\n",vector.size());

	
	return 0;
}