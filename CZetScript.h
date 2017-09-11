#pragma once





#include "core/zs_core.h"

#define ZETSCRIPT_MAJOR_VERSION 1
#define ZETSCRIPT_MINOR_VERSION 0
#define ZETSCRIPT_PATCH_VERSION 10

#define CURRENT_VM	CZetScript::getInstance()->getVirtualMachine()

#define GET_AST_FILENAME_LINE(_idx_ast_)   AST_FILENAME(_idx_ast_),AST_LINE(_idx_ast_)
#define ZS_WRITE_ERROR_MSG 		CZetScript::writeErrorMsg
#define ZS_GET_ERROR_MSG		CZetScript::getErrorMsg
#define ZS_CLEAR_ERROR_MSG		CZetScript::clearErrorMsg

#define MAX_BUFFER_STR_ERROR	4096
#define MAX_BUFFER_AUX_TMP		4096


#define ZS_GET_ERROR "¡¡¡not implemented error handling!!"

namespace zetscript{

	class CScriptVariable;
	class CScope;


	class  CZetScript{

		static char str_error[MAX_BUFFER_STR_ERROR];


		// calling C function with differnt parameters...
		CScriptVariable		*		m_mainObject;
		int						 	idxMainScriptFunctionObject;

		CVirtualMachine *vm;
		bool __init__;



		static vector<tInfoParsedSource> * m_parsedSource;



		bool isFilenameAlreadyParsed(const char *filename);

		bool parse_ast(const char *str, const char *filename=NULL);


		static CZetScript * m_instance;

		//CScriptFunctionObject m_structInfoMain;

		bool init();
		CZetScript();


		~CZetScript();

	public:



		static CZetScript * getInstance();
		static void setVectorInfoParsedFiles(vector<tInfoParsedSource> * parsedFiles);

		static void clearErrorMsg();
		static void writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);
		static const char * getErrorMsg();
		const char * getParsedFilenameFromIdx(unsigned idx);

		template<typename _R>
		static _R eval(const string & str_to_eval){

			_R value = _R();
			void *ptr=NULL;
			string typestr = typeid(_R).name();


			CZetScript *zetscript= CZetScript::getInstance();


			if(zetscript->eval(str_to_eval)){


				//if(zetscript->execute()){

					tStackElement *se=CURRENT_VM->getLastStackValue();

					if(se != NULL){

						if((typestr == typeid(int).name())  && (se->properties & STK_PROPERTY_TYPE_INTEGER)){
							ptr = &value;
							*((intptr_t *)ptr) = (intptr_t)se->stkValue;
						}
						else if((typestr == typeid(float).name())  && (se->properties & STK_PROPERTY_TYPE_NUMBER)){
							ptr = &value;
							memcpy(ptr,&se->stkValue,sizeof(float));
						}
						else if((typestr == typeid(string).name())  && (se->properties & STK_PROPERTY_TYPE_STRING)){
							ptr=&value;
							*((string *)ptr) = ((const char *)se->stkValue);
						}
						else if((typestr == typeid(bool).name())  && (se->properties & STK_PROPERTY_TYPE_BOOLEAN)){
							memcpy(&value, &se->stkValue, sizeof(bool));
						}
						else{
							fprintf(stderr,"eval<%s>(...): Error evaluating \"%s\". Property:0x%X",typestr.c_str(),str_to_eval.c_str(),se->properties);
							exit(-1);
						}


					}
				//}
			}

			return value;

		}

		//---------------
		// PRINT ASM INFO
		char print_aux_load_value[1024*8];
		const char * getStrMovVar(tInfoAsmOp * iao);
		const char * getStrTypeLoadValue(PtrStatment m_listStatements,int current_statment, int current_instruction);
		void printGeneratedCode_Recursive(tFunctionInfo *fs);
		void printGeneratedCode(tFunctionInfo *fs);
		void printGeneratedCodeAllClasses();
		// PRINT ASM INFO
		//---------------



		/*CScriptFunctionObject *getMainStructInfo(){return m_mainFunctionInfo;}
		tFunctionInfo *getMainObjectInfo(){return &m_mainFunctionInfo->object_info;}

		//CScriptFunction *getMainFunction(){return m_mainFunction;}*/
		CVirtualMachine * getVirtualMachine();
		CScriptVariable *getMainObject(){return m_mainObject;}



		//bool registerOperatorInternal(const string & _op_name, const string &  result_type,vector<string> * param_type, void(*fun_ptr)());

		std::function<CScriptVariable * (const std::vector<CScriptVariable *> & args)> * bind_function(const string &function_access_expression);
		//std::function<CScriptVariable * (void)> * function_bind_no_params(const string &script_function_name);

		CScriptVariable * execute();

		bool eval(const string & string, bool execute=true, const char *filename=NULL);
		bool eval_file(const char * filename);

	
		static void destroy();
	};

}
