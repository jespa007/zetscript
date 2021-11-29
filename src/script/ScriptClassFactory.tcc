/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	/**
	 * Register C variable
	 */
	template<typename V>
	 void  ScriptClassFactory::registerNativeGlobalVariable(
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
			THROW_RUNTIME_ERROR("main function is not created");
		}

		if(getIdxClassFromItsNativeType(var_type) == ZS_IDX_UNDEFINED){
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
	void ScriptClassFactory::registerNativeGlobalFunction(
		const zs_string &  function_name
		,F ptr_function
		,const char *registered_file
		,short registered_line)
	{
		zs_string error;
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;

		if(main_function == NULL){
			THROW_RUNTIME_ERROR("main function is not created");
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
				this
				,function_name
				,ptr_function
				,&params
				,&params_len
		);

		// Init struct...
		main_function->registerLocalFunction(
				 MAIN_SCOPE(this)
				,registered_file
				,registered_line
				,function_name
				,&params
				,params_len
				,idx_return_type
				,(zs_int)ptr_function
				,FUNCTION_PROPERTY_C_OBJECT_REF
			);

		ZS_LOG_DEBUG("Registered function name: %s",function_name);
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<class T>
	ScriptClass * ScriptClassFactory::registerNativeSingletonClass(
		const zs_string & class_name
		,const char *registered_file
		,short registered_line
	){//, const zs_string & base_class_name=""){

		ScriptClass *sc=NULL;
		const char * str_class_name_ptr = typeid( T *).name();
		//int size=script_classes->count;
		int idx_class=ZS_IDX_UNDEFINED;
		Scope * scope = NULL;


		checkClassName(class_name);

		if(getIdxNativeRegisteredClass(str_class_name_ptr)!=ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("Native class '%s' is already registered",zs_rtti::demangle(typeid( T).name()));
		}

		idx_class=script_classes->count;
		scope = NEW_SCOPE(this,ZS_IDX_UNDEFINED,NULL,SCOPE_PROPERTY_IS_SCOPE_CLASS|SCOPE_PROPERTY_IS_C_OBJECT_REF);
		MAIN_SCOPE(this)->registerSymbolType(registered_file,registered_line,class_name);

		sc = new ScriptClass(zs,idx_class,class_name,scope,str_class_name_ptr,SCRIPT_CLASS_PROPERTY_C_OBJECT_REF);
		scope->setScriptClass(sc);

		// in C there's no script constructor ...
		sc->idx_function_member_constructor=ZS_IDX_UNDEFINED;
		// allow dynamic constructor in function its parameters ...
		sc->c_constructor = NULL;
		sc->c_destructor = NULL;
		script_classes->push_back((zs_int)sc);

		sc->idx_class=script_classes->count-1;
		ZS_LOG_DEBUG("* C++ class '%s' registered as (%s).",class_name.c_str(),zs_rtti::demangle(str_class_name_ptr).c_str());

		return sc;
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<typename C>
	ScriptClass * ScriptClassFactory::registerNativeClass(
		const zs_string & class_name
		,C * (*_constructor)()
		,void (*_destructor)(C *)
		,const char *registered_file
		,short registered_line
	){

		ScriptClass *sc =registerNativeSingletonClass<C>(
				class_name
				,registered_file
				,registered_line
		);

		// get class...
		sc->c_constructor = (void *)_constructor;
		sc->c_destructor = (void *)_destructor;

		return sc;
	}

	template<class C,class B>
	void ScriptClassFactory::nativeClassInheritsFrom(){

		const char *base_class_name=typeid(B).name();
		const char * base_class_name_ptr=typeid(B *).name();
		const char * class_name=typeid(C).name();
		const char * class_name_ptr=typeid(C *).name();
		zs_string error;

		int idx_base_class = getIdxClassFromItsNativeType(base_class_name_ptr);
		if(idx_base_class == -1) {
			THROW_RUNTIME_ERROR("base class %s not registered",base_class_name_ptr);
		}


		int idx_register_class = getIdxClassFromItsNativeType(class_name_ptr);
		if(idx_register_class == ZS_IDX_UNDEFINED) {
			THROW_RUNTIME_ERROR("class %s not registered",class_name_ptr);
		}

		if(isClassInheritsFrom(idx_register_class,idx_base_class)){
			THROW_RUNTIME_ERROR("C++ class '%s' is already registered as base of '%s' ",zs_rtti::demangle(class_name), zs_rtti::demangle(base_class_name));
		}

		// check whether is in fact base of ...
		if(!std::is_base_of<B,C>::value){
			THROW_RUNTIME_ERROR("C++ class '%s' is not base of '%s' ",zs_rtti::demangle(class_name), zs_rtti::demangle(base_class_name));
		}

		// now only allows one inheritance!
		ScriptClass *sc=(ScriptClass *)script_classes->get(idx_register_class);

		for(unsigned i=0; i < sc->idx_base_classes->count; i++){
			sc=getScriptClass(sc->idx_base_classes->items[i]); // get base class...
			if(sc->str_class_ptr_type ==base_class_name_ptr){
				THROW_RUNTIME_ERROR("C++ class '%s' already base of '%s' ",zs_rtti::demangle(class_name), zs_rtti::demangle(base_class_name));
			}
		}

		ScriptClass *this_class = (ScriptClass *)script_classes->get(idx_register_class);
		this_class->idx_base_classes->push_back(idx_base_class);

		//----------------------------
		//
		// DERIVATE STATE
		//

		ScriptClass *base_class = (ScriptClass *)script_classes->get(idx_base_class);
		zs_vector *base_vars=base_class->class_scope->symbol_variables;
		zs_vector *base_functions=base_class->class_scope->symbol_variables;

		// register all c vars symbols ...
		for(unsigned i = 0; i < base_functions->count; i++){

			Symbol *symbol_src = (Symbol *)base_functions->items[i];

			bool is_metamethod_function =
										    zs_strutils::starts_with(symbol_src->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_GETTER)
										||  zs_strutils::starts_with(symbol_src->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_POST_INC)
										||  zs_strutils::starts_with(symbol_src->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_POST_DEC)
										||  zs_strutils::starts_with(symbol_src->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_PRE_INC)
										||  zs_strutils::starts_with(symbol_src->name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_PRE_DEC)
										;

			if(is_metamethod_function==false){ // try find setter
				ByteCodeMetamethod *it=MemberProperty::byte_code_metamethod_setter_list;
				zs_string symbol_name_methametod;
				while(*it!=0 && is_metamethod_function==false){
					symbol_name_methametod=byte_code_metamethod_to_symbol_str(*it);
					symbol_name_methametod.append('@');
					is_metamethod_function=zs_strutils::starts_with(symbol_src->name,symbol_name_methametod);
					it++;
				}
			}

			// we have to know whether function member is or not getter/setter because we create them in the property member case. If not, we could have
			// duplicated symbols.
			if(is_metamethod_function == false){

				ScriptFunction *script_function = (ScriptFunction *)symbol_src->ref_ptr;
				// build params...


				ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(script_function);
				size_t params_len=script_function->params_len;


				this_class->registerNativeMemberFunction(
					script_function->function_name,
					&params,
					params_len,
					script_function->idx_return_type,
					script_function->ref_native_function_ptr, // it contains script function pointer
					script_function->properties, //derivated_symbol_info_properties
					symbol_src->file,
					symbol_src->line
				);
			}
		}


		for(unsigned i = 0; i < base_vars->count; i++){
			Symbol *symbol_src = (Symbol *)base_vars->items[i];

			if(symbol_src->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){

				MemberProperty *ma_src=(MemberProperty *)symbol_src->ref_ptr;
				MemberProperty *ma_dst=NULL;

				Symbol *symbol_member_property=NULL;
				Symbol *symbol_function=NULL;

				symbol_member_property=this_class->registerMemberProperty(symbol_src->name,symbol_src->file,symbol_src->line);
				ma_dst=(MemberProperty *)symbol_member_property->ref_ptr;


				struct _PropertyMethodIt{
					ScriptFunction **dst_function;
					ScriptFunction *src_function;
				}property_methods[]={
					{&ma_dst->getter,ma_src->getter}
					,{&ma_dst->post_inc,ma_src->post_inc}
					,{&ma_dst->post_dec,ma_src->post_dec}
					,{&ma_dst->pre_inc,ma_src->pre_inc}
					,{&ma_dst->pre_dec,ma_src->pre_dec}
					,{0,0}
				};

				_PropertyMethodIt *it=property_methods;

				// register getter and setter
				while(it->dst_function!=0){

					*it->dst_function= NULL; // init to null

					if(it->src_function!=0){ // we have src method

						ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(it->src);
						size_t params_len=it->src_function->params_len;

						symbol_function=this_class->registerNativeMemberFunction(
								it->src_function->function_name,
								&params,
								params_len,
								it->src_function->idx_return_type,
								it->src_function->ref_native_function_ptr,
								it->src_function->properties
								//it->src_function->symbol.file,
								//it->src_function->symbol.line
						);

						*it->dst_function=(ScriptFunction *)symbol_function->ref_ptr;
					}
					it++;
				}

				ByteCodeMetamethod *it_setter=MemberProperty::byte_code_metamethod_setter_list;
				while(*it_setter!= 0){
					MemberPropertySetterInfo mp_info=ma_src->getSetterInfo(*it_setter);
					if(mp_info.setters!=NULL){
						for(unsigned i=0; i < mp_info.setters->count; i++){

							StackElement *stk_setter=(StackElement *)mp_info.setters->items[i];
							ScriptFunction *sf_setter=(ScriptFunction *)stk_setter->value;

							ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(sf_setter);
							size_t params_len=it->src_function->params_len;

							symbol_function=this_class->registerNativeMemberFunction(
									sf_setter->function_name,
									&params,
									params_len,
									sf_setter->idx_return_type,
									sf_setter->ref_native_function_ptr,
									sf_setter->properties
									//sf_setter->symbol->file,
									//sf_setter->symbol->line
							);

							ma_dst->addSetter(*it_setter,(ScriptFunction *)symbol_function->ref_ptr);
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
	void ScriptClassFactory::registerNativeConstructor(
		F function_type
		,const char *registered_file
		,short registered_line
	){

		zs_string str_class_name_ptr = typeid( C *).name();
		ScriptClass * c_class=	getScriptClassByNativeClassPtr(str_class_name_ptr);
		if(c_class == NULL){
			THROW_RUNTIME_ERROR("class '%s' is not registered",str_class_name_ptr.c_str());
		}
		return registerNativeMemberFunction<C>(c_class->class_name,function_type, registered_file,registered_line );
	}

	/**
	 * Register C Member var
	 */
	template <typename C, typename R>
	void ScriptClassFactory::registerNativeStaticConstMember(
			const zs_string & var_name
			,const R var_pointer
			,const char *registered_file
			,short registered_line) //unsigned int offset)
	{
		// to make compatible MSVC shared library
		const char *var_type = typeid(R).name(); // we need the pointer type ...
		const char *return_type;
		zs_string error;
		const char *str_class_name_ptr = typeid( C *).name();
		Symbol *symbol;

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr);
		}

		// 1. check all parameters ok.
		// check valid parameters ...
		if(getIdxClassFromItsNativeType(var_type) == ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("%s::%s has not valid type (%s)"
					,c_class->class_name.c_str()
					,var_name
					,zs_rtti::demangle(typeid(R).name()));
		}

		// register variable...
		symbol=c_class->registerNativeMemberVariable(
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
	void ScriptClassFactory::registerNativeMemberPropertySetter(
		const zs_string & _property_name
		,F _ptr_function
		,const char *registered_file
		,short registered_line
	){
		zs_string str_class_name_ptr = typeid( C *).name();
		ScriptFunctionParam *params=NULL;
		size_t *params_len=0;
		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
				this
				,_property_name
				,_ptr_function
				,&params
				,&params_len
				);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_SET
			,_ptr_function
			 ,&params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}



	/*
	 * register property getter
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyGetter(
		const zs_string & _property_name
		,F _ptr_function
		,const char *_registered_file
		,short _registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t *params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();
		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyGetter(
				 _property_name
				,idx_return_type
				,(zs_int)_ptr_function
				,_registered_file
				,_registered_line
		);
	}

	/*
	 * register member property  post_increment
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyPostIncrement(
		const zs_string & _property_name
		,F _ptr_function
		,const char *registered_file
		,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_POST_INC
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	/*
	 * register member property  post_decrement
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyPostDecrement(
			const zs_string & _property_name
			,F _ptr_function
			,const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_POST_DEC
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	/*
	 * register member property  pre_increment
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyPreIncrement(
			const zs_string & _property_name
			,F _ptr_function
			,const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_PRE_INC
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	/*
	 * register member property  pre_decrement
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyPreDecrement(
			const zs_string & _property_name
			,F _ptr_function
			,const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_PRE_DEC
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	// register member property add set operation
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyAddSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_ADD_SET
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	// register member property  sub set operation
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertySubSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_SUB_SET
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	// register member property mul set operation
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyMulSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_MUL_DEC
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	// register member property div set operation
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyDivSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_DIV_SET
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	// register member property mod set operation
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyModSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_MOD_SET
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	// register member property and set operation
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyAndSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_AND_SET
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	// register member property or set operation
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyOrSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_OR_SET
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	// register member property xor set operation
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyXorSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_XOR_SET
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	// register member property shl set operation
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyShlSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_SHL_SET
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}

	// register member property shr set operation
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberPropertyShrSet(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
	){
		ScriptFunctionParam *params=NULL;
		size_t params_len=0;
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		int idx_return_type=getNativeMemberFunctionRetArgsTypes(
			this
			,_property_name
			,_ptr_function
			,&params
			,&params_len
		);

		c_class->registerNativeMemberPropertyMetamethod(
			 _property_name
			 ,BYTE_CODE_METAMETHOD_SHR_SET
			,_ptr_function
			,params
			,params_len
			,idx_return_type
			,registered_file
			,registered_line
		);
	}


	/**
	 * Register C Member function Class
	 * like register function c but is added to member function list according type C
	 */
	template <typename C, typename F>
	void ScriptClassFactory::registerNativeMemberFunctionStatic(
		const zs_string & function_name
		,F ptr_function
		,const char *registered_file
		,short registered_line
	)
	{
		// to make compatible MSVC shared library
		zs_string str_class_name_ptr = typeid( C *).name();


		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		return c_class->registerNativeMemberFunctionStatic<F>(function_name
				,ptr_function
				,registered_file
				,registered_line);

	}

	/*
	 * register static function registerNativeMemberFunction as function member
	 * Is automatically added in function member list according first parameter type of function_type
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberFunction(
		const zs_string & function_name
		,F ptr_function
		,const char *registered_file
		,short registered_line
	){
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		c_class->registerNativeMemberFunction<F>(
			function_name
			,ptr_function
			,registered_file
			,registered_line
		);

	}
}
