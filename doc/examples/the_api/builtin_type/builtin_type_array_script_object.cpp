#include "zetscript.h"
int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	auto array=zs.newArrayScriptObject();
	array->push(10);
	//str="Hello World";

	delete array;

}
