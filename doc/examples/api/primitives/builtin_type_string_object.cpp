#include "zetscript.h"
int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	auto str=zs.newStringObject();
	str->set("Hello World");

	delete str;

}
