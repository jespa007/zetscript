#include "zetscript.h"
int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	auto str=zs.newStringScriptObject();
	str->set("Hello World");

	delete str;

}
