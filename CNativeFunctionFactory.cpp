/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	CNativeFunctionFactory::CNativeFunctionFactory(){

	}


	CNativeFunctionFactory::~CNativeFunctionFactory(){


		for(unsigned i = 0; i < c_void_function.size(); i++){
			delete_proxy_function<void>(c_void_function[i].n_args,c_void_function[i].fun_ptr);//=(int)new_proxy_function<void>(irs.m_arg.size(),function_ptr))==0){//(int)function_ptr;

		}

		for(unsigned i = 0; i < c_int_function.size(); i++){
			delete_proxy_function<int>(c_int_function[i].n_args,c_int_function[i].fun_ptr);
		}

		for(unsigned i = 0; i < class_function_member.size(); i++){
			delete ((std::function<void *(void *,PROXY_CREATOR)> *)	class_function_member[i]);
		}
	}
}
