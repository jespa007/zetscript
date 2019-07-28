namespace zetscript{

		/**
		 * Register C function
		 */
		template <typename F>
		bool CZetScript::register_C_Function(const char * function_name,F function_ptr, const char *registered_file,int registered_line)
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
			if((idx_return_type=script_class_factory->getIdxClassFromIts_C_Type(return_type))==-1){
				THROW_RUNTIME_ERROR(string::sformat("Return type \"%s\" for function \"%s\" not registered"
						,rtti::demangle(return_type).c_str()
						,function_name));
				return false;
			}

			for(unsigned int i = 0; i < arg.size(); i++){
				int idx_type = script_class_factory->getIdxClassFromIts_C_Type(arg[i]);

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
		 bool CZetScript::register_C_SingletonClass(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

			//std::vector<CScriptClass *> vec_script_class_node=getVectorScriptClassNode();
			// to make compatible MSVC shared library
			//std::vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();
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



			if(!script_class_factory->isClassRegistered(class_name)){

				std::string str_classPtr = typeid( T *).name();

				if(script_class_factory->getIdx_C_RegisteredClass(str_classPtr)!=ZS_INVALID_CLASS){
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
		bool CZetScript::register_C_Class(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

			if(register_C_SingletonClass<T>(class_name)){

				// get class...
				CScriptClass *irc =script_class_factory->getScriptClass(class_name);

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
		bool CZetScript::register_C_ClassBuiltIn(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

			if(register_C_SingletonClass<T>(class_name)){
				CScriptClass *irc =script_class_factory->getScriptClass(class_name);

				if(irc->idx_class >= MAX_BUILT_IN_TYPES){
					THROW_RUNTIME_ERROR(string::sformat("The class to register \"%s\" should be a built in class",irc->str_class_ptr_type.c_str()));
					return false;
				}

				// get class...


				if(irc==NULL){
					return false;
				}

				//put the constructor/destructor...
				CZetScript *zs=this;
				irc->c_constructor = new std::function<void *()>([zs](){
					T* t=new T(zs);
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
		bool CZetScript::class_C_Baseof(){

			// to make compatible MSVC shared library
			//std::vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();


			std::string base_class_name=typeid(B).name();
			std::string base_class_name_ptr=typeid(B *).name();
			std::string class_name=typeid(T).name();
			std::string class_name_ptr=typeid(T *).name();

			int idx_base_class = script_class_factory->getIdxClassFromIts_C_Type(base_class_name_ptr);
			if(idx_base_class == -1) {
				THROW_RUNTIME_ERROR(string::sformat("base class %s not registered",base_class_name_ptr.c_str()));
				return false;
			}


			int register_class = script_class_factory->getIdxClassFromIts_C_Type(class_name_ptr);
			if(register_class == -1) {
				THROW_RUNTIME_ERROR(string::sformat("class %s not registered",class_name_ptr.c_str()));
				return false;
			}

			if(script_class_factory->isIdxClassInstanceOf(register_class,idx_base_class)){
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

				sc=script_class_factory->getScriptClass(sc->idx_base_class[0]); // get base class...
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
		bool CZetScript::register_C_FunctionMember(
				const char *function_name
				,R (T:: *function_type)(_A...)
				, const char *registered_file
				,int registered_line
		)
		{

			// to make compatible MSVC shared library
			//std::vector<CScriptClass *> * 	local_vec_script_class_node = getVecScriptClassNode();

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

			CScriptClass * sc=script_class_factory->getScriptClass(str_classPtr);


			if(sc == NULL){
				return false;
			}

			//idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

			// 1. check all parameters ok.
			using Traits3 = function_traits<decltype(function_type)>;
			get_params_function<Traits3>(0,return_type, arg, make_index_sequence<Traits3::arity>{});


			// check valid parameters ...
			if((idx_return_type=script_class_factory->getIdxClassFromIts_C_Type(return_type)) == -1){
				THROW_RUNTIME_ERROR(string::sformat("Return type \"%s\" for function \"%s\" not registered",rtti::demangle(return_type).c_str(),function_name));
				return false;
			}

			for(unsigned int i = 0; i < arg.size(); i++){
				int idx_type=script_class_factory->getIdxClassFromIts_C_Type(arg[i]);

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

			if(STRCMP(getMetamethod(SET_METAMETHOD),==,function_name)){
				sc->metamethod_operator[SET_METAMETHOD].push_back(sf);
				zs_print_debug_cr("Registered metamethod %s::%s",rtti::demangle(typeid(C).name()).c_str(), function_name);
			}

			return true;
		}


		/**
		 * Register C Member function Class
		 */
		template <typename T, typename F>
		bool CZetScript::register_C_StaticFunctionMember(const char *function_name,F function_type, const char *registered_file,int registered_line)
		{
			// to make compatible MSVC shared library
			//std::vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();

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
		bool CZetScript::register_C_VariableMember(const char *var_name, R T::*var_pointer, const char *registered_file,int registered_line) //unsigned int offset)
		{
			// to make compatible MSVC shared library
			//std::vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();

			std::string var_type = typeid(R *).name(); // we need the pointer type ...
			std::string return_type;
			//std::vector<std::string> params;
			std::string str_classPtr = typeid( C *).name();
			unsigned int ref_ptr=offset_of<C>(var_pointer);

			CScriptClass *c_class = script_class_factory->getScriptClassBy_C_ClassPtr(str_classPtr);

			if(c_class == NULL){
				return false;
			}

			// 1. check all parameters ok.

			// check valid parameters ...
			if(script_class_factory->getIdxClassFromIts_C_Type(var_type) == -1){
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


		// Helpers...
		inline StackElement CZetScript::var_2_stk(intptr_t var_trans, int idx_type){
			//intptr_t var_trans = (intptr_t)input_var;
				std::string s_return_value;
				StackElement callc_result={0,0,STK_PROPERTY_TYPE_UNDEFINED};
				//int idx_type=getIdxClassFromIts_C_Type(typeid(T).name());
				// save return type ...
				switch(idx_type){
				 case IDX_TYPE_VOID_C:
					break;
				 case IDX_TYPE_INT_PTR_C:
					 if(var_trans==0) return callc_result;
					 callc_result={(void *)(*((intptr_t *)var_trans)),NULL,STK_PROPERTY_TYPE_INTEGER};
					 break;
				 case IDX_TYPE_UNSIGNED_INT_C:
				 case IDX_TYPE_INTPTR_T_C:
				 case IDX_TYPE_INT_C:
					 callc_result={(void *)(((intptr_t)var_trans)),NULL,STK_PROPERTY_TYPE_INTEGER};
					 break;
				 case IDX_TYPE_FLOAT_C:
					 callc_result.properties=STK_PROPERTY_TYPE_NUMBER;//{};
					 memcpy(&callc_result.stkValue,&var_trans,sizeof(float));

					 break;
				 case IDX_TYPE_FLOAT_PTR_C:
					 if(var_trans==0) return callc_result;
					 callc_result.properties=STK_PROPERTY_TYPE_NUMBER;//{};
					 memcpy(&callc_result.stkValue,&(*(float *)var_trans),sizeof(float));
					 break;
				 case IDX_TYPE_BOOL_PTR_C:
					 if(var_trans==0) return callc_result;
					 callc_result={(void *)(*((bool *)var_trans)),NULL,STK_PROPERTY_TYPE_BOOLEAN};
					 break;
				 case IDX_TYPE_BOOL_C:
					 callc_result={(void *)(((bool)var_trans)),NULL,STK_PROPERTY_TYPE_BOOLEAN};
					 break;
				 case IDX_TYPE_CONST_CHAR_PTR_C:
					 if(var_trans==0) return callc_result;
					 callc_result={(void *)var_trans,NULL,STK_PROPERTY_TYPE_STRING};//new std::string(*((std::string *)result))};
					 break;
				 case IDX_TYPE_STRING_PTR_C:
					 if(var_trans==0) return callc_result;
					 callc_result={(void *)((std::string *)var_trans)->c_str(),NULL,STK_PROPERTY_TYPE_STRING};//new std::string(*((std::string *)result))};
					 break;
				 case IDX_STACK_ELEMENT:
					 if(var_trans==0) return callc_result;
					 callc_result=*((StackElement *)var_trans);//{STK_PROPERTY_TYPE_STRING,(void *)((std::string *)var_trans)->c_str(),NULL};//new std::string(*((std::string *)result))};
					 break;
				 default:
					 if(var_trans==0) return callc_result;
					 callc_result = {NULL,script_class_factory->instanceScriptVariableByIdx(idx_type,(void *)var_trans),STK_PROPERTY_TYPE_SCRIPTVAR};
					 break;
				}

				return callc_result;
		}

		inline bool CZetScript::stk_2_var(StackElement *stk_src, int idx_dst_type, intptr_t *result, std::string & error){
			intptr_t val_ret=0;

			CScriptVariable *script_variable=NULL;


			// save return type ...
			if(stk_src->properties & STK_PROPERTY_IS_STACKVAR){
				stk_src=((StackElement *)stk_src->varRef);
			}

			if(idx_dst_type == IDX_STACK_ELEMENT){//*CScriptClass::STACK_ELEMENT_PTR)){// && (stk_src->properties & STK_PROPERTY_IS_STACKVAR)){ // set directly stackvar
				val_ret=(intptr_t)stk_src;
			}else{

				switch(GET_INS_PROPERTY_VAR_TYPE(stk_src->properties)){
				case STK_PROPERTY_TYPE_BOOLEAN:
					if(idx_dst_type == IDX_TYPE_BOOL_C){// *CScriptClass::STR_BOOL_TYPE){
						val_ret=(intptr_t)(stk_src->stkValue);
					}else if(idx_dst_type == IDX_TYPE_BOOL_PTR_C){//*CScriptClass::STR_BOOL_TYPE_PTR){
						val_ret=(intptr_t)(&stk_src->stkValue);
					}else{
						error="cannot convert "+rtti::demangle((STR_STRING_TYPE_PTR))+" into %s"+rtti::demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type)).c_str();

						return false;
					}

					break;
				case STK_PROPERTY_TYPE_NUMBER:
					switch(idx_dst_type){
					case IDX_TYPE_FLOAT_C:
						memcpy(&val_ret,&stk_src->stkValue,sizeof(float));
						break;
					case IDX_TYPE_FLOAT_PTR_C:
						val_ret=(intptr_t)(&stk_src->stkValue);
						break;
					case IDX_TYPE_UNSIGNED_INT_C:
					case IDX_TYPE_INTPTR_T_C:
					case IDX_TYPE_INT_C:
						{
							int *aux_dst = ((int *)&val_ret);
							float *aux_src=(float *)&stk_src->stkValue;
							*aux_dst=(int)(*aux_src);
						}
						break;
					default:
						error="cannot convert "+rtti::demangle((STR_STRING_TYPE_PTR))+" into %s"+rtti::demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
						return false;
					}
					break;
				case STK_PROPERTY_TYPE_INTEGER:
					switch(idx_dst_type){
					case IDX_TYPE_UNSIGNED_INT_C:
					case IDX_TYPE_INTPTR_T_C:
					case IDX_TYPE_INT_C:
						val_ret=(intptr_t)(stk_src->stkValue);
						break;
					case IDX_TYPE_INT_PTR_C:
						val_ret=(intptr_t)(&stk_src->stkValue);
						break;
					case IDX_TYPE_FLOAT_C:
						{
							float *aux_dst = ((float *)&val_ret);
							int *aux_src=(int *)&stk_src->stkValue;
							*aux_dst = (float)(*aux_src);
						}
						break;
					default:
						error= "cannot convert "+rtti::demangle((STR_STRING_TYPE_PTR))+" into "+rtti::demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
						return false;
					}
					break;

				case STK_PROPERTY_TYPE_STRING:
					if(idx_dst_type == IDX_TYPE_STRING_PTR_C){
						if(stk_src->varRef != 0){
							val_ret=(intptr_t)(&((CStringScriptVariable *)(stk_src->varRef))->m_strValue);
						}
						else{ // pass param std::string ...
							error= "(std::string *)Expected varRef not NULL";
							return false;
						}

					}else if (idx_dst_type == IDX_TYPE_CONST_CHAR_PTR_C){
						val_ret=(intptr_t)(stk_src->stkValue);
					}else{
						error= "cannot convert "+rtti::demangle((STR_STRING_TYPE_PTR))+" into "+rtti::demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
						return false;
					}


					break;
				default: // script variable by default ...

					script_variable=(CScriptVariable *)stk_src->varRef;
					CScriptClass *c_class=NULL;

					if(script_variable==NULL){

						error="Variable is not defined";
						return false;
					}

					if(script_variable->idx_class==IDX_CLASS_STRING){

						val_ret=(intptr_t)(&script_variable->m_strValue);
						if(idx_dst_type == IDX_TYPE_CONST_CHAR_PTR_C){
							val_ret=(intptr_t)script_variable->m_strValue.c_str();
						}
					}else if(

					   (script_variable->idx_class==IDX_CLASS_VECTOR
					|| script_variable->idx_class==IDX_CLASS_STRUCT)){

						if(idx_dst_type==script_variable->idx_class){
							val_ret=(intptr_t)script_variable->get_C_Object();
						}

					}else if((c_class=script_variable->get_C_Class())!=NULL){ // get the pointer directly ...

						if(c_class->idx_class==idx_dst_type){
							val_ret=(intptr_t)script_variable->get_C_Object();
						}
						else if((val_ret=doCast((intptr_t)script_variable->get_C_Object(),c_class->idx_class,idx_dst_type))==0){//c_class->idx_class==idx_dst_type){
							error = "cannot convert "+rtti::demangle(script_variable->getPointer_C_ClassName())+" into "+rtti::demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
							return false;
						}
					}else{ // CScriptVariable ?
						error = " Error calling function, no C-object parameter! Unexpected script variable ("+rtti::demangle(script_variable->getClassName())+")";
						return false;
					}
					break;
				}
			}

			*result = val_ret;

			return true;
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type
		{


			*f=((void *)(new std::function<void ()>(
				[&,calling_obj,fun_obj](){
					bool error=false;
					virtual_machine->execute(
										fun_obj,
										calling_obj,
										error);
					if(error){

						THROW_SCRIPT_ERROR();

					}
				}
			)));
		}

		template <typename R,typename T>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type
		{
			int idx_return = getIdxClassFromIts_C_Type(typeid(R).name());

			*f=((void *)(new std::function<R ()>(
				[&,calling_obj,fun_obj,idx_return](){
						std::string error_str;
						bool error=false;
						R ret_value;

						StackElement stk = virtual_machine->execute(
								fun_obj,
								calling_obj,
								error);


						if(error){
							THROW_SCRIPT_ERROR();//THROW_RUNTIME_ERROR(std::string("run-time error"));
						}


						if(!stk_2_var(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
						}
						return ret_value;
				}
			)));

		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f ,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			//return NULL;

			using tParam1 = typename T::template argument<0>::type;
			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());

			*f=((void *)(new std::function<void (tParam1)>(
				[&,calling_obj,fun_obj, idx_param1](tParam1 p1){

					bool error=false;
					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)

					};

					virtual_machine->execute(
								fun_obj,
								calling_obj,
								error,
								args);
					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}
			)));
		}

		template<typename R,typename T,  typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f ,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			using tParam1 = typename T::template argument<0>::type;

			int idx_return = getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());


			*f=((void *)(new std::function<R (tParam1)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1](tParam1 p1){

						R ret_value;
						std::string error_str;
						bool error=false;

						std::vector<StackElement> args={
								 var_2_stk((intptr_t)p1,idx_param1)
						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													error,
													args);

						if(error){
							THROW_SCRIPT_ERROR();//(std::string("run-time error"));
						}

						if(!stk_2_var(&stk,idx_return, (intptr_t*)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
						}
						return ret_value;
				}
			)));
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;


			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = getIdxClassFromIts_C_Type(typeid(tParam2).name());

			*f=((void *)(new std::function<void (tParam1,tParam2)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2](tParam1 p1,tParam2 p2){

					bool error=false;

					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)

					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									error,
									args);


					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{
			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;


			int idx_return = getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = getIdxClassFromIts_C_Type(typeid(tParam2).name());

			*f=((void *)(new std::function<R (tParam1,tParam2)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2](tParam1 p1,tParam2 p2){

						R ret_value;
						std::string error_str;
						bool error=false;

						std::vector<StackElement> args={
								 var_2_stk((intptr_t)p1,idx_param1)
								,var_2_stk((intptr_t)p2,idx_param2)

						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													error,
													args);

						if(error){
							THROW_SCRIPT_ERROR();//(std::string("run-time error"));
						}

						if(!stk_2_var(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
						}
						return ret_value;
				}
			)));

			//return NULL;
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;


			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = getIdxClassFromIts_C_Type(typeid(tParam3).name());


			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3)>(
				[&,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3](tParam1 p1,tParam2 p2,tParam3 p3){

					bool error=false;

					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									error,
									args);

					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{
			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;

			int idx_return = getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = getIdxClassFromIts_C_Type(typeid(tParam3).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3](tParam1 p1,tParam2 p2,tParam3 p3){
					R ret_value;
					std::string error_str;
					bool error=false;

					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
					};

					StackElement stk = virtual_machine->execute(
												fun_obj,
												calling_obj,
												error,
												args);

					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}

					if(!stk_2_var(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
						THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
					}
					return ret_value;
				}
			)));
		}


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;

			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = getIdxClassFromIts_C_Type(typeid(tParam4).name());


			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){

					bool error=false;

					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
							,var_2_stk((intptr_t)p4,idx_param4)

					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									error,
									args);

					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;

			int idx_return = getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = getIdxClassFromIts_C_Type(typeid(tParam4).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3,tParam4)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){
						R ret_value;
						bool error=false;
						std::string error_str;

						std::vector<StackElement> args={
								 var_2_stk((intptr_t)p1,idx_param1)
								,var_2_stk((intptr_t)p2,idx_param2)
								,var_2_stk((intptr_t)p3,idx_param3)
								,var_2_stk((intptr_t)p4,idx_param4)

						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													error,
													args);

						if(error){
							THROW_SCRIPT_ERROR();//(std::string("run-time error"));
						}

						if(!stk_2_var(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
						}
						return ret_value;

				}
			)));

		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;



			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = getIdxClassFromIts_C_Type(typeid(tParam4).name());
			int idx_param5 = getIdxClassFromIts_C_Type(typeid(tParam5).name());


			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5)>(
				[&,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){

					bool error=false;
					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
							,var_2_stk((intptr_t)p4,idx_param4)
							,var_2_stk((intptr_t)p5,idx_param5)

					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									error,
									args);
					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}

			)));
		}


		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;

			int idx_return = getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = getIdxClassFromIts_C_Type(typeid(tParam4).name());
			int idx_param5 = getIdxClassFromIts_C_Type(typeid(tParam5).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3,tParam4,tParam5)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){


					R ret_value;
					bool error=false;
					std::string error_str;

					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
							,var_2_stk((intptr_t)p4,idx_param4)
							,var_2_stk((intptr_t)p5,idx_param5)

					};

					StackElement stk = virtual_machine->execute(
												fun_obj,
												calling_obj,
												error,
												args);

					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}

					if(!stk_2_var(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
						THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
					}
					return ret_value;
				}
			)));
		}


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;
			using tParam6 = typename T::template argument<5>::type;


			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = getIdxClassFromIts_C_Type(typeid(tParam4).name());
			int idx_param5 = getIdxClassFromIts_C_Type(typeid(tParam5).name());
			int idx_param6 = getIdxClassFromIts_C_Type(typeid(tParam6).name());




			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){
					bool error=false;
					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
							,var_2_stk((intptr_t)p4,idx_param4)
							,var_2_stk((intptr_t)p5,idx_param5)
							,var_2_stk((intptr_t)p6,idx_param6)

					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									error,
									args);
					if(error){

						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type
		{
			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;
			using tParam6 = typename T::template argument<5>::type;

			int idx_return = getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = getIdxClassFromIts_C_Type(typeid(tParam4).name());
			int idx_param5 = getIdxClassFromIts_C_Type(typeid(tParam5).name());
			int idx_param6 = getIdxClassFromIts_C_Type(typeid(tParam6).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){

						R ret_value;
						bool error=false;
						std::string error_str;

						std::vector<StackElement> args={
								 var_2_stk((intptr_t)p1,idx_param1)
								,var_2_stk((intptr_t)p2,idx_param2)
								,var_2_stk((intptr_t)p3,idx_param3)
								,var_2_stk((intptr_t)p4,idx_param4)
								,var_2_stk((intptr_t)p5,idx_param5)
								,var_2_stk((intptr_t)p6,idx_param6)
						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													error,
													args);

						if(error){
							THROW_SCRIPT_ERROR();//(std::string("run-time error"));
						}

						if(!stk_2_var(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
						}
						return ret_value;

				}
			)));
		}


		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto CZetScript::bindScriptFunctionBuilderBase(void **f, CScriptVariable *calling_obj,CScriptFunction *fun_obj,index_sequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F,  typename F::template argument<Is>::type...>(f,calling_obj,fun_obj);
		}

		 template <typename F, std::size_t... Is>
		 auto CZetScript::bindScriptFunctionBuilderBase(void **f, CScriptVariable *calling_obj,CScriptFunction *fun_obj,index_sequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F>(f,calling_obj,fun_obj);
		}


		bool CZetScript::getScriptObject(const std::string &function_access,CScriptVariable **calling_obj,CScriptFunction **fun_obj ){

			//ZS_CLEAR_ERROR_MSG();

			std::vector<std::string> access_var = string::split(function_access,'.');
			//CScriptFunction * main_function = main_function;

			if(main_function == NULL){
				string::sformat("main_function is not initialized");
				return false;
			}

			*calling_obj = NULL;
			FunctionSymbol *is=NULL;
			StackElement *se=NULL;
			*fun_obj=NULL;

			// 1. some variable in main function ...
			if(access_var.size()>1){
				for(unsigned i=0; i < access_var.size()-1; i++){

					std::string symbol_to_find=access_var[i];
					if(i==0){ // get variable through main_class.main_function (global element)
						//symbol_to_find= CEval::makeSymbolRef(symbol_to_find,IDX_GLOBAL_SCOPE);
						for(unsigned j = 0; j < main_function->variable.size() && *calling_obj==NULL; j++){
							if(main_function->variable[j].symbol->name==symbol_to_find
							&& main_function->variable[j].symbol->idxScope == IDX_GLOBAL_SCOPE){
								StackElement *stk = virtual_machine->getStackElement(j); // main_function->object_info.local_symbols.variable[j].
								if(stk!=NULL){
									if(stk->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
										*calling_obj=(CScriptVariable *)stk->varRef;
									}
								}
								else{
									string::sformat("cannot access i (%i)",j);
									return false;
								}
							}
						}

						if((*calling_obj) == NULL){
							string::sformat("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
							return false;
						}

					}else{ // we have got the calling_obj from last iteration ...
						se = (*calling_obj)->getVariableSymbol(symbol_to_find);

						if(se!=NULL){

							if(se->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
								*calling_obj=(CScriptVariable *)se->varRef;
							}else{
								string::sformat("error evaluating \"%s\". Variable name \"%s\" not script variable",function_access.c_str(),symbol_to_find.c_str());
								return false;
							}
						}
						else{
							string::sformat("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
							return false;
						}
					}
				}

				is=(*calling_obj)->getFunctionSymbol(access_var[access_var.size()-1]);
				if(is!=NULL){
					if(is->object.properties & STK_PROPERTY_TYPE_FUNCTION){
						*fun_obj=(CScriptFunction *)is->object.stkValue;
					}
				}else{

					string::sformat("error evaluating \"%s\". Cannot find function \"%s\"",function_access.c_str(),access_var[access_var.size()-1].c_str());
					return false;
				}

			}else{ // some function in main function
				//*calling_obj = m_mainObject;
				std::string symbol_to_find=access_var[0];
				for(unsigned i = 0; i < main_function->local_function.size() && *fun_obj==NULL; i++){
					CScriptFunction *aux_fun_obj=main_function->local_function[i];
					if(		aux_fun_obj->symbol_info.symbol->name  == symbol_to_find
					  && aux_fun_obj->symbol_info.symbol->idxScope == IDX_GLOBAL_SCOPE){
						*fun_obj=aux_fun_obj;
					}
				}
			}

			if(*fun_obj==NULL){
				THROW_RUNTIME_ERROR(string::sformat("error evaluating \"%s\". Variable name \"%s\" is not function type",function_access.c_str(),access_var[access_var.size()-1].c_str()));
				return false;
			}

			return true;
		}


		template <  typename F>
		std::function<F> * CZetScript::bind_function(const std::string & function_access)
		{
			std::string return_type;
			std::vector<std::string> params;
			std::vector<std::string> arg;
			int idx_return_type=-1;
			void *ptr;
			CScriptFunction * fun=NULL;
			CScriptVariable *calling_obj=NULL;

			// get function symbol ref from global scope ...


			if(getScriptObject(function_access,&calling_obj,&fun))
			{

				// 1. check all parameters ok.
				using Traits3 = function_traits<F>;//decltype(function_type)>;
				get_params_function<Traits3>(0,return_type, arg, make_index_sequence<Traits3::arity>{});

				// 2. check valid parameters ...
				if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
					THROW_RUNTIME_ERROR(string::sformat("Return type \"%s\" for bind function not registered",rtti::demangle(return_type).c_str()));
					return NULL;
				}

				for(unsigned int i = 0; i < arg.size(); i++){
					if(getIdxClassFromIts_C_Type(arg[i])==-1){
						THROW_RUNTIME_ERROR(string::sformat("Argument (%i) type \"%s\" for bind function not registered",i,rtti::demangle(arg[i]).c_str()));
						return NULL;
					}
				}

				// 3. build custom function in function of parameter number ...
				bindScriptFunctionBuilderBase<Traits3>(&ptr,calling_obj,fun,make_index_sequence<Traits3::arity>{});
				return (std::function<F> *)ptr;

			}

			return NULL;
		}

		template<typename T>
		std::vector<T> CZetScript::vscript2vector(CVectorScriptVariable *v_in){
			std::vector<T> v_out;
			const char * dst_convert_type = typeid(T).name();
			float aux_flt;
			std::vector<StackElement> * variable = v_in->getVectorVariable();

			if(v_in){
				for(unsigned i = 0; i < variable->size(); i++){

					StackElement sv=variable->at(i);

					switch(sv.properties & MASK_VAR_PRIMITIVE_TYPES)
					{
						default:
						case STK_PROPERTY_TYPE_UNDEFINED:
						case STK_PROPERTY_TYPE_NULL:
						case STK_PROPERTY_TYPE_NUMBER:
							if(STRCMP(dst_convert_type, ==,typeid(float).name())){
								memcpy(&aux_flt, &sv.stkValue, sizeof(float));
								v_out.push_back(aux_flt);
							}else if(STRCMP(dst_convert_type, ==,typeid(int).name())){
								v_out.push_back((intptr_t)sv.stkValue);
							}else{
								THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<float>");
								return v_out;
							}
							break;
						case STK_PROPERTY_TYPE_BOOLEAN:
						case STK_PROPERTY_TYPE_STRING:
						case STK_PROPERTY_TYPE_FUNCTION:
							THROW_RUNTIME_ERROR("type not implemented yet");
							return v_out;
							break;
						case STK_PROPERTY_TYPE_INTEGER:
							if(STRCMP(dst_convert_type, ==,typeid(int).name()) || STRCMP(dst_convert_type, ==,typeid(float).name())){// typeid(int).name()) || ){
								v_out.push_back((intptr_t)sv.stkValue);
							}else{
								THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<int>");
								return v_out;
							}
							break;
					}
				}
			}
			return v_out;
		}

		template<typename T>
		CVectorScriptVariable * CZetScript::vector_2_vscript(const std::vector<T> & v){
			CVectorScriptVariable *vsv = new CVectorScriptVariable(this);

			for ( unsigned i = 0; i < v.size(); i++){
				StackElement *stk = vsv->push();
				//intptr_t uvar = (intptr_t)(v[i]);
				*stk = var_2_stk((intptr_t)(v[i]),getIdxClassFromIts_C_Type(typeid(T).name()));
			}

			return vsv;
		}

}
