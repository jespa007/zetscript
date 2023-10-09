#include "zetscript.h"
using zetscript::zs_vector;
int main(){
	zs_vector<int> vector;

	vector.push_back(1);
	vector.push_back(100);
	vector.push_back(3);

	vector.set(1,2); // vector = [1,2,3]
	
	return 0;
}