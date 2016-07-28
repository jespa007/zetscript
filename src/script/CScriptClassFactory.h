#pragma once

#include "ScriptDefinesStructs.h"
#include "RegisterFunctionHelper.h"


#define registerGlobal_C_Function(s) CScriptClassFactory::register_C_Function("Main",STR(s),s)
#define registerGlobal_C_Variable(s) CScriptClassFactory::register_C_Variable("Main",STR(s),s,typeid(s).name())



class CScope;
class CScriptClassFactory{


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

	enum C_FUNCTION_PROPERTIES{
		IS_C_FUNCTION = 0x1 <<0

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
	static tPrimitiveType *getPrimitiveTypeFromStr(const string & str);
	CObject * createObjectFromPrimitiveType(tPrimitiveType *pt);
	static fntConversionType getConversionType(string objectType, string conversionType);




public:


	static tDefOperator def_operator[MAX_OPERATORS];

	static int  registerVariableSymbol(const string & class_name,PASTNode * node);
	static int  registerFunctionSymbol(const string & class_name,PASTNode * node, unsigned int properties);
	static tInfoRegisteredFunctionSymbol *  getRegisteredFunctionSymbol(const string & class_name,unsigned idx);
	static bool addArgumentFunctionSymbol(const string & class_name,unsigned idxFunction,const string & arg_name);


	static tInfoRegisteredClass * getRegisteredClass(const string & v, bool print_msg=true);
	static tInfoRegisteredClass * registeredClassExists(const string & class_name);
	static tInfoRegisteredClass * registerClass(const string & class_name);


	static void destroySingletons();

	static CScriptFunction *newClass(const string & class_name);

	/**
	 * Register C function
	 */
	template <typename F>
	static void register_C_Function(const string & class_name, const char *function_str,F function_ptr)
	{
		string return_type;
		vector<string> params;
		tInfoRegisteredFunctionSymbol irs;

		tPrimitiveType *rt;
		//vector<tPrimitiveType *> pt;
		//CScope::tInfoScopeVar  *rs;

		//CScriptFunction *sf=NULL;

		tInfoRegisteredClass *rc = getRegisteredClass(class_name);

		if(rc != NULL){
			return;
		}

		// init struct...
		irs.symbol_info.properties = CScriptClassFactory::IS_C_FUNCTION;
		irs.symbol_info.scope = NULL;


		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,irs.return_type, irs.m_arg, make_index_sequence<Traits3::arity>{});

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
		/*if((rs = this->m_mainFunction->getScope()->registerSymbol(function_str,0)) == NULL) // already registered
			return;

		// 3. all test ok. Time to create script function ..

		sf=new CScriptFunction(m_mainFunction);

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

		print_info_cr("Registered function name: %s",function_str);

	 /* int temp=0;
	  f(temp);
	  std::cout << "Result is " << temp << std::endl;*/
	}


	/**
	 * Register C variable
	 */
	void register_C_Variable(const string & class_name, const string & var_str,void * var_ptr, const string & var_type);




private:
	static map<string,tInfoRegisteredClass *>  	 m_registeredClass;


};
