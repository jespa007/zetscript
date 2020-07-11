/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "ZetScript.h"

namespace zetscript{


	FunctionProxyFactory::FunctionProxyFactory(){

	}


	FunctionProxyFactory::~FunctionProxyFactory(){

		for(unsigned i = 0; i < class_function_member.size(); i++){
			delete ((std::function<void *(void *)> *)	class_function_member[i]);
		}
	}
}
