#pragma once

class CScriptClass;
class CScopeInfo;

#include "system/zg_system.h"
#include "utils/zg_utils.h"
#include "object/zg_object.h"
#include "factory/zg_factory.h"


#include "ScriptDefinesStructs.h"
#include "CSharedPointerManagement.h"
#include "ast/ast.h"
#include "ast/CScopeInfo.h"
#include "CCompiler.h"
#include "CScriptClassFactory.h"
#include "CALE.h"
#include "CVirtualMachine.h"


#include "CScriptClass.h"








class CZG_Script{


	// calling C function with differnt parameters...
	static int call_C_6p(int f, int arg1,int arg2,int arg3,int arg4,int arg5);
	static int call_C_5p(int f, int arg1,int arg2,int arg3,int arg4);
	static int call_C_3p(int f, int arg1,int arg2,int arg3);
	static int call_C_2p(int f, int arg1,int arg2);
	static int call_C_1p(int f, int arg1);
	static int call_C_0p(int f);

	CScriptClass *m_mainClass;

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

	tInfoRegisteredFunctionSymbol m_structInfoMain;

	void init();

	CZG_Script();


	~CZG_Script();

	//map<string,vector<tInfoObjectOperator> *> m_mapContainerOperators;
	//bool existOperatorSignature(const string & op,const string & result, vector<string> * param);
	//tInfoObjectOperator * getOperatorInfo(const string & op, string * type_op1, string * type_op2=NULL);

public:

	static CZG_Script * getInstance();
	static CAst       * m_ast;

	tInfoRegisteredFunctionSymbol *getMainStructInfo(){return &m_structInfoMain;}
	tScriptFunctionInfo *getMainObjectInfo(){return &m_structInfoMain.object_info;}
	CScriptClass *getMainObject(){return m_mainClass;}

	static int call_C_function(tInfoRegisteredFunctionSymbol *irfs, vector<CObject *> * argv);

	//bool registerOperatorInternal(const string & _op_name, const string &  result_type,vector<string> * param_type, void(*fun_ptr)());
	bool eval(const string & s);
	bool execute();



	static void destroy();
};

