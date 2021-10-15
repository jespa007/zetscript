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
			THROW_RUNTIME_ERROR("cannot register var \"%s\" with NULL reference value", var_name.c_str());
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
				,SYMBOL_PROPERTY_C_OBJECT_REF
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
		ZS_LOG_DEBUG("* C++ class \"%s\" registered as (%s).",class_name.c_str(),zs_rtti::demangle(str_class_name_ptr).c_str());

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
			THROW_RUNTIME_ERROR("C++ class \"%s\" is already registered as base of \"%s\" ",zs_rtti::demangle(class_name), zs_rtti::demangle(base_class_name));
		}

		// check whether is in fact base of ...
		if(!std::is_base_of<B,C>::value){
			THROW_RUNTIME_ERROR("C++ class \"%s\" is not base of \"%s\" ",zs_rtti::demangle(class_name), zs_rtti::demangle(base_class_name));
		}

		// now only allows one inheritance!
		ScriptClass *sc=(ScriptClass *)script_classes->get(idx_register_class);

		for(unsigned i=0; i < sc->idx_base_classes->count; i++){
			sc=getScriptClass(sc->idx_base_classes->items[i]); // get base class...
			if(sc->str_class_ptr_type ==base_class_name_ptr){
				THROW_RUNTIME_ERROR("C++ class \"%s\" already base of \"%s\" ",zs_rtti::demangle(class_name), zs_rtti::demangle(base_class_name));
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

			bool is_metamethod_function = 	zs_strutils::starts_with(symbol_src->name,ZS_PREFIX_SYMBOL_NAME_SETTER)
										||  zs_strutils::starts_with(symbol_src->name,ZS_PREFIX_SYMBOL_NAME_GETTER)
										||  zs_strutils::starts_with(symbol_src->name,ZS_PREFIX_SYMBOL_NAME_POST_INC)
										||  zs_strutils::starts_with(symbol_src->name,ZS_PREFIX_SYMBOL_NAME_POST_DEC)
										||  zs_strutils::starts_with(symbol_src->name,ZS_PREFIX_SYMBOL_NAME_PRE_INC)
										||  zs_strutils::starts_with(symbol_src->name,ZS_PREFIX_SYMBOL_NAME_PRE_DEC)
										;

			// we have to know whether function member is or not getter/setter because we create them in the attribute member case. If not, we could have
			// duplicated symbols.
			if(is_metamethod_function == false){

				ScriptFunction *script_function = (ScriptFunction *)symbol_src->ref_ptr;
				// build params...


				ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(script_function);
				size_t params_len=script_function->params_len;


				this_class->registerNativeMemberFunction(
					script_function->symbol.name,
					&params,
					params_len,
					script_function->idx_return_type,
					script_function->ref_native_function_ptr, // it contains script function pointer
					script_function->symbol.properties, //derivated_symbol_info_properties
					script_function->symbol.file,
					script_function->symbol.line
				);
			}
		}


		for(unsigned i = 0; i < base_vars->count; i++){
			Symbol *symbol_src = (Symbol *)base_vars->items[i];

			if(symbol_src->properties & SYMBOL_PROPERTY_MEMBER_ATTRIBUTE){

				MemberAttribute *ma_src=(MemberAttribute *)symbol_src->ref_ptr;
				MemberAttribute *ma_dst=NULL;

				zs_vector *sf_setters=&ma_src->setters;
				Symbol *symbol_attribute=NULL;
				Symbol *symbol_function=NULL;

				symbol_attribute=this_class->registerMemberAttribute(symbol_src->name,symbol_src->file,symbol_src->line);
				ma_dst=(MemberAttribute *)symbol_attribute->ref_ptr;


				struct _AttribMethodIt{
					ScriptFunction **dst;
					ScriptFunction *src;
				}attrib_methods[]={
					{&ma_dst->getter,ma_src->getter}
					,{&ma_dst->post_inc,ma_src->post_inc}
					,{&ma_dst->post_dec,ma_src->post_dec}
					,{&ma_dst->pre_inc,ma_src->pre_inc}
					,{&ma_dst->pre_dec,ma_src->pre_dec}
					,{0,0}
				};

				_AttribMethodIt *it=attrib_methods;

				// register getter and setter
				while(it->dst!=0){

					*it->dst = NULL; // init to null

					if(it->src!=0){ // we have src method

						ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(it->src);
						size_t params_len=it->src->params_len;

						symbol_function=this_class->registerNativeMemberFunction(
								it->src->symbol.name,
								&params,
								params_len,
								it->src->idx_return_type,
								it->src->ref_native_function_ptr,
								it->src->symbol.properties,
								it->src->symbol.file,
								it->src->symbol.line
						);

						*it->dst=(ScriptFunction *)symbol_function->ref_ptr;
					}
					it++;
				}

				for(unsigned i=0; i < sf_setters->count; i++){


					StackElement *stk_setter=(StackElement *)sf_setters->items[i];
					ScriptFunction *sf_setter=(ScriptFunction *)stk_setter->value;

					ScriptFunctionParam *params=ScriptFunctionParam::createArrayFromScriptFunction(sf_setter);
					size_t params_len=it->src->params_len;

					symbol_function=this_class->registerNativeMemberFunction(
							sf_setter->symbol.name,
							&params,
							params_len,
							sf_setter->idx_return_type,
							sf_setter->ref_native_function_ptr,
							sf_setter->symbol.properties,
							sf_setter->symbol.file,
							sf_setter->symbol.line
					);

					ma_dst->addSetter((ScriptFunction *)symbol_function->ref_ptr);
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
			THROW_RUNTIME_ERROR("class \"%s\" is not registered",str_class_name_ptr.c_str());
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
	 * register attribute setter
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberAttributeSetter(
		const zs_string & attr_name
		,F ptr_function_setter
		,const char *registered_file
		,short registered_line
	){
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		c_class->registerNativeMemberAttributeSetter<F>(
			 attr_name
			,ptr_function_setter
			,registered_file
			,registered_line
		);
	}

	/*
	 * register attribute getter
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberAttributeGetter(
		const zs_string & attr_name
		,F ptr_function_getter
		,const char *registered_file
		,short registered_line
	){
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		c_class->registerNativeMemberAttributeGetter<F>(
			 attr_name
			,ptr_function_getter
			,registered_file
			,registered_line
		);
	}

	/*
	 * register attribute post_increment
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberAttributePostIncrement(
		const zs_string & attr_name
		,F ptr_function_post_increment
		,const char *registered_file
		,short registered_line
	){
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		c_class->registerNativeMemberAttributePostIncrement<F>(
			 attr_name
			,ptr_function_post_increment
			,registered_file
			,registered_line
		);
	}

	/*
	 * register attribute post_decrement
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberAttributePostDecrement(
			const zs_string & attr_name
			,F ptr_function_post_decrement
			,const char *registered_file
			,short registered_line
	){
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		c_class->registerNativeMemberAttributePostDecrement<F>(
			 attr_name
			,ptr_function_post_decrement
			,registered_file
			,registered_line
		);
	}

	/*
	 * register attribute pre_increment
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberAttributePreIncrement(
			const zs_string & attr_name
			,F ptr_function_pre_increment
			,const char *registered_file
			,short registered_line
	){
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		c_class->registerNativeMemberAttributePreIncrement<F>(
			 attr_name
			,ptr_function_pre_increment
			,registered_file
			,registered_line
		);
	}

	/*
	 * register attribute pre_decrement
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberAttributePreDecrement(
			const zs_string & attr_name
			,F ptr_function_pre_decrement
			,const char *registered_file
			,short registered_line
	){
		zs_string str_class_name_ptr = typeid( C *).name();

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		c_class->registerNativeMemberAttributePreDecrement<F>(
			 attr_name
			,ptr_function_pre_decrement
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
