#include "zetscript.h"

using zetscript::zs_string;

int main(){
	zs_string string="the fox jumps over the lazy dog.";

	printf("string.find_last_of(\"the\") => %i\n",string.find_last_of("fo"));

	return 0;
}