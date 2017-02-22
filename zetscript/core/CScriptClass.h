#pragma once


#include "C_VariableFunctionFactory.h"
#include "ScriptDefinesStructs.h"
#include "RegisterFunctionHelper.h"
#include "ast/ast.h"

#define register_C_Function(s) CScriptClass::register_C_FunctionInt(STR(s),s)
#define register_C_Variable(s) CScriptClass::register_C_VariableInt(STR(s),&s,typeid(decltype(s) *).name())

#define getIdxGlobalVariable(s)  CScriptClass::register_C_FunctionInt(STR(s),s)
#define getIdxGlobalFunction(s)



#define register_C_VariableMember(o,s) 		register_C_VariableMemberInt<o, decltype(o::s)>(STR(s),offsetOf(&o::s))
#define register_C_FunctionMember(o,s)		register_C_FunctionMemberInt<o>(STR(s),&o::s)



#define NEW_CLASS_VAR_BY_IDX(idx) (CScriptClass::getInstance()->newClassByIdx(idx))

class CScope;

class CScriptClassFactory{


	static int idxClassInteger,
		idxClassNumber,
		idxClassString,
		idxClassBoolean,
		idxClassVector,
		idxClassFunctor,
		idxClassUndefined,
		idxClassVoid,
		idxClassNull,
		idxClassStruct;


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
	static int getIdxClassFromIts_C_Type(const string & c_type_str);

	//static CScriptClass*  getInstance();
	static void destroySingletons();

	/**
	 * This function registers a script class into factory.
	 */
	static tInfoRegisteredClass * registerScriptClass(const string & class_name, const string & base_class_name, PASTNode _ast);

	/**
	 * Class name given this function creates the object and initializes all variables.
	 */
	static CScriptVariable 		 * newClass(const string & class_name);
	static CScriptVariable 		 * newClassByIdx(unsigned idx, void * value_object = NULL);



	static bool updateReferenceSymbols();



	static tInfoVariableSymbol  * registerVariableSymbol(const string & class_name,const string & name,PASTNode  node);
	static tInfoVariableSymbol *  getRegisteredVariableSymbol(const string & class_name,const string & varname);
	static int 							 getIdxRegisteredVariableSymbol(const string & class_name,const string & varname, bool show_msg=true);
	static int 							 getIdxRegisteredVariableSymbol(tFunctionInfo *irf,const string & var_name, bool show_msg=true);


	static tScriptFunctionObject *  registerFunctionSymbol(const string & class_name, const string & name,PASTNode  node);
	static int getIdxScriptFunctionObjectByClassFunctionName(const string & class_name,const string & function_name, bool show_errors=true);



	static tFunctionInfo *  getSuperClass(tInfoRegisteredClass *irc, const string & fun_name);


	static bool addArgumentFunctionSymbol(const string & class_name,const string & function_name,const string & arg_name);


	//tInfoRegisteredClass * 	getRegisteredClass(const string & v, bool print_msg=true);
	//tInfoRegisteredClass *	getRegisteredClassByIdx(unsigned index);
	//tInfoRegisteredClass *  getRegisteredClassBy_C_ClassPtr(const string & v, bool print_msg=true);

	static int 					getIdxRegisteredClass(const string & v, bool print_msg=true);
	static bool isClassRegistered(const string & v);
	static fntConversionType getConversionType(string objectType, string conversionType, bool show_errors=true);

	static const char * getNameRegisteredClassByIdx(int idx);


	// internal var types ...
	static int getIdxClassVoid(){return idxClassVoid;}
	static tInfoRegisteredClass *  getRegisteredClassVoid(){return m_registeredClass[idxClassVoid];}

	static int getIdxClassUndefined(){return idxClassUndefined;}
	static tInfoRegisteredClass *  getRegisteredClassUndefined(){return m_registeredClass[idxClassUndefined];}

	static int getIdxClassInteger(){return idxClassInteger;}
	static tInfoRegisteredClass *  getRegisteredClassInteger(){return m_registeredClass[idxClassInteger];}

	static int getIdxClassNumber(){return idxClassNumber;}
	static tInfoRegisteredClass *  getRegisteredClassNumber(){return m_registeredClass[idxClassNumber];}

	static int getIdxClassStruct(){return idxClassStruct;}
	static tInfoRegisteredClass *  getRegisteredClassStruct(){return m_registeredClass[idxClassStruct];}

	static int getIdxClassString(){return idxClassString;}
	static tInfoRegisteredClass *  getRegisteredClassString(){return m_registeredClass[idxClassString];}

	static int getIdxClassBoolean(){return idxClassBoolean;}
	static tInfoRegisteredClass *  getRegisteredClassBoolean(){return m_registeredClass[idxClassBoolean];}

	static int getIdxClassVector(){return idxClassVector;}
	static tInfoRegisteredClass *  getRegisteredClassVector(){return m_registeredClass[idxClassVector];}

	static int getIdxClassFunctor(){return idxClassFunctor;}
	static tInfoRegisteredClass *  getRegisteredClassFunctor(){return m_registeredClass[idxClassFunctor];}

	static int getIdxClassNull(){return idxClassNull;}
	static tInfoRegisteredClass *  getRegisteredClassNull(){return m_registeredClass[idxClassNull];}






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
		//tInfoRegisteredClass *rc = CScriptClass::getInstance()->getRegisteredClass(class_name);

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
		if(irs.idx_return_type == getIdxClassVoid()){
			if((irs.object_info.symbol_info.ref_ptr=(int)C_VariableFunctionFactory::getInstance()->new_proxy_function<void>(irs.m_arg.size(),function_ptr))==0){//(int)function_ptr;
				return false;
			}
		}
		else{
			if((irs.object_info.symbol_info.ref_ptr=(int)C_VariableFunctionFactory::getInstance()->new_proxy_function<int>(irs.m_arg.size(),function_ptr))==0){//(int)function_ptr;
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
			for(unsigned i = 0; i < m_registeredClass.size(); i++){
				if(m_registeredClass[i]->classPtrType == str_classPtr){
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


			vector<tScriptFunctionObject> * vec_irfs = &m_registeredClass[index_class]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction;

			// ok check c_type
			for(unsigned i = 0; i < vec_irfs->size(); i++){
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

		/*tInfoRegisteredClass *base_class = NULL;

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
			tInfoRegisteredClass *irc = new tInfoRegisteredClass;

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

			irc->idxClass = m_registeredClass.size();

			irc->classPtrType=str_classPtr;
			irc->idxClass=m_registeredClass.size();
			irc->metadata_info.object_info.symbol_info.properties=PROPERTY_C_OBJECT_REF;

			irc->metadata_info.object_info.symbol_info.index=m_registeredClass.size();
			m_registeredClass.push_back(irc);

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

		int idxBaseClass = this->getIdxClassFromIts_C_Type(typeid(_B *).name());
		if(idxBaseClass == -1) return false;


		int register_class = this->getIdxClassFromIts_C_Type(typeid(_T *).name());
		if(register_class == -1) return false;


		// check whether is in fact base of ...
		if(!std::is_base_of<_B,_T>::value){
			print_error_cr("class \"%s\" is not base of \"%s\" ",demangle(class_name).c_str(), demangle(base_class_name).c_str());
			return false;
		}


		for(unsigned i = 0; i < m_registeredClass[register_class]->baseClass.size(); i++){
			if(m_registeredClass[register_class]->baseClass[i]->classPtrType ==base_class_name_ptr){
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

	 	tInfoRegisteredClass *irc_base = m_registeredClass[idxBaseClass];
	 	tInfoRegisteredClass *irc_class = m_registeredClass[register_class];
	 	irc_class->baseClass.push_back(irc_base);

		// register all symbols function from base ...
		// vars ...
		for(unsigned i = 0; i < irc_base->metadata_info.object_info.local_symbols.m_registeredVariable.size(); i++){

			tInfoVariableSymbol *irs_source = &irc_base->metadata_info.object_info.local_symbols.m_registeredVariable[i];

			tInfoVariableSymbol irs;
			// init struct...
			irs.idxClassInfo = idxBaseClass;//.class_info = m_registeredClass[base_class];
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
			irs.object_info.symbol_info.idxScopeVar = -1;
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
		tScriptFunctionObject irs;
		string str_classPtr = typeid( _T *).name();

		int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

		if(idxRegisterdClass == -1){
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_type)>;
		getParamsFunction<Traits3>(0,return_type, irs.m_arg, make_index_sequence<Traits3::arity>{});


		// check valid parameters ...
		if((irs.idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
			print_error_cr("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name);
			return false;
		}

		for(unsigned int i = 0; i < irs.m_arg.size(); i++){
			if(getIdxClassFromIts_C_Type(irs.m_arg[i])==-1){
				print_error_cr("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(irs.m_arg[i]).c_str(),function_name);
				return false;
			}

		}

		// init struct...
		irs.object_info.symbol_info.idxAstNode = -1;
		irs.object_info.symbol_info.idxScopeVar = -1;
		irs.object_info.symbol_info.symbol_name=function_name;


		irs.object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF;

		// ignores special type cast C++ member to ptr function
		// create binding function class
		if(irs.idx_return_type == getIdxClassVoid()){
			if((irs.object_info.symbol_info.ref_ptr=((int)C_VariableFunctionFactory::getInstance()->c_member_class_function_proxy<_T, void>(irs.m_arg.size(),function_type)))==0){
				return false;
			}
		}else{
			if((irs.object_info.symbol_info.ref_ptr=((int)C_VariableFunctionFactory::getInstance()->c_member_class_function_proxy<_T, int>(irs.m_arg.size(),function_type)))==0){
				return false;
			}
		}

		irs.object_info.symbol_info.index = m_registeredClass[idxRegisterdClass]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size();
		m_registeredClass[idxRegisterdClass]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.push_back(irs);
		//base_info->local_symbols.vec_idx_registeredFunction.push_back(irs);
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
			print_error_cr("%s::%s has not valid type (%s)",m_registeredClass[idxRegisterdClass]->metadata_info.object_info.symbol_info.symbol_name.c_str(),var_name,demangle(typeid(_V).name()).c_str());
			return false;
		}



		// init struct...
		irs.idxClassInfo = idxRegisterdClass;//m_registeredClass[idxRegisterdClass];
		irs.ref_ptr=offset;
		irs.c_type = var_type;
		//irs.
		irs.symbol_name=var_name;


		irs.properties = PROPERTY_C_OBJECT_REF;


		irs.index = m_registeredClass[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredVariable.size();
		m_registeredClass[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredVariable.push_back(irs);
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

	static CScriptClass *scriptClassFactory;
	static tPrimitiveType *getPrimitiveTypeFromStr(const string & str);
	static map<string,map<string,fntConversionType>> mapTypeConversion;

	 template<typename _S, typename _D, typename _F>
	 static bool addPrimitiveTypeConversion(_F f){

	 	bool valid_type = false;

	 	// check if any entry is int, *float, *bool , *string, *int or any from factory. Anyelese will be no allowed!
	 	valid_type|=CScriptClass::valid_C_PrimitiveType[CScriptClass::VOID_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(void).name(),"void",VOID_TYPE};
	 	valid_type|=CScriptClass::valid_C_PrimitiveType[CScriptClass::INT_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(int).name(),"int",INT_TYPE};
	 	valid_type|=CScriptClass::valid_C_PrimitiveType[CScriptClass::INT_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(int *).name(),"int *",INT_PTR_TYPE};
	 	valid_type|=CScriptClass::valid_C_PrimitiveType[CScriptClass::FLOAT_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(float *).name(),"float *",FLOAT_PTR_TYPE};
	 	valid_type|=CScriptClass::valid_C_PrimitiveType[CScriptClass::STRING_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(string *).name(),"string *",STRING_PTR_TYPE};
	 	valid_type|=CScriptClass::valid_C_PrimitiveType[CScriptClass::BOOL_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(bool *).name(),"bool *",BOOL_PTR_TYPE};

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

	//vector<tInfoRegisteredClass *>  	 m_registeredClass;
	//CScriptVariable * createObjectFromPrimitiveType(tPrimitiveType *pt);

	 static bool searchVarFunctionSymbol(int idxFunction, tInfoAsmOp *iao, int current_idx_function,SCOPE_TYPE scope_type=SCOPE_TYPE::UNKNOWN_SCOPE);

	 static bool buildScopeVariablesBlock(tScriptFunctionObject *root_class_irfs );
	 static void unloadRecursiveFunctions(tScriptFunctionObject * info_function);

	 static bool updateFunctionSymbols(int idxSxriptFunctionObject, const string & parent_symbol, int n_function);// is_main_class, bool is_main_function);



	//CScriptClass();
	//~CScriptClass();



};
