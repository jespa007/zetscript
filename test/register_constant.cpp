
#include "CZetScript.h"

using namespace zetscript;

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	REGISTER_C_CONSTANT("MY_CONSTANT",10);

	zetscript::evalString(
		"print(\"c:\"+MY_CONSTANT);"
	);



	CZetScript::destroy();

#ifdef __MEMMANAGER__
  MEM_ViewStatus();
#endif
}
