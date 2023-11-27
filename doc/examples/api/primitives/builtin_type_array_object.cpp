#include "zetscript.h"
int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	auto array=zs.newArrayObject();
	array->push<zetscript::zs_int>(10);
	//str="Hello World";

	delete array;

}
