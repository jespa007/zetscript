#pragma once



#include "ScriptDefinesStructs.h"
#include "RegisterFunctionHelper.h"
#include "ast/ast.h"

#define register_C_Function(s) CScriptClassFactory::register_C_FunctionInt(STR(s),s)
#define register_C_Variable(s) CScriptClassFactory::register_C_VariableInt(STR(s),&s,typeid(s).name())

#define getIdxGlobalVariable(s)  CScriptClassFactory::register_C_FunctionInt(STR(s),s)
#define getIdxGlobalFunction(s)



#define register_C_VariableMember(o,s) 		register_C_VariableMemberInt<o>(STR(s),typeid(decltype(o::s)*).name(),offsetOf(&o::s))
#define register_C_FunctionMember(o,s)		register_C_FunctionMemberInt<o>(STR(s),&o::s)


#define NEW_CLASS_VAR_BY_IDX(idx) (CScriptClassFactory::getInstance()->newClassByIdx(idx))

class CScopeInfo;

class CScriptClassFactory{


	int idxClassInteger, idxClassNumber, idxClassString, idxClassBoolean, idxClassVector, idxClassFunctor, idxClassUndefined, idxClassVoid;

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
		string      human_description_str;
		C_TYPE_VALID_PRIMITIVE_VAR  id;
	}tPrimitiveType;

	typedef struct{
		tPrimitiveType 				*return_type;
		vector<tPrimitiveType*>		params;
	}tRegisterFunction;

	static tPrimitiveType valid_C_PrimitiveType[MAX_C_TYPE_VALID_PRIMITIVE_VAR];
	static void registerPrimitiveTypes();
	bool isTypeStrValid(const string & type_str);
	static CScriptClassFactory*  getInstance();
	static void destroySingletons();

	/**
	 * This function registers a script class into factory.
	 */
	tInfoRegisteredClass * registerScriptClass(const string & class_name, const string & base_class_name, PASTNode _ast);

	/**
	 * Class name given this function creates the object and initializes all variables.
	 */
	CScriptVariable 		 * newClass(const string & class_name);
	CScriptVariable 		 * newClassByIdx(unsigned idx);



	bool updateReferenceSymbols();



	tInfoRegisteredVariableSymbol  * registerVariableSymbol(const string & class_name,const string & name,PASTNode  node);
	tInfoRegisteredVariableSymbol *  getRegisteredVariableSymbol(const string & class_name,const string & varname);
	int 							 getIdxRegisteredVariableSymbol(const string & class_name,const string & varname, bool show_msg=true);
	int 							 getIdxRegisteredVariableSymbol(tScriptFunctionInfo *irf,const string & var_name, bool show_msg=true);


	tInfoRegisteredFunctionSymbol *  registerFunctionSymbol(const string & class_name, const string & name,PASTNode  node);
	tInfoRegisteredFunctionSymbol *  getRegisteredFunctionSymbol(const string & class_name,const string & function_name, bool show_errors=true);
	int 							 getIdxRegisteredFunctionSymbol(tScriptFunctionInfo *irf,const string & function_name, bool show_msg=true);




	bool addArgumentFunctionSymbol(const string & class_name,const string & function_name,const string & arg_name);


	tInfoRegisteredClass * 	getRegisteredClass(const string & v, bool print_msg=true);
	tInfoRegisteredClass *	getRegisteredClassByIdx(unsigned index);

	int 					getIdxRegisteredClass(const string & v);
	bool isClassRegistered(const string & v);
	fntConversionType getConversionType(string objectType, string conversionType);

	const char * getNameRegisteredClassByIdx(int idx);


	// internal var types ...
	int getIdxClassVoid(){return idxClassVoid;}
	int getIdxClassUndefined(){return idxClassUndefined;}
	int getIdxClassInteger(){return idxClassInteger;}
	int getIdxClassNumber(){return idxClassNumber;}
	int getIdxClassString(){return idxClassString;}
	int getIdxClassBoolean(){return idxClassBoolean;}
	int getIdxClassVector(){return idxClassVector;}
	int getIdxClassFunctor(){return idxClassFunctor;}


	void printGeneratedCodeAllClasses();
	/**
	 * Register C function
	 */
	template <typename F>
	bool register_C_FunctionInt(const string & function_name,F function_ptr)
	{
		string return_type;
		vector<string> params;
		tInfoRegisteredFunctionSymbol irs;

		//tPrimitiveType *rt;
		//vector<tPrimitiveType *> pt;
		//CScopeInfo::tInfoScopeVar  *rs;

		//CScriptVariable *sf=NULL;
		tInfoRegisteredFunctionSymbol * mainFunctionInfo = getRegisteredFunctionSymbol(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_NAME);
		//tInfoRegisteredClass *rc = CScriptClassFactory::getInstance()->getRegisteredClass(class_name);

		if(mainFunctionInfo == NULL){
			print_error_cr("main function is not created");
			exit(EXIT_FAILURE);
		}




		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,irs.return_type, irs.m_arg, make_index_sequence<Traits3::arity>{});


		// check valid parameters ...
		if(!isTypeStrValid(irs.return_type)){
			print_info_cr("Return type \"%s\" for function \"%s\" is not valid",irs.return_type.c_str(),function_name.c_str());
			return false;
		}

		for(unsigned int i = 0; i < irs.m_arg.size(); i++){
			if(!isTypeStrValid(irs.m_arg[i])){
				print_info_cr("Argument (%i) type \"%s\" for function \"%s\" is not valid",i,irs.m_arg[i].c_str(),function_name.c_str());
				return false;
			}

		}

		// init struct...

		irs.object_info.symbol_info.ast = NULL;
		irs.object_info.symbol_info.info_var_scope = NULL;
		irs.object_info.symbol_info.symbol_name = function_name;

		irs.object_info.symbol_info.properties = ::PROPERTY_C_OBJECT_REF;
		irs.object_info.symbol_info.ref_ptr=(int)function_ptr;


		if((irs.object_info.symbol_info.info_var_scope=CAst::getInstance()->getRootScopeInfo()->registerSymbol(function_name))==NULL){
			return false;
		}



		mainFunctionInfo->object_info.local_symbols.m_registeredFunction.push_back(irs);

		print_info_cr("Registered function name: %s",function_name);

		return true;
	}


	/**
	 * Register C variable
	 */
	bool register_C_VariableInt(const string & var_str,void * var_ptr, const string & var_type);



	int getIdx_C_RegisteredClass(const string & str_classPtr, bool print_msg=true){
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

	int getIdx_C_RegisteredFunctionMemberClass(const string & str_classPtr, const string & str_functionName, bool print_msg=true){

			int index_class = getIdx_C_RegisteredClass(str_classPtr,print_msg);

			if(index_class == -1){
				return -1;
			}


			vector<tInfoRegisteredFunctionSymbol> * vec_irfs = &m_registeredClass[index_class]->metadata_info.object_info.local_symbols.m_registeredFunction;

			// ok check c_type
			for(unsigned i = 0; i < vec_irfs->size(); i++){
				if(vec_irfs->at(i).object_info.symbol_info.ast->value_symbol == str_classPtr){
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
	bool register_C_Class(const string & class_name, const string & base_class_name=""){

		tInfoRegisteredClass *base_class = NULL;

		if(base_class_name != ""){
			if((base_class = this->getRegisteredClass(base_class_name)) == NULL){
				return false;
			}
		}

		if(!isClassRegistered(class_name)){

			string str_classPtr = typeid( _T *).name();

			if(getIdx_C_RegisteredClass(str_classPtr,false)!=-1){
				print_error_cr("this %s is already registered");
				return false;
			}


			//print_error_cr("CHECK AND TODOOOOOO!");
			tInfoRegisteredClass *irc = new tInfoRegisteredClass;

			irc->metadata_info.object_info.symbol_info.ast = new tASTNode;
			irc->metadata_info.object_info.symbol_info.info_var_scope=NULL;
			irc->metadata_info.object_info.symbol_info.symbol_name = class_name;
			irc->baseClass = base_class; // identify extend class ?!?!!?

			irc->c_constructor = new std::function<void *()>([](){return new _T();});
			irc->c_destructor = new std::function<void (void *)>([](void *p){delete (_T *)p;});

			irc->class_idx = m_registeredClass.size();

			irc->classPtrType=str_classPtr;
			irc->class_idx=m_registeredClass.size();
			irc->metadata_info.object_info.symbol_info.properties=PROPERTY_C_OBJECT_REF;
			m_registeredClass.push_back(irc);

			print_info_cr("* class \"%10s\" registered (C ref: \"%s\").",class_name.c_str(),str_classPtr.c_str());

			return true;
		}
		else{
			print_error_cr("%s already exist", class_name);
		}

		return false;
	}

	/**
	 * Register C Member function Class
	 */
	template <class _T, typename F>
	bool register_C_FunctionMemberInt(const char *function_name,F function_ptr)
	{
		string return_type;
		vector<string> params;
		tInfoRegisteredFunctionSymbol irs;
		string str_classPtr = typeid( _T *).name();

		int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

		if(idxRegisterdClass == -1){
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,irs.return_type, irs.m_arg, make_index_sequence<Traits3::arity>{});


		// check valid parameters ...
		if(!isTypeStrValid(irs.return_type)){
			print_info_cr("Return type \"%s\" for function \"%s\" is not valid",irs.return_type.c_str(),function_name);
			return false;
		}

		for(unsigned int i = 0; i < irs.m_arg.size(); i++){
			if(!isTypeStrValid(irs.m_arg[i])){
				print_info_cr("Argument (%i) type \"%s\" for function \"%s\" is not valid",i,irs.m_arg[i].c_str(),function_name);
				return false;
			}

		}

		// init struct...
		irs.object_info.symbol_info.ast = NULL;
		irs.object_info.symbol_info.info_var_scope = NULL;
		irs.object_info.symbol_info.symbol_name=function_name;


		irs.object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF;

		// ignores special type cast C++ member to ptr function
#pragma GCC diagnostic ignored "-Wpmf-conversions"
		irs.object_info.symbol_info.ref_ptr=(unsigned int)((void *)function_ptr);
#pragma GCC diagnostic warning "-Wpmf-conversions"

		/*if((irs.object_info.symbol_info.info_var_scope=CAst::getInstance()->getRootScopeInfo()->registerSymbol(function_name))==NULL){
			return false;
		}*/




		m_registeredClass[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredFunction.push_back(irs);
		//base_info->local_symbols.m_registeredFunction.push_back(irs);

		return true;
	}

	/**
	 * Register C Member var
	 */


	template<typename T, typename U> constexpr size_t offsetOf(U T::*member)
	{
	    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
	}

	template <class _T>
	bool register_C_VariableMemberInt(const char *var_name, const string & var_type, unsigned int offset)
	{
		//decltype(var_type) var;
		//print_info_cr("%s",typeid(var).name());
		//string str_type = typeid(decltype(var_type)).name();

		//unsigned int offset = offsetOf(var_type);


		string return_type;
		vector<string> params;
		tInfoRegisteredVariableSymbol irs;
		string str_classPtr = typeid( _T *).name();





		int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

		if(idxRegisterdClass == -1){
			return false;
		}

		// 1. check all parameters ok.

		// check valid parameters ...
		if(!isTypeStrValid(var_type)){
			print_info_cr("%s::%s has not valid type (%s)",m_registeredClass[idxRegisterdClass]->metadata_info.object_info.symbol_info.symbol_name.c_str(),var_name,var_type.c_str());
			return false;
		}



		// init struct...
		irs.class_info = m_registeredClass[idxRegisterdClass];
		irs.ref_ptr=offset;
		//irs.
		irs.symbol_name=var_name;


		irs.properties = PROPERTY_C_OBJECT_REF;



		m_registeredClass[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredVariable.push_back(irs);
		//base_info->local_symbols.m_registeredFunction.push_back(irs);

		return true;


	}


private:

	static CScriptClassFactory *scriptClassFactory;
	static tPrimitiveType *getPrimitiveTypeFromStr(const string & str);


	int getIdxRegisteredClass_Internal(const string & class_name);
	//int getIdxRegisteredFunctionSymbol_Internal(const string & class_name,const string & function_name);
	//int getIdxRegisteredVariableSymbol_Internal(const string & class_name,const string & var_name);

	vector<tInfoRegisteredClass *>  	 m_registeredClass;
	//CScriptVariable * createObjectFromPrimitiveType(tPrimitiveType *pt);

	bool searchVarFunctionSymbol(tScriptFunctionInfo *script_info, tInfoAsmOp *iao);
	bool registerBase();

	//---------------
	// PRINT ASM INFO
	char print_aux_load_value[1024*8];
	const char * getStrMovVar(tInfoAsmOp * iao);
	const char * getStrTypeLoadValue(vector<tInfoStatementOp> * m_listStatements,int current_statment, int current_instruction);
	void printGeneratedCode_Recursive(tScriptFunctionInfo *fs);
	void printGeneratedCode(tScriptFunctionInfo *fs);
	// PRINT ASM INFO
	//---------------

	CScriptClassFactory();
	~CScriptClassFactory();



};
