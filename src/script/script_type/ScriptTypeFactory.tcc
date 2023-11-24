/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	/**
	 * Register T function
	 */
	template <typename F>
	void ScriptTypeFactory::registerFunction(
		const zs_string &  _function_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line)
	{
		zs_string error;
		ScriptFunctionParam *params=NULL;
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
				,FUNCTION_PROPERTY_C_OBJECT_REF
			);

		ZS_LOG_DEBUG("Registered function name '%s'",_function_name.c_str());
	}

	/**
	 * Register T Class. Return index registered class
	 */
	template<class T>
	ScriptType * ScriptTypeFactory::registerType(
		const zs_string & name
		,T * (*_new_native_instance)(ZetScript *_zs)
		,void (*_delete_native_instance)(ZetScript *_zs,T *)
		,const char *registered_file
		,short registered_line
	){//, const zs_string & base_class_name=""){

		ScriptType *script_type=NULL;
		const char * native_name = typeid( T *).name();
		//int size=script_types->size();
		ScriptTypeId script_type_id=SCRIPT_TYPE_ID_INVALID;
		Scope * scope = NULL;


		checkScriptTypeName(name);

		if(getScriptTypeIdFromTypeNamePtr(native_name)!=ZS_UNDEFINED_IDX){
			ZS_THROW_RUNTIME_ERROR(
			"Native type '%s' is already registered"
			,zs_rtti::demangle(typeid( T).name()).c_str()
			);
		}

		script_type_id=(ScriptTypeId)script_types->size();
		scope = ZS_NEW_SCOPE(this,ZS_UNDEFINED_IDX,NULL,ZS_SCOPE_PROPERTY_IS_SCOPE_CLASS|ZS_SCOPE_PROPERTY_IS_C_OBJECT_REF);
		ZS_MAIN_SCOPE(this)->registerSymbolScriptType(registered_file,registered_line,name);

		script_type = new ScriptType(zs,script_type_id,name,scope,native_name,ZS_SCRIPT_TYPE_PROPERTY_C_OBJECT_REF);
		scope->setScriptTypeOwner(script_type);

		// in T there's no script constructor ...
		script_type->idx_function_member_constructor=ZS_UNDEFINED_IDX;
		// allow dynamic constructor in function its parameters ...
		script_type->new_native_instance = NULL;
		script_type->delete_native_instance = NULL;
		script_types->push_back(script_type);


		if((_new_native_instance != NULL) && (_delete_native_instance != NULL)){ // it can instanced, not static
			script_type->new_native_instance = (void *)_new_native_instance;
			script_type->delete_native_instance = (void *)_delete_native_instance;

		}else{ // Cannot be instanced so is static
			script_type->properties|=ZS_SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE;
		}

		script_type->id=script_types->size()-1;
		ZS_LOG_DEBUG("* native type '%s' registered as (%s).",name.c_str(),zs_rtti::demangle(native_name).c_str());

		return script_type;
	}


	template<class T,class B>
	void ScriptTypeFactory::extends(){

		const char *base_class_name=typeid(B).name();
		const char * base_class_name_ptr=typeid(B *).name();
		const char * name=typeid(T).name();
		const char * class_name_ptr=typeid(T *).name();
		zs_string error;

		ScriptTypeId base_script_type_id = getScriptTypeIdFromTypeNamePtr(base_class_name_ptr);
		if(base_script_type_id == SCRIPT_TYPE_ID_INVALID) {
			ZS_THROW_RUNTIME_ERROR("base native type '%s' not registered",base_class_name_ptr);
		}


		ScriptTypeId register_script_type_id = getScriptTypeIdFromTypeNamePtr(class_name_ptr);
		if(register_script_type_id == SCRIPT_TYPE_ID_INVALID) {
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",class_name_ptr);
		}

		if(scriptTypeInheritsFrom(register_script_type_id,base_script_type_id)){
			ZS_THROW_RUNTIME_ERROR("native type '%s' is already registered as base of '%s' ",zs_rtti::demangle(name).c_str(), zs_rtti::demangle(base_class_name).c_str());
		}

		// check whether is in fact base of ...
		if(!std::is_base_of<B,T>::value){
			ZS_THROW_RUNTIME_ERROR("native type '%s' is not base of '%s' ",zs_rtti::demangle(name).c_str(), zs_rtti::demangle(base_class_name).c_str());
		}

		// now only allows one inheritance!
		ScriptType *main_class=script_types->get(register_script_type_id);


		for(int i=0; i < main_class->base_script_type_ids->size(); i++){
			ScriptType *script_type=getScriptType(main_class->base_script_type_ids->get(i)); // get base type...
			if(script_type->native_name ==base_class_name_ptr){
				ZS_THROW_RUNTIME_ERROR("native type '%s' already extends from '%s' "
						,zs_rtti::demangle(name).c_str()
						, zs_rtti::demangle(base_class_name).c_str());
			}
		}


		ScriptType *base=(ScriptType *)script_types->get(base_script_type_id);

		// search native types that already inherits type B
		for(int i=0; i < main_class->base_script_type_ids->size(); i++){
			ScriptType *script_type=getScriptType(main_class->base_script_type_ids->get(i)); // get base type...
			// check whether type inherits inheritates B
			if(script_type->extendsFrom(base_script_type_id)){
				ZS_THROW_RUNTIME_ERROR("Type '%s' cannot extend from '%s' because '%s' inherits '%s' that already is inherited by '%s'"
					,zs_rtti::demangle(name).c_str()
					, zs_rtti::demangle(base_class_name).c_str()
					,zs_rtti::demangle(name).c_str()
					,zs_rtti::demangle(script_type->name.c_str()).c_str()
					, zs_rtti::demangle(base_class_name).c_str()
				);
			}
			// check the viceversa, if B inheritates inherited types of main_class
			if(base->extendsFrom(script_type->id)){
				ZS_THROW_RUNTIME_ERROR("Type '%s' cannot extend from '%s' because '%s' has inherited type '%s' that also is inherited by '%s'"
					,zs_rtti::demangle(name).c_str()
					, zs_rtti::demangle(base_class_name).c_str()
					,zs_rtti::demangle(name).c_str()
					, zs_rtti::demangle(script_type->name.c_str()).c_str()
					, zs_rtti::demangle(base_class_name).c_str()
				);
			}
		}

		ScriptType *this_class = (ScriptType *)script_types->get(register_script_type_id);
		this_class->base_script_type_ids->push_back(base_script_type_id);

		//----------------------------
		//
		// DERIVATE STATE
		//

		ScriptType *base_class = script_types->get(base_script_type_id);
		zs_vector<Symbol *> *base_vars=base_class->scope->symbol_variables;
		zs_vector<Symbol *> *base_functions=base_class->scope->symbol_functions;

		// register all c vars symbols ...
		for(int i = 0; i < base_functions->size(); i++){

			Symbol *src_symbol = (Symbol *)base_functions->get(i);

			bool is_metamethod_function = MemberProperty::symbolNameMatchStartSymbolNameMetamethod(src_symbol->name);

			// we have to know whether function member is or not getter/setter because we create them in the property member case. If not, we could have
			// duplicated symbols.
			if(is_metamethod_function == false){
				ScriptFunction *src_script_function =NULL;
				if(src_symbol->properties & ZS_SYMBOL_PROPERTY_FUNCTION){ // function
					src_script_function=(ScriptFunction *)src_symbol->ref_ptr;
					Symbol *dst_symbol=NULL;
					ScriptFunction *dst_script_function=NULL;

					ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(src_script_function);
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
							,zs_rtti::demangle(base_class_name).c_str()
							,zs_rtti::demangle(base_class_name).c_str()
							, src_symbol->name.c_str());
				}
			}
		}


		for(int i = 0; i < base_vars->size(); i++){
			Symbol *src_symbol = (Symbol *)base_vars->get(i);

			if(src_symbol->properties & ZS_SYMBOL_PROPERTY_MEMBER_PROPERTY){

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

						ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(src_script_function);
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
						for(int h=0; h < mp_info.setters->size(); h++){

							StackElement *stk_setter=mp_info.setters->get(h);
							Symbol *symbol_setter=(Symbol *)stk_setter->value;
							ScriptFunction *sf_setter=(ScriptFunction *)symbol_setter->ref_ptr,
											*dst_script_function=NULL;

							ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(sf_setter);
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
	void ScriptTypeFactory::registerConstructor(
		F function_type
		,const char *registered_file
		,short registered_line
	){

		zs_string native_name = typeid( T *).name();
		ScriptType * script_type=	getScriptTypeFromTypeNamePtr(native_name);
		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' is not registered",native_name.c_str());
		}
		return registerMemberFunction<T>(ZS_CONSTRUCTOR_FUNCTION_NAME,function_type, registered_file,registered_line );
	}

	template <typename T,typename F>
	void ScriptTypeFactory::registerConstMemberProperty(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		script_type->registerConstMemberProperty<F>(
			 _property_name
			 ,_ptr_function
			,registered_file
			,registered_line
		);
	}


	// register member property shr set operation
	template <typename T,typename F>
	void ScriptTypeFactory::registerMemberPropertyMetamethod(
		const zs_string & _property_name
		,const zs_string & _metamethod_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		script_type->registerMemberPropertyMetamethod<F>(
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
	void ScriptTypeFactory::registerStaticMemberPropertyGetter(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();
		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.c_str());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerStaticMemberPropertyGetter(
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
	void ScriptTypeFactory::registerMemberPropertySetter(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		zs_string native_name = typeid( T *).name();
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.c_str());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyGetter(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();
		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.c_str());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyGetter(
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
	void ScriptTypeFactory::registerMemberPropertyNeg(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();
		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.c_str());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyNeg(
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
	void ScriptTypeFactory::registerMemberPropertyBwc(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();
		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.c_str());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyBwc(
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
	void ScriptTypeFactory::registerMemberPropertyPostInc(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.c_str());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyPostDec(
			const zs_string & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.c_str());
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyPreInc(
			const zs_string & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyPreDec(
			const zs_string & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyAddSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertySubSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			, script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyMulSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyDivSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyModSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyAndSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyOrSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyXorSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyShlSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerMemberPropertyShrSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR(
				"native type '%s' not registered"
				,native_name.c_str()
			);
		}

		int return_script_type_id=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::registerStaticMemberFunction(
		const zs_string & name
		,F ptr_function
		,const char *registered_file
		,short registered_line
	)
	{
		// to make compatible MSVC shared library
		zs_string native_name = typeid( T *).name();


		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.c_str());
		}

		return script_type->registerStaticMemberFunction<F>(name
				,ptr_function
				,registered_file
				,registered_line);

	}

	/*
	 * register static function registerMemberFunction as function member
	 * Is automatically added in function member list according first parameter type of function_type
	 */
	template <typename T,typename F>
	void ScriptTypeFactory::registerMemberFunction(
		const zs_string & name
		,F ptr_function
		,const char *registered_file
		,short registered_line
	){
		zs_string native_name = typeid( T *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(native_name);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("native type '%s' not registered",native_name.c_str());
		}

		script_type->registerMemberFunction<F>(
			name
			,ptr_function
			,registered_file
			,registered_line
		);

	}
}
