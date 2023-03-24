#include "zetscript.h"

int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	try{
		zs.evalFile(
			"file.zs"
		);
	}catch(std::exception & _ex){
		fprintf(stderr,"%s\n",_ex.what());
	}
}