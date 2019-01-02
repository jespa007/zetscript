namespace zetscript{

		/**
		 * Register C function
		 */
		template <typename F>
		bool CScriptClassFactory::register_C_FunctionInt(const char * function_name,F function_ptr)
		{
			int idx_return_type=-1;
			string return_type;
			vector<string> m_arg;
			vector<tArgumentInfo> m_infoArg;
			intptr_t ref_ptr=0;
			CScriptFunction *irs=NULL;

			if(!CScriptFunctionFactory::getInstance()->checkCanRegister_C_Function(function_name)){
				return false;
			}


			//CScriptVariable *sf=NULL;
			CScriptFunction * mainFunctionInfo = MAIN_FUNCTION;

			if(mainFunctionInfo == NULL){
				THROW_RUNTIME_ERROR("main function is not created");
				return false;
			}

			// 1. check all parameters ok.
			using Traits3 = function_traits<decltype(function_ptr)>;
			getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});


			// check valid parameters ...
			if((idx_return_type=GET_IDX_CLASS_FROM_ITS_C_TYPE(return_type))==-1){
				THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name);
				return false;
			}

			for(unsigned int i = 0; i < m_arg.size(); i++){
				int idx_type = GET_IDX_CLASS_FROM_ITS_C_TYPE(m_arg[i]);

				if(idx_type==IDX_TYPE_FLOAT_C || idx_type==IDX_TYPE_BOOL_C){
					THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,demangle(m_arg[i]).c_str(),function_name,demangle(m_arg[i]).c_str());
					return false;
				}

				if(idx_type ==-1){
					THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(m_arg[i]).c_str(),function_name);
					return false;
				}

				m_infoArg.push_back({idx_type,m_arg[i]});
			}

			if(idx_return_type == IDX_TYPE_VOID_C){
				if((ref_ptr=(intptr_t)CNativeFunction::getInstance()->new_proxy_function<void>(m_arg.size(),function_ptr))==0){//(int)function_ptr;
					return false;
				}
			}
			else{
				if((ref_ptr=(intptr_t)CNativeFunction::getInstance()->new_proxy_function<intptr_t>(m_arg.size(),function_ptr))==0){//(int)function_ptr;
					return false;
				}
			}


			// init struct...
			mainFunctionInfo->registerFunction(__registered_file__,__registered_line__,function_name,m_infoArg,idx_return_type,ref_ptr,PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF);

			zs_print_debug_cr("Registered function name: %s",function_name);
			return true;
		}


		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		ZETSCRIPT_MODULE_EXPORT CScriptClass * CScriptClassFactory::register_C_SingletonClassInt(const string & class_name){//, const string & base_class_name=""){

			// to make compatible MSVC shared library
			//vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();
			int size=vec_script_class_node.size();

			if(size>=MAX_REGISTER_CLASSES){
				THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
				return NULL;
			}

			// after MAX_BASIC_CLASS_TYPES all registered C classes should follow a registered C class ...
			if(size > 1){ // because = 0 is reserved for main class and >= 1 is for C registered classes
				if((

					((vec_script_class_node[size-1]->symbol_info.properties&PROPERTY_C_OBJECT_REF)!=PROPERTY_C_OBJECT_REF)
				)){
					THROW_RUNTIME_ERROR("C class \"%s\" should register after C classes. Register C classes after script classes are not allowed",class_name.c_str());
					return NULL;
				}
			}



			if(!isClassRegistered(class_name)){

				string str_classPtr = typeid( _T *).name();

				if(getIdx_C_RegisteredClass(str_classPtr)!=ZS_INVALID_CLASS){
					THROW_RUNTIME_ERROR("this %s is already registered",demangle(typeid( _T).name()).c_str());
					return NULL;
				}

				unsigned char idxClass=(short)(vec_script_class_node.size());
				CScope * scope = NEW_SCOPE(ZS_UNDEFINED_IDX,true);
				tSymbol *symbol=scope->registerSymbol(__registered_file__,__registered_line__,class_name, NO_PARAMS_IS_CLASS);
				CScriptClass *irc = new CScriptClass(idxClass,scope->idxScope);
				scope->setScriptClass(irc);

				irc->symbol_info.symbol=symbol;

				// in C there's no script constructor ...
				irc->idx_function_member_constructor=-1;
				// allow dynamic constructor in function its parameters ...

				irc->classPtrType=str_classPtr;
				irc->symbol_info.properties=PROPERTY_C_OBJECT_REF;


				irc->c_constructor = NULL;
				irc->c_destructor = NULL;
				vec_script_class_node.push_back(irc);

				irc->idxClass=(unsigned char)(vec_script_class_node.size()-1);
				zs_print_debug_cr("* C++ class \"%s\" registered as (%s).",class_name.c_str(),demangle(str_classPtr).c_str());

				return irc;
			}
			else{
				THROW_RUNTIME_ERROR("%s already exist", class_name.c_str());
			}

			return NULL;
		}

		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		CScriptClass * CScriptClassFactory::register_C_ClassInt(const string & class_name){//, const string & base_class_name=""){
			CScriptClass *irc = NULL;
			if((irc = register_C_SingletonClassInt<_T>(class_name))!=NULL){

				//put the constructor/destructor...
				irc->c_constructor = new std::function<void *()>([=](){
					_T* t=new _T;
					return t;
				});

				irc->c_destructor = new std::function<void (void *)>([=](void *p){
					delete (_T *)p;
				});

				return irc;
			}

			return NULL;
		}


		template<class _T, class _B>
		bool CScriptClassFactory::class_C_baseofInt(){

			// to make compatible MSVC shared library
			//vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();


			string base_class_name=typeid(_B).name();
			string base_class_name_ptr=typeid(_B *).name();
			string class_name=typeid(_T).name();
			string class_name_ptr=typeid(_T *).name();

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

			if(isIdxClassInstanceOf(register_class,idx_base_class)){
				THROW_RUNTIME_ERROR("C++ class \"%s\" is already registered as base of \"%s\" ",demangle(class_name).c_str(), demangle(base_class_name).c_str());
				return false;
			}

			// check whether is in fact base of ...
			if(!std::is_base_of<_B,_T>::value){
				THROW_RUNTIME_ERROR("C++ class \"%s\" is not base of \"%s\" ",demangle(class_name).c_str(), demangle(base_class_name).c_str());
				return false;
			}

			CScriptClass *sc=vec_script_class_node[register_class];
			while( sc->idxBaseClass.size()>0){

				sc=CScriptClassFactory::getScriptClass(sc->idxBaseClass[0]); // get base class...
				if(sc->classPtrType ==base_class_name_ptr){
					THROW_RUNTIME_ERROR("C++ class \"%s\" already base of \"%s\" ",demangle(class_name).c_str(), demangle(base_class_name).c_str());
					return false;
				}
			}


			CScriptClass *irc_class = vec_script_class_node[register_class];
			irc_class->idxBaseClass.push_back(idx_base_class);

			//map<int, map<int, fntConversionType>>  *	local_map_type_conversion=	getMapTypeConversion();
			(mapTypeConversion)[irc_class->idxClass][idx_base_class]=[](intptr_t entry){ return (intptr_t)(_B *)((_T *)entry);};


			if(register_c_base_symbols){
				//----------------------------
				//
				// DERIVATE STATE
				//
				// disabled for polymorphic classes because its function pointer map change at runtime)
				// https://stackoverflow.com/questions/48572734/is-possible-do-a-later-function-binding-knowing-its-function-type-and-later-the
				//

				CScriptClass *irc_base = vec_script_class_node[idx_base_class];

				unsigned short derivated_properties=PROPERTY_C_OBJECT_REF| PROPERTY_IS_DERIVATED;
				if(std::is_polymorphic<_B>::value==true){
					derivated_properties|=PROPERTY_IS_POLYMORPHIC;
				}

				// register all symbols function from base ...
				// vars ...
				for(unsigned i = 0; i < irc_base->m_variable.size(); i++){

					tVariableSymbolInfo *irs_source = &irc_base->m_variable[i];

					tVariableSymbolInfo irs;
					// init struct...
					//irs.idxClass = idx_base_class;
					irs.ref_ptr=irs_source->ref_ptr;
					irs.c_type = irs_source->c_type;
					//irs.
					irs.symbol=irs_source->symbol;
					irs.properties = derivated_properties;
					irs.idxSymbol = (short)(irc_class->m_variable.size());
					irc_class->m_variable.push_back(irs);

				}

				// functions ...
				for(unsigned i = 0; i < irc_base->m_function.size(); i++){

					CScriptFunction *irs_source = irc_base->m_function[i];
					irc_class->registerFunction(
							irs_source->symbol_info.symbol->file,
							irs_source->symbol_info.symbol->line,
							irs_source->symbol_info.symbol->name,
							irs_source->m_arg,
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
		template < typename _C, typename _R, class _T, typename..._A>
		bool CScriptClassFactory::register_C_FunctionMemberInt(const char *function_name,_R (_T:: *function_type)(_A...) )
		{

			// to make compatible MSVC shared library
			//vector<CScriptClass *> * 	local_vec_script_class_node = getVecScriptClassNode();

			string return_type;
			//vector<string> params;
			CScriptFunction *irs=NULL;
			vector<string> m_arg;
			vector<tArgumentInfo> m_argInfo;
			int idx_return_type=-1;
			intptr_t ref_ptr=0;
			string str_classPtr = typeid( _C *).name();

			if(!CScriptFunctionFactory::getInstance()->checkCanRegister_C_Function(function_name)){
				return false;
			}

			CScriptClass * sc=GET_SCRIPT_CLASS(str_classPtr);


			if(sc == NULL){
				return false;
			}

			//idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

			// 1. check all parameters ok.
			using Traits3 = function_traits<decltype(function_type)>;
			getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});


			// check valid parameters ...
			if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
				THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name);
				return false;
			}

			for(unsigned int i = 0; i < m_arg.size(); i++){
				int idx_type=GET_IDX_CLASS_FROM_ITS_C_TYPE(m_arg[i]);

				if(idx_type==IDX_TYPE_FLOAT_C || idx_type==IDX_TYPE_BOOL_C){
					THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,demangle(m_arg[i]).c_str(),function_name,demangle(m_arg[i]).c_str());
					return false;
				}

				if(idx_type==-1){
					THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(m_arg[i]).c_str(),function_name);
					return false;
				}

				m_argInfo.push_back({idx_type,m_arg[i]});

			}

			// ignores special type cast C++ member to ptr function
			// create binding function class
			if(idx_return_type == IDX_TYPE_VOID_C){
				if((ref_ptr=((intptr_t)CNativeFunction::getInstance()->c_member_class_function_proxy<_C,_T, void>(m_arg.size(),function_type)))==0){
					return false;
				}
			}else{
				if((ref_ptr=((intptr_t)CNativeFunction::getInstance()->c_member_class_function_proxy<_C,_T, _R>(m_arg.size(),function_type)))==0){
					return false;
				}
			}

			//CScriptClass *sc = vec_script_class_node[idxRegisterdClass];

			// register member function...
			CScriptFunction *sf = sc->registerFunction(CScriptClassFactory::__registered_file__,CScriptClassFactory::__registered_line__,function_name,m_argInfo,idx_return_type,ref_ptr,PROPERTY_C_OBJECT_REF);

			zs_print_debug_cr("Registered member function name %s::%s",demangle(typeid(_C).name()).c_str(), function_name);

			if(STRCMP(getMetamethod(SET_METAMETHOD),==,function_name)){
				sc->metamethod_operator[SET_METAMETHOD].push_back(sf);
				zs_print_debug_cr("Registered metamethod %s::%s",demangle(typeid(_C).name()).c_str(), function_name);
			}

			return true;
		}


		/**
		 * Register C Member function Class
		 */
		template <typename _T, typename _F>
		bool CScriptClassFactory::register_C_StaticFunctionMemberInt(const char *function_name,_F function_type)
		{
			// to make compatible MSVC shared library
			//vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();

			string return_type;
			vector<string> params;
			CScriptFunction *irs=NULL;
			vector<string> m_arg;
			vector<tArgumentInfo> m_argInfo;
			int idx_return_type=-1;
			intptr_t ref_ptr=0;
			string str_classPtr = typeid( _T *).name();
			string function_class_name = demangle(typeid(_T).name())+"::"+function_name;



			if(!CScriptFunctionFactory::getInstance()->checkCanRegister_C_Function(function_class_name.c_str())){
				return false;
			}

			CScriptClass *c_class = getScriptClassBy_C_ClassPtr(str_classPtr);

			if(c_class == NULL){
				return false;
			}

			// 1. check all parameters ok.
			using Traits3 = function_traits<decltype(function_type)>;
			getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});


			// check valid parameters ...
			if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
				THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name);
				return false;
			}

			for(unsigned int i = 0; i < m_arg.size(); i++){
				int idx_type = getIdxClassFromIts_C_Type(m_arg[i]);

				if(idx_type==IDX_TYPE_FLOAT_C || idx_type==IDX_TYPE_BOOL_C){
					THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,demangle(m_arg[i]).c_str(),function_name,demangle(m_arg[i]).c_str());
					return false;
				}

				if(idx_type==-1){
					THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(m_arg[i]).c_str(),function_name);
					return false;
				}

				m_argInfo.push_back({idx_type,m_arg[i]});

			}

			// ignores special type cast C++ member to ptr function
			// create binding function class
			if(idx_return_type == IDX_TYPE_VOID_C){
				if((ref_ptr=(intptr_t)CNativeFunction::getInstance()->new_proxy_function<void>(m_arg.size(),function_type))==0){//(int)function_ptr;
					return false;
				}
			}
			else{
				if((ref_ptr=(intptr_t)CNativeFunction::getInstance()->new_proxy_function<int>(m_arg.size(),function_type))==0){//(int)function_ptr;
					return false;
				}
			}

			// register member function...
			CScriptFunction * sf = c_class->registerFunction(__registered_file__,__registered_line__,function_name,m_argInfo, idx_return_type, ref_ptr, PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF);
			zs_print_debug_cr("Registered member function name %s::%s",demangle(typeid(_T).name()).c_str(), function_name);

			// check whether is static metamethod...
			if(STRCMP(getMetamethod(SET_METAMETHOD),!=,function_name)){

				for(int i = 0; i < MAX_METAMETHOD_OPERATORS; i++){
					if(STRCMP(getMetamethod((METAMETHOD_OPERATOR)i),==,function_name)){

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
								THROW_RUNTIME_ERROR("error registering metamethod %s::%s. Expected return bool but it was %s",
										demangle(typeid(_T).name()).c_str(),
										function_name,
										demangle(return_type.c_str()).c_str());
								return false;

							}
						}else if((return_type != str_classPtr) && (i!= SET_METAMETHOD)){

							THROW_RUNTIME_ERROR("error registering metamethod %s::%s. Expected return %s but it was %s",
									demangle(typeid(_T).name()).c_str(),
									function_name,
									demangle(str_classPtr.c_str()).c_str(),
									demangle(return_type.c_str()).c_str());
							return false;
						}

						c_class->metamethod_operator[i].push_back(sf);

						zs_print_debug_cr("Registered metamethod %s::%s",demangle(typeid(_T).name()).c_str(), function_name);
						break;
					}
				}
			}else{
				THROW_RUNTIME_ERROR("error! cannot register metamethod set on static function. Must be member function");
				return false;
			}

			return true;
		}

		/**
		 * Register C Member var
		 */
		//<o, decltype(o::s)>(STR(s),zetscript::offset_of(&o::s)) &CVar::mierda
		template <typename _C, typename _R,typename _T>
		bool CScriptClassFactory::register_C_VariableMemberInt(const char *var_name, _R _T::*var_pointer) //unsigned int offset)
		{
			// to make compatible MSVC shared library
			//vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();

			string var_type = typeid(_R *).name(); // we need the pointer type ...
			string return_type;
			//vector<string> params;
			string str_classPtr = typeid( _C *).name();
			unsigned int ref_ptr=zetscript::offset_of<_C>(var_pointer);

			CScriptClass *c_class = getScriptClassBy_C_ClassPtr(str_classPtr);

			if(c_class == NULL){
				return false;
			}

			// 1. check all parameters ok.

			// check valid parameters ...
			if(getIdxClassFromIts_C_Type(var_type) == -1){
				THROW_RUNTIME_ERROR("%s::%s has not valid type (%s)"
						,c_class->symbol_info.symbol->name.c_str()
						,var_name
						,demangle(typeid(_R).name()).c_str());
				return false;
			}

			// register variable...
			c_class->registerVariable(__registered_file__,__registered_line__,var_name,var_type,ref_ptr,PROPERTY_C_OBJECT_REF);
			return true;

		}

}
