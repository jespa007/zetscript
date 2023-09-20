#include "zetscript.h"
using zetscript::zs_string;
int main(){
	zs_string s="helilo world";

	printf("s => '%s'\n",s.c_str());
	s.erase(3);
	printf("s.erase(3) => '%s'\n",s);
	
	return 0;
}