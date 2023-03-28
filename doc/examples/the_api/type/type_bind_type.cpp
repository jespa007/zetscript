#include "zetscript.h"

typedef struct{
	int x;
	int y;
}MyType;


int main(){

	zetscript::ZetScript zs;

	zs.bindType<MyType>("MyType");

};


