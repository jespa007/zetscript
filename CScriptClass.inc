		/**
		 * Register C function
		 */
		template <typename F>
		bool CScriptClass::register_C_FunctionInt(const char * function_name,F function_ptr)
		{
			int idx_return_type=-1;
			string return_type;
			vector<string> m_arg;
			vector<tArgumentInfo> m_infoArg;
			intptr_t ref_ptr=0;
			CScriptFunctionObject *irs=NULL;

			if(!CScriptFunctionObject::checkCanRegister_C_Function(function_name)){
				return false;
			}


			//CScriptVariable *sf=NULL;
			CScriptFunctionObject * mainFunctionInfo = getScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME);
			//CScriptClass *rc = CScriptClass::getInstance()->getRegisteredClass(class_name);

			if(mainFunctionInfo == NULL){
				THROW_RUNTIME_ERROR("main function is not created");
				return false;
			}

			// 1. check all parameters ok.
			using Traits3 = function_traits<decltype(function_ptr)>;
			getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});


			// check valid parameters ...
			if((idx_return_type=getIdxClassFromIts_C_Type(return_type))==-1){
				THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name);
				return false;
			}

			for(unsigned int i = 0; i < m_arg.size(); i++){
				int idx_type = getIdxClassFromIts_C_Type(m_arg[i]);

				if(idx_type==IDX_CLASS_FLOAT_C || idx_type==IDX_CLASS_BOOL_C){
					THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,demangle(m_arg[i]).c_str(),function_name,demangle(m_arg[i]).c_str());
					return false;
				}

				if(idx_type ==-1){
					THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(m_arg[i]).c_str(),function_name);
					return false;
				}

				m_infoArg.push_back({idx_type,m_arg[i]});
			}

			if(idx_return_type == IDX_CLASS_VOID_C){
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
			mainFunctionInfo->newLocalFunction(IDX_C_CLASS_SCOPE,function_name,m_infoArg,idx_return_type,ref_ptr,PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF);
			/*irs = NEW_SCRIPT_FUNCTION_OBJECT();

			irs->m_arg = m_infoArg;
			irs->idx_return_type = idx_return_type;
			irs->symbol_info.ref_ptr = ref_ptr;

			irs->symbol_info.idxAstNode = -1;
			irs->symbol_info.symbol_ref = CCompiler::makeSymbolRef(function_name,IDX_GLOBAL_SCOPE); // <-- defined as global
			irs->symbol_info.properties = PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF;

			irs->symbol_info.idxSymbol = (short)(mainFunctionInfo->scope_info.local_symbols.vec_idx_registeredFunction.size());
			mainFunctionInfo->scope_info.local_symbols.vec_idx_registeredFunction.push_back(irs->idxScriptFunctionObject);*/

			zs_print_debug_cr("Registered function name: %s",function_name);
			return true;
		}


		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		ZETSCRIPT_MODULE_EXPORT bool CScriptClass::register_C_SingletonClassInt(const string & class_name){//, const string & base_class_name=""){

			// to make compatible MSVC shared library
			vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();
			int size=(*local_vec_script_class_node).size();

			if(size>=MAX_REGISTER_CLASSES){
				THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
				return false;
			}

			if(size>MAX_BASIC_CLASS_TYPES){ // after MAX_BASIC_CLASS_TYPES all registered C classes should follow a registered C class ...
				if((

					(((*local_vec_script_class_node)[size-1]->symbol_info.properties&PROPERTY_C_OBJECT_REF)!=PROPERTY_C_OBJECT_REF)
				)){
					THROW_RUNTIME_ERROR("C class \"%s\" should register after C classes. Register C classes after script classes are not allowed",class_name.c_str());
					return false;
				}

			}


			if(!isClassRegistered(class_name)){

				string str_classPtr = typeid( _T *).name();

				if(getIdx_C_RegisteredClass(str_classPtr,false)!=ZS_INVALID_CLASS){
					THROW_RUNTIME_ERROR("this %s is already registered",demangle(typeid( _T).name()).c_str());
					return false;
				}

				//THROW_RUNTIME_ERROR("CHECK AND TODOOOOOO!");
				CScriptClass *irc = new CScriptClass;

				CASTNode *ast =CASTNode::newASTNode();
				CASTNode *ast_var_symbols =CASTNode::newASTNode();
				CASTNode *ast_collection_var_symbols =CASTNode::newASTNode();
				CASTNode *ast_fun_symbols =CASTNode::newASTNode();

				// push var & fun symbols node...
				ast_var_symbols->children.push_back(ast_collection_var_symbols->idxAstNode);
				ast->children.push_back(ast_var_symbols->idxAstNode);
				ast->children.push_back(ast_fun_symbols->idxAstNode);

				irc->symbol_info.idxAstNode = ast->idxAstNode;
				//irc->metadata_info.object_info.symbol_info.idxScopeVar=-1;
				irc->symbol_info.symbol_ref = class_name;
				//irc->baseClass = base_class; // identify extend class ?!?!!?
				// in C there's no script constructor ...
				irc->idx_function_script_constructor=-1;
				// allow dynamic constructor in function its parameters ...


				irc->symbol_info.idxScriptClass = (short)((*local_vec_script_class_node).size());
				irc->classPtrType=str_classPtr;
				irc->symbol_info.properties=PROPERTY_C_OBJECT_REF;


				irc->c_constructor = NULL;
				irc->c_destructor = NULL;
				(*local_vec_script_class_node).push_back(irc);

				irc->idxClass=(*local_vec_script_class_node).size()-1;
				zs_print_debug_cr("* C++ class \"%s\" registered as (%s).",class_name.c_str(),demangle(str_classPtr).c_str());

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
		template<class _T>
		bool CScriptClass::register_C_ClassInt(const string & class_name){//, const string & base_class_name=""){
			if(register_C_SingletonClass<_T>(class_name)){
				CScriptClass *irc = CScriptClass::getScriptClassByName(class_name);

				//put the constructor/destructor...
				irc->c_constructor = new std::function<void *()>([=](){
					_T* t=new _T;
					return t;
				});

				irc->c_destructor = new std::function<void (void *)>([=](void *p){
					delete (_T *)p;
				});

				return true;
			}

			return false;
		}


		template<class _T, class _B>
		bool CScriptClass::class_C_baseofInt(){

			// to make compatible MSVC shared library
			vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();


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

			CScriptClass *sc=(*local_vec_script_class_node)[register_class];
			while( sc->idxBaseClass.size()>0){

				sc=CScriptClass::getScriptClassByIdx(sc->idxBaseClass[0]); // get base class...
				if(sc->classPtrType ==base_class_name_ptr){
					THROW_RUNTIME_ERROR("C++ class \"%s\" already base of \"%s\" ",demangle(class_name).c_str(), demangle(base_class_name).c_str());
					return false;
				}

			}


			CScriptClass *irc_class = (*local_vec_script_class_node)[register_class];
			irc_class->idxBaseClass.push_back(idx_base_class);

			map<int, map<int, fntConversionType>>  *	local_map_type_conversion=	getMapTypeConversion();
			(*local_map_type_conversion)[irc_class->idxClass][idx_base_class]=[](intptr_t entry){ return (intptr_t)(_B *)((_T *)entry);};


			if(CScriptClass::register_c_base_symbols){
				//----------------------------
				//
				// DERIVATE STATE
				//
				// enabled but with restrictions! This don't work for virtual classes (its map changes at runtime) and multiheritance
				// https://stackoverflow.com/questions/48572734/is-possible-do-a-later-function-binding-knowing-its-function-type-and-later-the
				//

				CScriptClass *irc_base = (*local_vec_script_class_node)[idx_base_class];

				unsigned short derivated_properties=PROPERTY_C_OBJECT_REF| PROPERTY_IS_DERIVATED;
				if(std::is_polymorphic<_B>::value==true){
					derivated_properties|=PROPERTY_IS_POLYMORPHIC;
				}

				// register all symbols function from base ...
				// vars ...
				for(unsigned i = 0; i < irc_base->scope_info.local_symbols.m_registeredVariable.size(); i++){

					tVariableSymbolInfo *irs_source = &irc_base->scope_info.local_symbols.m_registeredVariable[i];

					tVariableSymbolInfo irs;
					// init struct...
					irs.idxScriptClass = idx_base_class;
					irs.ref_ptr=irs_source->ref_ptr;
					irs.c_type = irs_source->c_type;
					//irs.
					irs.symbol_ref=irs_source->symbol_ref;
					irs.properties = derivated_properties;
					irs.idxSymbol = (short)(irc_class->scope_info.local_symbols.m_registeredVariable.size());
					irc_class->scope_info.local_symbols.m_registeredVariable.push_back(irs);

				}

				// functions ...
				for(unsigned i = 0; i < irc_base->scope_info.local_symbols.vec_idx_registeredFunction.size(); i++){

					CScriptFunctionObject *irs_source = GET_SCRIPT_FUNCTION_OBJECT(irc_base->scope_info.local_symbols.vec_idx_registeredFunction[i]);

					CScriptFunctionObject *irs=NEW_SCRIPT_FUNCTION_OBJECT;
					// init struct...
					irs->symbol_info.idxAstNode = -1;
					//irs.object_info.symbol_info.idxScopeVar = -1;
					irs->symbol_info.symbol_ref=irs_source->symbol_info.symbol_ref;


					irs->m_arg = irs_source->m_arg;
					irs->idx_return_type = irs_source->idx_return_type;

					irs->symbol_info.properties = derivated_properties;

					// ignores special type cast C++ member to ptr function
					// create binding function class
					irs->symbol_info.ref_ptr= irs_source->symbol_info.ref_ptr; // this is not correct due the pointer

					irs->symbol_info.idxSymbol = (short)(irc_class->scope_info.local_symbols.vec_idx_registeredFunction.size());
					irc_class->scope_info.local_symbols.vec_idx_registeredFunction.push_back(irs->idxScriptFunctionObject);


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
		bool CScriptClass::register_C_FunctionMemberInt(const char *function_name,_R (_T:: *function_type)(_A...) )
		{

			// to make compatible MSVC shared library
			vector<CScriptClass *> * 	local_vec_script_class_node = getVecScriptClassNode();

			string return_type;
			//vector<string> params;
			CScriptFunctionObject *irs=NULL;
			vector<string> m_arg;
			vector<tArgumentInfo> m_argInfo;
			int idx_return_type=-1;
			intptr_t ref_ptr=0;
			string str_classPtr = typeid( _C *).name();

			if(!CScriptFunctionObject::checkCanRegister_C_Function(function_name)){
				return false;
			}

			int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

			if(idxRegisterdClass == -1){
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
				int idx_type=getIdxClassFromIts_C_Type(m_arg[i]);

				if(idx_type==IDX_CLASS_FLOAT_C || idx_type==IDX_CLASS_BOOL_C){
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
			if(idx_return_type == IDX_CLASS_VOID_C){
				if((ref_ptr=((intptr_t)CNativeFunction::getInstance()->c_member_class_function_proxy<_C,_T, void>(m_arg.size(),function_type)))==0){
					return false;
				}
			}else{
				if((ref_ptr=((intptr_t)CNativeFunction::getInstance()->c_member_class_function_proxy<_C,_T, _R>(m_arg.size(),function_type)))==0){
					return false;
				}
			}

			// ok, function candidate to be added into class...
			irs = NEW_SCRIPT_FUNCTION_OBJECT;

			// init struct...
			CASTNode *ast_symbol = CASTNode::newASTNode();
			ast_symbol->idxScope=IDX_C_CLASS_SCOPE;
			ast_symbol->symbol_value = function_name;

			for(unsigned a=0; a < m_argInfo.size();a++){ // set number args...
				ast_symbol->children.push_back(-1);
			}

			// get ast symbols function member node...
			CASTNode *ast_symbol_node =AST_NODE(AST_NODE((*local_vec_script_class_node)[idxRegisterdClass]->symbol_info.idxAstNode)->children[1]);
			ast_symbol_node->children.push_back(ast_symbol->idxAstNode);



			//irs.object_info.symbol_info.idxScopeVar = -1;
			irs->symbol_info.symbol_ref=CCompiler::makeSymbolRef(function_name,IDX_C_CLASS_SCOPE);
			irs->symbol_info.properties = PROPERTY_C_OBJECT_REF;

			irs->symbol_info.ref_ptr = ref_ptr;
			irs->m_arg = m_argInfo;
			irs->idx_return_type = idx_return_type;

			irs->symbol_info.idxSymbol = (short)((*local_vec_script_class_node)[idxRegisterdClass]->scope_info.local_symbols.vec_idx_registeredFunction.size());
			(*local_vec_script_class_node)[idxRegisterdClass]->scope_info.local_symbols.vec_idx_registeredFunction.push_back(irs->idxScriptFunctionObject);
			zs_print_debug_cr("Registered member function name %s::%s",demangle(typeid(_C).name()).c_str(), function_name);

			if(STRCMP(getMetamethod(SET_METAMETHOD),==,function_name)){
				(*local_vec_script_class_node)[idxRegisterdClass]->metamethod_operator[SET_METAMETHOD].push_back(irs->idxScriptFunctionObject);
				zs_print_debug_cr("Registered metamethod %s::%s",demangle(typeid(_C).name()).c_str(), function_name);
			}


			return true;
		}


		/**
		 * Register C Member function Class
		 */
		template <typename _T, typename _F>
		bool CScriptClass::register_C_StaticFunctionMemberInt(const char *function_name,_F function_type)
		{
			// to make compatible MSVC shared library
			vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();

			string return_type;
			vector<string> params;
			CScriptFunctionObject *irs=NULL;
			vector<string> m_arg;
			vector<tArgumentInfo> m_argInfo;
			int idx_return_type=-1;
			intptr_t ref_ptr=0;
			string str_classPtr = typeid( _T *).name();
			string function_class_name = demangle(typeid(_T).name())+"::"+function_name;


			if(!CScriptFunctionObject::checkCanRegister_C_Function(function_class_name.c_str())){
				return false;
			}

			int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

			if(idxRegisterdClass == -1){
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

				if(idx_type==IDX_CLASS_FLOAT_C || idx_type==IDX_CLASS_BOOL_C){
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
			if(idx_return_type == IDX_CLASS_VOID_C){
				if((ref_ptr=(intptr_t)CNativeFunction::getInstance()->new_proxy_function<void>(m_arg.size(),function_type))==0){//(int)function_ptr;
					return false;
				}
			}
			else{
				if((ref_ptr=(intptr_t)CNativeFunction::getInstance()->new_proxy_function<int>(m_arg.size(),function_type))==0){//(int)function_ptr;
					return false;
				}
			}

			// init struct...
			CASTNode *ast_symbol = CASTNode::newASTNode();
			ast_symbol->idxScope = IDX_C_CLASS_SCOPE;
			ast_symbol->symbol_value = function_name;

			for(int a=0; a < m_argInfo.size();a++){ // set number args...
				ast_symbol->children.push_back(-1);
			}
			// get ast symbols function member node...
			CASTNode *ast_symbol_node =AST_NODE(AST_NODE((*local_vec_script_class_node)[idxRegisterdClass]->symbol_info.idxAstNode)->children[1]);
			ast_symbol_node->children.push_back(ast_symbol->idxAstNode);


			// ok, function candidate to be added into class...
			irs = NEW_SCRIPT_FUNCTION_OBJECT;



			//irs.object_info.symbol_info.idxScopeVar = -1;
			irs->symbol_info.symbol_ref=CCompiler::makeSymbolRef(function_name,IDX_C_CLASS_SCOPE);
			irs->symbol_info.properties = PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF;

			irs->symbol_info.ref_ptr = ref_ptr;
			irs->m_arg = m_argInfo;
			irs->idx_return_type = idx_return_type;
			irs->symbol_info.idxSymbol = (short)((*local_vec_script_class_node)[idxRegisterdClass]->scope_info.local_symbols.vec_idx_registeredFunction.size());
			(*local_vec_script_class_node)[idxRegisterdClass]->scope_info.local_symbols.vec_idx_registeredFunction.push_back(irs->idxScriptFunctionObject);
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

						(*local_vec_script_class_node)[idxRegisterdClass]->metamethod_operator[i].push_back(irs->idxScriptFunctionObject);

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
		bool CScriptClass::register_C_VariableMemberInt(const char *var_name, _R _T::*var_pointer) //unsigned int offset)
		{
			// to make compatible MSVC shared library
			vector<CScriptClass *> * local_vec_script_class_node = getVecScriptClassNode();

			string var_type = typeid(_R *).name(); // we need the pointer type ...
			string return_type;
			//vector<string> params;
			tVariableSymbolInfo irs;
			string str_classPtr = typeid( _C *).name();
			unsigned int offset=zetscript::offset_of<_C>(var_pointer);

			int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

			if(idxRegisterdClass == -1){
				return false;
			}

			// 1. check all parameters ok.

			// check valid parameters ...
			if(getIdxClassFromIts_C_Type(var_type) == -1){
				THROW_RUNTIME_ERROR("%s::%s has not valid type (%s)"
						,(*local_vec_script_class_node)[idxRegisterdClass]->symbol_info.symbol_ref.c_str()
						,var_name
						,demangle(typeid(_R).name()).c_str());
				return false;
			}

			// init struct...
			irs.idxScriptClass = idxRegisterdClass;
			irs.ref_ptr=offset;
			irs.c_type = var_type;
			//irs.
			irs.symbol_ref=CCompiler::makeSymbolRef(var_name,IDX_C_CLASS_SCOPE);

			// init ast
			CASTNode *ast_symbol = CASTNode::newASTNode();
			ast_symbol->idxScope=IDX_C_CLASS_SCOPE;
			ast_symbol->symbol_value = var_name;

			// get ast var symbol collection node ( because class has a var collection we need a children [0] )
			CASTNode *ast_symbol_node =AST_NODE(AST_NODE(AST_NODE((*local_vec_script_class_node)[idxRegisterdClass]->symbol_info.idxAstNode)->children[0])->children[0]);
			ast_symbol_node->children.push_back(ast_symbol->idxAstNode);


			irs.properties = PROPERTY_C_OBJECT_REF;
			irs.idxSymbol = (short)((*local_vec_script_class_node)[idxRegisterdClass]->scope_info.local_symbols.m_registeredVariable.size());
			(*local_vec_script_class_node)[idxRegisterdClass]->scope_info.local_symbols.m_registeredVariable.push_back(irs);
			//base_info->local_symbols.vec_idx_registeredFunction.push_back(irs);
			return true;

		}
