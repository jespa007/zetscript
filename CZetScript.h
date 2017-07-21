#pragma once





#include "core/zs_core.h"

#define ZETSCRIPT_MAJOR_VERSION 1
#define ZETSCRIPT_MINOR_VERSION 0
#define ZETSCRIPT_PATCH_VERSION 7

#define CURRENT_VM	CZetScript::getInstance()->getVirtualMachine()

#define MAX_BUFFER_STR_ERROR	4096
#define MAX_BUFFER_AUX_TMP		1024



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
		bool parse_ast(const char *str);


		static CZetScript * m_instance;

		//CScriptFunctionObject m_structInfoMain;

		bool init();
		CZetScript();


		~CZetScript();

	public:

		static CZetScript * getInstance();
		static void setVectorInfoParsedFiles(vector<tInfoParsedSource> * parsedFiles);
		static void write_error(const  char  *string_text, ...);

		template<typename _R>
		static _R eval(const string & str_to_eval){

			_R value = _R();
			void *ptr=NULL;
			string typestr = typeid(_R).name();


			CZetScript *zetscript= CZetScript::getInstance();

			if(zetscript->eval(str_to_eval)){

				if(zetscript->execute()){

					tStackElement *se=CURRENT_VM->getLastStackValue();

					if(se != NULL){

						if((typestr == typeid(int).name())  && (se->properties & INS_PROPERTY_TYPE_INTEGER)){
							value = (int)se->stkValue;
						}
						else if((typestr == typeid(float).name())  && (se->properties & INS_PROPERTY_TYPE_NUMBER)){
							memcpy(&value,&se->stkValue,sizeof(float));
						}
						else if((typestr == typeid(string).name())  && (se->properties & INS_PROPERTY_TYPE_STRING)){
							ptr=&value;
							*((string *)ptr) = *((string *)se->stkValue);
						}
						else if((typestr == typeid(bool).name())  && (se->properties & INS_PROPERTY_TYPE_BOOLEAN)){
							value = (bool)se->stkValue;
						}
						else{
							fprintf(stderr,"eval<%s>(...) not implemented",typestr.c_str());
							exit(-1);
						}


					}
				}
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

		//std::function<CScriptVariable * (std::vector<CScriptVariable *> args)> * script_call(const string &script_function_name);
		CScriptVariable * execute();

		bool eval(const string & string);
		bool eval_file(const char * filename);

	
		static void destroy();
	};

}
