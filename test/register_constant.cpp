
#include "CZetScript.h"

using namespace zs;

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	register_C_Constant("MY_CONSTANT",10);

	zs::eval_string(
		"print(\"c:\"+MY_CONSTANT);"
	);



	CZetScript::destroy();

#ifdef __MEMMANAGER__
  MEM_ViewStatus();
#endif
}
