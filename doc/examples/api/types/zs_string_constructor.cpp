#include "zetscript.h"

using zetscript::zs_string;

int main(void){

	// empty string
	zs_string empty_string;

	// initialized string
	zs_string string=zs_string("Hello world");

	return 0;
}