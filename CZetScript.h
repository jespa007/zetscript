/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */



#pragma once

#include "core/zs_core.h"

#define ZETSCRIPT_MAJOR_VERSION 1
#define ZETSCRIPT_MINOR_VERSION 2
#define ZETSCRIPT_PATCH_VERSION 0

#define CURRENT_VM	CZetScript::getInstance()->getVirtualMachine()



#define GET_AST_FILENAME_LINE(_idx_ast_)   AST_FILENAME(_idx_ast_),AST_LINE(_idx_ast_)
#define ZS_WRITE_ERROR_MSG 		CZetScript::writeErrorMsg
#define ZS_GET_ERROR_MSG		CZetScript::getErrorMsg
#define ZS_CLEAR_ERROR_MSG		CZetScript::clearErrorMsg

#define MAX_BUFFER_STR_ERROR	4096
#define MAX_BUFFER_AUX_TMP		4096


namespace zetscript{

	class CScriptVariable;
	class CScope;


	class  CZetScript{

		static char str_error[MAX_BUFFER_STR_ERROR];
		static CZetScript * m_instance;
		static vector<tInfoParsedSource> * m_parsedSource;
		static int getIdxParsedFilenameSource(const char *file);

		// calling C function with differnt parameters...
		CScriptVariable		*		m_mainObject;
		//int						 	idxMainScriptFunctionObject;

		CVirtualMachine *vm;
		bool __init__;




		bool isFilenameAlreadyParsed(const char *filename);

		bool parse_ast(const char *str, int idx_filename=-1);



		//CScriptFunctionObject m_structInfoMain;

		bool init();
		CZetScript();


		~CZetScript();

	public:




		ZETSCRIPT_MODULE_EXPORT static CZetScript * getInstance();
		ZETSCRIPT_MODULE_EXPORT static void setVectorInfoParsedFiles(vector<tInfoParsedSource> * parsedFiles);



		static void clearErrorMsg();
		static void writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);
		ZETSCRIPT_MODULE_EXPORT static const char * getErrorMsg();
		const char * getParsedFilenameFromIdx(unsigned idx);

		ZETSCRIPT_MODULE_EXPORT static int eval_int(const string & str_to_eval);
		ZETSCRIPT_MODULE_EXPORT static bool eval_bool(const string & str_to_eval);
		ZETSCRIPT_MODULE_EXPORT static float eval_float(const string & str_to_eval);
		ZETSCRIPT_MODULE_EXPORT static string eval_string(const string & str_to_eval);

		//---------------
		// PRINT ASM INFO
		char print_aux_load_value[1024*8];
		const char * getStrMovVar(tInfoAsmOp * iao);
		const char * getStrTypeLoadValue(PtrStatment m_listStatements,int current_statment, int current_instruction);
		ZETSCRIPT_MODULE_EXPORT void printGeneratedCode_Recursive(tFunctionInfo *fs);
		ZETSCRIPT_MODULE_EXPORT void printGeneratedCode(tFunctionInfo *fs);
		ZETSCRIPT_MODULE_EXPORT void printGeneratedCodeAllClasses();
		// PRINT ASM INFO
		//---------------

		/**
		 * Main bind function
		 */

		ZETSCRIPT_MODULE_EXPORT bool getScriptObjectFromFunctionAccess(const string &function_access_expression
														  ,CScriptVariable **calling_obj
														  ,CScriptFunctionObject **fun_obj);
		//----


		void destroyMainFunction();

		ZETSCRIPT_MODULE_EXPORT CVirtualMachine * getVirtualMachine();
		ZETSCRIPT_MODULE_EXPORT CScriptVariable *getMainObject(){return m_mainObject;}



		ZETSCRIPT_MODULE_EXPORT bool parse(const string & string,const char *filename_ref=NULL);
		ZETSCRIPT_MODULE_EXPORT bool parse_file(const char * filename);
		ZETSCRIPT_MODULE_EXPORT bool compile();
		ZETSCRIPT_MODULE_EXPORT bool execute();

		ZETSCRIPT_MODULE_EXPORT bool eval(const string & string, bool execute=true, const char *filename_ref=NULL);
		ZETSCRIPT_MODULE_EXPORT bool eval_file(const char * filename,bool execute=true);


		ZETSCRIPT_MODULE_EXPORT static void destroy();
	};




}

#include "helpers/CppBindingHelper.h"

