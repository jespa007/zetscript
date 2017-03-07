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
	CScriptVariable		*		m_mainObject;
	int						 	idxMainScriptFunctionObject;

	CVirtualMachine *vm;
	bool __init__;



	static vector<tInfoParsedSource> * m_parsedSource;



	bool isFilenameAlreadyParsed(const char *filename);
	bool parse_ast(const char *str);


	static CZetScript * m_instance;

	//CScriptFunctionObject m_structInfoMain;

	bool init();
	CZetScript();


	~CZetScript();

public:

	static CZetScript * getInstance();
	static void setVectorInfoParsedFiles(vector<tInfoParsedSource> * parsedFiles);

	//---------------
	// PRINT ASM INFO
	char print_aux_load_value[1024*8];
	const char * getStrMovVar(tInfoAsmOp * iao);
	const char * getStrTypeLoadValue(tInfoStatementOp * m_listStatements,int current_statment, int current_instruction);
	void printGeneratedCode_Recursive(tFunctionInfo *fs);
	void printGeneratedCode(tFunctionInfo *fs);
	void printGeneratedCodeAllClasses();
	// PRINT ASM INFO
	//---------------

	/*CScriptFunctionObject *getMainStructInfo(){return m_mainFunctionInfo;}
	tFunctionInfo *getMainObjectInfo(){return &m_mainFunctionInfo->object_info;}

	//CScriptFunction *getMainFunction(){return m_mainFunction;}*/

	CScriptVariable *getMainObject(){return m_mainObject;}

	static CScriptVariable * call_C_function(void *fun_ptr,CScriptFunctionObject *irfs, vector<CScriptVariable *> * argv);

	//bool registerOperatorInternal(const string & _op_name, const string &  result_type,vector<string> * param_type, void(*fun_ptr)());

	std::function<CScriptVariable * (std::vector<CScriptVariable *> args)> * script_call(const string &script_function_name);
	CScriptVariable * execute();

	bool eval(const string & string);
	bool eval_file(const char * filename);


	static void destroy();
};

