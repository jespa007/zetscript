#define 				MAX_REGISTER_CLASSES 	100


namespace zetscript{


	/**
	 * Register C variable
	 */
	template<typename V>
	 void  ScriptClassFactory::registerNativeGlobalVariable(
			 const zs_string & var_name
			 ,V var_ptr
			 , const char *registered_file
			 ,short registered_line)
	{
		//Scope *scope;
		zs_string var_type = typeid(V).name(); // we need the pointer type ...
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
			THROW_RUNTIME_ERROR("%s has not valid type (%s)",var_name.c_str(),var_type.c_str());
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
			,F function_ptr
			, const char *registered_file
			,short registered_line)
	{
		int idx_return_type=-1;
		zs_string return_type;
		std::vector<zs_string> str_arg;

		std::vector<ScriptFunctionParam> param_info;
		zs_int ref_ptr=0;

		if(main_function == NULL){
			THROW_RUNTIME_ERROR("main function is not created");
		}

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,return_type, str_arg, MakeIndexSequence<Traits3::arity>{});

		if(str_arg.size()>MAX_NATIVE_FUNCTION_ARGS){
			THROW_RUNTIME_ERROR("Max argyments reached");
		}

		// check valid parameters ...
		if((idx_return_type=getIdxClassFromItsNativeType(return_type))==-1){
			THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered"
					,zs_rtti::demangle(return_type).c_str()
					,function_name);
		}

		for(unsigned int i = 0; i < str_arg.size(); i++){
			int idx_type = getIdxClassFromItsNativeType(str_arg[i]);
			//StackElement default_value=k_stk_undefined;

			if(idx_type==IDX_BUILTIN_TYPE_ZS_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR("Argument %i type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)"
						,i+1
						,zs_rtti::demangle(str_arg[i]).c_str()
						,function_name
						,zs_rtti::demangle(str_arg[i]).c_str());
			}

			if(idx_type ==ZS_IDX_UNDEFINED){
				THROW_RUNTIME_ERROR("Argument %i type \"%s\" for function \"%s\" not registered"
						,i+1
						,zs_rtti::demangle(str_arg[i]).c_str()
						,function_name);
			}

			param_info.push_back(ScriptFunctionParam(idx_type,str_arg[i]));
		}

		ref_ptr=(zs_int)function_ptr;

		// Init struct...
		main_function->registerLocalFunction(
				 MAIN_SCOPE(this)
				,registered_file
				,registered_line
				,function_name
				,param_info
				,idx_return_type
				,ref_ptr
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
			, const char *registered_file
			,short registered_line
	){//, const zs_string & base_class_name=""){

		ScriptClass *sc=NULL;
		zs_string str_class_name_ptr = typeid( T *).name();
		//int size=script_classes->count;
		int idx_class=ZS_IDX_UNDEFINED;
		Scope * scope = NULL;
		Symbol *symbol=NULL;

		checkClassName(class_name);

		if(getIdxNativeRegisteredClass(str_class_name_ptr)!=ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("Native class '%s' is already registered",zs_rtti::demangle(typeid( T).name()).c_str());
		}

		idx_class=script_classes->count;
		scope = NEW_SCOPE(this,ZS_IDX_UNDEFINED,NULL,SCOPE_PROPERTY_IS_SCOPE_CLASS|SCOPE_PROPERTY_IS_C_OBJECT_REF);
		symbol=MAIN_SCOPE(this)->registerSymbolClass(registered_file,registered_line,class_name);

		sc = new ScriptClass(zs,idx_class,class_name,scope,str_class_name_ptr,SCRIPT_CLASS_PROPERTY_C_OBJECT_REF);
		scope->setScriptClass(sc);

		//irc->symbol_class=*symbol;

		// in C there's no script constructor ...
		sc->idx_function_member_constructor=ZS_IDX_UNDEFINED;
		// allow dynamic constructor in function its parameters ...

		//irc->str_class_ptr_type=str_class_name_ptr;
		//irc->symbol_class.properties|=SYMBOL_PROPERTY_C_OBJECT_REF;

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
	void ScriptClassFactory::registerNativeClass(
			const zs_string & class_name
			, const char *registered_file
			,short registered_line
	){//, const zs_string & base_class_name=""){

		ScriptClass *irc =registerNativeSingletonClass<C>(class_name);
		// get class...


		if(irc->idx_class < IDX_BUILTIN_TYPE_MAX && irc->idx_class != IDX_BUILTIN_TYPE_STACK_ELEMENT){
			THROW_RUNTIME_ERROR("The class to register \"%s\"  should not built in class",irc->str_class_ptr_type.c_str());
		}

		//put the constructor/destructor...
		irc->c_constructor = new std::function<void *()>([=](){
			C *t=new C();
			return t;
		});

		irc->c_destructor = new std::function<void (void *)>([=](void *p){
			delete (C *)p;
		});
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<typename C>
	ScriptClass * ScriptClassFactory::registerNativeClassStatic(
			const zs_string & class_name
			, C * (*_constructor)()
			, void (*_destructor)(C *)
			, const char *registered_file
			,short registered_line
	){//, const zs_string & base_class_name=""){

		ScriptClass *sc =registerNativeSingletonClass<C>(class_name);
		// get class...

		sc->c_constructor = (void *)_constructor;
		sc->c_destructor = (void *)_destructor;

		return sc;
		//irc->static_constructor_destructor = true;
	}


	/**
	 * Register C Class. Return index registered class
	 */
	/*template<typename C>
	void ScriptClassFactory::registerNativeClassBuiltin(
			const zs_string & class_name
			, C * (*_constructor)()
			, void (*_destructor)(C *)
			, const char *registered_file
			,short registered_line
	){//, const zs_string & base_class_name=""){

		ScriptClass *irc =registerNativeSingletonClass<C>(class_name);

		if(irc->idx_class >= IDX_BUILTIN_TYPE_MAX){
			THROW_RUNTIME_ERROR("The class to register \"%s\" should be a built in class",irc->str_class_ptr_type.c_str());
		}


		irc->c_constructor = (void *)_constructor;
		irc->c_destructor = (void *)_destructor;
		irc->static_constructor_destructor = true;
		//put the constructor/destructor...
	}*/



	template<class C,class B>
	void ScriptClassFactory::nativeClassInheritsFrom(){
		zs_string base_class_name=typeid(B).name();
		zs_string base_class_name_ptr=typeid(B *).name();
		zs_string class_name=typeid(C).name();
		zs_string class_name_ptr=typeid(C *).name();
		zs_string error;

		int idx_base_class = getIdxClassFromItsNativeType(base_class_name_ptr);
		if(idx_base_class == -1) {
			THROW_RUNTIME_ERROR("base class %s not registered",base_class_name_ptr.c_str());
		}


		int idx_register_class = getIdxClassFromItsNativeType(class_name_ptr);
		if(idx_register_class == ZS_IDX_UNDEFINED) {
			THROW_RUNTIME_ERROR("class %s not registered",class_name_ptr.c_str());
		}

		if(isClassInheritsFrom(idx_register_class,idx_base_class)){
			THROW_RUNTIME_ERROR("C++ class \"%s\" is already registered as base of \"%s\" ",zs_rtti::demangle(class_name).c_str(), zs_rtti::demangle(base_class_name).c_str());
		}

		// check whether is in fact base of ...
		if(!std::is_base_of<B,C>::value){
			THROW_RUNTIME_ERROR("C++ class \"%s\" is not base of \"%s\" ",zs_rtti::demangle(class_name).c_str(), zs_rtti::demangle(base_class_name).c_str());
		}

		// now only allows one inheritance!
		/*
		if(sc->idx_base_class != ZS_INVALID_CLASS){
			ScriptClass * base_class=getScriptClass(sc->idx_base_class);
			THROW_RUNTIME_ERROR("C++ class \"%s\" already is inherited from \"%s\" ",zs_rtti::demangle(class_name).c_str(), zs_rtti::demangle(base_class->str_class_ptr_type).c_str());
		}*/
		ScriptClass *sc=(ScriptClass *)script_classes->get(idx_register_class);

		for(unsigned i=0; i < sc->idx_base_classes->count; i++){
			sc=getScriptClass(sc->idx_base_classes->items[i]); // get base class...
			if(sc->str_class_ptr_type ==base_class_name_ptr){
				THROW_RUNTIME_ERROR("C++ class \"%s\" already base of \"%s\" ",zs_rtti::demangle(class_name).c_str(), zs_rtti::demangle(base_class_name).c_str());
			}
		}


		ScriptClass *this_class = (ScriptClass *)script_classes->get(idx_register_class);
		this_class->idx_base_classes->push_back(idx_base_class);

		// add conversion type for this class
		((zs_map_int *)conversion_types->get(this_class->idx_class))->set(idx_base_class,(zs_int)(new std::function<zs_int (zs_int entry)>([](zs_int entry){ return (zs_int)(B *)((C *)entry);})));


		//if(register_c_base_symbols){ // by default is disabled. User has to re-register! --> increases time and binary!!!
			//----------------------------
			//
			// DERIVATE STATE
			//
			// disabled for polymorphic classes because its function pointer std::map change at runtime)
			// https://stackoverflow.com/questions/48572734/is-possible-do-a-later-function-binding-knowing-its-function-type-and-later-the
			//

			ScriptClass *base_class = (ScriptClass *)script_classes->get(idx_base_class);
			zs_vector *base_vars=base_class->class_scope->symbol_variables;
			zs_vector *base_functions=base_class->class_scope->symbol_variables;

			/*unsigned short derivated_symbol_info_properties=SYMBOL_PROPERTY_C_OBJECT_REF;//| SYMBOL_PROPERTY_IS_DERIVATED;
			if(std::is_polymorphic<B>::value==true){

				//if((calling_function->symbol.properties & SYMBOL_PROPERTY_IS_POLYMORPHIC)){ // cannot call...
				THROW_RUNTIME_ERROR("class \"%s\" is polymorphic and cannot be processed due function/variable pointer it changes at runtime and could crash your application. You have two options:\n"
						"1. Set registerNativeBaseSymbols(false) and  re-register the function using REGISTER_NATIVE_FUNCTION_MEMBER\n"
						"2. Rewrite all virtual functions/classes to no non-virtual and do virtual operation through C function pointers\n"
						,(class_name+"::").c_str()
				);
			}*/

			//derivated_symbol_info_properties|=SYMBOL_PROPERTY_IS_POLYMORPHIC; // set as polymorphic and show error if you try to call a polymorphic function
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
					std::vector<ScriptFunctionParam> params;
					for(unsigned j=0; j < script_function->params_count;j++){
						params.push_back(script_function->params[j]);
					}

					this_class->registerNativeMemberFunction(
						script_function->symbol.name,
						params,
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

							std::vector<ScriptFunctionParam> param_info;

							for(unsigned i=0; i < it->src->params_count; i++){
								param_info.push_back(it->src->params[i]);
							}

							symbol_function=this_class->registerNativeMemberFunction(
									it->src->symbol.name,
									param_info,
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
						std::vector<ScriptFunctionParam> param_info;
						StackElement *stk_setter=(StackElement *)sf_setters->items[i];
						ScriptFunction *sf_setter=(ScriptFunction *)stk_setter->value;


						for(unsigned i=0; i < sf_setter->params_count; i++){
							param_info.push_back(sf_setter->params[i]);
						}


						/*this_class->registerNativeMemberAttributeSetter(
								symbol_src->name
								,param_info
								,sf_setter->ref_native_function_ptr
								,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_MEMBER_FUNCTION
								,symbol_src->file
								,symbol_src->line
						);*/

						symbol_function=this_class->registerNativeMemberFunction(
								sf_setter->symbol.name,
								param_info,
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
			, const char *registered_file
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
			, const R var_pointer
			, const char *registered_file
			,short registered_line) //unsigned int offset)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		zs_string var_type = typeid(R).name(); // we need the pointer type ...
		zs_string return_type;
		zs_string error;
		//std::vector<zs_string> params;
		zs_string str_class_name_ptr = typeid( C *).name();
		Symbol *symbol;

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		// 1. check all parameters ok.
		// check valid parameters ...
		if(getIdxClassFromItsNativeType(var_type) == ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("%s::%s has not valid type (%s)"
					,c_class->class_name.c_str()
					,var_name
					,zs_rtti::demangle(typeid(R).name()).c_str());
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
				, const char *registered_file
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
				, const char *registered_file
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
				, const char *registered_file
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
				, const char *registered_file
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
				, const char *registered_file
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
				, const char *registered_file
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
			, const char *registered_file
			,short registered_line
		)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

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
			, const char *registered_file
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
