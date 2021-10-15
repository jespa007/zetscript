/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	template < typename R>
	void ScriptClass::registerNativeMemberVariableStaticConst(
			const zs_string & var_name
			, const R var_pointer
			, const char *registered_file
			,short registered_line)
	{
		// to make compatible MSVC shared library
		zs_string var_type = typeid(R).name();
		zs_string error;

			// register variable...
		Symbol *symbol=this->registerNativeMemberVariable(
				var_name
				,var_type
				,(zs_int)var_pointer
				,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC | SYMBOL_PROPERTY_CONST
				,registered_file
				,registered_line
		);
	}

	/*
	 * register C setter
	 */
	template <typename F>
	void ScriptClass::registerNativeMemberAttributeSetter(
			const zs_string & attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string function_class_name;
		zs_string error;

		// 1. check all parameters ok.
		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
				this->script_class_factory
				,attrib_name
				,ptr_function
				,&params
				,&params_len
		);

		if(idx_return_type != IDX_TYPE_VOID_C){
			THROW_RUNTIME_ERROR("Error register setter attrib '%s': should return void ",attrib_name);
		}

		registerNativeMemberAttributeSetter(
				attrib_name
				,&params
				,params_len
				,(zs_int)ptr_function
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_MEMBER_FUNCTION
				,registered_file
				,registered_line
		);
	}

	/*
	 * register C getter
	 */
	template <typename F>
	void ScriptClass::registerNativeMemberAttributeGetter(
			const zs_string & _attrib_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string function_class_name;
		zs_string error;

		// 1. check all parameters ok.
		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
				this->script_class_factory
				,_attrib_name
				,_ptr_function
				,&params
				,&params_len
				);

		registerNativeMemberAttributeGetter(
				 _attrib_name
				 ,&params
				 ,params_len
				,idx_return_type
				,(zs_int)_ptr_function
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_MEMBER_FUNCTION
				,_registered_file
				,_registered_line
		);
	}

	/*
	 * register C post increment
	 */
	template <typename F>
	void ScriptClass::registerNativeMemberAttributePostIncrement(
			const zs_string & _attrib_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string function_class_name;
		zs_string error;

		// 1. check all parameters ok.
		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
				_attrib_name
				,_ptr_function
				,&params
				,&params_len
		);

		Symbol *symbol_result = registerNativeMemberAttributePostIncrement(
				 _attrib_name
				 ,&params
				 ,params_len
				,idx_return_type
				,(zs_int)_ptr_function
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_MEMBER_FUNCTION
				,_registered_file
				,_registered_line
		);
	}

	/*
	 * register C post decrement
	 */
	template <typename F>
	void ScriptClass::registerNativeMemberAttributePostDecrement(
			const zs_string & _attrib_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string function_class_name;
		zs_string error;

		// 1. check all parameters ok.
		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
				_attrib_name
				,_ptr_function
				,&params
				,&params_len
		);

		Symbol *symbol_result = registerNativeMemberAttributePostDecrement(
				 _attrib_name
				 ,&params
				 ,params_len
				,idx_return_type
				,(zs_int)_ptr_function
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_MEMBER_FUNCTION
				,_registered_file
				,_registered_line
		);
	}

	/*
	 * register C pre increment
	 */
	template <typename F>
	void ScriptClass::registerNativeMemberAttributePreIncrement(
			const zs_string & _attrib_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string function_class_name;
		zs_string error;

		// 1. check all parameters ok.
		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
				_attrib_name
				,_ptr_function
				,&params
				,&params_len
		);

		Symbol *symbol_result = registerNativeMemberAttributePreIncrement(
				 _attrib_name
				 ,&params
				 ,params_len
				,idx_return_type
				,(zs_int)_ptr_function
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_MEMBER_FUNCTION
				,_registered_file
				,_registered_line
		);
	}

	/*
	 * register C pre decrement
	 */
	template <typename F>
	void ScriptClass::registerNativeMemberAttributePreDecrement(
			const zs_string & _attrib_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string function_class_name;
		zs_string error;

		// 1. check all parameters ok.
		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
				_attrib_name
				,_ptr_function
				,&params
				,&params_len
		);

		Symbol *symbol_result = registerNativeMemberAttributePreDecrement(
				 _attrib_name
				 ,&params
				 ,params_len
				,idx_return_type
				,(zs_int)_ptr_function
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_MEMBER_FUNCTION
				,_registered_file
				,_registered_line
		);
	}

	/**
	 * Register C Member function Class
	 * like register function c but is added to member function list according type C
	 */
	template <typename F>
	void ScriptClass::registerNativeMemberFunctionStatic(
			const zs_string & function_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		)
	{
		zs_string error;
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		const char *return_type;

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
				this->script_class_factory
				,function_name
				,ptr_function
				,&params
				,&params_len
				,&return_type
		);

		// register member function...
		Symbol * symbol_sf = this->registerNativeMemberFunction(
				 function_name
				,&params
				,params_len
				, idx_return_type
				, (zs_int)ptr_function
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC
				,registered_file
				,registered_line

		);

		ZS_LOG_DEBUG("Registered member function name %s::%s",this->name.c_str(), function_name.c_str());

		// check whether is static metamethod...
		if(ZS_STRCMP(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_SET),!=,function_name.c_str())){

			for(int i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				if(ZS_STRCMP(byte_code_metamethod_to_symbol_str((ByteCodeMetamethod)i),==,function_name.c_str())){

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
						if(ZS_STRCMP(return_type, != ,typeid(bool).name())){
							THROW_RUNTIME_ERROR("error registering metamethod %s::%s. Expected return bool but it was %s",
									this->class_name.c_str(),
									function_name,
									zs_rtti::demangle(return_type));
						}
					}else if((ZS_STRCMP(return_type, != ,this->str_class_ptr_type)) && (i!= BYTE_CODE_METAMETHOD_SET)){

						THROW_RUNTIME_ERROR("error registering metamethod %s::%s. Expected return %s but it was %s",
								this->class_name.c_str(),
								function_name,
								zs_rtti::demangle(return_type));
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
			const zs_string & _function_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;

		zs_string error;
		
		zs_string function_class_name = this->class_name+"::"+_function_name;

		// 1. check all parameters ok.
		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
				this->script_class_factory
				,_function_name
				,_ptr_function
				,&params
				,&params_len
		);

		if(params_len==0){
			THROW_RUNTIME_ERROR("register native member function '%s::%s': needs to have FIRST parameter as pointer type '%s'"
				,function_class_name.c_str()
				,this->str_class_ptr_type
				,this->str_class_ptr_type
			);
		}

		ScriptClass * c_class_first_arg=	getScriptClass(params[0].idx_type);
		if(c_class_first_arg == NULL){
			THROW_RUNTIME_ERROR("register native member function '%s::%s': needs to have FIRST parameter as pointer type '%s')"
				,function_class_name.c_str()
				,this->str_class_ptr_type
				,this->str_class_ptr_type
			);
		}

		if(c_class_first_arg->str_class_ptr_type !=  this->str_class_ptr_type){
			THROW_RUNTIME_ERROR("register native member function '%s::%s': expected to have FIRST parameter as pointer type '%s' but it was '%s')"
				,function_class_name.c_str()
				,this->str_class_ptr_type
				,this->str_class_ptr_type
				,params[0].name
			);
		}


		// register member function...
		this->registerNativeMemberFunction(
				 _function_name
				, &params
				, params_len
				, idx_return_type
				, (zs_int)_ptr_function
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_MEMBER_FUNCTION
				,_registered_file
				, _registered_line
		);


		ZS_LOG_DEBUG("Registered C function '%s' as function member '%s'",function_name, function_class_name.c_str());
	}

}
