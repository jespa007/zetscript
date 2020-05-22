/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


namespace zetscript{

	class  CNativeFunctionFactory{

	public:

		CNativeFunctionFactory(CZetScript *_zs);


		//----------------------------------------
		//
		// VOID PROXY FUNCTIONS Member function C++
		//
		template <typename _C, typename _R, class _T, typename..._A>
		auto  c_member_class_function_proxy(int nargs, _R (_T:: *fun_obj)(_A...))
		->typename std::enable_if<(std::is_same<_R,void>::value==true)>::type *
		{

			std::function<void *(void *)> *c_function_builder=NULL;
			switch(nargs){
			case 0:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerVoidParam0Caller<_C,_T>((_C *)obj,(void (_T::*)())(fun_obj)));
				});
				break;
			case 1:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj ){
					return (void *)(new CMemberFunctionPointerVoidParam1Caller<_C,_T>((_C *)obj,(void (_T::*)(intptr_t))(fun_obj)));
				});
				break;
			case 2:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerVoidParam2Caller<_C,_T>((_C *)obj,(void (_T::*)(intptr_t,intptr_t))(fun_obj)));
				});
				break;
			case 3:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerVoidParam3Caller<_C,_T>((_C *)obj,(void (_T::*)(intptr_t,intptr_t,intptr_t))(fun_obj)));
				});
				break;
			case 4:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerVoidParam4Caller<_C,_T>((_C *)obj,(void (_T::*)(intptr_t,intptr_t,intptr_t,intptr_t))(fun_obj)));
				});
				break;
			case 5:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerVoidParam5Caller<_C,_T>((_C *)obj,(void (_T::*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(fun_obj)));
				});
				break;
			case 6:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerVoidParam6Caller<_C,_T>((_C *)obj,(void (_T::*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(fun_obj)));
				});
				break;
			default:
				THROW_RUNTIME_ERROR("Max argyments reached");
				break;
			}

			class_function_member.push_back(c_function_builder);

			return (void *)c_function_builder;
		}


		//----------------------------------------
		//
		// RET PROXY FUNCTIONS Member function C++
		//

		template <typename _C, typename _R, class _T, typename..._A>
		auto  c_member_class_function_proxy(int nargs, _R (_T:: *fun_obj)(_A...))
		->typename std::enable_if<(std::is_same<_R,void>::value==false)>::type *
		{
			std::function<void *(void *)> *c_function_builder=NULL;

			switch(nargs){
			case 0:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerRetParam0Caller<_C,_T>((_C *)obj,(intptr_t (_T::*)())(fun_obj)));
				});
				break;
			case 1:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerRetParam1Caller<_C,_T>((_C *)obj,(intptr_t (_T::*)(intptr_t))(fun_obj)));
				});
				break;
			case 2:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerRetParam2Caller<_C,_T>((_C *)obj,(intptr_t (_T::*)(intptr_t,intptr_t))(fun_obj)));
				});
				break;
			case 3:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerRetParam3Caller<_C,_T>((_C *)obj,(intptr_t (_T::*)(intptr_t,intptr_t,intptr_t))(fun_obj)));
				});
				break;
			case 4:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerRetParam4Caller<_C,_T>((_C *)obj,(intptr_t (_T::*)(intptr_t,intptr_t,intptr_t,intptr_t))(fun_obj)));
				});
				break;
			case 5:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerRetParam5Caller<_C,_T>((_C *)obj,(intptr_t (_T::*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(fun_obj)));
				});
				break;
			case 6:
				c_function_builder= new std::function<void *(void *)> ([fun_obj](void *obj){
					return (void *)(new CMemberFunctionPointerRetParam6Caller<_C,_T>((_C *)obj,(intptr_t (_T::*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(fun_obj)));
				});
				break;
			default:
				THROW_RUNTIME_ERROR("Max argyments reached");
				break;
			}

			class_function_member.push_back(c_function_builder);

			return (void *) c_function_builder;
		}


		~CNativeFunctionFactory();

	private:



		CZetScript *zs;


		std::vector<void *>	 class_function_member;

	};

}
