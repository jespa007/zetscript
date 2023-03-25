#include "zetscript.h"

using zetscript::ZetScript;

// c function
void fun(ZetScript *_zs){
    printf("Hello world\n");
}

int main(int argc, char *argv[]){
	ZetScript zs;

    zs.bindFunction("fun",fun);

    // call c function 
    zs.eval(
        "fun();"
 	);

    return 0;
}

