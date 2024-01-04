#include "zetscript.h"

int main(){
	zetscript::String string="the quick brown fox jumps over the lazy dog.";

	printf("string.find(\"the\") => %i\n",string.find("the"));
	printf("string.find(\"fox\") => %i\n",string.find("fox"));

	return 0;
}