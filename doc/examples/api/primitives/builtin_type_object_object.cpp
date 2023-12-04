#include "zetscript.h"
int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	auto object=zs.newObjectScriptObject();

	object->set<zetscript::zs_int>("a",10);

	delete object;

}
