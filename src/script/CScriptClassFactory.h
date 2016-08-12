#pragma once



#include "ScriptDefinesStructs.h"
#include "RegisterFunctionHelper.h"
#include "ast/ast.h"

#define registerGlobal_C_Function(s) CScriptClassFactory::register_C_Function(CZG_Script::getMainObjectInfo(),STR(s),s)
#define registerGlobal_C_Variable(s) CScriptClassFactory::register_C_Variable(CZG_Script::getMainObjectInfo(),STR(s),s,typeid(s).name())



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

	tInfoRegisteredVariableSymbol  * registerVariableSymbol(tScriptFunctionInfo *object_info,PASTNode  node);
	tInfoRegisteredFunctionSymbol * registerFunctionSymbol(tScriptFunctionInfo *object_info,PASTNode  node, unsigned int properties);
	tInfoRegisteredFunctionSymbol *  getRegisteredFunctionSymbol(tScriptFunctionInfo *object_info,unsigned idx);
	bool addArgumentFunctionSymbol(tScriptFunctionInfo *object_info,unsigned idxFunction,const string & arg_name);


	tInfoRegisteredClass * getRegisteredClass(const string & v, bool print_msg=true);
	int getIdxRegisteredClass(const string & v);
	int registeredClassExists(const string & class_name);
	tInfoRegisteredClass * registerClass(const string & class_name);
	fntConversionType getConversionType(string objectType, string conversionType);



	CScriptClass *newClass(const string & class_name);

	/**
	 * Register C function
	 */
	template <typename F>
	static bool register_C_Function(tScriptFunctionInfo * base_info, const char *function_name,F function_ptr)
	{
		string return_type;
		vector<string> params;
		tInfoRegisteredFunctionSymbol irs;

		//tPrimitiveType *rt;
		//vector<tPrimitiveType *> pt;
		//CScopeInfo::tInfoScopeVar  *rs;

		//CScriptClass *sf=NULL;

		//tInfoRegisteredClass *rc = getRegisteredClass(class_name);

		/*if(rc != NULL){
			return;
		}*/

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

		base_info->local_symbols.m_registeredFunction.push_back(irs);

		// check whether function is ok or not.
		/*if((rt=getPrimitiveTypeFromStr(return_type)) == NULL){return;}

		for(unsigned int i=0; i < params.size(); i++){
			if((rt=getPrimitiveTypeFromStr(params[i])) == NULL){
				return;
			}
			irs.m_arg.push_back();


			//pt.push_back(rt);

		}*/

		// 2. create script function ...
		/*if((rs = this->m_mainClass->getScope()->registerSymbol(function_str,0)) == NULL) // already registered
			return;

		// 3. all test ok. Time to create script function ..

		sf=new CScriptClass(m_mainClass);

		// register return type ...
		sf->setReturnObject(createObjectFromPrimitiveType(rt));

		sf->setName(function_str);

		// set function pointer...
		sf->setupAsFunctionPointer((void *)function_ptr);

		// assign object;
		rs->m_obj = sf;

		// register all args...
		for(unsigned int i=0; i < params.size(); i++){
			sf->add_C_function_argument(params[i]);
		}*/

		print_info_cr("Registered function name: %s",function_name);

		return true;

	 /* int temp=0;
	  f(temp);
	  std::cout << "Result is " << temp << std::endl;*/
	}


	/**
	 * Register C variable
	 */
	void register_C_Variable(const string & class_name, const string & var_str,void * var_ptr, const string & var_type);



	/**
	 * Register C Class
	 */
	template<class _T>
	bool register_C_Class(const string & class_name){
		if(registeredClassExists(class_name)==-1){

			print_error_cr("CHECK AND TODOOOOOO!");
			// ok check c_type
			/*for(int i = 0; i < m_registeredClass.size(); i++){
				m_registeredClass[i]->
			}*/
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
	static bool register_C_FunctionMember(const char *function_name,F function_ptr)
	{
		string return_type;
		vector<string> params;
		tInfoRegisteredFunctionSymbol irs;

		//tPrimitiveType *rt;
		//vector<tPrimitiveType *> pt;
		//CScopeInfo::tInfoScopeVar  *rs;

		//CScriptClass *sf=NULL;

		//tInfoRegisteredClass *rc = getRegisteredClass(class_name);

		/*if(rc != NULL){
			return;
		}*/

		// init struct...
		irs.object_info.symbol_info.ast = NULL;
		irs.object_info.symbol_info.info_var_scope = NULL;

		irs.object_info.symbol_info.properties = C_FUNCTION_CLASS;
		irs.object_info.symbol_info.ref_aux=0;


		if((irs.object_info.symbol_info.info_var_scope=CAst::getInstance()->getRootScopeInfo()->registerSymbol(function_name))==NULL){
			return false;
		}

		print_error_cr("CHECK AND TODO  111111111111!");

		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_ptr)>;


		getParamsFunction<Traits3>(0,irs.return_type, irs.m_arg, make_index_sequence<Traits3::arity>{});

		for(unsigned i = 0; i < irs.m_arg.size(); i++){
			print_info_cr("ARG[%i]: %s",i,irs.m_arg[i].c_str());
		}

		//base_info->local_symbols.m_registeredFunction.push_back(irs);

		return true;
	}


private:

	static CScriptClassFactory *scriptClassFactory;
	static tPrimitiveType *getPrimitiveTypeFromStr(const string & str);

	vector<tInfoRegisteredClass *>  	 m_registeredClass;
	CObject * createObjectFromPrimitiveType(tPrimitiveType *pt);

	CScriptClassFactory();
	~CScriptClassFactory();



};
