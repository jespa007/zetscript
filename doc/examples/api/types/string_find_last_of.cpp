#include "zetscript.h"

int main(){
	zetscript::String string="the fox jumps over the lazy dog.";

	printf("string.findLastOf(\"the\") => %i\n",string.findLastOf("fo"));

	return 0;
}