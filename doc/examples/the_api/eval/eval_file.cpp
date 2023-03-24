#include "zetscript.h"

int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	zs.evalFile(
		"file.zs"
	);
}