#define 				MAX_REGISTER_CLASSES 	100
#define 				ZS_INVALID_CLASS		((unsigned char)ZS_UNDEFINED_IDX)

namespace zetscript{

	/**
	 * Register C function
	 */
	template <typename F>
	bool CScriptClassFactory::register_C_Function(const char * function_name,F function_ptr, const char *registered_file,int registered_line)
	{
		int idx_return_type=-1;
		std::string return_type;
		std::vector<std::string> arg;
		std::vector<ParamArgInfo> arg_info;
		intptr_t ref_ptr=0;
		CScriptFunction *irs=NULL;

		if(!script_function_factory->checkCanRegister_C_Function(function_name)){
			return false;
		}

		if(main_function == NULL){
			THROW_RUNTIME_ERROR("main function is not created");
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_ptr)>;
		get_params_function<Traits3>(0,return_type, arg, make_index_sequence<Traits3::arity>{});


		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type))==-1){
			THROW_RUNTIME_ERROR(string::sformat("Return type \"%s\" for function \"%s\" not registered"
					,rtti::demangle(return_type).c_str()
					,function_name));
			return false;
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromIts_C_Type(arg[i]);

			if(idx_type==IDX_TYPE_FLOAT_C || idx_type==IDX_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR(string::sformat("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)"
						,i
						,rtti::demangle(arg[i]).c_str()
						,function_name
						,rtti::demangle(arg[i]).c_str()));
				return false;
			}

			if(idx_type ==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR(string::sformat("Argument (%i) type \"%s\" for function \"%s\" not registered"
						,i
						,rtti::demangle(arg[i]).c_str()
						,function_name));
				return false;
			}

			arg_info.push_back({idx_type,arg[i]});
		}

		if(idx_return_type == IDX_TYPE_VOID_C){
			if((ref_ptr=(intptr_t)native_function_factory->new_proxy_function<void>(arg.size(),function_ptr))==0){//(int)function_ptr;
				return false;
			}
		}
		else{
			if((ref_ptr=(intptr_t)native_function_factory->new_proxy_function<intptr_t>(arg.size(),function_ptr))==0){//(int)function_ptr;
				return false;
			}
		}


		// init struct...
		main_function->registerFunction(registered_file,registered_line,function_name,arg_info,idx_return_type,ref_ptr,PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF);

		zs_print_debug_cr("Registered function name: %s",function_name);
		return true;
	}


	/**
	 * Register C Class. Return index registered class
	 */
	template<class T>
	 bool CScriptClassFactory::register_C_SingletonClass(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		//std::vector<CScriptClass *> vec_script_class_node=getVectorScriptClassNode();
		// to make compatible MSVC shared library
		int size=vec_script_class_node.size();

		if(size>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR(string::sformat("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES));
			return NULL;
		}

		// after MAX_BASIC_CLASS_TYPES all registered C classes should follow a registered C class ...
		if(size > 1){ // because = 0 is reserved for main class and >= 1 is for C registered classes
			if((

				((vec_script_class_node[size-1]->symbol_info.properties&PROPERTY_C_OBJECT_REF)!=PROPERTY_C_OBJECT_REF)
			)){
				THROW_RUNTIME_ERROR(string::sformat("C class \"%s\" should register after C classes. Register C classes after script classes are not allowed",class_name.c_str()));
				return NULL;
			}
		}



		if(!isClassRegistered(class_name)){

			std::string str_classPtr = typeid( T *).name();

			if(getIdx_C_RegisteredClass(str_classPtr)!=ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR(string::sformat("this %s is already registered",rtti::demangle(typeid( T).name()).c_str()));
				return NULL;
			}

			unsigned char idx_class=(short)(vec_script_class_node.size());
			CScope * scope = scope_factory->newScope(ZS_UNDEFINED_IDX,true);
			Symbol *symbol=scope->registerSymbol(registered_file,registered_line,class_name, NO_PARAMS_IS_CLASS);
			if(symbol == NULL){
				return NULL;
			}

			CScriptClass *irc = new CScriptClass(zs,idx_class);
			scope->setScriptClass(irc);

			irc->symbol_info.symbol=symbol;

			// in C there's no script constructor ...
			irc->idx_function_member_constructor=-1;
			// allow dynamic constructor in function its parameters ...

			irc->str_class_ptr_type=str_classPtr;
			irc->symbol_info.properties=PROPERTY_C_OBJECT_REF;


			irc->c_constructor = NULL;
			irc->c_destructor = NULL;
			vec_script_class_node.push_back(irc);

			irc->idx_class=(unsigned char)(vec_script_class_node.size()-1);
			zs_print_debug_cr("* C++ class \"%s\" registered as (%s).",class_name.c_str(),rtti::demangle(str_classPtr).c_str());

			return true;
		}
		else{
			THROW_RUNTIME_ERROR(string::sformat("%s already exist", class_name.c_str()));
		}

		return false;
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<typename T>
	bool CScriptClassFactory::register_C_Class(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		if(register_C_SingletonClass<T>(class_name)){

			// get class...
			CScriptClass *irc =getScriptClass(class_name);

			if(irc->idx_class < MAX_BUILT_IN_TYPES){
				THROW_RUNTIME_ERROR(string::sformat("The class to register \"%s\"  should NOT BE a built in class",irc->str_class_ptr_type.c_str()));
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
	bool CScriptClassFactory::register_C_ClassBuiltIn(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		if(register_C_SingletonClass<T>(class_name)){
			CScriptClass *irc =getScriptClass(class_name);

			if(irc->idx_class >= MAX_BUILT_IN_TYPES){
				THROW_RUNTIME_ERROR(string::sformat("The class to register \"%s\" should be a built in class",irc->str_class_ptr_type.c_str()));
				return false;
			}

			// get class...


			if(irc==NULL){
				return false;
			}

			//put the constructor/destructor...
			CZetScript *_zs=zs;
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
	bool CScriptClassFactory::class_C_BaseOf(){

		// to make compatible MSVC shared library
		//std::vector<CScriptClass *> * vec_script_class_node = getVecScriptClassNode();


		std::string base_class_name=typeid(B).name();
		std::string base_class_name_ptr=typeid(B *).name();
		std::string class_name=typeid(T).name();
		std::string class_name_ptr=typeid(T *).name();

		int idx_base_class = getIdxClassFromIts_C_Type(base_class_name_ptr);
		if(idx_base_class == -1) {
			THROW_RUNTIME_ERROR(string::sformat("base class %s not registered",base_class_name_ptr.c_str()));
			return false;
		}


		int register_class = getIdxClassFromIts_C_Type(class_name_ptr);
		if(register_class == -1) {
			THROW_RUNTIME_ERROR(string::sformat("class %s not registered",class_name_ptr.c_str()));
			return false;
		}

		if(isIdxClassInstanceOf(register_class,idx_base_class)){
			THROW_RUNTIME_ERROR(string::sformat("C++ class \"%s\" is already registered as base of \"%s\" ",rtti::demangle(class_name).c_str(), rtti::demangle(base_class_name).c_str()));
			return false;
		}

		// check whether is in fact base of ...
		if(!std::is_base_of<B,T>::value){
			THROW_RUNTIME_ERROR(string::sformat("C++ class \"%s\" is not base of \"%s\" ",rtti::demangle(class_name).c_str(), rtti::demangle(base_class_name).c_str()));
			return false;
		}

		CScriptClass *sc=vec_script_class_node[register_class];
		while( sc->idx_base_class.size()>0){

			sc=getScriptClass(sc->idx_base_class[0]); // get base class...
			if(sc->str_class_ptr_type ==base_class_name_ptr){
				THROW_RUNTIME_ERROR(string::sformat("C++ class \"%s\" already base of \"%s\" ",rtti::demangle(class_name).c_str(), rtti::demangle(base_class_name).c_str()));
				return false;
			}
		}


		CScriptClass *irc_class = vec_script_class_node[register_class];
		irc_class->idx_base_class.push_back(idx_base_class);

		//std::map<int, std::map<int, fntConversionType>>  *	local_map_type_conversion=	getMapTypeConversion();
		(mapTypeConversion)[irc_class->idx_class][idx_base_class]=[](intptr_t entry){ return (intptr_t)(B *)((T *)entry);};


		if(register_c_base_symbols){
			//----------------------------
			//
			// DERIVATE STATE
			//
			// disabled for polymorphic classes because its function pointer std::map change at runtime)
			// https://stackoverflow.com/questions/48572734/is-possible-do-a-later-function-binding-knowing-its-function-type-and-later-the
			//

			CScriptClass *irc_base = vec_script_class_node[idx_base_class];

			unsigned short derivated_properties=PROPERTY_C_OBJECT_REF| PROPERTY_IS_DERIVATED;
			if(std::is_polymorphic<B>::value==true){
				derivated_properties|=PROPERTY_IS_POLYMORPHIC;
			}

			// register all symbols function from base ...
			// vars ...
			for(unsigned i = 0; i < irc_base->local_variable.size(); i++){

				VariableSymbolInfo *irs_source = &irc_base->local_variable[i];

				VariableSymbolInfo irs;
				// init struct...
				//irs.idx_class = idx_base_class;
				irs.ref_ptr=irs_source->ref_ptr;
				irs.c_type = irs_source->c_type;
				//irs.
				irs.symbol=irs_source->symbol;
				irs.properties = derivated_properties;
				irs.idxSymbol = (short)(irc_class->local_variable.size());
				irc_class->local_variable.push_back(irs);

			}

			// functions ...
			for(unsigned i = 0; i < irc_base->local_function.size(); i++){

				CScriptFunction *irs_source = irc_base->local_function[i];
				irc_class->registerFunction(
						irs_source->symbol_info.symbol->file,
						irs_source->symbol_info.symbol->line,
						irs_source->symbol_info.symbol->name,
						irs_source->arg_info,
						irs_source->idx_return_type,
						irs_source->symbol_info.ref_ptr,
						derivated_properties
						);
			}
		}

		//
		// DERIVATE STATE
		//
		//----------------------------

		// finally maps object type ...
		return true;
	}


	/**
	 * Register C Member function Class
	 */
	template < typename C, typename R, class T, typename..._A>
	bool CScriptClassFactory::register_C_FunctionMember(
			const char *function_name
			,R (T:: *function_type)(_A...)
			, const char *registered_file
			,int registered_line
	)
	{

		// to make compatible MSVC shared library
		//std::vector<CScriptClass *> * 	vec_script_class_node = getVecScriptClassNode();

		std::string return_type;
		//std::vector<std::string> params;
		CScriptFunction *irs=NULL;
		std::vector<std::string> arg;
		std::vector<ParamArgInfo> arg_info;
		int idx_return_type=-1;
		intptr_t ref_ptr=0;
		std::string str_classPtr = typeid( C *).name();

		if(!script_function_factory->checkCanRegister_C_Function(function_name)){
			return false;
		}

		CScriptClass * sc=getScriptClass(str_classPtr);


		if(sc == NULL){
			return false;
		}

		//idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_type)>;
		get_params_function<Traits3>(0,return_type, arg, make_index_sequence<Traits3::arity>{});


		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
			THROW_RUNTIME_ERROR(string::sformat("Return type \"%s\" for function \"%s\" not registered",rtti::demangle(return_type).c_str(),function_name));
			return false;
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type=getIdxClassFromIts_C_Type(arg[i]);

			if(idx_type==IDX_TYPE_FLOAT_C || idx_type==IDX_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR(string::sformat("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,rtti::demangle(arg[i]).c_str(),function_name,rtti::demangle(arg[i]).c_str()));
				return false;
			}

			if(idx_type==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR(string::sformat("Argument (%i) type \"%s\" for function \"%s\" not registered",i,rtti::demangle(arg[i]).c_str(),function_name));
				return false;
			}

			arg_info.push_back({idx_type,arg[i]});

		}

		// ignores special type cast C++ member to ptr function
		// create binding function class
		if(idx_return_type == IDX_TYPE_VOID_C){
			if((ref_ptr=((intptr_t)native_function_factory->c_member_class_function_proxy<C,T, void>(arg.size(),function_type)))==0){
				return false;
			}
		}else{
			if((ref_ptr=((intptr_t)native_function_factory->c_member_class_function_proxy<C,T, R>(arg.size(),function_type)))==0){
				return false;
			}
		}

		//CScriptClass *sc = vec_script_class_node[idxRegisterdClass];

		// register member function...
		CScriptFunction *sf = sc->registerFunction(registered_file,registered_line,function_name,arg_info,idx_return_type,ref_ptr,PROPERTY_C_OBJECT_REF);

		zs_print_debug_cr("Registered member function name %s::%s",rtti::demangle(typeid(C).name()).c_str(), function_name);

		if(STRCMP(OpCodeInstruction::metamethodOpCodeToStr(SET_METAMETHOD),==,function_name)){
			sc->metamethod_operator[SET_METAMETHOD].push_back(sf);
			zs_print_debug_cr("Registered metamethod %s::%s",rtti::demangle(typeid(C).name()).c_str(), function_name);
		}

		return true;
	}


	/**
	 * Register C Member function Class
	 */
	template <typename T, typename F>
	bool CScriptClassFactory::register_C_StaticFunctionMember(const char *function_name,F function_type, const char *registered_file,int registered_line)
	{
		// to make compatible MSVC shared library
		//std::vector<CScriptClass *> * vec_script_class_node = getVecScriptClassNode();

		std::string return_type;
		std::vector<std::string> params;
		CScriptFunction *irs=NULL;
		std::vector<std::string> arg;
		std::vector<ParamArgInfo> arg_info;
		int idx_return_type=-1;
		intptr_t ref_ptr=0;
		std::string str_classPtr = typeid( T *).name();
		std::string function_class_name = rtti::demangle(typeid(T).name())+"::"+function_name;



		if(!script_function_factory->checkCanRegister_C_Function(function_class_name.c_str())){
			return false;
		}

		CScriptClass *c_class = getScriptClassBy_C_ClassPtr(str_classPtr);

		if(c_class == NULL){
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_type)>;
		get_params_function<Traits3>(0,return_type, arg, make_index_sequence<Traits3::arity>{});


		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
			THROW_RUNTIME_ERROR(string::sformat("Return type \"%s\" for function \"%s\" not registered",rtti::demangle(return_type).c_str(),function_name));
			return false;
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromIts_C_Type(arg[i]);

			if(idx_type==IDX_TYPE_FLOAT_C || idx_type==IDX_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR(string::sformat("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,rtti::demangle(arg[i]).c_str(),function_name,rtti::demangle(arg[i]).c_str()));
				return false;
			}

			if(idx_type==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR(string::sformat("Argument (%i) type \"%s\" for function \"%s\" not registered",i,rtti::demangle(arg[i]).c_str(),function_name));
				return false;
			}

			arg_info.push_back({idx_type,arg[i]});

		}

		// ignores special type cast C++ member to ptr function
		// create binding function class
		if(idx_return_type == IDX_TYPE_VOID_C){
			if((ref_ptr=(intptr_t)native_function_factory->new_proxy_function<void>(arg.size(),function_type))==0){//(int)function_ptr;
				return false;
			}
		}
		else{
			if((ref_ptr=(intptr_t)native_function_factory->new_proxy_function<int>(arg.size(),function_type))==0){//(int)function_ptr;
				return false;
			}
		}

		// register member function...
		CScriptFunction * sf = c_class->registerFunction(registered_file,registered_line,function_name,arg_info, idx_return_type, ref_ptr, PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF);
		zs_print_debug_cr("Registered member function name %s::%s",rtti::demangle(typeid(T).name()).c_str(), function_name);

		// check whether is static metamethod...
		if(STRCMP(OpCodeInstruction::metamethodOpCodeToStr(SET_METAMETHOD),!=,function_name)){

			for(int i = 0; i < MAX_METAMETHOD_OPERATORS; i++){
				if(STRCMP(OpCodeInstruction::metamethodOpCodeToStr((METAMETHOD_OP_CODE)i),==,function_name)){

					// check if they are gte,gt,equ, not_equ, lt, lte
					if(  i == EQU_METAMETHOD //STRCMP(function_name, == ,"_equ")
					  || i == NOT_EQU_METAMETHOD //STRCMP(function_name, ==, "_nequ")
					  || i == LT_METAMETHOD//STRCMP(function_name, ==, "_lt")
					  || i == LTE_METAMETHOD//STRCMP(function_name, ==, "_lte")
					  || i == GT_METAMETHOD//STRCMP(function_name, ==, "_gt")
					  || i == GTE_METAMETHOD//STRCMP(function_name, ==, "_gte")
					  || i == NOT_METAMETHOD//STRCMP(function_name, ==, "_gte")
					  ){
						// return type must be bool...
						if(STRCMP(return_type.c_str(), != ,typeid(bool).name())){
							THROW_RUNTIME_ERROR(string::sformat("error registering metamethod %s::%s. Expected return bool but it was %s",
									rtti::demangle(typeid(T).name()).c_str(),
									function_name,
									rtti::demangle(return_type.c_str()).c_str()));
							return false;

						}
					}else if((return_type != str_classPtr) && (i!= SET_METAMETHOD)){

						THROW_RUNTIME_ERROR(string::sformat("error registering metamethod %s::%s. Expected return %s but it was %s",
								rtti::demangle(typeid(T).name()).c_str(),
								function_name,
								rtti::demangle(str_classPtr.c_str()).c_str(),
								rtti::demangle(return_type.c_str()).c_str()));
						return false;
					}

					c_class->metamethod_operator[i].push_back(sf);

					zs_print_debug_cr("Registered metamethod %s::%s",rtti::demangle(typeid(T).name()).c_str(), function_name);
					break;
				}
			}
		}else{
			THROW_RUNTIME_ERROR(string::sformat("error! cannot register metamethod set on static function. Must be member function"));
			return false;
		}

		return true;
	}

	/**
	 * Register C Member var
	 */
	//<o, decltype(o::s)>(STR(s),zetscript::offset_of(&o::s)) &CVar::mierda
	template <typename C, typename R,typename T>
	bool CScriptClassFactory::register_C_VariableMember(const char *var_name, R T::*var_pointer, const char *registered_file,int registered_line) //unsigned int offset)
	{
		// to make compatible MSVC shared library
		//std::vector<CScriptClass *> * vec_script_class_node = getVecScriptClassNode();

		std::string var_type = typeid(R *).name(); // we need the pointer type ...
		std::string return_type;
		//std::vector<std::string> params;
		std::string str_classPtr = typeid( C *).name();
		unsigned int ref_ptr=offset_of<C>(var_pointer);

		CScriptClass *c_class = getScriptClassBy_C_ClassPtr(str_classPtr);

		if(c_class == NULL){
			return false;
		}

		// 1. check all parameters ok.

		// check valid parameters ...
		if(getIdxClassFromIts_C_Type(var_type) == -1){
			THROW_RUNTIME_ERROR(string::sformat("%s::%s has not valid type (%s)"
					,c_class->symbol_info.symbol->name.c_str()
					,var_name
					,rtti::demangle(typeid(R).name()).c_str()));
			return false;
		}

		// register variable...
		c_class->registerVariable(registered_file,registered_line,var_name,var_type,ref_ptr,PROPERTY_C_OBJECT_REF);
		return true;

	}

}
