#include "zetscript.h"

int main(){
    zetscript::String s="My blue car with blue door and blue wheel";

    printf("s.replace(\"blue\",\"green\") => '%s'\n",s.replace("blue","green").toConstChar());
	return 0;
}