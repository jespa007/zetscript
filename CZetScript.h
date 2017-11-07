/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */



#pragma once

#include "core/zs_core.h"

#define ZETSCRIPT_MAJOR_VERSION 1
#define ZETSCRIPT_MINOR_VERSION 0
#define ZETSCRIPT_PATCH_VERSION 2

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
		static CZetScript * m_instance;
		static vector<tInfoParsedSource> * m_parsedSource;
		static int getIdxParsedFilenameSource(const char *file);

		// calling C function with differnt parameters...
		CScriptVariable		*		m_mainObject;
		int						 	idxMainScriptFunctionObject;

		CVirtualMachine *vm;
		bool __init__;






		bool isFilenameAlreadyParsed(const char *filename);

		bool parse_ast(const char *str, const char *filename=NULL);

		CScriptFunctionObject * getScriptObjectFromFunctionAccess(const string &function_access_expression);


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

		static int eval_int(const string & str_to_eval);
		static bool eval_bool(const string & str_to_eval);
		static float eval_float(const string & str_to_eval);
		static string eval_string(const string & str_to_eval);

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

		// we implement as many interface C++ functions as max parameters we can manage (6)

		// trivial case when parameters (argIdx == 0).
		template <typename _T>
		auto bind_script_function_builder(void **f)
		{

			using tReturn = typename _T::return_type;
			//return NULL;
			 // NO ARGS CASE
			*f=((void *)(new std::function<tReturn ()>(
				[](){
					printf("hello 0");
				}
			)));
		}

		// template for last parameter argIdx == 1
		template<typename _T,  typename... ArgTypes>
		auto bind_script_function_builder(void **f )
			-> typename std::enable_if<sizeof...(ArgTypes) == 1>::type
		{
			//return NULL;
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;

			*f=((void *)(new std::function<tReturn (tParam1)>([](tParam1 p1){
				printf("hello 1");
			})));

		}

		// template when parameters argIdx == 2
		template <typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f)
			-> typename std::enable_if<(sizeof...(ArgTypes) == 2)>::type
		{
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;
			using tParam2 = typename _T::template argument<1>::type;

			*f=((void *)(new std::function<tReturn (tParam1,tParam2)>([](tParam1 p1,tParam2 p2){
				printf("hello 2");
			})));


			//return NULL;
		}

		// template when parameters argIdx == 3
		template <typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f)
			-> typename std::enable_if<(sizeof...(ArgTypes) == 3)>::type
		{
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;
			using tParam2 = typename _T::template argument<1>::type;
			using tParam3 = typename _T::template argument<2>::type;


			*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3)>(
				[](tParam1 p1,tParam2 p2,tParam3 p3){
					printf("hello 3");
				}
			)));;
		}

		// template when parameters argIdx == 4
		template <typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f)
			-> typename std::enable_if<(sizeof...(ArgTypes) == 4)>::type
		{
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;
			using tParam2 = typename _T::template argument<1>::type;
			using tParam3 = typename _T::template argument<2>::type;
			using tParam4 = typename _T::template argument<3>::type;

			*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3,tParam4)>(
				[](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){
					printf("hello 4");
				}
			)));
		}


		// template when parameters argIdx == 5
		template <typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f)
			-> typename std::enable_if<(sizeof...(ArgTypes) == 5)>::type
		{
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;
			using tParam2 = typename _T::template argument<1>::type;
			using tParam3 = typename _T::template argument<2>::type;
			using tParam4 = typename _T::template argument<3>::type;
			using tParam5 = typename _T::template argument<4>::type;

			*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3,tParam4,tParam5)>(
				[](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){
					printf("hello 5");
				}
			)));
			//return NULL;
		}

		// template when parameters argIdx == 6
		template <typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f)
			-> typename std::enable_if<(sizeof...(ArgTypes) == 6)>::type
		{
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;
			using tParam2 = typename _T::template argument<1>::type;
			using tParam3 = typename _T::template argument<2>::type;
			using tParam4 = typename _T::template argument<3>::type;
			using tParam5 = typename _T::template argument<4>::type;
			using tParam6 = typename _T::template argument<5>::type;

			*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
				[](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){
					printf("hello 6");
				}
			)));
			//return NULL;
		}


		 template <typename _T, std::size_t... Is>
		void bind_script_function_builder_base(void **f, index_sequence<Is...>)
		{

			 bind_script_function_builder<_T, typename _T::template argument<Is>::type...>(f);
		}

					// 3. build custom function in function of parameter number ...
					//bind_script_function_builder_base<Traits3>(&ptr,make_index_sequence<Traits3::arity>{});

		/**
		 * Main bind function
		 */

		template <  typename _F>
		std::function<_F> * bind_function(const string & function_access)
		{
			string return_type;
			vector<string> params;
			vector<string> m_arg;
			int idx_return_type=-1;
			void *ptr;
			CScriptFunctionObject * fun = getScriptObjectFromFunctionAccess(function_access);

			if(fun)
			{

				// 1. check all parameters ok.
				using Traits3 = function_traits<_F>;//decltype(function_type)>;
				getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});

				// 2. check valid parameters ...
				if((idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(return_type)) == -1){
					zs_print_error_cr("Return type \"%s\" for bind function not registered",demangle(return_type).c_str());
					return NULL;
				}

				for(unsigned int i = 0; i < m_arg.size(); i++){
					if(CScriptClass::getIdxClassFromIts_C_Type(m_arg[i])==-1){
						zs_print_error_cr("Argument (%i) type \"%s\" for bind function not registered",i,demangle(m_arg[i]).c_str());
						return NULL;
					}

				}

				// 3. build custom function in function of parameter number ...
				bind_script_function_builder_base<Traits3>(&ptr,make_index_sequence<Traits3::arity>{});
				return (std::function<_F> *)ptr;

			}

			return NULL;

		}

		/*CScriptFunctionObject *getMainStructInfo(){return m_mainFunctionInfo;}
		tFunctionInfo *getMainObjectInfo(){return &m_mainFunctionInfo->object_info;}

		//CScriptFunction *getMainFunction(){return m_mainFunction;}*/
		CVirtualMachine * getVirtualMachine();
		CScriptVariable *getMainObject(){return m_mainObject;}



		//bool registerOperatorInternal(const string & _op_name, const string &  result_type,vector<string> * param_type, void(*fun_ptr)());


		//std::function<CScriptVariable * (void)> * function_bind_no_params(const string &script_function_name);

		CScriptVariable * execute();

		bool eval(const string & string, bool execute=true, const char *filename=NULL);
		bool eval_file(const char * filename);

	
		static void destroy();
	};

}
