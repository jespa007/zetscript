#include "zetscript.h"
using zetscript::zs_vector;
int main(){
	zs_vector<int> vector;

	vector.push_back(10);
	vector.push_back(100);
	vector.push_back(1000);

	printf("vector.get(1) => %i\n",vector.get(1));
	
	return 0;
}