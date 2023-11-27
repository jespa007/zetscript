/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


	/**
	 * Register static member property metamethod. If property is not registered it's registered automatically
	 */
	template <typename F>
	void Type::registerConstMemberProperty(
			const String & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
		)
	{
		String error;
		FunctionParam *params=NULL;
		int params_len=0;
		//const char *return_type;
		Symbol *symbol_member_property=NULL;


		TypeId return_type_id=getNativeFunctionRetArgsTypes(
			this->type_factory
			,NULL
			,_ptr_function
			,&params
			,&params_len
		);


		//---
		// Property should be not registered
		symbol_member_property=registerMemberProperty(_property_name,_registered_file,_registered_line);
		symbol_member_property->properties|=SYMBOL_PROPERTY_STATIC;

		// register member function...
		this->registerMemberPropertyMetamethod(
				_property_name
				,METAMETHOD_SYMBOL_NAME_GETTER
				,&params
				,params_len
				,FUNCTION_PROPERTY_C_OBJECT_REF | FUNCTION_PROPERTY_STATIC
				, return_type_id
				, (zs_int)_ptr_function
				,_registered_file
				,_registered_line

		);

		ZS_LOG_DEBUG("Registered const 'getter' property '%s::%s'",this->name.toConstChar(), _property_name.toConstChar());
	}

	/**
	 * Register member property metamethod. If property is not registered it's registered automatically
	 */
	template <typename F>
	void Type::registerMemberPropertyMetamethod(
			const String & _property_name
			,const String & _metamethod_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
		)
	{
		String error;
		FunctionParam *params=NULL;
		int params_len=0;
		const char *return_type;

		TypeId return_type_id=getNativeFunctionRetArgsTypes(
				this->type_factory
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
				, return_type_id
				, (zs_int)_ptr_function
				,_registered_file
				,_registered_line

		);

		ZS_LOG_DEBUG("Registered metamethod '%s' for property '%s::%s'",_metamethod_name.toConstChar(),this->name.toConstChar(), _property_name.toConstChar());
	}


	/**
	 * Register C Member function Class
	 * like register function c but is added to member function list according type C
	 */
	template <typename F>
	void Type::registerStaticMemberFunction(
			const String & _function_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
		)
	{
		String error;
		FunctionParam *params=NULL;
		int params_len=0;
		const char *return_type;

		TypeId return_type_id=getNativeFunctionRetArgsTypes(
				this->type_factory
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
				, return_type_id
				, (zs_int)_ptr_function
				,_registered_file
				,_registered_line

		);

		ZS_LOG_DEBUG("Registered member function name %s::%s",this->name.toConstChar(), _function_name.toConstChar());
	}

	/*
	 * register static function registerMemberFunction as function member
	 * Is automatically added in function member list according first parameter type of function_type
	 */
	template <typename F>
	void Type::registerMemberFunction(
			const String & _function_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;

		String error;

		// 1. check all parameters ok.
		TypeId return_type_id=getNativeFunctionRetArgsTypes(
				this->type_factory
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
				, return_type_id
				, (zs_int)_ptr_function
				,_registered_file
				, _registered_line
		);


		ZS_LOG_DEBUG("Registered C function '%s' as function member '%s::%s'"
				,_function_name.toConstChar()
				,this->name.toConstChar()
				,_function_name.toConstChar()
		);
	}

}
