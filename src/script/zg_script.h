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
		string 	    str;
		C_TYPE_VAR  id;
	}tPrimitiveType;

	typedef struct{
		tPrimitiveType 				*return_type;
		vector<tPrimitiveType*>		params;
	}tRegisterFunction;


	static tPrimitiveType primitiveType[MAX_VAR_C_TYPES];

	static void registerPrimitiveTypes();
	static tPrimitiveType *getPrimitiveType(const string & str);



	template <typename F>
	static void registerFunction(F f)
	{
		string return_type;
		vector<string> params;
		print_info_cr("Function name: %s",typeid(f).name());

		using Traits3 = function_traits<decltype(f)>;
		getParamsFunction<Traits3>(0,return_type, params, make_index_sequence<Traits3::arity>{});

		// check whether function is ok or not.
		getPrimitiveType(return_type);

		for(unsigned int i=0; i < params.size(); i++){
			getPrimitiveType(params[i]);
		}


	 /* int temp=0;
	  f(temp);
	  std::cout << "Result is " << temp << std::endl;*/
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


	void init();

	static void destroy();
};

