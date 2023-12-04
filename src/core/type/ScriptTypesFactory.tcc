/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	/**
	 * Register T function
	 */
	template <typename F>
	void ScriptTypesFactory::registerFunction(
		const String &  _function_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line)
	{
		String error;
		FunctionParam *params=NULL;
		int params_len=0;

		if(main_function == NULL){
			ZS_THROW_RUNTIME_ERRORF("main function is not created");
		}

		ScriptTypeId return_script_type_id=getNativeFunctionRetArgsTypes(
				this
				,NULL
				,_ptr_function
				,&params
				,&params_len
		);

		// Init struct...
		main_function->registerLocalFunction(
				 ZS_MAIN_SCOPE(this)
				,_registered_file
				,_registered_line
				,_function_name
				,&params
				,params_len
				,return_script_type_id
				,(zs_int)_ptr_function
				,SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF
			);

		ZS_LOG_DEBUG("Registered function name '%s'",_function_name.toConstChar());
	}

	/**
	 * Register T Class. Return index registered class
	 */
	template<class T>
	ScriptType * ScriptTypesFactory::registerScriptType(
		const String & name
		,T * (*_new_native_instance)(ZetScript *_zs)
		,void (*_delete_native_instance)(ZetScript *_zs,T *)
		,const char *registered_file
		,short registered_line
	){//, const String & base_class_name=""){

		ScriptType *type=NULL;
		const char * native_name = typeid( T *).name();
		//int size=types->length();
		ScriptTypeId script_type_id=SCRIPT_TYPE_ID_INVALID;
		ScriptScope * scope = NULL;


		checkTypeName(name);

		if(getScriptTypeIdFromNamePtr(native_name)!=ZS_UNDEFINED_IDX){
			ZS_THROW_RUNTIME_ERROR(
			"Native type '%s' is already registered"
			,Rtti::demangle(typeid( T).name()).toConstChar()
			);
		}

		script_type_id=(ScriptTypeId)types->length();
		scope = ZS_NEW_SCOPE(this,ZS_UNDEFINED_IDX,NULL,SCOPE_PROPERTY_IS_SCOPE_CLASS|SCOPE_PROPERTY_IS_C_OBJECT_REF);
		ZS_MAIN_SCOPE(this)->registerSymbolType(registered_file,registered_line,name);

		type = new ScriptType(zs,script_type_id,name,scope,native_name,SCRIPT_TYPE_PROPERTY_NATIVE_OBJECT_REF);
		scope->setTypeOwner(type);

		// in T there's no script constructor ...
		type->idx_constructor_member_function=ZS_UNDEFINED_IDX;
		// allow dynamic constructor in function its parameters ...
		type->new_native_instance = NULL;
		type->delete_native_instance = NULL;
		types->push(type);


		if((_new_native_instance != NULL) && (_delete_native_instance != NULL)){ // it can instanced, not static
			type->new_native_instance = (void *)_new_native_instance;
			type->delete_native_instance = (void *)_delete_native_instance;

		}else{ // Cannot be instanced so is static
			type->properties|=SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE;
		}

		type->id=types->length()-1;
		ZS_LOG_DEBUG("* native type '%s' registered as (%s).",name.toConstChar(),Rtti::demangle(native_name).toConstChar());

		return type;
	}


	template<class T,class B>
	void ScriptTypesFactory::extends(){

		const char *base_class_name=typeid(B).name();
		const char * base_class_name_ptr=typeid(B *).name();
		const char * name=typeid(T).name();
		const char * class_name_ptr=typeid(T *).name();
		String error;

		ScriptTypeId base_script_type_id = getScriptTypeIdFromNamePtr(base_class_name_ptr);
		if(base_script_type_id == SCRIPT_TYPE_ID_INVALID) {
			ZS_THROW_RUNTIME_ERROR("base native type '%s' not registered",base_class_name_ptr);
		}


		ScriptTypeId register_script_type_id = getScriptTypeIdFromNamePtr(class_name_ptr);
		if(register_script_type_id == SCRIPT_TYPE_ID_INVALID) {
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",class_name_ptr);
		}

		if(scriptTypeInheritsFrom(register_script_type_id,base_script_type_id)){
			ZS_THROW_RUNTIME_ERROR("native type '%s' is already registered as base of '%s' ",Rtti::demangle(name).toConstChar(), Rtti::demangle(base_class_name).toConstChar());
		}

		// check whether is in fact base of ...
		if(!std::is_base_of<B,T>::value){
			ZS_THROW_RUNTIME_ERROR("native type '%s' is not base of '%s' ",Rtti::demangle(name).toConstChar(), Rtti::demangle(base_class_name).toConstChar());
		}

		// now only allows one inheritance!
		ScriptType *main_class=types->get(register_script_type_id);


		for(int i=0; i < main_class->base_script_type_ids->length(); i++){
			ScriptType *type=getScriptType(main_class->base_script_type_ids->get(i)); // get base type...
			if(type->native_name ==base_class_name_ptr){
				ZS_THROW_RUNTIME_ERROR("native type '%s' already extends from '%s' "
						,Rtti::demangle(name).toConstChar()
						, Rtti::demangle(base_class_name).toConstChar());
			}
		}


		ScriptType *base=(ScriptType *)types->get(base_script_type_id);

		// search native types that already inherits type B
		for(int i=0; i < main_class->base_script_type_ids->length(); i++){
			ScriptType *type=getScriptType(main_class->base_script_type_ids->get(i)); // get base type...
			// check whether type inherits inheritates B
			if(type->extendsFrom(base_script_type_id)){
				ZS_THROW_RUNTIME_ERROR("ScriptType '%s' cannot extend from '%s' because '%s' inherits '%s' that already is inherited by '%s'"
					,Rtti::demangle(name).toConstChar()
					, Rtti::demangle(base_class_name).toConstChar()
					,Rtti::demangle(name).toConstChar()
					,Rtti::demangle(type->name.toConstChar()).toConstChar()
					, Rtti::demangle(base_class_name).toConstChar()
				);
			}
			// check the viceversa, if B inheritates inherited types of main_class
			if(base->extendsFrom(type->id)){
				ZS_THROW_RUNTIME_ERROR("ScriptType '%s' cannot extend from '%s' because '%s' has inherited type '%s' that also is inherited by '%s'"
					,Rtti::demangle(name).toConstChar()
					, Rtti::demangle(base_class_name).toConstChar()
					,Rtti::demangle(name).toConstChar()
					, Rtti::demangle(type->name.toConstChar()).toConstChar()
					, Rtti::demangle(base_class_name).toConstChar()
				);
			}
		}

		ScriptType *this_class = (ScriptType *)types->get(register_script_type_id);
		this_class->base_script_type_ids->push(base_script_type_id);

		//----------------------------
		//
		// DERIVATE STATE
		//

		ScriptType *base_class = types->get(base_script_type_id);
		Vector<Symbol *> *base_vars=base_class->scope->symbol_variables;
		Vector<Symbol *> *base_functions=base_class->scope->symbol_functions;

		// register all c vars symbols ...
		for(int i = 0; i < base_functions->length(); i++){

			Symbol *src_symbol = (Symbol *)base_functions->get(i);

			bool is_metamethod_function = MemberProperty::symbolNameMatchStartSymbolNameMetamethod(src_symbol->name);

			// we have to know whether function member is or not getter/setter because we create them in the property member case. If not, we could have
			// duplicated symbols.
			if(is_metamethod_function == false){
				ScriptFunction *src_script_function =NULL;
				if(src_symbol->properties & SYMBOL_PROPERTY_FUNCTION){ // function
					src_script_function=(ScriptFunction *)src_symbol->ref_ptr;
					Symbol *dst_symbol=NULL;
					ScriptFunction *dst_script_function=NULL;

					FunctionParam *params=FunctionParam::createArrayFromFunction(src_script_function);
					int8_t params_len=src_script_function->params_len;


					dst_symbol=this_class->registerMemberFunction(
						src_script_function->name,
						&params,
						params_len,
						src_script_function->properties, //derivated_symbol_info_properties
						src_script_function->return_script_type_id,
						src_script_function->ref_native_function_ptr, // it contains script function pointer
						src_symbol->file,
						src_symbol->line
					);

					dst_script_function=(ScriptFunction *)dst_symbol->ref_ptr;
					dst_script_function->scope=src_script_function->scope;

				}else{
					ZS_THROW_RUNTIME_ERROR("Error adding functions from base elements '%s': '%s::%s' is not a function"
							,Rtti::demangle(base_class_name).toConstChar()
							,Rtti::demangle(base_class_name).toConstChar()
							, src_symbol->name.toConstChar());
				}
			}
		}


		for(int i = 0; i < base_vars->length(); i++){
			Symbol *src_symbol = (Symbol *)base_vars->get(i);

			if(src_symbol->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){

				MemberProperty *mp_src=(MemberProperty *)src_symbol->ref_ptr;
				MemberProperty *mp_dst=NULL;

				Symbol *symbol_member_property=NULL;
				Symbol *symbol_function=NULL;

				symbol_member_property=this_class->registerMemberProperty(src_symbol->name,src_symbol->file,src_symbol->line);
				mp_dst=(MemberProperty *)symbol_member_property->ref_ptr;


				struct _PropertyMethodIt{
					Symbol **dst_symbol_function;
					Symbol *src_symbol_function;
				}property_methods[]={
					{&mp_dst->metamethod_members.getter,mp_src->metamethod_members.getter}
					,{&mp_dst->metamethod_members.postinc,mp_src->metamethod_members.postinc}
					,{&mp_dst->metamethod_members.postdec,mp_src->metamethod_members.postdec}
					,{&mp_dst->metamethod_members.preinc,mp_src->metamethod_members.preinc}
					,{&mp_dst->metamethod_members.predec,mp_src->metamethod_members.predec}
					//,{&mp_dst->metamethod_members.neg,mp_src->metamethod_members.neg}
					,{0,0}
				};

				_PropertyMethodIt *it=property_methods;

				// register getter and setter
				while(it->dst_symbol_function!=0){

					*it->dst_symbol_function= NULL; // init to null

					if(it->src_symbol_function!=0){ // we have src method
						ScriptFunction *src_script_function=(ScriptFunction *)it->src_symbol_function->ref_ptr
									   ,*dst_script_function=NULL;

						FunctionParam *params=FunctionParam::createArrayFromFunction(src_script_function);
						int8_t params_len=src_script_function->params_len;

						symbol_function=this_class->registerMemberFunction(
								src_script_function->name,
								&params,
								params_len,
								src_script_function->properties,
								src_script_function->return_script_type_id,
								src_script_function->ref_native_function_ptr
						);

						*it->dst_symbol_function=symbol_function;
						dst_script_function=(ScriptFunction *)symbol_function->ref_ptr;
						dst_script_function->scope=src_script_function->scope;
					}
					it++;
				}

				const Metamethod *it_setter=MetamethodMembers::member_setter_list;
				while(*it_setter!= 0){
					MetamethodMemberSetterInfo mp_info=mp_src->metamethod_members.getSetterInfo(*it_setter);
					if(mp_info.setters!=NULL){
						for(int h=0; h < mp_info.setters->length(); h++){

							StackElement *stk_setter=mp_info.setters->get(h);
							Symbol *symbol_setter=(Symbol *)stk_setter->value;
							ScriptFunction *sf_setter=(ScriptFunction *)symbol_setter->ref_ptr,
											*dst_script_function=NULL;

							FunctionParam *params=FunctionParam::createArrayFromFunction(sf_setter);
							int8_t params_len=sf_setter->params_len;

							symbol_function=this_class->registerMemberFunction(
								sf_setter->name,
								&params,
								params_len,
								sf_setter->properties,
								sf_setter->return_script_type_id,
								sf_setter->ref_native_function_ptr
							);

							mp_dst->metamethod_members.addSetter(*it_setter,symbol_function);

							dst_script_function=(ScriptFunction *)symbol_function->ref_ptr;
							dst_script_function->scope=sf_setter->scope;
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
	 * Register T Member constructor
	 */
	template <typename T,typename F>
	void ScriptTypesFactory::registerConstructor(
		F function_type
		,const char *registered_file
		,short registered_line
	){

		String native_name = typeid( T *).name();
		ScriptType * type=	getScriptTypeFromNamePtr(native_name);
		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' is not registered",native_name.toConstChar());
		}
		return registerMemberFunction<T>(ZS_CONSTRUCTOR_FUNCTION_NAME,function_type, registered_file,registered_line );
	}

	template <typename T,typename F>
	void ScriptTypesFactory::registerConstMemberProperty(
			const String & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		type->registerConstMemberProperty<F>(
			 _property_name
			 ,_ptr_function
			,registered_file
			,registered_line
		);
	}


	// register member property shr set operation
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyMetamethod(
		const String & _property_name
		,const String & _metamethod_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		type->registerMemberPropertyMetamethod<F>(
			 _property_name
			 ,_metamethod_name
			 ,_ptr_function
			,registered_file
			,registered_line
		);
	}

	/*
	 * register static property getter
	 */
	/*template <typename T,typename F>
	void ScriptTypesFactory::registerStaticMemberPropertyGetter(
		const String & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();
		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.toConstChar());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerStaticMemberPropertyGetter(
				 _property_name
				,&params
				,params_len
				,return_script_type_id
				,(zs_int)_ptr_function
				,_registered_file
				,_registered_line
		);


	}*/

	/*
	 * register property member setter
	 */
	/*template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertySetter(
		const String & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		String native_name = typeid( T *).name();
		FunctionParam *params=NULL;
		int params_len=0;
		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.toConstChar());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_SET
			 ,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}
*/
	/*
	 * register property getter
	 */
	/*template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyGetter(
		const String & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();
		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.toConstChar());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyGetter(
				 _property_name
				,&params
				,params_len
				,return_script_type_id
				,(zs_int)_ptr_function
				,_registered_file
				,_registered_line
		);


	}*/


	/*
	 * register property neg
	 */
	/*template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyNeg(
		const String & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();
		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.toConstChar());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyNeg(
				 _property_name
				,&params
				,params_len
				,return_script_type_id
				,(zs_int)_ptr_function
				,_registered_file
				,_registered_line
		);

	}*/

	/*
	 * register property neg
	 */
	/*template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyBwc(
		const String & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();
		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.toConstChar());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyBwc(
				 _property_name
				,&params
				,params_len
				,return_script_type_id
				,(zs_int)_ptr_function
				,_registered_file
				,_registered_line
		);

	}*/

	/*
	 * register member property  post_increment
	 */
	/*template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyPostInc(
		const String & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.toConstChar());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_POST_INC
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}*/

	/*
	 * register member property  post_decrement
	 */
	/*template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyPostDec(
			const String & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.toConstChar());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_POST_DEC
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}*/

	/*
	 * register member property  pre_increment
	 */
	/*template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyPreInc(
			const String & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_PRE_INC
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}*/

	/*
	 * register member property  pre_decrement
	 */
	/*template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyPreDec(
			const String & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_PRE_DEC
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}

	// register member property add set operation
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyAddSetter(
			const String & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_ADD_ASSIGN
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}

	// register member property  sub set operation
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertySubSetter(
			const String & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			, type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_SUB_ASSIGN
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,_registered_file
			,_registered_line
		);
	}

	// register member property mul set operation
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyMulSetter(
			const String & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_MUL_ASSIGN
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property div set operation
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyDivSetter(
			const String & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_DIV_ASSIGN
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property mod set operation
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyModSetter(
			const String & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_MOD_ASSIGN
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property and set operation
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyAndSetter(
			const String & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_AND_ASSIGN
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property or set operation
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyOrSetter(
			const String & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_OR_ASSIGN
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property xor set operation
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyXorSetter(
			const String & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_XOR_ASSIGN
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property shl set operation
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyShlSetter(
			const String & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_SHL_ASSIGN
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}

	// register member property shr set operation
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberPropertyShrSetter(
			const String & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		FunctionParam *params=NULL;
		int params_len=0;
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.toConstChar()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,type
			,_ptr_function
			,&params
			,&params_len
		);

		type->registerMemberPropertyMetamethod(
			 _property_name
			 ,METAMETHOD_SHR_ASSIGN
			,&params
			,params_len
			,return_script_type_id
			,(zs_int)_ptr_function
			,registered_file
			,registered_line
		);
	}*/


	/**
	 * Register T Member function Class
	 * like register function c but is added to member function list according type T
	 */
	template <typename T, typename F>
	void ScriptTypesFactory::registerStaticMemberFunction(
		const String & name
		,F ptr_function
		,const char *registered_file
		,short registered_line
	)
	{
		// to make compatible MSVC shared library
		String native_name = typeid( T *).name();


		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.toConstChar());
		}

		return type->registerStaticMemberFunction<F>(name
				,ptr_function
				,registered_file
				,registered_line);

	}

	/*
	 * register static function registerMemberFunction as function member
	 * Is automatically added in function member list according first parameter type of function_type
	 */
	template <typename T,typename F>
	void ScriptTypesFactory::registerMemberFunction(
		const String & name
		,F ptr_function
		,const char *registered_file
		,short registered_line
	){
		String native_name = typeid( T *).name();

		ScriptType *type = getScriptTypeFromNamePtr(native_name);

		if(type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.toConstChar());
		}

		type->registerMemberFunction<F>(
			name
			,ptr_function
			,registered_file
			,registered_line
		);

	}
}