#include "zetscript.h"
using zetscript::zs_string;
int main(){
	
	printf("zs_string(\"Hello world!\").empty() => '%s' \n"
	,zs_string("Hello World").empty()?"true":"false");

	printf("zs_string().empty() => '%s' \n"
	,zs_string("Hello World").empty()?"true":"false");

	
	return 0;
}