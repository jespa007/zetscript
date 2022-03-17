/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	/**
	 * Register C variable
	 */
	template<typename V>
	 void  ScriptTypeFactory::registerNativeGlobalVariable(
		 const zs_string & var_name
		 ,V var_ptr
		 ,const char *registered_file
		 ,short registered_line)
	{
		//Scope *scope;
		const char *var_type = typeid(V).name(); // we need the pointer type ...
		Symbol *irs;
		//int idxVariable;

		if(var_ptr==NULL){
			THROW_RUNTIME_ERROR("cannot register var '%s' with NULL reference value", var_name.c_str());
		}

		ScriptFunction *main_function=MAIN_FUNCTION(this);

		if(main_function == NULL){
			THROW_RUNTIME_ERRORF("main function is not created");
		}

		if(getIdxScriptTypeFromTypeNamePtr(var_type) == ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("%s has not valid type (%s)",var_name.c_str(),var_type);
		}

		if((irs = main_function->registerLocalVariable(
				MAIN_SCOPE(this)
				,registered_file
				,registered_line
				,var_name
				,var_type
				,(zs_int)var_ptr
				,SYMBOL_PROPERTY_C_OBJECT_REF)) != NULL
		){
			ZS_LOG_DEBUG("Registered variable name: %s",var_name.c_str());
		}
	}

	/**
	 * Register C function
	 */
	template <typename F>
	void ScriptTypeFactory::registerNativeGlobalFunction(
		const zs_string &  _function_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line)
	{
		zs_string error;
		ScriptFunctionParam *params=NULL;
		int params_len=0;

		if(main_function == NULL){
			THROW_RUNTIME_ERRORF("main function is not created");
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
				this
				,NULL
				,_function_name
				,_ptr_function
				,&params
				,&params_len
		);

		// Init struct...
		main_function->registerLocalFunction(
				 MAIN_SCOPE(this)
				,_registered_file
				,_registered_line
				,_function_name
				,&params
				,params_len
				,idx_script_type_return
				,(zs_int)_ptr_function
				,FUNCTION_PROPERTY_C_OBJECT_REF
			);

		ZS_LOG_DEBUG("Registered function name: %s",name_script_function);
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<class T>
	ScriptType * ScriptTypeFactory::registerNativeType(
		const zs_string & script_type_name
		,T * (*_constructor)(ZetScript *_zs)
		,void (*_destructor)(ZetScript *_zs,T *)
		,const char *registered_file
		,short registered_line
	){//, const zs_string & base_class_name=""){

		ScriptType *sc=NULL;
		const char * script_type_name_ptr = typeid( T *).name();
		//int size=script_types->count;
		int idx_script_type=ZS_IDX_UNDEFINED;
		Scope * scope = NULL;


		checkClassName(script_type_name);

		if(getIdxScriptTypeFromTypeNamePtr(script_type_name_ptr)!=ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR(
			"Native class '%s' is already registered"
			,zs_rtti::demangle(typeid( T).name()).c_str()
			);
		}

		idx_script_type=script_types->count;
		scope = NEW_SCOPE(this,ZS_IDX_UNDEFINED,NULL,SCOPE_PROPERTY_IS_SCOPE_CLASS|SCOPE_PROPERTY_IS_C_OBJECT_REF);
		MAIN_SCOPE(this)->registerSymbolScriptType(registered_file,registered_line,script_type_name);

		sc = new ScriptType(zs,idx_script_type,script_type_name,scope,script_type_name_ptr,SCRIPT_TYPE_PROPERTY_C_OBJECT_REF);
		scope->setScriptTypeOwner(sc);

		// in C there's no script constructor ...
		sc->idx_function_member_constructor=ZS_IDX_UNDEFINED;
		// allow dynamic constructor in function its parameters ...
		sc->c_constructor = NULL;
		sc->c_destructor = NULL;
		script_types->push_back((zs_int)sc);


		if((_constructor != NULL) && (_destructor != NULL)){ // it can instanced, not static
			sc->c_constructor = (void *)_constructor;
			sc->c_destructor = (void *)_destructor;

		}else{ // Cannot be instanced so is static
			sc->properties|=SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE;
		}

		sc->idx_script_type=script_types->count-1;
		ZS_LOG_DEBUG("* C++ class '%s' registered as (%s).",script_type_name.c_str(),zs_rtti::demangle(script_type_name_ptr).c_str());

		return sc;
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<class T>
	ScriptType * ScriptTypeFactory::registerNativeTypeStatic(
		const zs_string & script_type_name
		,const char *registered_file
		,short registered_line
	){
		return registerNativeType<T>(
				script_type_name
				,NULL
				,NULL
				,registered_file
				,registered_line
		);
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<class T>
	ScriptType * ScriptTypeFactory::registerNativeStaticClass(
		const zs_string & script_type_name
		,const char *registered_file
		,short registered_line
	){
		return registerNativeType<T>(
				script_type_name
				,NULL
				,NULL
				,registered_file
				,registered_line
		);
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<typename C>
	ScriptType * ScriptTypeFactory::registerNativeClass(
		const zs_string & script_type_name
		,C * (*_constructor)(ZetScript *_zs)
		,void (*_destructor)(ZetScript *_zs,C *)
		,const char *registered_file
		,short registered_line
	){

		return registerNativeType<C>(
				script_type_name
				,_constructor
				,_destructor
				,registered_file
				,registered_line
		);
	}

	/**
	 * Register C Class. Return index registered class
	 */
	/*template<typename C>
	ScriptType * ScriptTypeFactory::registerNativeType(
		const zs_string & _type_name
		,const char *_registered_file
		,short _registered_line
	){

		return ScriptTypeFactory::registerNativeClass<C>(_type_name,NULL,NULL,_registered_file,_registered_line);
	}*/

	template<class C,class B>
	void ScriptTypeFactory::nativeClassInheritsFrom(){

		const char *base_class_name=typeid(B).name();
		const char * base_class_name_ptr=typeid(B *).name();
		const char * script_type_name=typeid(C).name();
		const char * class_name_ptr=typeid(C *).name();
		zs_string error;

		int idx_base_type = getIdxScriptTypeFromTypeNamePtr(base_class_name_ptr);
		if(idx_base_type == -1) {
			THROW_RUNTIME_ERROR("base class %s not registered",base_class_name_ptr);
		}


		int idx_register_class = getIdxScriptTypeFromTypeNamePtr(class_name_ptr);
		if(idx_register_class == ZS_IDX_UNDEFINED) {
			THROW_RUNTIME_ERROR("class %s not registered",class_name_ptr);
		}

		if(isScriptClassTypeInheritsFrom(idx_register_class,idx_base_type)){
			THROW_RUNTIME_ERROR("C++ class '%s' is already registered as base of '%s' ",zs_rtti::demangle(script_type_name).c_str(), zs_rtti::demangle(base_class_name).c_str());
		}

		// check whether is in fact base of ...
		if(!std::is_base_of<B,C>::value){
			THROW_RUNTIME_ERROR("C++ class '%s' is not base of '%s' ",zs_rtti::demangle(script_type_name).c_str(), zs_rtti::demangle(base_class_name).c_str());
		}

		// now only allows one inheritance!
		ScriptType *sc=(ScriptType *)script_types->get(idx_register_class);

		for(int i=0; i < sc->idx_base_types->count; i++){
			sc=getScriptType(sc->idx_base_types->items[i]); // get base class...
			if(sc->script_type_name_ptr ==base_class_name_ptr){
				THROW_RUNTIME_ERROR("C++ class '%s' already base of '%s' ",zs_rtti::demangle(script_type_name).c_str(), zs_rtti::demangle(base_class_name).c_str());
			}
		}

		ScriptType *this_class = (ScriptType *)script_types->get(idx_register_class);
		this_class->idx_base_types->push_back(idx_base_type);

		//----------------------------
		//
		// DERIVATE STATE
		//

		ScriptType *base_class = (ScriptType *)script_types->get(idx_base_type);
		zs_vector *base_vars=base_class->script_type_scope->symbol_variables;
		zs_vector *base_functions=base_class->script_type_scope->symbol_functions;

		// register all c vars symbols ...
		for(int i = 0; i < base_functions->count; i++){

			Symbol *symbol_src = (Symbol *)base_functions->items[i];

			bool is_metamethod_function = MemberProperty::symbolNameMatchStartSymbolNameMetamethod(symbol_src->name);
/*										    zs_strutils::starts_with(symbol_src->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_GETTER)
										||  zs_strutils::starts_with(symbol_src->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_POST_INC)
										||  zs_strutils::starts_with(symbol_src->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_POST_DEC)
										||  zs_strutils::starts_with(symbol_src->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_PRE_INC)
										||  zs_strutils::starts_with(symbol_src->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_PRE_DEC)
										;

			if(is_metamethod_function==false){ // try find setter
				ByteCodeMetamethod *it=MemberProperty::byte_code_metamethod_member_setter_list;
				zs_string symbol_name_methametod;
				while(*it!=0 && is_metamethod_function==false){
					symbol_name_methametod=byte_code_metamethod_to_symbol_str(*it);
					symbol_name_methametod.append('@');
					is_metamethod_function=zs_strutils::starts_with(symbol_src->name,symbol_name_methametod);
					it++;
				}
			}*/

			// we have to know whether function member is or not getter/setter because we create them in the property member case. If not, we could have
			// duplicated symbols.
			if(is_metamethod_function == false){
				ScriptFunction *script_function =NULL;
				/*if(symbol_src->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){ // property
					MemberProperty *mp=(MemberProperty *)symbol_src->ref_ptr;

					this_class->registerMemberFunction(
						script_function->name_script_function,
						&params,
						params_len,
						script_function->properties, //derivated_symbol_info_properties
						script_function->idx_script_type_return,
						script_function->ref_native_function_ptr, // it contains script function pointer
						symbol_src->file,
						symbol_src->line
					);

				}else */
				if(symbol_src->properties & SYMBOL_PROPERTY_FUNCTION){ // function
					script_function=(ScriptFunction *)symbol_src->ref_ptr;

					ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(script_function);
					char params_len=script_function->params_len;


					this_class->registerMemberFunction(
						script_function->name_script_function,
						&params,
						params_len,
						script_function->properties, //derivated_symbol_info_properties
						script_function->idx_script_type_return,
						script_function->ref_native_function_ptr, // it contains script function pointer
						symbol_src->file,
						symbol_src->line
					);
				}else{
					THROW_RUNTIME_ERROR("Error adding functions from base elements '%s': '%s::%s' is not a function"
							,zs_rtti::demangle(base_class_name).c_str()
							,zs_rtti::demangle(base_class_name).c_str()
							, symbol_src->name.c_str());
				}


				// build params...



			}
		}


		for(int i = 0; i < base_vars->count; i++){
			Symbol *symbol_src = (Symbol *)base_vars->items[i];

			if(symbol_src->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){

				MemberProperty *mp_src=(MemberProperty *)symbol_src->ref_ptr;
				MemberProperty *mp_dst=NULL;

				Symbol *symbol_member_property=NULL;
				Symbol *symbol_function=NULL;

				symbol_member_property=this_class->registerMemberProperty(symbol_src->name,symbol_src->file,symbol_src->line);
				mp_dst=(MemberProperty *)symbol_member_property->ref_ptr;


				struct _PropertyMethodIt{
					Symbol **dst_symbol_function;
					Symbol *src_symbol_function;
				}property_methods[]={
					{&mp_dst->metamethod_members.getter,mp_src->metamethod_members.getter}
					,{&mp_dst->metamethod_members.post_inc,mp_src->metamethod_members.post_inc}
					,{&mp_dst->metamethod_members.post_dec,mp_src->metamethod_members.post_dec}
					,{&mp_dst->metamethod_members.pre_inc,mp_src->metamethod_members.pre_inc}
					,{&mp_dst->metamethod_members.pre_dec,mp_src->metamethod_members.pre_dec}
					,{&mp_dst->metamethod_members.neg,mp_src->metamethod_members.neg}
					,{0,0}
				};

				_PropertyMethodIt *it=property_methods;

				// register getter and setter
				while(it->dst_symbol_function!=0){

					*it->dst_symbol_function= NULL; // init to null

					if(it->src_symbol_function!=0){ // we have src method
						ScriptFunction *src_function=(ScriptFunction *)it->src_symbol_function->ref_ptr;
						ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(src_function);
						char params_len=src_function->params_len;

						symbol_function=this_class->registerMemberFunction(
								src_function->name_script_function,
								&params,
								params_len,
								src_function->properties,
								src_function->idx_script_type_return,
								src_function->ref_native_function_ptr
								//it->src_function->symbol.file,
								//it->src_function->symbol.line
						);

						*it->dst_symbol_function=symbol_function;
					}
					it++;
				}

				const ByteCodeMetamethod *it_setter=MetamethodMembers::byte_code_metamethod_member_setter_list;
				while(*it_setter!= 0){
					MetamethodMemberSetterInfo mp_info=mp_src->metamethod_members.getSetterInfo(*it_setter);
					if(mp_info.setters!=NULL){
						for(int i=0; i < mp_info.setters->count; i++){

							StackElement *stk_setter=(StackElement *)mp_info.setters->items[i];
							Symbol *symbol_setter=(Symbol *)stk_setter->value;
							ScriptFunction *sf_setter=(ScriptFunction *)symbol_setter->ref_ptr;

							ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(sf_setter);
							char params_len=sf_setter->params_len;

							symbol_function=this_class->registerMemberFunction(
									sf_setter->name_script_function,
									&params,
									params_len,
									sf_setter->properties,
									sf_setter->idx_script_type_return,
									sf_setter->ref_native_function_ptr

									//sf_setter->symbol->file,
									//sf_setter->symbol->line
							);

							mp_dst->metamethod_members.addSetter(*it_setter,symbol_function);
						}
					}

					it_setter++;
				}
			}
		}

		//
		// DERIVATE STATE
		//
		//----------------------------
	}

	/**
	 * Register C Member constructor
	 */
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeConstructor(
		F function_type
		,const char *registered_file
		,short registered_line
	){

		zs_string script_type_name_ptr = typeid( C *).name();
		ScriptType * script_type=	getScriptTypeFromTypeNamePtr(script_type_name_ptr);
		if(script_type == NULL){
			THROW_RUNTIME_ERROR("class '%s' is not registered",script_type_name_ptr);
		}
		return registerNativeMemberFunction<C>(CONSTRUCTOR_FUNCTION_NAME,function_type, registered_file,registered_line );
	}

	/**
	 * Register C Member var
	 */
	template <typename C, typename R>
	void ScriptTypeFactory::registerNativeStaticConstMember(
			const zs_string & var_name
			,const R var_pointer
			,const char *registered_file
			,short registered_line) //unsigned int offset)
	{
		// to make compatible MSVC shared library
		const char *var_type = typeid(R).name(); // we need the pointer type ...
		const char *return_type;
		zs_string error;
		const char *script_type_name_ptr = typeid( C *).name();
		Symbol *symbol;

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		// 1. check all parameters ok.
		// check valid parameters ...
		if(getIdxScriptTypeFromTypeNamePtr(var_type) == ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("%s::%s has not valid type (%s)"
					,script_type->script_type_name.c_str()
					,var_name
					,zs_rtti::demangle(typeid(R).name()).c_str());
		}

		// register variable...
		symbol=script_type->registerNativeMemberVariable(
				var_name
				,var_type
				,(zs_int)var_pointer
				,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC | SYMBOL_PROPERTY_CONST
				,registered_file
				,registered_line
		);
	}

	/*
	 * register property member setter
	 */
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertySetter(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		zs_string script_type_name_ptr = typeid( C *).name();
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
				this
				,script_type
				,_property_name
				,_ptr_function
				,&params
				,&params_len
				);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_SET
			 ,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}



	/*
	 * register property getter
	 */
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyGetter(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();
		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr.c_str());
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethodGetter(
				 _property_name
				,&params
				,params_len
				,idx_script_type_return
				,(zs_int)_ptr_function
				,_registered_file
				,_registered_line
		);
	}

	/*
	 * register member property  post_increment
	 */
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyPostIncrement(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_POST_INC
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}

	/*
	 * register member property  post_decrement
	 */
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyPostDecrement(
			const zs_string & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_POST_DEC
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}

	/*
	 * register member property  pre_increment
	 */
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyPreIncrement(
			const zs_string & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_PRE_INC
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}

	/*
	 * register member property  pre_decrement
	 */
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyPreDecrement(
			const zs_string & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_PRE_DEC
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}

	// register member property add set operation
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyAddSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_ADD_SET
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}

	// register member property  sub set operation
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertySubSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			, script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_SUB_SET
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}

	// register member property mul set operation
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyMulSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_MUL_SET
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property div set operation
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyDivSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_DIV_SET
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property mod set operation
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyModSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_MOD_SET
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property and set operation
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyAndSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_AND_SET
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property or set operation
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyOrSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_OR_SET
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property xor set operation
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyXorSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_XOR_SET
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property shl set operation
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyShlSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_SHL_SET
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property shr set operation
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberPropertyShrSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_SHR_SET
			,&params
			,params_len
			,idx_script_type_return
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}


	/**
	 * Register C Member function Class
	 * like register function c but is added to member function list according type C
	 */
	template <typename C, typename F>
	void ScriptTypeFactory::registerNativeMemberFunctionStatic(
		const zs_string & name_script_function
		,F ptr_function
		,const char *registered_file
		,short registered_line
	)
	{
		// to make compatible MSVC shared library
		zs_string script_type_name_ptr = typeid( C *).name();


		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr.c_str());
		}

		return script_type->registerNativeMemberFunctionStatic<F>(name_script_function
				,ptr_function
				,registered_file
				,registered_line);

	}

	/*
	 * register static function registerNativeMemberFunction as function member
	 * Is automatically added in function member list according first parameter type of function_type
	 */
	template <typename C,typename F>
	void ScriptTypeFactory::registerNativeMemberFunction(
		const zs_string & name_script_function
		,F ptr_function
		,const char *registered_file
		,short registered_line
	){
		zs_string script_type_name_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(script_type_name_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",script_type_name_ptr.c_str());
		}

		script_type->registerNativeMemberFunction<F>(
			name_script_function
			,ptr_function
			,registered_file
			,registered_line
		);

	}
}
