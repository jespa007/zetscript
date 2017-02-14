#pragma once

class CScriptVariable;
class CScope;

#include "core/zs_core.h"


//#include "CScriptVariable.h"
//#include "CScriptFunction.h.old"



#define ZETSCRIPT_MAJOR_VERSION 1
#define ZETSCRIPT_MINOR_VERSION 1
#define ZETSCRIPT_PATCH_VERSION 0





class CZetScript{


	// calling C function with differnt parameters...
	tInfoRegisteredClass *m_mainClassInfo;
	tScriptFunctionObject *m_mainFunctionInfo;

	CScriptVariable *m_mainClass;
	CVirtualMachine *vm;
	bool __init__;


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




	static CZetScript * m_instance;

	//tScriptFunctionObject m_structInfoMain;

	bool init();
	CZetScript();


	~CZetScript();

	//map<string,vector<tInfoObjectOperator> *> m_mapContainerOperators;
	//bool existOperatorSignature(const string & op,const string & result, vector<string> * param);
	//tInfoObjectOperator * getOperatorInfo(const string & op, string * type_op1, string * type_op2=NULL);

public:

	static CZetScript * getInstance();
	static CAst       * m_ast;

	//---------------
	// PRINT ASM INFO
	char print_aux_load_value[1024*8];
	const char * getStrMovVar(tInfoAsmOp * iao);
	const char * getStrTypeLoadValue(vector<tInfoStatementOp> * m_listStatements,int current_statment, int current_instruction);
	void printGeneratedCode_Recursive(tFunctionInfo *fs);
	void printGeneratedCode(tFunctionInfo *fs);
	void printGeneratedCodeAllClasses();
	// PRINT ASM INFO
	//---------------

	tScriptFunctionObject *getMainStructInfo(){return m_mainFunctionInfo;}
	tFunctionInfo *getMainObjectInfo(){return &m_mainFunctionInfo->object_info;}
	CScriptVariable *getMainObject(){return m_mainClass;}
	//CScriptFunction *getMainFunction(){return m_mainFunction;}

	static CScriptVariable * call_C_function(void *fun_ptr,tScriptFunctionObject *irfs, vector<CScriptVariable *> * argv);

	//bool registerOperatorInternal(const string & _op_name, const string &  result_type,vector<string> * param_type, void(*fun_ptr)());
	bool eval(const string & s);
	std::function<CScriptVariable * (std::vector<CScriptVariable *> args)> * script_call(const string &script_function_name);
	CScriptVariable * execute();



	static void destroy();
};

