#include "zetscript.h"

int main(){
	zetscript::String s="The quick brown fox jumps over the lazy dog.";
	
	zetscript::Vector<zetscript::String> v=s.split(' ');
	
	for(size_t i=0; i < v.length();i++ ){
		printf("%s\n",v.get(i).toConstChar());
	}
}