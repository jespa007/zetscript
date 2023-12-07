#include "zetscript.h"
using zetscript::String;
int main(){

	String s="The quick brown fox jumps over the lazy dog.";
	
	printf("s.contains(\"fo\") => '%s'\n",s.contains("fo")?"true":"false");
	printf("s.contains(\"foy\") => '%s'\n",s.contains("foy")?"true":"false");
	return 0;
}