/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	/**
	 * Register C variable
	 */
	template<typename V>
	 void  ScriptTypeFactory::bindGlobalVariable(
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
			THROW_RUNTIME_ERROR("'%s' has not valid type '%s'",var_name.c_str(),var_type);
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
			ZS_LOG_DEBUG("Registered variable name '%s'",var_name.c_str());
		}
	}

	/**
	 * Register C function
	 */
	template <typename F>
	void ScriptTypeFactory::bindFunction(
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

		ZS_LOG_DEBUG("Registered function name '%s'",name_script_function);
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<class T>
	ScriptType * ScriptTypeFactory::bindType(
		const zs_string & str_script_type
		,T * (*_constructor)(ZetScript *_zs)
		,void (*_destructor)(ZetScript *_zs,T *)
		,const char *registered_file
		,short registered_line
	){//, const zs_string & base_class_name=""){

		ScriptType *sc=NULL;
		const char * str_script_type_ptr = typeid( T *).name();
		//int size=script_types->count;
		int idx_script_type=ZS_IDX_UNDEFINED;
		Scope * scope = NULL;


		checkScriptTypeName(str_script_type);

		if(getIdxScriptTypeFromTypeNamePtr(str_script_type_ptr)!=ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR(
			"Native type '%s' is already registered"
			,zs_rtti::demangle(typeid( T).name()).c_str()
			);
		}

		idx_script_type=script_types->count;
		scope = NEW_SCOPE(this,ZS_IDX_UNDEFINED,NULL,SCOPE_PROPERTY_IS_SCOPE_CLASS|SCOPE_PROPERTY_IS_C_OBJECT_REF);
		MAIN_SCOPE(this)->registerSymbolScriptType(registered_file,registered_line,str_script_type);

		sc = new ScriptType(zs,idx_script_type,str_script_type,scope,str_script_type_ptr,SCRIPT_TYPE_PROPERTY_C_OBJECT_REF);
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
		ZS_LOG_DEBUG("* native type '%s' registered as (%s).",str_script_type.c_str(),zs_rtti::demangle(str_script_type_ptr).c_str());

		return sc;
	}


	template<class C,class B>
	void ScriptTypeFactory::extends(){

		const char *base_class_name=typeid(B).name();
		const char * base_class_name_ptr=typeid(B *).name();
		const char * str_script_type=typeid(C).name();
		const char * class_name_ptr=typeid(C *).name();
		zs_string error;

		int idx_base_type = getIdxScriptTypeFromTypeNamePtr(base_class_name_ptr);
		if(idx_base_type == -1) {
			THROW_RUNTIME_ERROR("base native type '%s' not registered",base_class_name_ptr);
		}


		int idx_register_class = getIdxScriptTypeFromTypeNamePtr(class_name_ptr);
		if(idx_register_class == ZS_IDX_UNDEFINED) {
			THROW_RUNTIME_ERROR("native type '%s' not registered",class_name_ptr);
		}

		if(scriptTypeInheritsFrom(idx_register_class,idx_base_type)){
			THROW_RUNTIME_ERROR("native type '%s' is already registered as base of '%s' ",zs_rtti::demangle(str_script_type).c_str(), zs_rtti::demangle(base_class_name).c_str());
		}

		// check whether is in fact base of ...
		if(!std::is_base_of<B,C>::value){
			THROW_RUNTIME_ERROR("native type '%s' is not base of '%s' ",zs_rtti::demangle(str_script_type).c_str(), zs_rtti::demangle(base_class_name).c_str());
		}

		// now only allows one inheritance!
		ScriptType *main_class=(ScriptType *)script_types->get(idx_register_class);


		for(int i=0; i < main_class->idx_base_types->count; i++){
			ScriptType *sc=getScriptType(main_class->idx_base_types->items[i]); // get base type...
			if(sc->str_script_type_ptr ==base_class_name_ptr){
				THROW_RUNTIME_ERROR("native type '%s' already extends from '%s' "
						,zs_rtti::demangle(str_script_type).c_str()
						, zs_rtti::demangle(base_class_name).c_str());
			}
		}


		ScriptType *base=(ScriptType *)script_types->get(idx_base_type);

		// search native types that already inherits type B
		for(int i=0; i < main_class->idx_base_types->count; i++){
			ScriptType *sc=getScriptType(main_class->idx_base_types->items[i]); // get base type...
			// check whether type inherits inheritates B
			if(sc->extendsFrom(idx_base_type)){
				THROW_RUNTIME_ERROR("Type '%s' cannot extend from '%s' because '%s' inherits '%s' that already is inherited by '%s'"
					,zs_rtti::demangle(str_script_type).c_str()
					, zs_rtti::demangle(base_class_name).c_str()
					,zs_rtti::demangle(str_script_type).c_str()
					,zs_rtti::demangle(sc->str_script_type.c_str()).c_str()
					, zs_rtti::demangle(base_class_name).c_str()
				);
			}
			// check the viceversa, if B inheritates inherited types of main_class
			if(base->extendsFrom(sc->idx_script_type)){
				THROW_RUNTIME_ERROR("Type '%s' cannot extend from '%s' because '%s' has inherited type '%s' that also is inherited by '%s'"
					,zs_rtti::demangle(str_script_type).c_str()
					, zs_rtti::demangle(base_class_name).c_str()
					,zs_rtti::demangle(str_script_type).c_str()
					, zs_rtti::demangle(sc->str_script_type.c_str()).c_str()
					, zs_rtti::demangle(base_class_name).c_str()
				);
			}
		}

		ScriptType *this_class = (ScriptType *)script_types->get(idx_register_class);
		this_class->idx_base_types->push_back(idx_base_type);

		//----------------------------
		//
		// DERIVATE STATE
		//

		ScriptType *base_class = (ScriptType *)script_types->get(idx_base_type);
		zs_vector *base_vars=base_class->scope_script_type->symbol_variables;
		zs_vector *base_functions=base_class->scope_script_type->symbol_functions;

		// register all c vars symbols ...
		for(int i = 0; i < base_functions->count; i++){

			Symbol *src_symbol = (Symbol *)base_functions->items[i];

			bool is_metamethod_function = MemberProperty::symbolNameMatchStartSymbolNameMetamethod(src_symbol->name);
/*										    zs_strutils::starts_with(src_symbol->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_GETTER)
										||  zs_strutils::starts_with(src_symbol->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_POST_INC)
										||  zs_strutils::starts_with(src_symbol->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_POST_DEC)
										||  zs_strutils::starts_with(src_symbol->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_PRE_INC)
										||  zs_strutils::starts_with(src_symbol->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_PRE_DEC)
										;

			if(is_metamethod_function==false){ // try find setter
				ByteCodeMetamethod *it=MemberProperty::byte_code_metamethod_member_setter_list;
				zs_string symbol_name_methametod;
				while(*it!=0 && is_metamethod_function==false){
					symbol_name_methametod=byte_code_metamethod_to_symbol_str(*it);
					symbol_name_methametod.append('@');
					is_metamethod_function=zs_strutils::starts_with(src_symbol->name,symbol_name_methametod);
					it++;
				}
			}*/

			// we have to know whether function member is or not getter/setter because we create them in the property member case. If not, we could have
			// duplicated symbols.
			if(is_metamethod_function == false){
				ScriptFunction *src_script_function =NULL;
				/*if(src_symbol->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){ // property
					MemberProperty *mp=(MemberProperty *)src_symbol->ref_ptr;

					this_class->registerMemberFunction(
						script_function->name_script_function,
						&params,
						params_len,
						script_function->properties, //derivated_symbol_info_properties
						script_function->idx_script_type_return,
						script_function->ref_native_function_ptr, // it contains script function pointer
						src_symbol->file,
						src_symbol->line
					);

				}else */
				if(src_symbol->properties & SYMBOL_PROPERTY_FUNCTION){ // function
					src_script_function=(ScriptFunction *)src_symbol->ref_ptr;
					Symbol *dst_symbol=NULL;
					ScriptFunction *dst_script_function=NULL;

					ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(src_script_function);
					char params_len=src_script_function->params_len;


					dst_symbol=this_class->registerMemberFunction(
						src_script_function->name_script_function,
						&params,
						params_len,
						src_script_function->properties, //derivated_symbol_info_properties
						src_script_function->idx_script_type_return,
						src_script_function->ref_native_function_ptr, // it contains script function pointer
						src_symbol->file,
						src_symbol->line
					);

					dst_script_function=(ScriptFunction *)dst_symbol->ref_ptr;
					dst_script_function->scope_script_function=src_script_function->scope_script_function;

				}else{
					THROW_RUNTIME_ERROR("Error adding functions from base elements '%s': '%s::%s' is not a function"
							,zs_rtti::demangle(base_class_name).c_str()
							,zs_rtti::demangle(base_class_name).c_str()
							, src_symbol->name.c_str());
				}


				// build params...



			}
		}


		for(int i = 0; i < base_vars->count; i++){
			Symbol *src_symbol = (Symbol *)base_vars->items[i];

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
						ScriptFunction *src_script_function=(ScriptFunction *)it->src_symbol_function->ref_ptr
									   ,*dst_script_function=NULL;

						ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(src_script_function);
						char params_len=src_script_function->params_len;

						symbol_function=this_class->registerMemberFunction(
								src_script_function->name_script_function,
								&params,
								params_len,
								src_script_function->properties,
								src_script_function->idx_script_type_return,
								src_script_function->ref_native_function_ptr
								//it->src_function->symbol.file,
								//it->src_function->symbol.line
						);

						*it->dst_symbol_function=symbol_function;
						dst_script_function=(ScriptFunction *)symbol_function->ref_ptr;
						dst_script_function->scope_script_function=src_script_function->scope_script_function;
					}
					it++;
				}

				const ByteCodeMetamethod *it_setter=MetamethodMembers::byte_code_metamethod_member_setter_list;
				while(*it_setter!= 0){
					MetamethodMemberSetterInfo mp_info=mp_src->metamethod_members.getSetterInfo(*it_setter);
					if(mp_info.setters!=NULL){
						for(int h=0; h < mp_info.setters->count; h++){

							StackElement *stk_setter=(StackElement *)mp_info.setters->items[h];
							Symbol *symbol_setter=(Symbol *)stk_setter->value;
							ScriptFunction *sf_setter=(ScriptFunction *)symbol_setter->ref_ptr,
											*dst_script_function=NULL;

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

							dst_script_function=(ScriptFunction *)symbol_function->ref_ptr;
							dst_script_function->scope_script_function=sf_setter->scope_script_function;
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
	void ScriptTypeFactory::bindConstructor(
		F function_type
		,const char *registered_file
		,short registered_line
	){

		zs_string str_script_type_ptr = typeid( C *).name();
		ScriptType * script_type=	getScriptTypeFromTypeNamePtr(str_script_type_ptr);
		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' is not registered",str_script_type_ptr.c_str());
		}
		return bindMemberFunction<C>(CONSTRUCTOR_FUNCTION_NAME,function_type, registered_file,registered_line );
	}

	/**
	 * Register C Member var
	 */
	template <typename C, typename R>
	void ScriptTypeFactory::bindStaticConstMember(
			const zs_string & var_name
			,const R var_pointer
			,const char *registered_file
			,short registered_line) //unsigned int offset)
	{
		// to make compatible MSVC shared library
		const char *var_type = typeid(R).name(); // we need the pointer type ...
		const char *return_type;
		zs_string error;
		const char *str_script_type_ptr = typeid( C *).name();
		Symbol *symbol;

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		// 1. check all parameters ok.
		// check valid parameters ...
		if(getIdxScriptTypeFromTypeNamePtr(var_type) == ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("%s::%s has not valid type (%s)"
					,script_type->str_script_type.c_str()
					,var_name
					,zs_rtti::demangle(typeid(R).name()).c_str());
		}

		// register variable...
		symbol=script_type->registerMemberVariable(
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
	void ScriptTypeFactory::bindMemberPropertySetter(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		zs_string str_script_type_ptr = typeid( C *).name();
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr.c_str());
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
				this
				,script_type
				,_property_name
				,_ptr_function
				,&params
				,&params_len
				);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyGetter(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();
		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr.c_str());
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyGetter(
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
	void ScriptTypeFactory::bindMemberPropertyPostIncrement(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr.c_str());
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyPostDecrement(
			const zs_string & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr.c_str());
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyPreIncrement(
			const zs_string & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyPreDecrement(
			const zs_string & _property_name
			,F _ptr_function
			,const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyAddSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertySubSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			, script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyMulSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyDivSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyModSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyAndSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyOrSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyXorSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyShlSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberPropertyShrSetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		int params_len=0;
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr);
		}

		int idx_script_type_return=getNativeFunctionRetArgsTypes(
			this
			,script_type
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		script_type->registerMemberPropertyMetamethod(
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
	void ScriptTypeFactory::bindMemberFunctionStatic(
		const zs_string & name_script_function
		,F ptr_function
		,const char *registered_file
		,short registered_line
	)
	{
		// to make compatible MSVC shared library
		zs_string str_script_type_ptr = typeid( C *).name();


		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr.c_str());
		}

		return script_type->bindMemberFunctionStatic<F>(name_script_function
				,ptr_function
				,registered_file
				,registered_line);

	}

	/*
	 * register static function bindMemberFunction as function member
	 * Is automatically added in function member list according first parameter type of function_type
	 */
	template <typename C,typename F>
	void ScriptTypeFactory::bindMemberFunction(
		const zs_string & name_script_function
		,F ptr_function
		,const char *registered_file
		,short registered_line
	){
		zs_string str_script_type_ptr = typeid( C *).name();

		ScriptType *script_type = getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			THROW_RUNTIME_ERROR("native type '%s' not registered",str_script_type_ptr.c_str());
		}

		script_type->bindMemberFunction<F>(
			name_script_function
			,ptr_function
			,registered_file
			,registered_line
		);

	}
}
