#pragma once

class CScriptFunction;

#include "system/zg_system.h"
#include "utils/zg_utils.h"
#include "object/zg_object.h"
#include "factory/zg_factory.h"

#include "ast.h"
#include "CVirtualMachine.h"
#include "CCompiler.h"
#include "CScriptFunction.h"
#include "CScope.h"



#define registerFunction(s) CZG_Script::getInstance()->registerFunctionInternal(STR(s),s)
#define registerVariable(s) CZG_Script::getInstance()->registerVariableInternal(STR(s),s,typeid(s).name())



class CZG_Script{

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
	static tPrimitiveType *getPrimitiveTypeFromStr(const string & str);


	/**
	 * Runtime Object to primitive conversion
	 */
	bool object2float(CObject *obj, float & v);
	bool object2int(CObject *obj, int & v);
	bool object2bool(CObject *obj, bool & v);
	bool object2string(CObject *obj, string & v);

	CObject * createObjectFromPrimitiveType(CZG_Script::tPrimitiveType *pt);

	/**
	 * Register C function
	 */
	template <typename F>
	void registerFunctionInternal(const char *function_str,F function_type)
	{
		string return_type;
		vector<string> params;

		tPrimitiveType *rt;
		vector<tPrimitiveType *> pt;
		CScope::tInfoRegisteredVar  *rs;

		CScriptFunction *sf=NULL;



		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_type)>;
		getParamsFunction<Traits3>(0,return_type, params, make_index_sequence<Traits3::arity>{});

		// check whether function is ok or not.
		if((rt=getPrimitiveTypeFromStr(return_type)) == NULL){return;}

		for(unsigned int i=0; i < params.size(); i++){
			if((rt=getPrimitiveTypeFromStr(params[i])) == NULL){
				return;
			}

			pt.push_back(rt);
		}

		// 2. create script function ...
		if((rs = this->m_mainFunction->getScope()->registerSymbol(function_str,0)) == NULL) // already registered
			return;

		// 3. all test ok. Time to create script function ..

		sf=new CScriptFunction(m_mainFunction);

		// register return type ...
		sf->setReturnObject(createObjectFromPrimitiveType(rt));

		sf->setName(function_str);

		// set function pointer...
		sf->setupAsFunctionPointer(&function_type);

		// register all args...
		for(unsigned int i=0; i < pt.size(); i++){

			char param_name[100]={0};
			sprintf(param_name,"%s_arg_%02i",function_str,i);


			// register symbol ...
			if((rs=sf->registerSymbolAsFunctionArgument(param_name))==NULL)
				return;

			// set explicit ...
			if((rs->m_obj = createObjectFromPrimitiveType(pt[i]))==NULL)
				return;
		}

		print_info_cr("Registered function name: %s",function_str);

	 /* int temp=0;
	  f(temp);
	  std::cout << "Result is " << temp << std::endl;*/
	}



	/**
	 * Register C variable
	 */
	void registerVariableInternal(const char *var_str,void * var_ptr, const string & var_type)
	{
		if(var_ptr==NULL){
			print_error_cr("cannot register var \"%s\" with NULL reference value", var_str);
			return;
		}


		CZG_Script::tPrimitiveType *type;
		CScope::tInfoRegisteredVar *irv;
		string human_str;

		if((irv=m_mainFunction->getScope()->registerSymbol(var_str,-1))!= NULL){ // registered symbol var...


				if(CFactoryContainer::getInstance()->classPtrIsRegistered(var_type)){ // type exists ...
					irv->m_obj = (CObject *)var_ptr;

					human_str = var_type;
				}else{
					type = getPrimitiveTypeFromStr(var_type);
					if(type != NULL){

						human_str = type->human_description_str;

					switch(type->id){
					// 1. first try primitives ...
					default:
						print_error_cr("Cannot know how to set \"%s\" as \"%s\" type variable.",var_str,type->human_description_str.c_str());
						return;
						break;
					case INT_PTR_TYPE:
						irv->m_obj=NEW_INTEGER();
						((CInteger *)irv->m_obj)->m_ptr = (int *)var_ptr;
						break;
					case FLOAT_PTR_TYPE:
						irv->m_obj=NEW_NUMBER();
						((CNumber *)irv->m_obj)->m_ptr = (float *)var_ptr;
						break;
					case STRING_PTR_TYPE:
						irv->m_obj=NEW_STRING();
						((CString *)irv->m_obj)->m_ptr = (string *)var_ptr;
						break;
					case BOOL_PTR_TYPE:
						irv->m_obj=NEW_BOOLEAN();
						((CBoolean *)irv->m_obj)->m_ptr = (bool *)var_ptr;
						break;

					}
					}

				}



			if(irv->m_obj != NULL){
				print_info_cr("Registered \"%s\" as \"%s\"",var_str, human_str.c_str());
			}
		}
	}

private:

	/*enum TYPE_{
		LOAD_VALUE=1,
		MOV_VAR,
		OPERATOR
	};*/


	/*typedef struct{
		string result_type;
		vector <string> * param_type;
		void (* fun_ptr)();
	}tInfoObjectOperator;*/


	static CZG_Script * m_instance;

	CScriptFunction *m_mainFunction;

	void init();

	CZG_Script();


	~CZG_Script();

	//map<string,vector<tInfoObjectOperator> *> m_mapContainerOperators;
	//bool existOperatorSignature(const string & op,const string & result, vector<string> * param);
	//tInfoObjectOperator * getOperatorInfo(const string & op, string * type_op1, string * type_op2=NULL);

public:

	static CZG_Script * getInstance();

	//bool registerOperatorInternal(const string & _op_name, const string &  result_type,vector<string> * param_type, void(*fun_ptr)());
	bool eval(const string & s);
	bool execute();



	static void destroy();
};

