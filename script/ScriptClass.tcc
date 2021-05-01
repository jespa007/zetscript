/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	template < typename R>
	void ScriptClass::registerNativeMemberVariableStaticConst(
			const char *var_name
			, const R var_pointer
			, const char *registered_file
			,short registered_line)
	{
			// to make compatible MSVC shared library
			//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();
		std::string var_type = typeid(R).name();
		std::string error;

			// register variable...
			Symbol *symbol=this->registerNativeMemberVariable(
					error
					,registered_file
					,registered_line
					,var_name
					,var_type
					,(zs_int)var_pointer
					,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC | SYMBOL_PROPERTY_CONST
			);

			if(symbol == NULL){
				THROW_RUNTIME_ERROR(error.c_str());
			}
	}

	template <typename F>
	int ScriptClass::getNativeMemberFunctionRetArgsTypes(
			const char *function_name
			,F ptr_function
			,std::string & return_type
			,std::vector<ScriptFunctionArg> & arg_info
		)
	{
		int idx_return_type=-1;
		std::vector<std::string> arg;
		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(ptr_function)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});

		if(arg.size()>MAX_NATIVE_FUNCTION_ARGS){
			THROW_RUNTIME_ERROR("Max argyments reached");
		}

		// check valid parameters ...
		if((idx_return_type=getIdxClassFromItsNativeType(return_type)) == -1){
			THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",zs_rtti::demangle(return_type).c_str(),function_name);
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromItsNativeType(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,zs_rtti::demangle(arg[i]).c_str(),function_name,zs_rtti::demangle(arg[i]).c_str());
			}

			if(idx_type==ZS_IDX_UNDEFINED){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered",i,zs_rtti::demangle(arg[i]).c_str(),function_name);
			}

			arg_info.push_back({idx_type,arg[i]});
		}

		return idx_return_type;
	}

	/*
	 * register C setter
	 */
	template <typename F>
	void ScriptClass::registerNativeMemberAttributeSetter(
			const char *attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
	){

	}

	/*
	 * register C getter
	 */
	template <typename F>
	void ScriptClass::registerNativeMemberAttributeGetter(
			const char *attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
	){

	}

	/**
	 * Register C Member function Class
	 * like register function c but is added to member function list according type C
	 */
	template <typename F>
	void ScriptClass::registerNativeMemberFunctionStatic(
			const char *function_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string return_type;
		std::string error;
		std::vector<std::string> params;

		std::vector<ScriptFunctionArg> arg_info;

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(function_name,ptr_function,return_type,arg_info);

		// register member function...
		Symbol * symbol_sf = this->registerNativeMemberFunction(
				error
				,registered_file
				,registered_line
				,function_name
				,arg_info
				, idx_return_type
				, (zs_int)ptr_function
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC
		);

		if(symbol_sf == NULL){
			THROW_RUNTIME_ERROR(error.c_str());
		}

		ZS_PRINT_DEBUG("Registered member function name %s::%s",zs_rtti::demangle(typeid(C).name()).c_str(), function_name);

		// check whether is static metamethod...
		if(ZS_STRCMP(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_SET),!=,function_name)){

			for(int i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				if(ZS_STRCMP(byte_code_metamethod_to_symbol_str((ByteCodeMetamethod)i),==,function_name)){

					// check if they are gte,gt,equ, not_equ, lt, lte
					if(  i == BYTE_CODE_METAMETHOD_EQU //STRCMP(function_name, == ,"_equ")
					  || i == BYTE_CODE_METAMETHOD_NOT_EQU //STRCMP(function_name, ==, "_nequ")
					  || i == BYTE_CODE_METAMETHOD_LT//STRCMP(function_name, ==, "_lt")
					  || i == BYTE_CODE_METAMETHOD_LTE//STRCMP(function_name, ==, "_lte")
					  || i == BYTE_CODE_METAMETHOD_GT//STRCMP(function_name, ==, "_gt")
					  || i == BYTE_CODE_METAMETHOD_GTE//STRCMP(function_name, ==, "_gte")
					  || i == BYTE_CODE_METAMETHOD_NOT//STRCMP(function_name, ==, "_gte")
					  ){
						// return type must be bool...
						if(ZS_STRCMP(return_type.c_str(), != ,typeid(bool).name())){
							THROW_RUNTIME_ERROR("error registering metamethod %s::%s. Expected return bool but it was %s",
									this->symbol_class.name.c_str(),
									function_name,
									zs_rtti::demangle(return_type.c_str()).c_str());
						}
					}else if((return_type != this->str_class_ptr_type) && (i!= BYTE_CODE_METAMETHOD_SET)){

						THROW_RUNTIME_ERROR("error registering metamethod %s::%s. Expected return %s but it was %s",
								this->symbol_class.name.c_str(),
								function_name,
								zs_rtti::demangle(return_type.c_str()).c_str());
					}
					break;
				}
			}
		}else{
			THROW_RUNTIME_ERROR("error! cannot register metamethod set on static function. Must be member function");
		}
	}

	/*
	 * register static function registerNativeMemberFunction as function member
	 * Is automatically added in function member list according first parameter type of function_type
	 */
	template <typename F>
	void ScriptClass::registerNativeMemberFunction(
			const char *function_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
	){
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string return_type;
		std::vector<std::string> params;
		std::vector<ScriptFunctionArg> arg_info;
		zs_int ref_ptr=0;
		std::string function_class_name;// = zs_rtti::demangle(typeid(T).name())+"::"+function_name;
		std::string error;
		Symbol *symbol;

		// 1. check all parameters ok.
		int idx_return_type=getNativeMemberFunctionRetArgsTypes(function_name,ptr_function,return_type,arg_info);

		if(arg_info.size()==0){
			THROW_RUNTIME_ERROR("registerNativeMemberFunction at least need first parameter that defines the object to add function %s",function_name);
		}

		ScriptClass * c_class_first_arg=	getScriptClass(arg_info[0].idx_type);
		if(c_class_first_arg == NULL){
			THROW_RUNTIME_ERROR("class %s is not registered",arg_info[0].arg_name.c_str());
		}

		if(c_class_first_arg->str_class_ptr_type !=  this->str_class_ptr_type){
			THROW_RUNTIME_ERROR("first parameter is \"%s\" and should be the same type as the class (i.e \"%s\")",arg_info[0].arg_name.c_str(),str_class_ptr_type.c_str());
		}

		function_class_name = c_class_first_arg->symbol_class.name+"::"+function_name;


		// register member function...
		symbol = this->registerNativeMemberFunction(
				error
				,registered_file
				, registered_line
				, function_name
				, arg_info
				, idx_return_type
				, (zs_int)ptr_function
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_MEMBER_FUNCTION
		);


		if(symbol == NULL){
			THROW_RUNTIME_ERROR(error.c_str());
		}

		ZS_PRINT_DEBUG("Registered C function %s as function member %s::%s",function_name, function_class_name.c_str(),function_name);
	}

}
