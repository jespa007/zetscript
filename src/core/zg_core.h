#pragma once

class CScriptVariable;
class CScopeInfo;

#include "system/zg_system.h"
#include "utils/zg_utils.h"
#include "var/zg_variable.h"


#include "ScriptDefinesStructs.h"
#include "CSharedPointerManagement.h"
#include "ast/ast.h"
#include "ast/CScopeInfo.h"
#include "CCompiler.h"
#include "CScriptClassFactory.h"
#include "vm/CALE.h"
#include "vm/CVirtualMachine.h"


//#include "CScriptVariable.h"
//#include "CScriptFunction.h.old"



#define ZETSCRIPT_MAJOR_VERSION 1
#define ZETSCRIPT_MINOR_VERSION 1
#define ZETSCRIPT_PATCH_VERSION 0





class CZG_ScriptCore{


	// calling C function with differnt parameters...
	tInfoRegisteredClass *m_mainClassInfo;
	tInfoRegisteredFunctionSymbol *m_mainFunctionInfo;

	CScriptVariable *m_mainClass;


	//CScriptFunction *m_mainFunction;

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




	static CZG_ScriptCore * m_instance;

	//tInfoRegisteredFunctionSymbol m_structInfoMain;

	bool init();

	CZG_ScriptCore();


	~CZG_ScriptCore();

	//map<string,vector<tInfoObjectOperator> *> m_mapContainerOperators;
	//bool existOperatorSignature(const string & op,const string & result, vector<string> * param);
	//tInfoObjectOperator * getOperatorInfo(const string & op, string * type_op1, string * type_op2=NULL);

public:

	static CZG_ScriptCore * getInstance();
	static CAst       * m_ast;

	tInfoRegisteredFunctionSymbol *getMainStructInfo(){return m_mainFunctionInfo;}
	tScriptFunctionInfo *getMainObjectInfo(){return &m_mainFunctionInfo->object_info;}
	CScriptVariable *getMainObject(){return m_mainClass;}
	//CScriptFunction *getMainFunction(){return m_mainFunction;}

	static bool call_C_function(tInfoRegisteredFunctionSymbol *irfs, int & result, vector<CVariable *> * argv);

	//bool registerOperatorInternal(const string & _op_name, const string &  result_type,vector<string> * param_type, void(*fun_ptr)());
	bool eval(const string & s);
	bool execute();



	static void destroy();
};

