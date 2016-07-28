#pragma once

class CScriptFunction;
class CScope;

#include "system/zg_system.h"
#include "utils/zg_utils.h"
#include "object/zg_object.h"
#include "factory/zg_factory.h"


#include "ScriptDefinesStructs.h"
#include "ast/ast.h"
#include "ast/CScope.h"
#include "CCompiler.h"
#include "CScriptClassFactory.h"
#include "CALE.h"
#include "CVirtualMachine.h"


#include "CScriptFunction.h"








class CZG_Script{

public:






private:

	tInfoRegisteredFunctionSymbol main_function;

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

	// calling C function with differnt parameters...
	CObject * call_C_6p(int f, CObject *arg1,CObject *arg2,CObject *arg3,CObject *arg4,CObject *arg5);
	CObject * call_C_5p(int f, CObject *arg1,CObject *arg2,CObject *arg3,CObject *arg4);
	CObject * call_C_3p(int f, CObject *arg1,CObject *arg2,CObject *arg3);
	CObject * call_C_2p(int f, CObject *arg1,CObject *arg2);
	CObject * call_C_1p(int f, CObject *arg);
	CObject * call_C_0p(int f);

	//bool registerOperatorInternal(const string & _op_name, const string &  result_type,vector<string> * param_type, void(*fun_ptr)());
	bool eval(const string & s);
	bool execute();



	static void destroy();
};

