/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


	/**
	 * Register static member property metamethod. If property is not registered it's registered automatically
	 */
	template <typename F>
	void ScriptType::registerConstMemberProperty(
			const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
		)
	{
		zs_string error;
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		//const char *return_type;
		Symbol *symbol_member_property=NULL;


		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this->script_type_factory
			,NULL
			,_ptr_function
			,&params
			,&params_len
		);


		//---
		// Property should be not registered
		symbol_member_property=registerMemberProperty(_property_name,_registered_file,_registered_line);
		symbol_member_property->properties|=ZS_SYMBOL_PROPERTY_STATIC;

		// register member function...
		this->registerMemberPropertyMetamethod(
				_property_name
				,ZS_METAMETHOD_ID_SYMBOL_NAME_GETTER
				,&params
				,params_len
				,FUNCTION_PROPERTY_C_OBJECT_REF | FUNCTION_PROPERTY_STATIC
				, return_script_type_id
				, (zs_int)_ptr_function
				,_registered_file
				,_registered_line

		);

		ZS_LOG_DEBUG("Registered const 'getter' property '%s::%s'",this->name.c_str(), _property_name.c_str());
	}

	/**
	 * Register member property metamethod. If property is not registered it's registered automatically
	 */
	template <typename F>
	void ScriptType::registerMemberPropertyMetamethod(
			const zs_string & _property_name
			,const zs_string & _metamethod_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
		)
	{
		zs_string error;
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		const char *return_type;

		int return_script_type_id=getNativeFunctionRetArgsTypes(
				this->script_type_factory
				,NULL
				,_ptr_function
				,&params
				,&params_len
				,&return_type
		);

     	// register member function...
		this->registerMemberPropertyMetamethod(
				_property_name
				,_metamethod_name
				,&params
				,params_len
				,FUNCTION_PROPERTY_C_OBJECT_REF | FUNCTION_PROPERTY_MEMBER_FUNCTION
				, return_script_type_id
				, (zs_int)_ptr_function
				,_registered_file
				,_registered_line

		);

		ZS_LOG_DEBUG("Registered metamethod '%s' for property '%s::%s'",_metamethod_name.c_str(),this->name.c_str(), _property_name.c_str());
	}


	/**
	 * Register C Member function Class
	 * like register function c but is added to member function list according type C
	 */
	template <typename F>
	void ScriptType::registerStaticMemberFunction(
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

		int return_script_type_id=getNativeFunctionRetArgsTypes(
				this->script_type_factory
				,NULL
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
				, return_script_type_id
				, (zs_int)_ptr_function
				,_registered_file
				,_registered_line

		);

		ZS_LOG_DEBUG("Registered member function name %s::%s",this->name.c_str(), _function_name.c_str());
	}

	/*
	 * register static function registerMemberFunction as function member
	 * Is automatically added in function member list according first parameter type of function_type
	 */
	template <typename F>
	void ScriptType::registerMemberFunction(
			const zs_string & _function_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;

		zs_string error;

		// 1. check all parameters ok.
		int return_script_type_id=getNativeFunctionRetArgsTypes(
				this->script_type_factory
				,this
				,_ptr_function
				,&params
				,&params_len
		);

		// register member function...
		this->registerMemberFunction(
				 _function_name
				, &params
				, params_len
				, FUNCTION_PROPERTY_C_OBJECT_REF | FUNCTION_PROPERTY_MEMBER_FUNCTION
				, return_script_type_id
				, (zs_int)_ptr_function
				,_registered_file
				, _registered_line
		);


		ZS_LOG_DEBUG("Registered C function '%s' as function member '%s::%s'"
				,_function_name.c_str()
				,this->name.c_str()
				,_function_name.c_str()
		);
	}

}
