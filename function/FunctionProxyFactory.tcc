

namespace zetscript{

	template <typename _C, typename _R, class _T, typename..._A>
	auto FunctionProxyFactory::newProxyFunctionMember(int nargs, _R (_T:: *fun_obj)(_A...))
	->typename std::enable_if<(std::is_same<_R,void>::value==true)>::type *
	{
		void *fun_ptr=NULL;

		if(nargs > MAX_NATIVE_FUNCTION_ARGS){
			THROW_RUNTIME_ERROR("Max arguments reached");
		}

		switch(nargs){
		case 0:
			fun_ptr=(void *)(new std::function<void (
				intptr_t
			)> (
				[fun_obj](
					intptr_t obj
				){
					(((_C *)obj)->* ((void (_T    ::*)())fun_obj)) (
					);
				}
			));
			break;
		case 1:
			fun_ptr=(void *)(new std::function<void (
				intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
				){
					(((_C *)obj)->* ((void (_T    ::*)(
							intptr_t
					))fun_obj)) (
						param1
					);
				}
			));
			break;
		case 2:
			fun_ptr=(void *)(new std::function<void (
				intptr_t
				,intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
					, intptr_t param2
				){
					(((_C *)obj)->* ((void (_T    ::*)(
						 intptr_t
						,intptr_t
					))fun_obj)) (
						param1
						,param2
					);
				}
			));
			break;
		case 3:
			fun_ptr=(void *)(new std::function<void (
				intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
					, intptr_t param2
					, intptr_t param3
				){
					(((_C *)obj)->* ((void (_T    ::*)(
							intptr_t
							,intptr_t
							,intptr_t
					))fun_obj)) (
						param1
						,param2
						,param3
					);
				}
			));
			break;
		case 4:
			fun_ptr=(void *)(new std::function<void (
				intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
					, intptr_t param2
					, intptr_t param3
					, intptr_t param4
				){
					(((_C *)obj)->* ((void (_T    ::*)(
						intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
					))fun_obj)) (
						param1
						,param2
						,param3
						,param4
					);
				}
			));
			break;
		case 5:
			fun_ptr=(void *)(new std::function<void (
				intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
					,intptr_t param2
					,intptr_t param3
					,intptr_t param4
					,intptr_t param5
				){
					(((_C *)obj)->* ((void (_T    ::*)(
						intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
					))fun_obj)) (
						param1
						,param2
						,param3
						,param4
						,param5
					);
				}
			));
			break;
		case 6:
			fun_ptr=(void *)(new std::function<void (
				intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
					, intptr_t param2
					, intptr_t param3
					, intptr_t param4
					, intptr_t param5
					, intptr_t param6
				){
					(((_C *)obj)->* ((void (_T    ::*)(
						intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
					))fun_obj)) (
						param1
						,param2
						,param3
						,param4
						,param5
						,param6
					);
				}
			));
			break;
		default:

			break;
		}


		function_proxies->push_back(
			new FunctionProxyData(
					fun_ptr
					,nargs
					,true
			)
		);


	}

	//----------------------------------------
	//
	// BYTE_CODE_RET PROXY FUNCTIONS Member function C++
	//
	template <typename _C, typename _R, class _T, typename..._A>
	auto FunctionProxyFactory::newProxyFunctionMember(int nargs, _R (_T:: *fun_obj)(_A...))
	->typename std::enable_if<(std::is_same<_R,void>::value==false)>::type *
	{
		void *fun_ptr=NULL;

		if(nargs > MAX_NATIVE_FUNCTION_ARGS){
			THROW_RUNTIME_ERROR("Max arguments reached");
		}

		switch(nargs){
		case 0:
			fun_ptr=(void *)(new std::function<intptr_t (
				intptr_t
			)> (
				[fun_obj](
					intptr_t obj
				){
					return (((_C *)obj)->* ((intptr_t (_T    ::*)())fun_obj)) (
					);
				}
			));
			break;
		case 1:
			fun_ptr=(void *)(new std::function<intptr_t (
				intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
				){
				return  (((_C *)obj)->* ((intptr_t (_T    ::*)(
							intptr_t
					))fun_obj)) (
						param1
					);
				}
			));
			break;
		case 2:
			fun_ptr=(void *)(new std::function<intptr_t (
				intptr_t
				,intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
					, intptr_t param2
				){
				return  (((_C *)obj)->* ((intptr_t (_T    ::*)(
						 intptr_t
						,intptr_t
					))fun_obj)) (
						param1
						,param2
					);
				}
			));
			break;
		case 3:
			fun_ptr=(void *)(new std::function<intptr_t (
				intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
					, intptr_t param2
					, intptr_t param3
				){
				return  (((_C *)obj)->* ((intptr_t (_T    ::*)(
							intptr_t
							,intptr_t
							,intptr_t
					))fun_obj)) (
						param1
						,param2
						,param3
					);
				}
			));
			break;
		case 4:
			fun_ptr=(void *)(new std::function<intptr_t (
				intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
					, intptr_t param2
					, intptr_t param3
					, intptr_t param4
				){
				return (((_C *)obj)->* ((intptr_t (_T    ::*)(
						intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
					))fun_obj)) (
						param1
						,param2
						,param3
						,param4
					);
				}
			));
			break;
		case 5:
			fun_ptr=(void *)(new std::function<intptr_t (
				intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
					,intptr_t param2
					,intptr_t param3
					,intptr_t param4
					,intptr_t param5
				){
				return (((_C *)obj)->* ((intptr_t (_T    ::*)(
						intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
					))fun_obj)) (
						param1
						,param2
						,param3
						,param4
						,param5
					);
				}
			));
			break;
		case 6:
			fun_ptr=(void *)(new std::function<intptr_t (
				intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
				,intptr_t
			)> (
				[fun_obj](
					intptr_t obj
					,intptr_t param1
					, intptr_t param2
					, intptr_t param3
					, intptr_t param4
					, intptr_t param5
					, intptr_t param6
				){
				return (((_C *)obj)->* ((intptr_t (_T    ::*)(
						intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
						,intptr_t
					))fun_obj)) (
						param1
						,param2
						,param3
						,param4
						,param5
						,param6
					);
				}
			));
			break;
		}

		function_proxies->push_back(
			new FunctionProxyData(
				fun_ptr
				,nargs
				,false
			)
		);
	}
}
