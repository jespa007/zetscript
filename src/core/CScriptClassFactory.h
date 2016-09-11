#pragma once



#include "ScriptDefinesStructs.h"
#include "RegisterFunctionHelper.h"
#include "ast/ast.h"

#define registerGlobal_C_Function(s) CScriptClassFactory::register_C_Function(MAIN_SCRIPT_CLASS_NAME,STR(s),s)
#define registerGlobal_C_Variable(s) CScriptClassFactory::register_C_Variable(MAIN_SCRIPT_CLASS_NAME,STR(s),s,typeid(s).name())



class CScopeInfo;

class CScriptClassFactory{





public:


	enum C_TYPE_VAR{
		VOID_TYPE,
		INT_TYPE,
		INT_PTR_TYPE,
		FLOAT_TYPE,
		FLOAT_PTR_TYPE,
		STRING_TYPE,
		STRING_PTR_TYPE,
		BOOL_TYPE,
		BOOL_PTR_TYPE,
		MAX_VAR_C_TYPES
	};



	typedef struct{
		string 	    type_str;
		string      human_description_str;
		C_TYPE_VAR  id;
	}tPrimitiveType;

	typedef struct{
		tPrimitiveType 				*return_type;
		vector<tPrimitiveType*>		params;
	}tRegisterFunction;

	static tPrimitiveType primitiveType[MAX_VAR_C_TYPES];
	static void registerPrimitiveTypes();
	static CScriptClassFactory*  getInstance();
	static void destroySingletons();

	/**
	 * This function registers a script class into factory.
	 */
	tInfoRegisteredClass * registerScriptClass(const string & class_name);

	/**
	 * Class name given this function creates the object and initializes all variables.
	 */
	CScriptClass 		 * newClass(const string & class_name);




	tInfoRegisteredVariableSymbol  * registerVariableSymbol(const string & class_name,const string & name,PASTNode  node);
	tInfoRegisteredVariableSymbol *  getRegisteredVariableSymbol(const string & class_name,const string & varname);
	//int 							 getIdxRegisteredVariableSymbol(const string & class_name,const string & varname);


	tInfoRegisteredFunctionSymbol *  registerFunctionSymbol(const string & class_name, const string & name);
	tInfoRegisteredFunctionSymbol *  registerFunctionSymbol(const string & class_name, const string & name,PASTNode  node);

	tInfoRegisteredFunctionSymbol *  getRegisteredFunctionSymbol(const string & class_name,const string & function_name, bool show_errors=true);
	//int 							 getIdxRegisteredFunctionSymbol(const string & class_name,const string & function_name);


	bool addArgumentFunctionSymbol(const string & class_name,const string & function_name,const string & arg_name);


	tInfoRegisteredClass * 	getRegisteredClass(const string & v, bool print_msg=true);
	int 					getIdxRegisteredClass(const string & v);
	bool isClassRegistered(const string & v);
	fntConversionType getConversionType(string objectType, string conversionType);

	const char * getNameRegisteredClassByIdx(int idx);




	/**
	 * Register C function
	 */
	template <typename F>
	static bool register_C_Function(const string & class_name, const string & function_name,F function_ptr)
	{
		string return_type;
		vector<string> params;
		tInfoRegisteredFunctionSymbol irs;

		//tPrimitiveType *rt;
		//vector<tPrimitiveType *> pt;
		//CScopeInfo::tInfoScopeVar  *rs;

		//CScriptClass *sf=NULL;

		tInfoRegisteredClass *rc = CScriptClassFactory::getInstance()->getRegisteredClass(class_name);

		if(rc == NULL){
			return false;
		}

		// init struct...
		irs.object_info.symbol_info.ast = NULL;
		irs.object_info.symbol_info.info_var_scope = NULL;

		irs.object_info.symbol_info.properties = ::C_OBJECT_REF;
		irs.object_info.symbol_info.ref_aux=(int)function_ptr;


		if((irs.object_info.symbol_info.info_var_scope=CAst::getInstance()->getRootScopeInfo()->registerSymbol(function_name))==NULL){
			return false;
		}



		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,irs.return_type, irs.m_arg, make_index_sequence<Traits3::arity>{});

		rc->object_info.local_symbols.m_registeredFunction.push_back(irs);

		print_info_cr("Registered function name: %s",function_name);

		return true;
	}


	/**
	 * Register C variable
	 */
	void register_C_Variable(const string & class_name, const string & var_str,void * var_ptr, const string & var_type);



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


			vector<tInfoRegisteredFunctionSymbol> * vec_irfs = &m_registeredClass[index_class]->object_info.local_symbols.m_registeredFunction;

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
	bool register_C_Class(const string & class_name){
		if(!isClassRegistered(class_name)){

			string str_classPtr = typeid( _T *).name();

			if(getIdx_C_RegisteredClass(str_classPtr,false)!=-1){
				print_error_cr("this %s is already registered");
			}

			//print_error_cr("CHECK AND TODOOOOOO!");
			tInfoRegisteredClass *irc = new tInfoRegisteredClass;

			irc->object_info.symbol_info.ast = new tASTNode;
			irc->object_info.symbol_info.info_var_scope=NULL;
			irc->object_info.symbol_info.symbol_name = class_name;

			irc->classPtrType=str_classPtr;
			irc->object_info.symbol_info.properties=C_OBJECT_REF;
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
	bool register_C_FunctionMember(const char *function_name,F function_ptr)
	{
		string return_type;
		vector<string> params;
		tInfoRegisteredFunctionSymbol irs;
		string str_classPtr = typeid( _T *).name();

		int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

		if(idxRegisterdClass == -1){
			return false;
		}

		// init struct...
		irs.object_info.symbol_info.ast = new tASTNode;
		irs.object_info.symbol_info.ast->value_symbol=function_name;
		irs.object_info.symbol_info.info_var_scope = NULL;

		irs.object_info.symbol_info.properties = C_OBJECT_REF;

		// ignores special type cast C++ member to ptr function
#pragma GCC diagnostic ignored "-Wpmf-conversions"
		irs.object_info.symbol_info.ref_aux=(int)((void *)function_ptr);
#pragma GCC diagnostic warning "-Wpmf-conversions"

		if((irs.object_info.symbol_info.info_var_scope=CAst::getInstance()->getRootScopeInfo()->registerSymbol(function_name))==NULL){
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,irs.return_type, irs.m_arg, make_index_sequence<Traits3::arity>{});


		m_registeredClass[idxRegisterdClass]->object_info.local_symbols.m_registeredFunction.push_back(irs);
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
	CVariable * createObjectFromPrimitiveType(tPrimitiveType *pt);

	CScriptClassFactory();
	~CScriptClassFactory();



};
