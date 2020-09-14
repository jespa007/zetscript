/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	FunctionProxyFactory::FunctionProxyFactory(){
		function_proxies=new std::vector<FunctionProxyData *>;
	}


	FunctionProxyFactory::~FunctionProxyFactory(){

		for(unsigned i = 0; i < function_proxies->size(); i++){
			delete function_proxies->at(i);
		}

		delete function_proxies;
	}
}
