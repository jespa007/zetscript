#include "zetscript.h"

typedef struct{
	int x;
	int y;
}MyType;


int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	zs.bindType<MyType>("MyType");

};


