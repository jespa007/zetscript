#include "zetscript.h"
int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	try{
		zs.eval(
			"Console::outln(\"Hello world\")"
		);
	}catch(std::exception & _ex){
		fprintf(stderr,"%s\n",_ex.what());
	}
}