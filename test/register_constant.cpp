
#include "ZetScript.h"

using namespace zetscript;

int main(){

	ZetScript *zs = new ZetScript(); // instance ZetScript

	zs->registerConstantValue("MY_CONSTANT",10);

	zs->evalString(
		"print(\"c:\"+MY_CONSTANT);"
	);

	delete zs;

#ifdef __MEMMANAGER__
  MEM_ViewStatus();
#endif
}
