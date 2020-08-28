#define 				MAX_REGISTER_CLASSES 	100
#define 				ZS_INVALID_CLASS		((unsigned char)ZS_IDX_UNDEFINED)

namespace zetscript{

	/**
	 * Register C function
	 */
	template <typename F>
	bool ScriptClassFactory::registerNativeFunction(const char * function_name,F function_ptr, const char *registered_file,int registered_line)
	{
		int idx_return_type=-1;
		std::string return_type;
		std::vector<std::string> arg;
		std::vector<FunctionParam> arg_info;
		intptr_t ref_ptr=0;

		if(!script_function_factory->checkCanregisterNativeFunction(function_name)){
			return false;
		}

		if(main_function == NULL){
			THROW_RUNTIME_ERROR("main function is not created");
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});

		if(arg.size()>6){
			THROW_RUNTIME_ERROR("Max argyments reached");
		}

		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type))==-1){
			THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered"
					,zs_rtti::demangle(return_type).c_str()
					,function_name);
			return false;
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromIts_C_Type(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)"
						,i
						,zs_rtti::demangle(arg[i]).c_str()
						,function_name
						,zs_rtti::demangle(arg[i]).c_str());
				return false;
			}

			if(idx_type ==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered"
						,i
						,zs_rtti::demangle(arg[i]).c_str()
						,function_name);
				return false;
			}

			arg_info.push_back({idx_type,arg[i]});
		}

		ref_ptr=(intptr_t)function_ptr;

		// Init struct...
		main_function->addFunction(
				 MAIN_SCOPE(this)
				,registered_file
				,registered_line
				,function_name
				,arg_info
				,idx_return_type
				,ref_ptr
				,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC_REF
			);

		ZS_PRINT_DEBUG("Registered function name: %s",function_name);
		return true;
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<class T>
	 bool ScriptClassFactory::registerNativeSingletonClass(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		//std::vector<ScriptClass *> script_classes=getScriptClasses();
		// to make compatible MSVC shared library
		int size=script_classes.size();

		if(size>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
			return NULL;
		}

		// after MAX_BASIC_CLASS_TYPES all registered C classes should follow a registered C class ...
		if(size > 1){ // because = 0 is reserved for main class and >= 1 is for C registered classes
			if((
				((script_classes[size-1]->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF )!=SYMBOL_PROPERTY_C_OBJECT_REF)
			)){
				THROW_RUNTIME_ERROR("C class \"%s\" should register after C classes. Register C classes after script classes are not allowed",class_name.c_str());
				return NULL;
			}
		}

		if(!isClassRegistered(class_name)){

			std::string str_class_name_ptr = typeid( T *).name();

			if(getIdx_C_RegisteredClass(str_class_name_ptr)!=ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR("this %s is already registered",zs_rtti::demangle(typeid( T).name()).c_str());
				return NULL;
			}

			unsigned char idx_class=(short)(script_classes.size());
			Scope * scope = scope_factory->newScope(NULL,true);
			Symbol *symbol=scope->registerSymbol(registered_file,registered_line,class_name, NO_PARAMS_IS_CLASS);
			if(symbol == NULL){
				return NULL;
			}

			ScriptClass *irc = new ScriptClass(zs,idx_class);
			scope->setScriptClass(irc);

			irc->symbol=*symbol;

			// in C there's no script constructor ...
			irc->idx_function_member_constructor=-1;
			// allow dynamic constructor in function its parameters ...

			irc->str_class_ptr_type=str_class_name_ptr;
			irc->symbol.symbol_properties|=SYMBOL_PROPERTY_C_OBJECT_REF;

			irc->c_constructor = NULL;
			irc->c_destructor = NULL;
			script_classes.push_back(irc);

			irc->idx_class=(unsigned char)(script_classes.size()-1);
			ZS_PRINT_DEBUG("* C++ class \"%s\" registered as (%s).",class_name.c_str(),zs_rtti::demangle(str_class_name_ptr).c_str());

			return true;
		}
		else{
			THROW_RUNTIME_ERROR("%s already exist", class_name.c_str());
		}
		return false;
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<typename T>
	bool ScriptClassFactory::registerNativeClass(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		if(registerNativeSingletonClass<T>(class_name)){
			// get class...
			ScriptClass *irc =getScriptClass(class_name);

			if(irc->idx_class < IDX_BUILTIN_TYPE_MAX && irc->idx_class != IDX_BUILTIN_TYPE_STACK_ELEMENT){
				THROW_RUNTIME_ERROR("The class to register \"%s\"  should BYTE_CODE_NOT BE a built in class",irc->str_class_ptr_type.c_str());
				return false;
			}

			if(irc==NULL){
				return false;
			}

			//put the constructor/destructor...
			irc->c_constructor = new std::function<void *()>([=](){
				T *t=new T();
				return t;
			});

			irc->c_destructor = new std::function<void (void *)>([=](void *p){
				delete (T *)p;
			});

			return true;
		}
		return NULL;
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<typename T>
	bool ScriptClassFactory::registerNativeClassBuiltIn(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		if(registerNativeSingletonClass<T>(class_name)){
			ScriptClass *irc =getScriptClass(class_name);

			if(irc->idx_class >= IDX_BUILTIN_TYPE_MAX){
				THROW_RUNTIME_ERROR("The class to register \"%s\" should be a built in class",irc->str_class_ptr_type.c_str());
				return false;
			}

			// get class...
			if(irc==NULL){
				return false;
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

			return true;
		}

		return NULL;
	}


	template<class T, class B>
	bool ScriptClassFactory::class_C_BaseOf(){
		std::string base_class_name=typeid(B).name();
		std::string base_class_name_ptr=typeid(B *).name();
		std::string class_name=typeid(T).name();
		std::string class_name_ptr=typeid(T *).name();

		int idx_base_class = getIdxClassFromIts_C_Type(base_class_name_ptr);
		if(idx_base_class == -1) {
			THROW_RUNTIME_ERROR("base class %s not registered",base_class_name_ptr.c_str());
			return false;
		}


		int register_class = getIdxClassFromIts_C_Type(class_name_ptr);
		if(register_class == -1) {
			THROW_RUNTIME_ERROR("class %s not registered",class_name_ptr.c_str());
			return false;
		}

		if(class_C_BaseOf(register_class,idx_base_class)){
			THROW_RUNTIME_ERROR("C++ class \"%s\" is already registered as base of \"%s\" ",zs_rtti::demangle(class_name).c_str(), zs_rtti::demangle(base_class_name).c_str());
			return false;
		}

		// check whether is in fact base of ...
		if(!std::is_base_of<B,T>::value){
			THROW_RUNTIME_ERROR("C++ class \"%s\" is not base of \"%s\" ",zs_rtti::demangle(class_name).c_str(), zs_rtti::demangle(base_class_name).c_str());
			return false;
		}

		ScriptClass *sc=script_classes[register_class];
		while( sc->idx_base_classes->count>0){

			sc=getScriptClass(sc->idx_base_classes->items[0]); // get base class...
			if(sc->str_class_ptr_type ==base_class_name_ptr){
				THROW_RUNTIME_ERROR("C++ class \"%s\" already base of \"%s\" ",zs_rtti::demangle(class_name).c_str(), zs_rtti::demangle(base_class_name).c_str());
				return false;
			}
		}


		ScriptClass *this_class = script_classes[register_class];
		this_class->idx_base_classes->push_back(idx_base_class);

		// add conversion type for this class
		conversion_types[this_class->idx_class][idx_base_class]=[](intptr_t entry){ return (intptr_t)(B *)((T *)entry);};


		if(register_c_base_symbols){
			//----------------------------
			//
			// DERIVATE STATE
			//
			// disabled for polymorphic classes because its function pointer std::map change at runtime)
			// https://stackoverflow.com/questions/48572734/is-possible-do-a-later-function-binding-knowing-its-function-type-and-later-the
			//

			ScriptClass *base_class = script_classes[idx_base_class];

			unsigned short derivated_symbol_info_properties=SYMBOL_PROPERTY_C_OBJECT_REF| SYMBOL_PROPERTY_IS_DERIVATED;
			if(std::is_polymorphic<B>::value==true){
				derivated_symbol_info_properties|=SYMBOL_PROPERTY_IS_POLYMORPHIC;
			}

			// register all c vars symbols ...
			for(unsigned i = 0; i < base_class->symbol_native_variable_members->count; i++){

				Symbol *symbol_src = (Symbol *)base_class->symbol_native_variable_members->items[i];

				Symbol *symbol_dst=new Symbol();
				symbol_dst->ref_ptr=symbol_src->ref_ptr;
				symbol_dst->c_type = symbol_src->c_type;
				symbol_dst->scope=symbol_src->scope;
				symbol_dst->symbol_properties = derivated_symbol_info_properties;
				symbol_dst->idx_position = (short)(this_class->symbol_native_variable_members->count);
				this_class->symbol_native_variable_members->push_back((intptr_t)symbol_dst);
			}

			// register all functions ...
			for(unsigned i = 0; i < base_class->function_members->count; i++){
				ScriptFunction *script_function = (ScriptFunction *)base_class->function_members->items[i];
				// build params...
				std::vector<FunctionParam> function_params;
				for(unsigned j=0; j < script_function->function_params->count;j++){
					function_params.push_back(*((FunctionParam *) script_function->function_params->items[j]));
				}
				
				this_class->registerFunctionMember(
					script_function->symbol.file,
					script_function->symbol.line,
					script_function->symbol.name,
					function_params,
					script_function->idx_return_type,
					script_function->symbol.ref_ptr,
					derivated_symbol_info_properties
				);
			}
		}

		//
		// DERIVATE STATE
		//
		//----------------------------
		return true;
	}


	/**
	 * Register C Member function Class
	 */
	template < typename C, typename R, class T, typename..._A>
	bool ScriptClassFactory::registerNativeFunctionMember(
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
		int idx_return_type=-1;
		intptr_t ref_ptr=0;
		std::string str_class_name_ptr = typeid( C *).name();

		if(!script_function_factory->checkCanregisterNativeFunction(function_name)){
			return false;
		}

		ScriptClass * sc=getScriptClass(str_class_name_ptr);

		if(sc == NULL){
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_type)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});


		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
			THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",zs_rtti::demangle(return_type).c_str(),function_name);
			return false;
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type=getIdxClassFromIts_C_Type(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,zs_rtti::demangle(arg[i]).c_str(),function_name,zs_rtti::demangle(arg[i]).c_str());
				return false;
			}

			if(idx_type==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered",i,zs_rtti::demangle(arg[i]).c_str(),function_name);
				return false;
			}

			arg_info.push_back({idx_type,arg[i]});

		}

		if((ref_ptr=((intptr_t)proxy_function_factory->newProxyFunctionMember<C>(arg.size(),function_type)))==0){
			return false;
		}
		// register member function...
		ScriptFunction *sf = sc->registerFunctionMember(
				 registered_file
				,registered_line
				,function_name
				,arg_info
				,idx_return_type
				,ref_ptr
				,SYMBOL_PROPERTY_C_OBJECT_REF
		);

		ZS_PRINT_DEBUG("Registered member function name %s::%s",zs_rtti::demangle(typeid(C).name()).c_str(), function_name);

		if(ZS_STRCMP(ByteCodeMetamethodToStr(BYTE_CODE_METAMETHOD_SET),==,function_name)){
			sc->metamethod_operator[BYTE_CODE_METAMETHOD_SET]->push_back((intptr_t)sf);
			ZS_PRINT_DEBUG("Registered metamethod %s::%s",zs_rtti::demangle(typeid(C).name()).c_str(), function_name);
		}
		return true;
	}


	/**
	 * Register C Member function Class
	 */
	template <typename C, typename F>
	bool ScriptClassFactory::registerNativeFunctionMemberStatic(const char *function_name,F function_ptr, const char *registered_file,int registered_line)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string return_type;
		std::vector<std::string> params;
		std::vector<std::string> arg;
		std::vector<FunctionParam> arg_info;
		int idx_return_type=-1;
		intptr_t ref_ptr=0;
		std::string str_class_name_ptr = typeid( C *).name();
		std::string function_class_name = zs_rtti::demangle(typeid(C).name())+"::"+function_name;


		if(!script_function_factory->checkCanregisterNativeFunction(function_class_name)){
			return false;
		}

		ScriptClass *c_class = getScriptClassBy_C_ClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});

		if(arg.size()>6){
			THROW_RUNTIME_ERROR("Max argyments reached");
		}

		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
			THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",zs_rtti::demangle(return_type).c_str(),function_name);
			return false;
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromIts_C_Type(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,zs_rtti::demangle(arg[i]).c_str(),function_name,zs_rtti::demangle(arg[i]).c_str());
				return false;
			}

			if(idx_type==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered",i,zs_rtti::demangle(arg[i]).c_str(),function_name);
				return false;
			}

			arg_info.push_back({idx_type,arg[i]});
		}

		ref_ptr=(intptr_t)function_ptr;

		// register member function...
		ScriptFunction * sf = c_class->registerFunctionMember(
				 registered_file
				,registered_line
				,function_name
				,arg_info
				, idx_return_type
				, ref_ptr
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC_REF
		);
		ZS_PRINT_DEBUG("Registered member function name %s::%s",zs_rtti::demangle(typeid(T).name()).c_str(), function_name);

		// check whether is static metamethod...
		if(ZS_STRCMP(ByteCodeMetamethodToStr(BYTE_CODE_METAMETHOD_SET),!=,function_name)){

			for(int i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				if(ZS_STRCMP(ByteCodeMetamethodToStr((ByteCodeMetamethod)i),==,function_name)){

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
							return false;

						}
					}else if((return_type != str_class_name_ptr) && (i!= BYTE_CODE_METAMETHOD_SET)){

						THROW_RUNTIME_ERROR("error registering metamethod %s::%s. Expected return %s but it was %s",
								zs_rtti::demangle(typeid(C).name()).c_str(),
								function_name,
								zs_rtti::demangle(str_class_name_ptr.c_str()).c_str(),
								zs_rtti::demangle(return_type.c_str()).c_str());
						return false;
					}

					c_class->metamethod_operator[i]->push_back((intptr_t)sf);

					ZS_PRINT_DEBUG("Registered metamethod %s::%s",zs_rtti::demangle(typeid(T).name()).c_str(), function_name);
					break;
				}
			}
		}else{
			THROW_RUNTIME_ERROR("error! cannot register metamethod set on static function. Must be member function");
			return false;
		}
		return true;
	}

	template <typename F>
	bool ScriptClassFactory::registerNativeFunctionMember(
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
		intptr_t ref_ptr=0;
		std::string function_class_name;// = zs_rtti::demangle(typeid(T).name())+"::"+function_name;

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_type)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});

		if(arg.size()>6){
			THROW_RUNTIME_ERROR("Max argyments reached");
		}

		if(arg.size()==0){
			THROW_RUNTIME_ERROR("registerNativeFunctionMember at least need first parameter that defines the object to add function %s",function_name);
		}

		ScriptClass * c_class=	getScriptClassBy_C_ClassPtr(arg[0]);
		if(c_class == NULL){
			THROW_RUNTIME_ERROR("class %s is not registered",arg[0].c_str());
			return false;
		}

		function_class_name = c_class->symbol.name+"::"+function_name;

		if(!script_function_factory->checkCanregisterNativeFunction(function_class_name)){
			return false;
		}

		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
			THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",zs_rtti::demangle(return_type).c_str(),function_name);
			return false;
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromIts_C_Type(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,zs_rtti::demangle(arg[i]).c_str(),function_name,zs_rtti::demangle(arg[i]).c_str());
				return false;
			}

			if(idx_type==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered",i,zs_rtti::demangle(arg[i]).c_str(),function_name);
				return false;
			}

			arg_info.push_back({idx_type,arg[i]});
		}

		ref_ptr=(intptr_t)function_type;

		// register member function...
		c_class->registerFunctionMember(
				  registered_file
				, registered_line
				, function_name
				, arg_info
				, idx_return_type
				, ref_ptr
				, SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC_REF | SYMBOL_PROPERTY_SET_FIRST_PARAMETER_AS_THIS
		);
		ZS_PRINT_DEBUG("Registered C function %s as function member %s::%s",function_name, function_class_name.c_str());

		return true;
	}

	/**
	 * Register C Member var
	 */
	//<o, decltype(o::s)>(STR(s),ZetScript::offset_of(&o::s)) &CVar::mierda
	template <typename C, typename R,typename T>
	bool ScriptClassFactory::registerNativeVariableMember(const char *var_name, R T::*var_pointer, const char *registered_file,int registered_line) //unsigned int offset)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string var_type = typeid(R *).name(); // we need the pointer type ...
		std::string return_type;
		//std::vector<std::string> params;
		std::string str_class_name_ptr = typeid( C *).name();
		unsigned int ref_ptr=offsetOf<C>(var_pointer);

		ScriptClass *c_class = getScriptClassBy_C_ClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			return false;
		}

		// 1. check all parameters ok.
		// check valid parameters ...
		if(getIdxClassFromIts_C_Type(var_type) == -1){
			THROW_RUNTIME_ERROR("%s::%s has not valid type (%s)"
					,c_class->symbol.name.c_str()
					,var_name
					,zs_rtti::demangle(typeid(R).name()).c_str());
			return false;
		}

		// register variable...
		c_class->registerNativeSymbolVariableMember(
				 registered_file
				,registered_line
				,var_name
				,var_type
				,ref_ptr
				,SYMBOL_PROPERTY_C_OBJECT_REF
		);
		return true;

	}
	
	/**
	 * Register C Member var
	 */
	//<o, decltype(o::s)>(STR(s),ZetScript::offset_of(&o::s)) &CVar::mierda
	template <typename C, typename R>
	bool ScriptClassFactory::registerNativeStaticConstMember(const char *var_name, const R *var_pointer, const char *registered_file,int registered_line) //unsigned int offset)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string var_type = typeid(R *).name(); // we need the pointer type ...
		std::string return_type;
		//std::vector<std::string> params;
		std::string str_class_name_ptr = typeid( C *).name();


		ScriptClass *c_class = getScriptClassBy_C_ClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			return false;
		}

		// 1. check all parameters ok.
		// check valid parameters ...
		if(getIdxClassFromIts_C_Type(var_type) == -1){
			THROW_RUNTIME_ERROR("%s::%s has not valid type (%s)"
					,c_class->symbol.name.c_str()
					,var_name
					,zs_rtti::demangle(typeid(R).name()).c_str());
			return false;
		}

		
		
		// register variable...
		c_class->registerNativeSymbolVariableMember(
				 registered_file
				,registered_line
				,var_name
				,var_type
				,(intptr_t)var_pointer
				,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC_REF | SYMBOL_PROPERTY_CONST
		);
		return true;

	}	

}
