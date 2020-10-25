/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


namespace zetscript{

	class  FunctionProxyFactory{

	public:

		FunctionProxyFactory();

		template <typename _C, typename _R>
		std::function<void * (void *)> * newProxyMemberVariable(_R (_C:: *ptr_member_var));


		template <typename _C, typename _R, typename _T, typename..._A>
			auto newProxyMemberFunction(int nargs, _R (_T:: *ptr_member_fun)(_A...))
			->typename std::enable_if<(std::is_same<_R,void>::value==true)>::type *;

		template <typename _C, typename _R,  typename _T, typename..._A>
			auto newProxyMemberFunction(int nargs, _R (_T:: *ptr_member_fun)(_A...))
			->typename std::enable_if<(std::is_same<_R,void>::value==false)>::type *;

		~FunctionProxyFactory();

	private:

		struct FunctionProxyData{
			unsigned char n_args;
			void *function_ptr;
			bool is_void;

			FunctionProxyData(void *_function_ptr,unsigned char _n_args, bool _is_void)
			{
				function_ptr = _function_ptr;
				n_args= _n_args;
				is_void=_is_void;
			}

			~FunctionProxyData(){
				if(is_void){
					switch(n_args){
					case 0:
						delete ((std::function<void(zs_int)> *)function_ptr);
						break;
					case 1:
						delete ((std::function<void(zs_int,zs_int)> *)function_ptr);
						break;
					case 2:
						delete ((std::function<void(zs_int,zs_int,zs_int)> *)function_ptr);
						break;
					case 3:
						delete ((std::function<void(zs_int,zs_int,zs_int,zs_int)> *)function_ptr);
						break;
					case 4:
						delete ((std::function<void(zs_int,zs_int,zs_int,zs_int,zs_int)> *)function_ptr);
						break;
					case 5:
						delete ((std::function<void(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)function_ptr);
						break;
					case 6:
						delete ((std::function<void(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)function_ptr);
						break;
					}
				}
				else{
					switch(n_args){
						case 0:
							delete ((std::function<zs_int(zs_int)> *)function_ptr);
							break;
						case 1:
							delete ((std::function<zs_int(zs_int,zs_int)> *)function_ptr);
							break;
						case 2:
							delete ((std::function<zs_int(zs_int,zs_int,zs_int)> *)function_ptr);
							break;
						case 3:
							delete ((std::function<zs_int(zs_int,zs_int,zs_int,zs_int)> *)function_ptr);
							break;
						case 4:
							delete ((std::function<zs_int(zs_int,zs_int,zs_int,zs_int,zs_int)> *)function_ptr);
							break;
						case 5:
							delete ((std::function<zs_int(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)function_ptr);
							break;
						case 6:
							delete ((std::function<zs_int(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)function_ptr);
							break;
						}
				}
			}
		};

		std::vector<FunctionProxyData *>	 				*	member_function_proxies;
		std::vector<std::function<void *(void *)> *>	 	*	member_variable_proxies;

	};

}

#include "FunctionProxyFactory.tcc"
