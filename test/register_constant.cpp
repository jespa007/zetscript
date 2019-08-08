
#include "zetscript.h"

using namespace zetscript;

int main(){

	CZetScript *zs = new CZetScript(); // instance zetscript

	zs->registerConstantValue("MY_CONSTANT",10);

	zs->evalString(
		"print(\"c:\"+MY_CONSTANT);"
	);

	delete zs;

#ifdef __MEMMANAGER__
  MEM_ViewStatus();
#endif
}
