/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	template < typename R>
	void ScriptType::bindMemberVariableStaticConst(
			const zs_string & var_name
			, const R var_pointer
			, const char *registered_file
			,short registered_line)
	{
		// to make compatible MSVC shared library
		zs_string var_type = typeid(R).name();
		zs_string error;

			// register variable...
		this->registerMemberVariable(
				var_name
				,var_type
				,(zs_int)var_pointer
				,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC | SYMBOL_PROPERTY_CONST
				,registered_file
				,registered_line
		);
	}

	/**
	 * Register C Member function Class
	 * like register function c but is added to member function list according type C
	 */
	template <typename F>
	void ScriptType::bindMemberFunctionStatic(
			const zs_string & _function_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
		)
	{
		zs_string error;
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		const char *return_type;

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
				this->script_type_factory
				,this
				,_function_name
				,_ptr_function
				,&params
				,&params_len
				,&return_type
		);

		// register member function...
		this->registerMemberFunction(
				 _function_name
				,&params
				,params_len
				, FUNCTION_PROPERTY_C_OBJECT_REF | FUNCTION_PROPERTY_STATIC
				, idx_script_type_return
				, (zs_int)_ptr_function
				,_registered_file
				,_registered_line

		);

		ZS_LOG_DEBUG("Registered member function name %s::%s",this->name.c_str(), name_script_function.c_str());
	}

	/*
	 * register static function bindMemberFunction as function member
	 * Is automatically added in function member list according first parameter type of function_type
	 */
	template <typename F>
	void ScriptType::bindMemberFunction(
			const zs_string & _function_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;

		zs_string error;

		// 1. check all parameters ok.
		int idx_script_type_return=getNativeFunctionRetArgsTypes(
				this->script_type_factory
				,this
				,_function_name
				,_ptr_function
				,&params
				,&params_len
		);

		ScriptType * c_class_first_arg=	params_len>1?getScriptType(params[1].idx_script_type):NULL;
		if(c_class_first_arg == NULL){
			THROW_RUNTIME_ERROR("register native function '%s::%s': needs to have SECOND parameter as pointer type '%s')"
				,str_script_type.c_str()
				,_function_name.c_str()
				,this->str_script_type.c_str()
			);
		}

		if(strcmp(c_class_first_arg->str_script_type_ptr,this->str_script_type_ptr)!=0){
			THROW_RUNTIME_ERROR(
				"register native function '%s::%s': expected to have SECOND parameter as pointer type '%s' but it was '%s')"
				,str_script_type.c_str()
				,_function_name.c_str()
				,this->str_script_type.c_str()
				,params[1].name.c_str()
			);
		}

		// register member function...
		this->registerMemberFunction(
				 _function_name
				, &params
				, params_len
				, FUNCTION_PROPERTY_C_OBJECT_REF | FUNCTION_PROPERTY_MEMBER_FUNCTION
				, idx_script_type_return
				, (zs_int)_ptr_function
				,_registered_file
				, _registered_line
		);


		ZS_LOG_DEBUG("Registered C function '%s' as function member '%s'",_function_name, function_class_name.c_str());
	}

}
