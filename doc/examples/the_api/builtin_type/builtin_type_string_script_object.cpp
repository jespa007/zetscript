#include "zetscript.h"
int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	auto str=zs.newStringScriptObject();
	//str="Hello World";

	delete str;

}
