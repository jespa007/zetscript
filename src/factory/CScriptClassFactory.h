#pragma once



#include "CFunction_C_TypeFactory.h"
#include "RegisterFunctionHelper.h"


#define register_C_Function(s) CScriptClass::register_C_FunctionInt(STR(s),s)
#define register_C_Variable(s) CScriptClass::register_C_VariableInt(STR(s),&s,typeid(decltype(s) *).name())

#define getIdxGlobalVariable(s)  CScriptClass::register_C_FunctionInt(STR(s),s)
#define getIdxGlobalFunction(s)



#define register_C_VariableMember(o,s) 		register_C_VariableMemberInt<o, decltype(o::s)>(STR(s),offsetOf(&o::s))
#define register_C_FunctionMember(o,s)		register_C_FunctionMemberInt<o>(STR(s),&o::s)



#define NEW_CLASS_VAR_BY_IDX(idx) (CScriptClass::getInstance()->newScriptVariableByIdx(idx))


#define REGISTERED_CLASS_NODE(idx) 				CScriptClassFactory::getInfoScriptClassByIdx(idx)
#define MAIN_CLASS_NODE							(CScriptClassFactory::getInfoScriptClassByIdx(0))    // 0 is the main class
#define GET_CLASS(idx)							(CScriptClassFactory::getInfoScriptClassByIdx(idx))    // 0 is the main class
#define GET_CLASS_BY_NAME(s)					(CScriptClassFactory::getInfoScriptClassByName(s))    // 0 is the main class
#define GET_CLASS_C_PTR_NAME(s)					(CScriptClassFactory::getInfoScriptClassBy_C_ClassPtr(s))    // 0 is the main class



class CScriptClassFactory{

	static vector<tInfoScriptClass *> 			* vec_registered_class_node;
public:

	enum C_TYPE_VALID_PRIMITIVE_VAR{
		VOID_TYPE,
		INT_TYPE,
		INT_PTR_TYPE,
		FLOAT_PTR_TYPE,
		STRING_PTR_TYPE,
		BOOL_PTR_TYPE,
		MAX_C_TYPE_VALID_PRIMITIVE_VAR
	};

	static void createSingletons();
	static void destroySingletons();
	static void set(vector<tInfoScriptClass *> 	& set_vec);

	/**
	 * Class Manager
	 */
	static tInfoScriptClass 				*	newInfoScriptClass();
	static vector<tInfoScriptClass *> 		*	getVectorInfoScriptClassList();

	static tInfoScriptClass 				* 	getInfoScriptClassByIdx(int idx);
	static tInfoScriptClass 				* 	getInfoScriptClassByName(const string & name, bool print_msg=true);
	static tInfoScriptClass 				* 	getInfoScriptClassBy_C_ClassPtr(const string & class_type, bool print_msg=true);
	static int 										getIdxInfoScriptClass_Internal(const string & class_name);
	static int 										getIdxInfoScriptClass(const string & v, bool print_msg=true);
	static int 										getIdxClassFromIts_C_Type(const string & c_type_str);
	static bool 									isClassRegistered(const string & v);


	enum BASIC_CLASS_TYPE{

		IDX_CLASS_MAIN=0, // First class ...
		IDX_CLASS_INTEGER,
		IDX_CLASS_NUMBER,
		IDX_CLASS_STRING,
		IDX_CLASS_BOOLEAN,
		IDX_CLASS_VECTOR,
		IDX_CLASS_FUNCTOR,
		IDX_CLASS_UNDEFINED,
		IDX_CLASS_VOID,
		IDX_CLASS_NULL,
		IDX_CLASS_STRUCT,
		MAX_BASIC_CLASS_TYPES
	};





	typedef struct{
		string 	    type_str;
		C_TYPE_VALID_PRIMITIVE_VAR  id;
	}tPrimitiveType;

	typedef struct{
		tPrimitiveType 				*return_type;
		vector<tPrimitiveType*>		params;
	}tRegisterFunction;

	static tPrimitiveType valid_C_PrimitiveType[MAX_C_TYPE_VALID_PRIMITIVE_VAR];
	static void registerPrimitiveTypes();




	/**
	 * This function registers a script class into factory.
	 */
	static tInfoScriptClass * registerScriptClass(const string & class_name, const string & base_class_name, PASTNode _ast);

	/**
	 * Class name given this function creates the object and initializes all variables.
	 */
	static CScriptVariable 		 * newScriptVariableByName(const string & class_name);
	static CScriptVariable 		 * newScriptVariableByIdx(unsigned idx, void * value_object = NULL);



	static bool updateReferenceSymbols();



	static tInfoVariableSymbol  * registerVariableSymbol(const string & class_name,const string & name,PASTNode  node);
	static tInfoVariableSymbol *  getRegisteredVariableSymbol(const string & class_name,const string & varname);
	static int 							 getIdxRegisteredVariableSymbol(const string & class_name,const string & varname, bool show_msg=true);
	static int 							 getIdxRegisteredVariableSymbol(tFunctionInfo *irf,const string & var_name, bool show_msg=true);


	static tScriptFunctionObject *  registerFunctionSymbol(const string & class_name, const string & name,PASTNode  node);
	static int getIdxScriptFunctionObjectByClassFunctionName(const string & class_name,const string & function_name, bool show_errors=true);



	static tFunctionInfo *  getSuperClass(tInfoScriptClass *irc, const string & fun_name);


	static bool addArgumentFunctionSymbol(const string & class_name,const string & function_name,const string & arg_name);


	//tInfoScriptClass * 	getRegisteredClass(const string & v, bool print_msg=true);
	//tInfoScriptClass *	getRegisteredClassByIdx(unsigned index);
	//tInfoScriptClass *  getRegisteredClassBy_C_ClassPtr(const string & v, bool print_msg=true);

	static int 					getIdxRegisteredClass(const string & v, bool print_msg=true);
	static fntConversionType getConversionType(string objectType, string conversionType, bool show_errors=true);

	static const char * getNameRegisteredClassByIdx(int idx);


	// internal var types ...
	/*static int getIdxClassVoid(){return IDX_CLASS_VOID;}
	static tInfoScriptClass *  getRegisteredClassVoid(){return (*vec_registered_class_node)[IDX_CLASS_VOID];}

	static int getIdxClassUndefined(){return IDX_CLASS_UNDEFINED;}
	static tInfoScriptClass *  getRegisteredClassUndefined(){return (*vec_registered_class_node)[IDX_CLASS_UNDEFINED];}

	static int getIdxClassInteger(){return IDX_CLASS_INTEGER;}
	static tInfoScriptClass *  getRegisteredClassInteger(){return (*vec_registered_class_node)[IDX_CLASS_INTEGER];}

	static int getIdxClassNumber(){return IDX_CLASS_NUMBER;}
	static tInfoScriptClass *  getRegisteredClassNumber(){return (*vec_registered_class_node)[IDX_CLASS_NUMBER];}

	static int getIdxClassStruct(){return IDX_CLASS_STRUCT;}
	static tInfoScriptClass *  getRegisteredClassStruct(){return (*vec_registered_class_node)[IDX_CLASS_STRUCT];}

	static int getIdxClassString(){return IDX_CLASS_STRING;}
	static tInfoScriptClass *  getRegisteredClassString(){return (*vec_registered_class_node)[IDX_CLASS_STRING];}

	static int getIdxClassBoolean(){return IDX_CLASS_BOOLEAN;}
	static tInfoScriptClass *  getRegisteredClassBoolean(){return (*vec_registered_class_node)[IDX_CLASS_BOOLEAN];}

	static int getIdxClassVector(){return IDX_CLASS_VECTOR;}
	static tInfoScriptClass *  getRegisteredClassVector(){return (*vec_registered_class_node)[IDX_CLASS_VECTOR];}

	static int getIdxClassFunctor(){return IDX_CLASS_FUNCTOR;}
	static tInfoScriptClass *  getRegisteredClassFunctor(){return (*vec_registered_class_node)[IDX_CLASS_FUNCTOR];}

	static int getIdxClassNull(){return IDX_CLASS_NULL;}
	static tInfoScriptClass *  getRegisteredClassNull(){return (*vec_registered_class_node)[IDX_CLASS_NULL];}

*/




	/**
	 * Register C function
	 */
	template <typename F>
	static bool register_C_FunctionInt(const string & function_name,F function_ptr)
	{
		string return_type;
		vector<string> params;
		tScriptFunctionObject irs;

		//tPrimitiveType *rt;
		//vector<tPrimitiveType *> pt;
		//CScope::tScopeVar  *rs;

		//CScriptVariable *sf=NULL;
		tScriptFunctionObject * mainFunctionInfo = getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_NAME);
		//tInfoScriptClass *rc = CScriptClass::getInstance()->getRegisteredClass(class_name);

		if(mainFunctionInfo == NULL){
			print_error_cr("main function is not created");
			exit(EXIT_FAILURE);
		}

		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,return_type, irs.m_arg, make_index_sequence<Traits3::arity>{});


		// check valid parameters ...
		if((irs.idx_return_type=getIdxClassFromIts_C_Type(return_type))==-1){
			print_error_cr("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name.c_str());
			return false;
		}

		for(unsigned int i = 0; i < irs.m_arg.size(); i++){
			if(getIdxClassFromIts_C_Type(irs.m_arg[i])==-1){
				print_error_cr("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(irs.m_arg[i]).c_str(),function_name.c_str());
				return false;
			}
		}

		// init struct...

		irs.object_info.symbol_info.idxAstNode = -1;
		irs.object_info.symbol_info.idxScopeVar=-1;//info_var_scope = NULL;
		irs.object_info.symbol_info.symbol_name = function_name;

		irs.object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF;
		if(irs.idx_return_type == IDX_CLASS_VOID){
			if((irs.object_info.symbol_info.ref_ptr=(int)CFunction_C_TypeFactory::getInstance()->new_proxy_function<void>(irs.m_arg.size(),function_ptr))==0){//(int)function_ptr;
				return false;
			}
		}
		else{
			if((irs.object_info.symbol_info.ref_ptr=(int)CFunction_C_TypeFactory::getInstance()->new_proxy_function<int>(irs.m_arg.size(),function_ptr))==0){//(int)function_ptr;
				return false;
			}
		}

		irs.object_info.symbol_info.index = mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction.size();
		mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction.push_back(irs);

		print_info_cr("Registered function name: %s",function_name.c_str());

		return true;
	}


	/**
	 * Register C variable
	 */
	static bool register_C_VariableInt(const string & var_str,void * var_ptr, const string & var_type);



	static int getIdx_C_RegisteredClass(const string & str_classPtr, bool print_msg=true){
			// ok check c_type
			for(unsigned i = 0; i < (*vec_registered_class_node).size(); i++){
				if((*vec_registered_class_node)[i]->classPtrType == str_classPtr){
					return i;
				}
			}

			if(print_msg){
				print_error_cr("C class %s is not registered",str_classPtr.c_str());
			}

			return -1;
	}

	static int getIdx_C_RegisteredFunctionMemberClass(const string & str_classPtr, const string & str_functionName, bool print_msg=true){

			int index_class = getIdx_C_RegisteredClass(str_classPtr,print_msg);

			if(index_class == -1){
				return -1;
			}


			vector<int> * vec_irfs = &(*vec_registered_class_node)[index_class]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction;

			// ok check c_type
			for(unsigned i = 0; i < vec_irfs->size(); i++){
				tScriptFunctionObject *sfo = GET_SCRIPT_FUNCTION_OBJECT(vec_irfs->at(i));
				if(AST_SYMBOL_VALUE(vec_irfs->at(i).object_info.symbol_info.idxAstNode) == str_classPtr){
					return i;
				}
			}

			if(print_msg){
				print_error_cr("C class %s is not registered",str_classPtr.c_str());
			}

			return -1;
	}


	/**
	 * Register C Class. Return index registered class
	 */
	template<class _T>
	static bool register_C_Class(const string & class_name){//, const string & base_class_name=""){

		/*tInfoScriptClass *base_class = NULL;

		// get base class
		if(base_class_name != ""){
			if((base_class = this->getRegisteredClass(base_class_name)) == NULL){
				return false;
			}
		}*/

		if(!isClassRegistered(class_name)){

			string str_classPtr = typeid( _T *).name();

			if(getIdx_C_RegisteredClass(str_classPtr,false)!=-1){
				print_error_cr("this %s is already registered",demangle(typeid( _T).name()).c_str());
				return false;
			}


			//print_error_cr("CHECK AND TODOOOOOO!");
			tInfoScriptClass *irc = new tInfoScriptClass;

			tASTNode *ast =new tASTNode;
			irc->metadata_info.object_info.symbol_info.idxAstNode = ast->idxAstNode;
			irc->metadata_info.object_info.symbol_info.idxScopeVar=-1;
			irc->metadata_info.object_info.symbol_info.symbol_name = class_name;
			//irc->baseClass = base_class; // identify extend class ?!?!!?

			// in C there's no script constructor ...
			irc->idx_function_script_constructor=-1;

			// allow dynamic constructor in function its parameters ...
			irc->c_constructor = new std::function<void *()>([](){return new _T;});
			irc->c_destructor = new std::function<void (void *)>([](void *p){delete (_T *)p;});

			irc->idxClass = (*vec_registered_class_node).size();

			irc->classPtrType=str_classPtr;
			irc->idxClass=(*vec_registered_class_node).size();
			irc->metadata_info.object_info.symbol_info.properties=PROPERTY_C_OBJECT_REF;

			irc->metadata_info.object_info.symbol_info.index=(*vec_registered_class_node).size();
			(*vec_registered_class_node).push_back(irc);

			print_info_cr("* class \"%10s\" registered as (%s).",class_name.c_str(),demangle(str_classPtr).c_str());

			return true;
		}
		else{
			print_error_cr("%s already exist", class_name.c_str());
		}

		return false;
	}

	template<class _T, class _B>
	static bool class_C_baseof(){

		string base_class_name=typeid(_B).name();
		string base_class_name_ptr=typeid(_B *).name();
		string class_name=typeid(_T).name();
		string class_name_ptr=typeid(_T *).name();

		int idxBaseClass = getIdxClassFromIts_C_Type(typeid(_B *).name());
		if(idxBaseClass == -1) return false;


		int register_class = getIdxClassFromIts_C_Type(typeid(_T *).name());
		if(register_class == -1) return false;


		// check whether is in fact base of ...
		if(!std::is_base_of<_B,_T>::value){
			print_error_cr("class \"%s\" is not base of \"%s\" ",demangle(class_name).c_str(), demangle(base_class_name).c_str());
			return false;
		}


		for(unsigned i = 0; i < (*vec_registered_class_node)[register_class]->baseClass.size(); i++){
			if((*vec_registered_class_node)[register_class]->baseClass[i]->classPtrType ==base_class_name_ptr){
				print_error_cr("class \"%s\" already base of \"%s\" ",demangle(class_name).c_str(), demangle(base_class_name).c_str());
				return false;
			}
		}


		/*if(!addPrimitiveTypeConversion<_T *,_B *>( [] (CScriptVariable *obj){return (int)reinterpret_cast<_B *>(obj);})){
			return false;
		}*/
	 	if(mapTypeConversion.count(class_name_ptr) == 1){ // create new map...
	 		if(mapTypeConversion[class_name_ptr].count(base_class_name_ptr)==1){
	 			print_error_cr("Conversion type \"%s\" -> \"%s\" already inserted",demangle(class_name).c_str(),demangle(base_class_name).c_str());
	 			return false;
	 		}
	 	}

	 	mapTypeConversion[class_name_ptr][base_class_name_ptr]=[](CScriptVariable *s){ return (int)reinterpret_cast<_B *>(s);};

	 	tInfoScriptClass *irc_base = (*vec_registered_class_node)[idxBaseClass];
	 	tInfoScriptClass *irc_class = (*vec_registered_class_node)[register_class];
	 	irc_class->baseClass.push_back(irc_base);

		// register all symbols function from base ...
		// vars ...
		for(unsigned i = 0; i < irc_base->metadata_info.object_info.local_symbols.m_registeredVariable.size(); i++){

			tInfoVariableSymbol *irs_source = &irc_base->metadata_info.object_info.local_symbols.m_registeredVariable[i];

			tInfoVariableSymbol irs;
			// init struct...
			irs.idxClassInfo = idxBaseClass;//.class_info = (*vec_registered_class_node)[base_class];
			irs.ref_ptr=irs_source->ref_ptr;
			irs.c_type = irs_source->c_type;
			//irs.
			irs.symbol_name=irs_source->symbol_name;
			irs.properties = PROPERTY_C_OBJECT_REF| PROPERTY_IS_DERIVATED;
			irs.index = irc_class->metadata_info.object_info.local_symbols.m_registeredVariable.size();
			irc_class->metadata_info.object_info.local_symbols.m_registeredVariable.push_back(irs);

		}

		// functions ...
		for(unsigned i = 0; i < irc_base->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size(); i++){

			tScriptFunctionObject *irs_source = &irc_base->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[i];

			tScriptFunctionObject irs;
			// init struct...
			irs.object_info.symbol_info.idxAstNode = -1;
			//irs.object_info.symbol_info.idxScopeVar = -1;
			irs.object_info.symbol_info.symbol_name=irs_source->object_info.symbol_info.symbol_name;


			irs.m_arg = irs_source->m_arg;
			irs.idx_return_type = irs_source->idx_return_type;

			irs.object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF | PROPERTY_IS_DERIVATED;

			// ignores special type cast C++ member to ptr function
			// create binding function class
			irs.object_info.symbol_info.ref_ptr= irs_source->object_info.symbol_info.ref_ptr; // this is not correct due the pointer

			irs.object_info.symbol_info.index = irc_class->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size();
			irc_class->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.push_back(irs);


		}
		return true;

	}




	/**
	 * Register C Member function Class
	 */
	template <class _T, typename F>
	static bool register_C_FunctionMemberInt(const char *function_name,F function_type)
	{
		string return_type;
		vector<string> params;
		tScriptFunctionObject *irs=NULL;
		vector<string> m_arg;
		int idx_return_type=-1;
		void *ref_ptr=NULL;
		string str_classPtr = typeid( _T *).name();

		int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

		if(idxRegisterdClass == -1){
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_type)>;
		getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});


		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
			print_error_cr("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name);
			return false;
		}

		for(unsigned int i = 0; i < m_arg.size(); i++){
			if(getIdxClassFromIts_C_Type(m_arg[i])==-1){
				print_error_cr("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(m_arg[i]).c_str(),function_name);
				return false;
			}

		}

		// ignores special type cast C++ member to ptr function
		// create binding function class
		if(idx_return_type == IDX_CLASS_VOID){
			if((ref_ptr=((int)CFunction_C_TypeFactory::getInstance()->c_member_class_function_proxy<_T, void>(m_arg.size(),function_type)))==0){
				return false;
			}
		}else{
			if((ref_ptr=((int)CFunction_C_TypeFactory::getInstance()->c_member_class_function_proxy<_T, int>(m_arg.size(),function_type)))==0){
				return false;
			}
		}

		// ok, function candidate to be added into class...
		irs = CScriptFunctionObjectFactory::newScriptFunctionObject();

		// init struct...
		irs->object_info.symbol_info.idxAstNode = -1;
		//irs.object_info.symbol_info.idxScopeVar = -1;
		irs->object_info.symbol_info.symbol_name=function_name;
		irs->object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF;

		irs->object_info.symbol_info.ref_ptr = ref_ptr;
		irs->m_arg = m_arg;
		irs->idx_return_type = idx_return_type;

		irs->object_info.symbol_info.index = (*vec_registered_class_node)[idxRegisterdClass]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size();
		(*vec_registered_class_node)[idxRegisterdClass]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.push_back(irs->object_info.idxFunctionSymbol);
		print_info_cr("Registered member function name %s::%s",demangle(typeid(_T).name()).c_str(), function_name);

		return true;
	}

	/**
	 * Register C Member var
	 */


	template<typename T, typename U> constexpr size_t offsetOf(U T::*member)
	{
	    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
	}

	template <class _T, typename _V>
	static bool register_C_VariableMemberInt(const char *var_name, unsigned int offset)
	{
		string var_type = typeid(_V *).name(); // we need the pointer type ...
		//decltype(var_type) var;
		//print_info_cr("%s",typeid(var).name());
		//string str_type = typeid(decltype(var_type)).name();

		//unsigned int offset = offsetOf(var_type);


		string return_type;
		vector<string> params;
		tInfoVariableSymbol irs;
		string str_classPtr = typeid( _T *).name();



		int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

		if(idxRegisterdClass == -1){
			return false;
		}

		// 1. check all parameters ok.

		// check valid parameters ...
		if(getIdxClassFromIts_C_Type(var_type) == -1){
			print_error_cr("%s::%s has not valid type (%s)",(*vec_registered_class_node)[idxRegisterdClass]->metadata_info.object_info.symbol_info.symbol_name.c_str(),var_name,demangle(typeid(_V).name()).c_str());
			return false;
		}



		// init struct...
		irs.idxClassInfo = idxRegisterdClass;//(*vec_registered_class_node)[idxRegisterdClass];
		irs.ref_ptr=offset;
		irs.c_type = var_type;
		//irs.
		irs.symbol_name=var_name;


		irs.properties = PROPERTY_C_OBJECT_REF;


		irs.index = (*vec_registered_class_node)[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredVariable.size();
		(*vec_registered_class_node)[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredVariable.push_back(irs);
		//base_info->local_symbols.vec_idx_registeredFunction.push_back(irs);

		return true;


	}

	bool registerBase();

	/**
	 * Save current state and giving the index to restore whenever the programmer wants...
	 * @return: >=0 save state index, -1 error
	 */
	int saveState();

	/**
	 * Restore the state. Warning: All current changes will be losed!
	 * @return: success or not
	 */
	bool restoreState(int state);


private:


	static tPrimitiveType *getPrimitiveTypeFromStr(const string & str);
	static map<string,map<string,fntConversionType>> mapTypeConversion;

	 template<typename _S, typename _D, typename _F>
	 static bool addPrimitiveTypeConversion(_F f){

	 	bool valid_type = false;

	 	// check if any entry is int, *float, *bool , *string, *int or any from factory. Anyelese will be no allowed!
	 	valid_type|=valid_C_PrimitiveType[VOID_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(void).name(),"void",VOID_TYPE};
	 	valid_type|=valid_C_PrimitiveType[INT_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(int).name(),"int",INT_TYPE};
	 	valid_type|=valid_C_PrimitiveType[INT_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(int *).name(),"int *",INT_PTR_TYPE};
	 	valid_type|=valid_C_PrimitiveType[FLOAT_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(float *).name(),"float *",FLOAT_PTR_TYPE};
	 	valid_type|=valid_C_PrimitiveType[STRING_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(string *).name(),"string *",STRING_PTR_TYPE};
	 	valid_type|=valid_C_PrimitiveType[BOOL_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(bool *).name(),"bool *",BOOL_PTR_TYPE};

	 	if(!valid_type){
	 		print_error_cr("Conversion type \"%s\" not valid",typeid(_D).name());
	 		return false;
	 	}



	 	if(mapTypeConversion.count(typeid(_S).name()) == 1){ // create new map...
	 		if(mapTypeConversion[typeid(_S).name()].count(typeid(_D).name())==1){
	 			print_error_cr("type conversion \"%s\" to \"%s\" already inserted",typeid(_S).name(),typeid(_D).name());
	 			return false;
	 		}
	 	}

	 	mapTypeConversion[typeid(_S).name()][typeid(_D).name()]=f;

	 	return true;
	 	//typeConversion["P7CNumber"]["Ss"](&n);
	 }

	//int getIdxRegisteredClass_Internal(const string & class_name);
	//int getIdxFunctionSymbol_Internal(const string & class_name,const string & function_name);
	//int getIdxRegisteredVariableSymbol_Internal(const string & class_name,const string & var_name);

	//vector<tInfoScriptClass *>  	 (*vec_registered_class_node);
	//CScriptVariable * createObjectFromPrimitiveType(tPrimitiveType *pt);

	 static bool searchVarFunctionSymbol(int idxFunction, tInfoAsmOp *iao, int current_idx_function,SCOPE_TYPE scope_type=SCOPE_TYPE::UNKNOWN_SCOPE);

	 static bool buildScopeVariablesBlock(tScriptFunctionObject *root_class_irfs );
	 static void unloadRecursiveFunctions(tScriptFunctionObject * info_function);

	 static bool updateFunctionSymbols(int idxSxriptFunctionObject, const string & parent_symbol, int n_function);// is_main_class, bool is_main_function);



	//CScriptClass();
	//~CScriptClass();





};
