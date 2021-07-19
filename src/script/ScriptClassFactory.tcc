#define 				MAX_REGISTER_CLASSES 	100


namespace zetscript{


	/**
	 * Register C variable
	 */
	template<typename V>
	 void  ScriptClassFactory::registerNativeGlobalVariable(
			 const std::string & var_name
			 ,V var_ptr
			 , const char *registered_file
			 ,short registered_line)
	{
		//Scope *scope;
		std::string var_type = typeid(V).name(); // we need the pointer type ...
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
			const char * function_name
			,F function_ptr
			, const char *registered_file
			,short registered_line)
	{
		int idx_return_type=-1;
		std::string return_type;
		std::vector<std::string> str_arg;

		std::vector<ScriptFunctionArg> arg_info;
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
			StackElement default_value=k_stk_undefined;

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

			arg_info.push_back(ScriptFunctionArg(idx_type,str_arg[i]));
		}

		ref_ptr=(zs_int)function_ptr;

		// Init struct...
		main_function->registerLocalFunction(
				 MAIN_SCOPE(this)
				,registered_file
				,registered_line
				,function_name
				,arg_info
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
			const std::string & class_name
			, const char *registered_file
			,short registered_line
	){//, const std::string & base_class_name=""){

		ScriptClass *irc=NULL;
		std::string str_class_name_ptr = typeid( T *).name();
		int size=script_classes->count;
		int idx_class=ZS_IDX_UNDEFINED;
		Scope * scope = NULL;
		Symbol *symbol=NULL;

		checkClassName(class_name);

		if(getIdxNativeRegisteredClass(str_class_name_ptr)!=ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("Native class '%s' is already registered",zs_rtti::demangle(typeid( T).name()).c_str());
		}

		idx_class=script_classes->count;
		scope = scope_factory->newScope(ZS_IDX_UNDEFINED,NULL,true);
		symbol=scope->registerSymbol(registered_file,registered_line,class_name, NO_PARAMS_SYMBOL_ONLY);

		irc = new ScriptClass(zs,idx_class,symbol);
		scope->setScriptClass(irc);

		//irc->symbol_class=*symbol;

		// in C there's no script constructor ...
		irc->idx_function_member_constructor=ZS_IDX_UNDEFINED;
		// allow dynamic constructor in function its parameters ...

		irc->str_class_ptr_type=str_class_name_ptr;
		irc->symbol_class.properties|=SYMBOL_PROPERTY_C_OBJECT_REF;

		irc->c_constructor = NULL;
		irc->c_destructor = NULL;
		script_classes->push_back((zs_int)irc);

		irc->idx_class=script_classes->count-1;
		ZS_LOG_DEBUG("* C++ class \"%s\" registered as (%s).",class_name.c_str(),zs_rtti::demangle(str_class_name_ptr).c_str());

		return irc;
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<typename C>
	void ScriptClassFactory::registerNativeClass(
			const std::string & class_name
			, const char *registered_file
			,short registered_line
	){//, const std::string & base_class_name=""){

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
			const std::string & class_name
			, C * (*_constructor)()
			, void (*_destructor)(C *)
			, const char *registered_file
			,short registered_line
	){//, const std::string & base_class_name=""){

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
			const std::string & class_name
			, C * (*_constructor)()
			, void (*_destructor)(C *)
			, const char *registered_file
			,short registered_line
	){//, const std::string & base_class_name=""){

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
		std::string base_class_name=typeid(B).name();
		std::string base_class_name_ptr=typeid(B *).name();
		std::string class_name=typeid(C).name();
		std::string class_name_ptr=typeid(C *).name();
		std::string error;

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
		conversion_types[this_class->idx_class][idx_base_class]=[](zs_int entry){ return (zs_int)(B *)((C *)entry);};


		//if(register_c_base_symbols){ // by default is disabled. User has to re-register! --> increases time and binary!!!
			//----------------------------
			//
			// DERIVATE STATE
			//
			// disabled for polymorphic classes because its function pointer std::map change at runtime)
			// https://stackoverflow.com/questions/48572734/is-possible-do-a-later-function-binding-knowing-its-function-type-and-later-the
			//

			ScriptClass *base_class = (ScriptClass *)script_classes->get(idx_base_class);

			unsigned short derivated_symbol_info_properties=SYMBOL_PROPERTY_C_OBJECT_REF;//| SYMBOL_PROPERTY_IS_DERIVATED;
			/*if(std::is_polymorphic<B>::value==true){

				//if((calling_function->symbol.properties & SYMBOL_PROPERTY_IS_POLYMORPHIC)){ // cannot call...
				THROW_RUNTIME_ERROR("class \"%s\" is polymorphic and cannot be processed due function/variable pointer it changes at runtime and could crash your application. You have two options:\n"
						"1. Set registerNativeBaseSymbols(false) and  re-register the function using REGISTER_NATIVE_FUNCTION_MEMBER\n"
						"2. Rewrite all virtual functions/classes to no non-virtual and do virtual operation through C function pointers\n"
						,(class_name+"::").c_str()
				);
			}*/

			//derivated_symbol_info_properties|=SYMBOL_PROPERTY_IS_POLYMORPHIC; // set as polymorphic and show error if you try to call a polymorphic function
			// register all c vars symbols ...
			for(unsigned i = 0; i < base_class->symbol_members->count; i++){

				Symbol *symbol_src = (Symbol *)base_class->symbol_members->items[i];

				if((symbol_src->properties & SYMBOL_PROPERTY_FUNCTION)!=0 ){ // is function

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
						std::vector<ScriptFunctionArg> params;
						for(unsigned j=0; j < script_function->params->count;j++){
							params.push_back(*((ScriptFunctionArg *) script_function->params->items[j]));
						}

						Symbol *symbol_result = this_class->registerNativeMemberFunction(
							script_function->symbol.name,
							params,
							script_function->idx_return_type,
							script_function->ref_native_function_ptr, // it contains script function pointer
							script_function->symbol.properties, //derivated_symbol_info_properties
							script_function->symbol.file,
							script_function->symbol.line
						);
					}

				}else if(symbol_src->properties & SYMBOL_PROPERTY_MEMBER_ATTRIBUTE){

					MemberAttribute *ma_src=(MemberAttribute *)symbol_src->ref_ptr;
					MemberAttribute *ma_dst=NULL;
					ScriptFunction *sf_getter=ma_src->getter;
					zs_vector *sf_setters=&ma_src->setters;
					Symbol *symbol_attribute=NULL;
					Symbol *symbol_function=NULL;

					symbol_attribute=this_class->registerMemberAttribute(symbol_src->name,symbol_src->file,symbol_src->line);
					ma_dst=(MemberAttribute *)symbol_attribute->ref_ptr;


					// register getter and setter
					if(sf_getter != NULL){

						std::vector<ScriptFunctionArg> arg_info;

						for(int i=0; i < sf_getter->params->count; i++){
							arg_info.push_back(*((ScriptFunctionArg *)sf_getter->params->items[i]));
						}

						/*this_class->registerNativeMemberAttributeGetter(
								symbol_src->name
								,arg_info
								,sf_getter->idx_return_type
								,sf_getter->ref_native_function_ptr
								,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_MEMBER_FUNCTION
								,symbol_src->file
								,symbol_src->line
						);*/
						symbol_function=this_class->registerNativeMemberFunction(
								sf_getter->symbol.name,
								arg_info,
								sf_getter->idx_return_type,
								sf_getter->ref_native_function_ptr,
								sf_getter->symbol.properties,
								sf_getter->symbol.file,
								sf_getter->symbol.line
						);

						ma_dst->getter=(ScriptFunction *)symbol_function->ref_ptr;

					}

					for(unsigned i=0; i < sf_setters->count; i++){
						std::vector<ScriptFunctionArg> arg_info;
						ScriptFunction *sf_setter=(ScriptFunction *)sf_setters->items[i];


						for(int i=0; i < sf_setter->params->count; i++){
							arg_info.push_back(*((ScriptFunctionArg *)sf_setter->params->items[i]));
						}


						/*this_class->registerNativeMemberAttributeSetter(
								symbol_src->name
								,arg_info
								,sf_setter->ref_native_function_ptr
								,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_MEMBER_FUNCTION
								,symbol_src->file
								,symbol_src->line
						);*/

						symbol_function=this_class->registerNativeMemberFunction(
								sf_setter->symbol.name,
								arg_info,
								sf_setter->idx_return_type,
								sf_setter->ref_native_function_ptr,
								sf_setter->symbol.properties,
								sf_setter->symbol.file,
								sf_setter->symbol.line
						);

						ma_dst->addSetter((ScriptFunction *)symbol_function->ref_ptr);
					}
				}

					/*else{ // register built-in variable member

					Symbol *symbol_result = this_class->registerNativeMemberVariable(
							error
							,symbol_src->file
							,symbol_src->line
							,symbol_src->name
							,symbol_src->str_native_type
							,symbol_src->ref_ptr // it has a pointer to function that returns the right offset according initialized object
							,symbol_src->properties
							//, //derivated_symbol_info_properties
					);

					if(symbol_result == NULL){
						THROW_RUNTIME_ERROR(error.c_str());
					}

				}
			}*/

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

		std::string str_class_name_ptr = typeid( C *).name();
		ScriptClass * c_class=	getScriptClassByNativeClassPtr(str_class_name_ptr);
		if(c_class == NULL){
			THROW_RUNTIME_ERROR("class \"%s\" is not registered",str_class_name_ptr.c_str());
		}
		return registerNativeMemberFunction<C>(c_class->symbol_class.name.c_str(),function_type, registered_file,registered_line );
	}

	/**
	 * Register C Member var
	 */
	//<o, decltype(o::s)>(STR(s),ZetScript::offset_of(&o::s)) &CVar::mierda
	template <typename C, typename R>
	void ScriptClassFactory::registerNativeStaticConstMember(
			const char *var_name
			, const R var_pointer
			, const char *registered_file
			,short registered_line) //unsigned int offset)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string var_type = typeid(R).name(); // we need the pointer type ...
		std::string return_type;
		std::string error;
		//std::vector<std::string> params;
		std::string str_class_name_ptr = typeid( C *).name();
		Symbol *symbol;

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		// 1. check all parameters ok.
		// check valid parameters ...
		if(getIdxClassFromItsNativeType(var_type) == ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("%s::%s has not valid type (%s)"
					,c_class->symbol_class.name.c_str()
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
				const char *attr_name
				,F ptr_function_setter
				, const char *registered_file
				,short registered_line
		){
			std::string str_class_name_ptr = typeid( C *).name();

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
				const char *attr_name
				,F ptr_function_getter
				, const char *registered_file
				,short registered_line
		){
			std::string str_class_name_ptr = typeid( C *).name();

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
				const char *attr_name
				,F ptr_function_post_increment
				, const char *registered_file
				,short registered_line
		){
			std::string str_class_name_ptr = typeid( C *).name();

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
				const char *attr_name
				,F ptr_function_post_decrement
				, const char *registered_file
				,short registered_line
		){
			std::string str_class_name_ptr = typeid( C *).name();

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
				const char *attr_name
				,F ptr_function_pre_increment
				, const char *registered_file
				,short registered_line
		){
			std::string str_class_name_ptr = typeid( C *).name();

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
				const char *attr_name
				,F ptr_function_pre_decrement
				, const char *registered_file
				,short registered_line
		){
			std::string str_class_name_ptr = typeid( C *).name();

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
			const char *function_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string str_class_name_ptr = typeid( C *).name();


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
			const char *function_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
	){
		std::string str_class_name_ptr = typeid( C *).name();

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
