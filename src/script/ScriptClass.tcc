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
		this->registerMemberFunction(
				 function_name
				,&params
				,params_len
				, FUNCTION_PROPERTY_C_OBJECT_REF | FUNCTION_PROPERTY_STATIC
				, idx_return_type
				, (zs_int)ptr_function
				,registered_file
				,registered_line

		);

		ZS_LOG_DEBUG("Registered member function name %s::%s",this->name.c_str(), function_name.c_str());
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
		this->registerMemberFunction(
				 _function_name
				, &params
				, params_len
				, FUNCTION_PROPERTY_C_OBJECT_REF | FUNCTION_PROPERTY_MEMBER_FUNCTION
				, idx_return_type
				, (zs_int)_ptr_function
				,_registered_file
				, _registered_line
		);


		ZS_LOG_DEBUG("Registered C function '%s' as function member '%s'",_function_name, function_class_name.c_str());
	}

}
