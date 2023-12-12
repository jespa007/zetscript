#include "zetscript.h"
using zetscript::String;
int main(){
	zetscript::ScriptEngine se;

    String s="My blue car with blue door and blue wheel";

    printf("s.replace(\"blue\",\"green\") => '%s'\n",s.replace("blue","green").toConstChar());
	return 0;
}