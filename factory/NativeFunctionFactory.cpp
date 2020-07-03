/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "ZetScript.h"

namespace ZetScript{


	NativeFunctionFactory::NativeFunctionFactory(ZetScript *_zs){
		this->zs=_zs;
	}


	NativeFunctionFactory::~NativeFunctionFactory(){

		for(unsigned i = 0; i < class_function_member.size(); i++){
			delete ((std::function<void *(void *)> *)	class_function_member[i]);
		}
	}
}
