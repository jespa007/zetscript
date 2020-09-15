/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	FunctionProxyFactory::FunctionProxyFactory(){
		member_function_proxies=new std::vector<FunctionProxyData *>;
		member_variable_proxies=new std::vector<std::function<void *(void *)> *>;
	}


	FunctionProxyFactory::~FunctionProxyFactory(){

		for(unsigned i = 0; i < member_variable_proxies->size(); i++){
			delete member_variable_proxies->at(i);
		}

		for(unsigned i = 0; i < member_function_proxies->size(); i++){
			delete member_function_proxies->at(i);
		}

		delete member_function_proxies;
		delete member_variable_proxies;
	}
}
