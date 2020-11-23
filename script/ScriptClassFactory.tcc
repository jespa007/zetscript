#define 				MAX_REGISTER_CLASSES 	100
#define 				ZS_INVALID_CLASS		((unsigned char)ZS_IDX_UNDEFINED)

namespace zetscript{

	/**
	 * Register C variable
	 */
	template<typename V>
	 void  ScriptClassFactory::registerNativeGlobalVariable(
			 const std::string & var_name
			 ,V var_ptr
			 , const char *registered_file
			 ,int registered_line)
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

		if(getIdxClassFromItsNativeType(var_type) == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR("%s has not valid type (%s)",var_name.c_str(),var_type.c_str());
		}

		if((irs = main_function->registerLocalVariable(
				MAIN_SCOPE(this)
				,registered_file
				,registered_line
				,var_name
				,var_type
				,(zs_int)var_ptr
				,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC)) != NULL
		){
			ZS_PRINT_DEBUG("Registered variable name: %s",var_name.c_str());
		}
	}

	/**
	 * Register C function
	 */
	template <typename F>
	void ScriptClassFactory::registerNativeGlobalFunction(const char * function_name,F function_ptr, const char *registered_file,int registered_line)
	{
		int idx_return_type=-1;
		std::string return_type;
		std::vector<std::string> arg;
		std::vector<FunctionParam> arg_info;
		zs_int ref_ptr=0;

		if(main_function == NULL){
			THROW_RUNTIME_ERROR("main function is not created");
		}

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});

		if(arg.size()>MAX_NATIVE_FUNCTION_ARGS){
			THROW_RUNTIME_ERROR("Max argyments reached");
		}

		// check valid parameters ...
		if((idx_return_type=getIdxClassFromItsNativeType(return_type))==-1){
			THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered"
					,zs_rtti::demangle(return_type).c_str()
					,function_name);
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromItsNativeType(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)"
						,i
						,zs_rtti::demangle(arg[i]).c_str()
						,function_name
						,zs_rtti::demangle(arg[i]).c_str());
			}

			if(idx_type ==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered"
						,i
						,zs_rtti::demangle(arg[i]).c_str()
						,function_name);
			}

			arg_info.push_back({idx_type,arg[i]});
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
				,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC
			);

		ZS_PRINT_DEBUG("Registered function name: %s",function_name);
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<class T>
	ScriptClass * ScriptClassFactory::registerNativeSingletonClass(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		ScriptClass *irc=NULL;
		std::string str_class_name_ptr = typeid( T *).name();
		int size=script_classes->count;
		ClassTypeIdx idx_class=ZS_IDX_UNDEFINED;
		Scope * scope = NULL;
		Symbol *symbol=NULL;

		if(size>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
		}

		// after MAX_BASIC_CLASS_TYPES all registered C classes should follow a registered C class ...

		if(isClassRegistered(class_name)){
			THROW_RUNTIME_ERROR("%s already exist", class_name.c_str());
		}


		if(getIdx_C_RegisteredClass(str_class_name_ptr)!=ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR("this %s is already registered",zs_rtti::demangle(typeid( T).name()).c_str());
		}

		idx_class=(ClassTypeIdx)(script_classes->count);
		scope = scope_factory->newScope(NULL,true);
		symbol=scope->registerSymbol(registered_file,registered_line,class_name, NO_PARAMS_SYMBOL_ONLY);

		irc = new ScriptClass(zs,idx_class);
		scope->setScriptClass(irc);

		irc->symbol_class=*symbol;

		// in C there's no script constructor ...
		irc->idx_function_member_constructor=-1;
		// allow dynamic constructor in function its parameters ...

		irc->str_class_ptr_type=str_class_name_ptr;
		irc->symbol_class.properties|=SYMBOL_PROPERTY_C_OBJECT_REF;

		irc->c_constructor = NULL;
		irc->c_destructor = NULL;
		script_classes->push_back((zs_int)irc);

		irc->idx_class=(ClassTypeIdx)(script_classes->count-1);
		ZS_PRINT_DEBUG("* C++ class \"%s\" registered as (%s).",class_name.c_str(),zs_rtti::demangle(str_class_name_ptr).c_str());

		return irc;
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<typename T>
	void ScriptClassFactory::registerNativeClass(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		ScriptClass *irc =registerNativeSingletonClass<T>(class_name);
		// get class...


		if(irc->idx_class < IDX_BUILTIN_TYPE_MAX && irc->idx_class != IDX_BUILTIN_TYPE_STACK_ELEMENT){
			THROW_RUNTIME_ERROR("The class to register \"%s\"  should not built in class",irc->str_class_ptr_type.c_str());
		}


		//put the constructor/destructor...
		irc->c_constructor = new std::function<void *()>([=](){
			T *t=new T();
			return t;
		});

		irc->c_destructor = new std::function<void (void *)>([=](void *p){
			delete (T *)p;
		});
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<typename T>
	void ScriptClassFactory::registerNativeClassBuiltIn(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		ScriptClass *irc =registerNativeSingletonClass<T>(class_name);

		if(irc->idx_class >= IDX_BUILTIN_TYPE_MAX){
			THROW_RUNTIME_ERROR("The class to register \"%s\" should be a built in class",irc->str_class_ptr_type.c_str());
		}

		//put the constructor/destructor...
		ZetScript *_zs=zs;
		irc->c_constructor = new std::function<void *()>([_zs](){
			T* t=new T(_zs);
			return t;
		});

		irc->c_destructor = new std::function<void (void *)>([=](void *p){
			delete (T *)p;
		});
	}

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


		if(register_c_base_symbols){ // by default is disabled. User has to re-register! --> increases time and binary!!!
			//----------------------------
			//
			// DERIVATE STATE
			//
			// disabled for polymorphic classes because its function pointer std::map change at runtime)
			// https://stackoverflow.com/questions/48572734/is-possible-do-a-later-function-binding-knowing-its-function-type-and-later-the
			//

			ScriptClass *base_class = (ScriptClass *)script_classes->get(idx_base_class);

			unsigned short derivated_symbol_info_properties=SYMBOL_PROPERTY_C_OBJECT_REF;//| SYMBOL_PROPERTY_IS_DERIVATED;
			if(std::is_polymorphic<B>::value==true){

				//if((calling_function->symbol.properties & SYMBOL_PROPERTY_IS_POLYMORPHIC)){ // cannot call...
				THROW_RUNTIME_ERROR("class \"%s\" is polymorphic and cannot be processed due function/variable pointer it changes at runtime and could crash your application. You have two options:\n"
						"1. Set registerNativeBaseSymbols(false) and  re-register the function using REGISTER_NATIVE_FUNCTION_MEMBER\n"
						"2. Rewrite all virtual functions/classes to no non-virtual and do virtual operation through C function pointers\n"
						,(class_name+"::").c_str()
				);
			}

			//derivated_symbol_info_properties|=SYMBOL_PROPERTY_IS_POLYMORPHIC; // set as polymorphic and show error if you try to call a polymorphic function
			// register all c vars symbols ...
			for(unsigned i = 0; i < base_class->symbol_members->count; i++){

				Symbol *symbol_src = (Symbol *)base_class->symbol_members->items[i];

				if(symbol_src->properties & SYMBOL_PROPERTY_FUNCTION){ // is function
					ScriptFunction *script_function = (ScriptFunction *)symbol_src->ref_ptr;
					// build params...
					std::vector<FunctionParam> params;
					for(unsigned j=0; j < script_function->params->count;j++){
						params.push_back(*((FunctionParam *) script_function->params->items[j]));
					}

					Symbol *symbol_result = this_class->registerNativeMemberFunction(
						error,
						script_function->symbol.file,
						script_function->symbol.line,
						script_function->symbol.name,
						params,
						script_function->idx_return_type,
						script_function->symbol.ref_ptr, // it contains script function pointer
						script_function->symbol.properties //derivated_symbol_info_properties
					);

					if(symbol_result == NULL){
						THROW_RUNTIME_ERROR(error.c_str());
					}

				}else{ // register built-in variable member

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
			}

		}

		//
		// DERIVATE STATE
		//
		//----------------------------
	}


	/**
	 * Register C Member var
	 */
	//<o, decltype(o::s)>(STR(s),ZetScript::offset_of(&o::s)) &CVar::mierda
	template <typename C, typename R,typename T>
	void ScriptClassFactory::registerNativeMemberVariable(const char *var_name, R T::*var_pointer, const char *registered_file,int registered_line) //unsigned int offset)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string var_type = typeid(R *).name(); // we need the pointer type ...
		std::string error="";
		std::string return_type;
		//std::vector<std::string> params;
		std::string str_class_name_ptr = typeid( C *).name();
		zs_int ref_ptr=offsetOf<C>(var_pointer);
		Symbol *symbol;

		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		// 1. check all parameters ok.
		// check valid parameters ...
		if(getIdxClassFromItsNativeType(var_type) == -1){
			THROW_RUNTIME_ERROR("%s::%s has not valid type (%s)"
					,c_class->symbol_class.name.c_str()
					,var_name
					,zs_rtti::demangle(typeid(R).name()).c_str());
		}

		// register variable...
		symbol=c_class->registerNativeMemberVariable(
				error
				,registered_file
				,registered_line
				,var_name
				,var_type
				,ref_ptr
				,SYMBOL_PROPERTY_C_OBJECT_REF

		);

		if(symbol == NULL){
			THROW_RUNTIME_ERROR(error.c_str());
		}
	}

	/**
	 * Register C Member var
	 */
	//<o, decltype(o::s)>(STR(s),ZetScript::offset_of(&o::s)) &CVar::mierda
	template <typename C, typename R>
	void ScriptClassFactory::registerNativeStaticConstMember(const char *var_name, const R var_pointer, const char *registered_file,int registered_line) //unsigned int offset)
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
		if(getIdxClassFromItsNativeType(var_type) == -1){
			THROW_RUNTIME_ERROR("%s::%s has not valid type (%s)"
					,c_class->symbol_class.name.c_str()
					,var_name
					,zs_rtti::demangle(typeid(R).name()).c_str());
		}



		// register variable...
		symbol=c_class->registerNativeMemberVariable(
				error
				,registered_file
				,registered_line
				,var_name
				,var_type
				,(zs_int)var_pointer
				,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC | SYMBOL_PROPERTY_CONST
		);

		if(symbol == NULL){
			THROW_RUNTIME_ERROR(error.c_str());
		}
	}

	/**
	 * Register C Member function Class
	 */
	template < typename C, typename R, class T, typename..._A>
	void ScriptClassFactory::registerNativeMemberFunction(
			const char *function_name
			,R (T:: *function_type)(_A...)
			, const char *registered_file
			,int registered_line
	)
	{
		// to make compatible MSVC shared library
		std::string return_type;
		std::vector<std::string> arg;
		std::vector<FunctionParam> arg_info;
		std::string error;
		int idx_return_type=-1;
		zs_int ref_ptr=0;
		std::string str_class_name_ptr = typeid( C *).name();

		ScriptClass * sc=getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(sc == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_type)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});


		// check valid parameters ...
		if((idx_return_type=getIdxClassFromItsNativeType(return_type)) == -1){
			THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",zs_rtti::demangle(return_type).c_str(),function_name);
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type=getIdxClassFromItsNativeType(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,zs_rtti::demangle(arg[i]).c_str(),function_name,zs_rtti::demangle(arg[i]).c_str());
			}

			if(idx_type==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered",i,zs_rtti::demangle(arg[i]).c_str(),function_name);
			}
			arg_info.push_back({idx_type,arg[i]});
		}

		ref_ptr=((zs_int)function_proxy_factory->newProxyMemberFunction<C>(arg.size(),function_type));

		// register member function...
		Symbol *symbol = sc->registerNativeMemberFunction(
				error
				,registered_file
				,registered_line
				,function_name
				,arg_info
				,idx_return_type
				,ref_ptr
				,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_SET_FIRST_PARAMETER_AS_THIS
		);

		if(symbol == NULL){
			THROW_RUNTIME_ERROR(error.c_str());
		}

		ZS_PRINT_DEBUG("Registered member function name %s::%s",zs_rtti::demangle(typeid(C).name()).c_str(), function_name);

		/*if(ZS_STRCMP(ByteCodeMetamethodToSymbolStr(BYTE_CODE_METAMETHOD_SET),==,function_name)){
			if(sc->metamethod_operator[BYTE_CODE_METAMETHOD_SET]==NULL){
				sc->metamethod_operator[BYTE_CODE_METAMETHOD_SET]=new zs_vector();
			}

			StackElement *stk_element = (StackElement *)malloc(sizeof(StackElement));
			*stk_element = {0,(ScriptFunction *)symbol->ref_ptr,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION};

			sc->metamethod_operator[BYTE_CODE_METAMETHOD_SET]->push_back((zs_int)stk_element);
			ZS_PRINT_DEBUG("Registered metamethod %s::%s",zs_rtti::demangle(typeid(C).name()).c_str(), function_name);
		}*/
	}


	/**
	 * Register C Member function Class
	 * like register function c but is added to member function list according type C
	 */
	template <typename C, typename F>
	void ScriptClassFactory::registerNativeMemberFunctionStatic(const char *function_name,F function_ptr, const char *registered_file,int registered_line)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string return_type;
		std::string error;
		std::vector<std::string> params;
		std::vector<std::string> arg;
		std::vector<FunctionParam> arg_info;
		int idx_return_type=-1;
		zs_int ref_ptr=0;
		std::string str_class_name_ptr = typeid( C *).name();
		std::string function_class_name = zs_rtti::demangle(typeid(C).name())+"::"+function_name;


		ScriptClass *c_class = getScriptClassByNativeClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			THROW_RUNTIME_ERROR("native class %s not registered",str_class_name_ptr.c_str());
		}

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});

		if(arg.size()>MAX_NATIVE_FUNCTION_ARGS){
			THROW_RUNTIME_ERROR("Max argyments reached");
		}

		// check valid parameters ...
		if((idx_return_type=getIdxClassFromItsNativeType(return_type)) == -1){
			THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",zs_rtti::demangle(return_type).c_str(),function_name);
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromItsNativeType(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,zs_rtti::demangle(arg[i]).c_str(),function_name,zs_rtti::demangle(arg[i]).c_str());
			}

			if(idx_type==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered",i,zs_rtti::demangle(arg[i]).c_str(),function_name);
			}

			arg_info.push_back({idx_type,arg[i]});
		}

		ref_ptr=(zs_int)function_ptr;

		// register member function...
		Symbol * symbol_sf = c_class->registerNativeMemberFunction(
				error
				,registered_file
				,registered_line
				,function_name
				,arg_info
				, idx_return_type
				, ref_ptr
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC
		);

		if(symbol_sf == NULL){
			THROW_RUNTIME_ERROR(error.c_str());
		}

		ZS_PRINT_DEBUG("Registered member function name %s::%s",zs_rtti::demangle(typeid(C).name()).c_str(), function_name);

		// check whether is static metamethod...
		if(ZS_STRCMP(ByteCodeMetamethodToSymbolStr(BYTE_CODE_METAMETHOD_SET),!=,function_name)){

			for(int i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				if(ZS_STRCMP(ByteCodeMetamethodToSymbolStr((ByteCodeMetamethod)i),==,function_name)){

					// check if they are gte,gt,equ, not_equ, lt, lte
					if(  i == BYTE_CODE_METAMETHOD_EQU //STRCMP(function_name, == ,"_equ")
					  || i == BYTE_CODE_METAMETHOD_NOT_EQU //STRCMP(function_name, ==, "_nequ")
					  || i == BYTE_CODE_METAMETHOD_LT//STRCMP(function_name, ==, "_lt")
					  || i == BYTE_CODE_METAMETHOD_LTE//STRCMP(function_name, ==, "_lte")
					  || i == BYTE_CODE_METAMETHOD_GT//STRCMP(function_name, ==, "_gt")
					  || i == BYTE_CODE_METAMETHOD_GTE//STRCMP(function_name, ==, "_gte")
					  || i == BYTE_CODE_METAMETHOD_NOT//STRCMP(function_name, ==, "_gte")
					  ){
						// return type must be bool...
						if(ZS_STRCMP(return_type.c_str(), != ,typeid(bool).name())){
							THROW_RUNTIME_ERROR("error registering metamethod %s::%s. Expected return bool but it was %s",
									zs_rtti::demangle(typeid(C).name()).c_str(),
									function_name,
									zs_rtti::demangle(return_type.c_str()).c_str());
						}
					}else if((return_type != str_class_name_ptr) && (i!= BYTE_CODE_METAMETHOD_SET)){

						THROW_RUNTIME_ERROR("error registering metamethod %s::%s. Expected return %s but it was %s",
								zs_rtti::demangle(typeid(C).name()).c_str(),
								function_name,
								zs_rtti::demangle(str_class_name_ptr.c_str()).c_str(),
								zs_rtti::demangle(return_type.c_str()).c_str());
					}

					/*if(c_class->metamethod_operator[i]==NULL){
						c_class->metamethod_operator[i]=new zs_vector();
					}

					StackElement *stk_element = (StackElement *)malloc(sizeof(StackElement));
					*stk_element = {0,(void *)symbol_sf->ref_ptr,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION};

					c_class->metamethod_operator[i]->push_back((zs_int)stk_element);

					ZS_PRINT_DEBUG("Registered metamethod %s::%s",zs_rtti::demangle(typeid(C).name()).c_str(), function_name);*/
					break;
				}
			}
		}else{
			THROW_RUNTIME_ERROR("error! cannot register metamethod set on static function. Must be member function");
		}
	}

	/*
	 * register static function registerNativeMemberFunction as function member
	 * Is automatically added in function member list according first parameter type of function_type
	 */
	template <typename C,typename F>
	void ScriptClassFactory::registerNativeMemberFunction(
			const char *function_name
			,F function_type
			, const char *registered_file
			,int registered_line
	){
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string return_type;
		std::vector<std::string> params;
		std::vector<std::string> arg;
		std::vector<FunctionParam> arg_info;
		int idx_return_type=-1;
		zs_int ref_ptr=0;
		std::string str_class_name_ptr = typeid( C *).name();
		std::string function_class_name;// = zs_rtti::demangle(typeid(T).name())+"::"+function_name;
		std::string error;
		Symbol *symbol;

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_type)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});

		if(arg.size()>MAX_NATIVE_FUNCTION_ARGS){
			THROW_RUNTIME_ERROR("Max argyments reached");
		}

		if(arg.size()==0){
			THROW_RUNTIME_ERROR("registerNativeMemberFunction at least need first parameter that defines the object to add function %s",function_name);
		}

		ScriptClass * c_class=	getScriptClassByNativeClassPtr(arg[0]);
		if(c_class == NULL){
			THROW_RUNTIME_ERROR("class %s is not registered",arg[0].c_str());
		}

		if(c_class->str_class_ptr_type !=  str_class_name_ptr){
			THROW_RUNTIME_ERROR("first parameter is \"%s\" and should be the same type as the class (i.e \"%s\")",arg[0].c_str(),str_class_name_ptr.c_str());
		}


		function_class_name = c_class->symbol_class.name+"::"+function_name;

		// check valid parameters ...
		if((idx_return_type=getIdxClassFromItsNativeType(return_type)) == -1){
			THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",zs_rtti::demangle(return_type).c_str(),function_name);
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromItsNativeType(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,zs_rtti::demangle(arg[i]).c_str(),function_name,zs_rtti::demangle(arg[i]).c_str());
			}

			if(idx_type==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered",i,zs_rtti::demangle(arg[i]).c_str(),function_name);
			}

			arg_info.push_back({idx_type,arg[i]});
		}

		ref_ptr=(zs_int)function_type;

		// register member function...
		symbol = c_class->registerNativeMemberFunction(
				error
				,registered_file
				, registered_line
				, function_name
				, arg_info
				, idx_return_type
				, ref_ptr
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC | SYMBOL_PROPERTY_SET_FIRST_PARAMETER_AS_THIS
		);


		if(symbol == NULL){
			THROW_RUNTIME_ERROR(error.c_str());
		}

		ZS_PRINT_DEBUG("Registered C function %s as function member %s::%s",function_name, function_class_name.c_str(),function_name);
	}
}
