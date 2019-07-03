/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

using namespace zs;

void say_helloworld(){
	printf("Hello World!\n");
}

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	REGISTER_C_FUNCTION("say_helloworld",say_helloworld);

	zs::eval_string("say_helloworld();"); // Call c function and prints hello world!


	CZetScript::destroy();

#ifdef __MEMMANAGER__
  MEM_ViewStatus();
#endif

	return 0;
}
