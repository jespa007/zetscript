#include "zetscript.h"

using zetscript::zs_string;

int main(){
	zs_string s="The quick brown fox jumps over the lazy dog.";

	printf("s.indexOf(\"fo\") =>%i\n",s.find("fo"));
	printf("s.indexOf(\"foy\") => %i\n",s.find("foy"));

	return 0;
}