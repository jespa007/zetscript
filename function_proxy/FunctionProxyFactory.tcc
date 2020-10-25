

namespace zetscript{

	template <typename _C, typename _R>
	std::function<void * (void *)> * FunctionProxyFactory::newProxyMemberVariable(_R (_C:: *ptr_member_var)){
		std::function<void * (void *)> * fun_ptr=(void *)(new std::function<void * (
				void *
		)> (
			[ptr_member_var](
					void * obj
			){
				return &((((_C *)obj)->*ptr_member_var));
			}
		));

		member_variable_proxies->push_back(fun_ptr);
	}

	template <typename _C, typename _R, typename _T, typename..._A>
	auto FunctionProxyFactory::newProxyMemberFunction(int nargs, _R (_T:: *ptr_member_fun)(_A...))
	->typename std::enable_if<(std::is_same<_R,void>::value==true)>::type *
	{
		void *fun_ptr=NULL;

		if(nargs > MAX_NATIVE_FUNCTION_ARGS){
			THROW_RUNTIME_ERROR("Max arguments reached");
		}

		switch(nargs){
		case 0:
			fun_ptr=(void *)(new std::function<void (
				zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
				){
					(((_C *)obj)->* ((void (_T    ::*)())ptr_member_fun)) (
					);
				}
			));
			break;
		case 1:
			fun_ptr=(void *)(new std::function<void (
				zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
				){
					(((_C *)obj)->* ((void (_T    ::*)(
							zs_int
					))ptr_member_fun)) (
						param1
					);
				}
			));
			break;
		case 2:
			fun_ptr=(void *)(new std::function<void (
				zs_int
				,zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
					, zs_int param2
				){
					(((_C *)obj)->* ((void (_T    ::*)(
						 zs_int
						,zs_int
					))ptr_member_fun)) (
						param1
						,param2
					);
				}
			));
			break;
		case 3:
			fun_ptr=(void *)(new std::function<void (
				zs_int
				,zs_int
				,zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
					, zs_int param2
					, zs_int param3
				){
					(((_C *)obj)->* ((void (_T    ::*)(
							zs_int
							,zs_int
							,zs_int
					))ptr_member_fun)) (
						param1
						,param2
						,param3
					);
				}
			));
			break;
		case 4:
			fun_ptr=(void *)(new std::function<void (
				zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
					, zs_int param2
					, zs_int param3
					, zs_int param4
				){
					(((_C *)obj)->* ((void (_T    ::*)(
						zs_int
						,zs_int
						,zs_int
						,zs_int
					))ptr_member_fun)) (
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
				zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
					,zs_int param2
					,zs_int param3
					,zs_int param4
					,zs_int param5
				){
					(((_C *)obj)->* ((void (_T    ::*)(
						zs_int
						,zs_int
						,zs_int
						,zs_int
						,zs_int
					))ptr_member_fun)) (
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
				zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
					, zs_int param2
					, zs_int param3
					, zs_int param4
					, zs_int param5
					, zs_int param6
				){
					(((_C *)obj)->* ((void (_T    ::*)(
						zs_int
						,zs_int
						,zs_int
						,zs_int
						,zs_int
						,zs_int
					))ptr_member_fun)) (
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


		member_function_proxies->push_back(
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
	template <typename _C, typename _R, typename _T, typename..._A>
	auto FunctionProxyFactory::newProxyMemberFunction(int nargs, _R (_T:: *ptr_member_fun)(_A...))
	->typename std::enable_if<(std::is_same<_R,void>::value==false)>::type *
	{
		void *fun_ptr=NULL;

		if(nargs > MAX_NATIVE_FUNCTION_ARGS){
			THROW_RUNTIME_ERROR("Max arguments reached");
		}

		switch(nargs){
		case 0:
			fun_ptr=(void *)(new std::function<zs_int (
				zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
				){
					return (((_C *)obj)->* ((zs_int (_T    ::*)())ptr_member_fun)) (
					);
				}
			));
			break;
		case 1:
			fun_ptr=(void *)(new std::function<zs_int (
				zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
				){
				return  (((_C *)obj)->* ((zs_int (_T    ::*)(
							zs_int
					))ptr_member_fun)) (
						param1
					);
				}
			));
			break;
		case 2:
			fun_ptr=(void *)(new std::function<zs_int (
				zs_int
				,zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
					, zs_int param2
				){
				return  (((_C *)obj)->* ((zs_int (_T    ::*)(
						 zs_int
						,zs_int
					))ptr_member_fun)) (
						param1
						,param2
					);
				}
			));
			break;
		case 3:
			fun_ptr=(void *)(new std::function<zs_int (
				zs_int
				,zs_int
				,zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
					, zs_int param2
					, zs_int param3
				){
				return  (((_C *)obj)->* ((zs_int (_T    ::*)(
							zs_int
							,zs_int
							,zs_int
					))ptr_member_fun)) (
						param1
						,param2
						,param3
					);
				}
			));
			break;
		case 4:
			fun_ptr=(void *)(new std::function<zs_int (
				zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
					, zs_int param2
					, zs_int param3
					, zs_int param4
				){
				return (((_C *)obj)->* ((zs_int (_T    ::*)(
						zs_int
						,zs_int
						,zs_int
						,zs_int
					))ptr_member_fun)) (
						param1
						,param2
						,param3
						,param4
					);
				}
			));
			break;
		case 5:
			fun_ptr=(void *)(new std::function<zs_int (
				zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
					,zs_int param2
					,zs_int param3
					,zs_int param4
					,zs_int param5
				){
				return (((_C *)obj)->* ((zs_int (_T    ::*)(
						zs_int
						,zs_int
						,zs_int
						,zs_int
						,zs_int
					))ptr_member_fun)) (
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
			fun_ptr=(void *)(new std::function<zs_int (
				zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
				,zs_int
			)> (
				[ptr_member_fun](
					zs_int obj
					,zs_int param1
					, zs_int param2
					, zs_int param3
					, zs_int param4
					, zs_int param5
					, zs_int param6
				){
				return (((_C *)obj)->* ((zs_int (_T    ::*)(
						zs_int
						,zs_int
						,zs_int
						,zs_int
						,zs_int
						,zs_int
					))ptr_member_fun)) (
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

		member_function_proxies->push_back(
			new FunctionProxyData(
				fun_ptr
				,nargs
				,false
			)
		);
	}
}
