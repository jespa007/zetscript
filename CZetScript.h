/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */



#pragma once

#include "core/zs_core.h"

#define ZETSCRIPT_MAJOR_VERSION 1
#define ZETSCRIPT_MINOR_VERSION 1
#define ZETSCRIPT_PATCH_VERSION 0

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

		static inline tStackElement var2stk(intptr_t var_trans, int idx_type){
			//intptr_t var_trans = (intptr_t)input_var;
				string s_return_value;
				tStackElement callc_result={0};
				//int idx_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(_T).name());
				// save return type ...
				switch(idx_type){
				 case IDX_CLASS_VOID_C:
					break;
				 case IDX_CLASS_INT_PTR_C:
					 callc_result={STK_PROPERTY_TYPE_INTEGER,(void *)(*((intptr_t *)var_trans)),NULL};
					 break;
				 case IDX_CLASS_INT_C:
					 callc_result={STK_PROPERTY_TYPE_INTEGER,(void *)(((intptr_t)var_trans)),NULL};
					 break;
				 case IDX_CLASS_FLOAT_C:
					 callc_result.properties=STK_PROPERTY_TYPE_NUMBER;//{};
					 memcpy(&callc_result.stkValue,&var_trans,sizeof(float));

					 break;
				 case IDX_CLASS_FLOAT_PTR_C:
					 callc_result.properties=STK_PROPERTY_TYPE_NUMBER;//{};
					 memcpy(&callc_result.stkValue,&(*(float *)var_trans),sizeof(float));
					 break;
				 case IDX_CLASS_BOOL_PTR_C:
					 callc_result={STK_PROPERTY_TYPE_BOOLEAN,(void *)(*((bool *)var_trans)),NULL};
					 break;
				 case IDX_CLASS_BOOL_C:
					 callc_result={STK_PROPERTY_TYPE_BOOLEAN,(void *)(((bool)var_trans)),NULL};
					 break;
				 case CONST_CONST_CHAR_PTR_TYPE_STR:
					 callc_result={STK_PROPERTY_TYPE_STRING,(void *)var_trans,NULL};//new string(*((string *)result))};
					 break;
				 case IDX_CLASS_STRING_PTR_C:
					 s_return_value = *((string *)var_trans);
					 callc_result={STK_PROPERTY_TYPE_STRING,(void *)s_return_value.c_str(),NULL};//new string(*((string *)result))};
					 break;
				 default:
					 callc_result = {STK_PROPERTY_TYPE_SCRIPTVAR,NULL,CScriptClass::instanceScriptVariableByIdx(idx_type,(void *)var_trans)};
					 break;
				}

				return callc_result;
		}

		static inline bool stk2var(tStackElement *stk_ret, int type_return_idx, const string & type_return_str, void **result, string & error){
			void * val_ret=0;
			//string type_return_str = typeid(_T).name();
			CScriptVariable *script_variable=NULL;


			// save return type ...
			if(stk_ret->properties & STK_PROPERTY_IS_STACKVAR){
				stk_ret=((tStackElement *)stk_ret->varRef);
			}

			if((type_return_str == *CScriptClass::STACK_ELEMENT_PTR)){// && (stk_ret->properties & STK_PROPERTY_IS_STACKVAR)){ // set directly stackvar
				val_ret=stk_ret;
			}else{

				switch(GET_INS_PROPERTY_VAR_TYPE(stk_ret->properties)){
				case STK_PROPERTY_TYPE_BOOLEAN:
					if(type_return_str == *CScriptClass::BOOL_TYPE_STR){
						val_ret=(stk_ret->stkValue);
					}else if(type_return_str != *CScriptClass::BOOL_PTR_TYPE_STR){
						val_ret=(&stk_ret->stkValue);
					}else{
						error="cannot convert "+demangle((*CScriptClass::STRING_PTR_TYPE_STR))+" into %s"+demangle(type_return_str).c_str();

						return false;
					}

					break;
				case STK_PROPERTY_TYPE_NUMBER:
					if(type_return_str == *CScriptClass::FLOAT_TYPE_STR){
						val_ret=stk_ret->stkValue;
					}else if(type_return_str == *CScriptClass::FLOAT_PTR_TYPE_STR){
						val_ret=(&stk_ret->stkValue);
					}else if(type_return_str == *CScriptClass::INT_TYPE_STR){
						int *aux_dst = ((int *)&val_ret);
						float *aux_src=(float *)&stk_ret->stkValue;
						*aux_dst=*aux_src;
					}else{
						error="cannot convert "+demangle((*CScriptClass::STRING_PTR_TYPE_STR))+" into %s"+demangle(type_return_str);
						return false;
					}
					break;
				case STK_PROPERTY_TYPE_INTEGER:
					if(type_return_str == *CScriptClass::INT_TYPE_STR){
						val_ret=(stk_ret->stkValue);
					}else if(type_return_str == *CScriptClass::INT_PTR_TYPE_STR){
						val_ret=(&stk_ret->stkValue);
					}else if(type_return_str == *CScriptClass::FLOAT_TYPE_STR){
						float *aux_dst = ((float *)&val_ret);
						int *aux_src=(int *)&stk_ret->stkValue;
						*aux_dst = *aux_src;
					}else{
						error= "cannot convert "+demangle((*CScriptClass::STRING_PTR_TYPE_STR))+" into "+demangle(type_return_str);
						return false;
					}
					break;

				case STK_PROPERTY_TYPE_STRING:
					if(type_return_str == *CScriptClass::STRING_PTR_TYPE_STR){
						if(stk_ret->varRef != 0){
							val_ret=(&((CStringScriptVariable *)(stk_ret->varRef))->m_strValue);
						}
						else{ // pass param string ...
							error= "(string *)Expected varRef not NULL";
							return false;
						}

					}else if (type_return_str == *CScriptClass::CONST_CHAR_PTR_TYPE_STR){
						val_ret=(stk_ret->stkValue);
					}else{
						error= "cannot convert "+demangle((*CScriptClass::STRING_PTR_TYPE_STR))+" into "+demangle(type_return_str);
						return false;
					}


					break;
				default: // script variable by default ...

					script_variable=(CScriptVariable *)stk_ret->varRef;
					CScriptClass *c_class=NULL;

					if(script_variable==NULL){

						error="Variable is not defined";
						return false;
					}

					if(script_variable->idxScriptClass==IDX_CLASS_STRING){
						val_ret=(&script_variable->m_strValue);
					}else if(

					   (script_variable->idxScriptClass==IDX_CLASS_VECTOR
					|| script_variable->idxScriptClass==IDX_CLASS_STRUCT)){

						if(type_return_str==script_variable->getPointer_C_ClassName()){
							val_ret=script_variable->get_C_Object();
						}

					}else if((c_class=script_variable->get_C_Class())!=NULL){ // get the pointer directly ...
						fntConversionType paramConv=0;
						if(c_class->classPtrType==type_return_str){
							val_ret=script_variable->get_C_Object();
						}else if((paramConv=CScriptClass::getConversionType(c_class->classPtrType,type_return_str))!=0){
							val_ret=(void *)paramConv(script_variable);
						}else { // try get C object ..

							error = "cannot convert "+script_variable->getPointer_C_ClassName()+" into "+type_return_str;
							return false;
						}
					}else{ // CScriptVariable ?
						error = " Error calling function, no C-object parameter! Unexpected script variable ("+script_variable->getClassName()+")!";
						return false;
					}
					break;
				}
			}

			*result = val_ret;

			return true;
		}

		template <typename _T>
		static auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
		{

			using tReturn = typename _T::return_type;
			int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());
			 // NO ARGS CASE
			if(idx_return_type == IDX_CLASS_VOID_C){
				*f=((void *)(new std::function<void ()>(
					[&,calling_obj,fun_obj,idx_return_type](){

					CURRENT_VM->execute(
										fun_obj,
										calling_obj);

					}
				)));
			}
			else{ // return type...

				*f=((void *)(new std::function<tReturn ()>(
					[&,calling_obj,fun_obj,idx_return_type](){
							string error;
							tReturn ret_value;

							tStackElement *stk = CURRENT_VM->execute(
									fun_obj,
									calling_obj);

							if(!stk2var(stk,idx_return_type, typeid(tReturn).name(), (void **)(&ret_value),error)){
								zs_print_error_cr("error converting result value: %s",error.c_str());
							}
							return ret_value;
					}
				)));
			}
		}

		// template for last parameter argIdx == 1
		template<typename _T,  typename... ArgTypes>
		static auto bind_script_function_builder(void **f ,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
			-> typename std::enable_if<sizeof...(ArgTypes) == 1>::type
		{
			//return NULL;
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;
			int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());

			if(idx_return_type == IDX_CLASS_VOID_C){

				*f=((void *)(new std::function<void (tParam1)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1){

						vector<tStackElement> args={
								 var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
						};
						CURRENT_VM->execute(
									fun_obj,
									calling_obj,
									args);
					}
				)));
			}
			else{
				*f=((void *)(new std::function<tReturn (tParam1)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1){

							tReturn ret_value;
							string error;

							vector<tStackElement> args={
									var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
							};

							tStackElement *stk = CURRENT_VM->execute(
														fun_obj,
														calling_obj,
														args);

							if(!stk2var(stk,idx_return_type, typeid(tReturn).name(), (void **)(&ret_value),error)){
								zs_print_error_cr("error converting result value: %s",error.c_str());
							}
							return ret_value;
					}
				)));
			}

		}

		// template when parameters argIdx == 2
		template <typename _T, typename... ArgTypes>
		static auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
			-> typename std::enable_if<(sizeof...(ArgTypes) == 2)>::type
		{
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;
			using tParam2 = typename _T::template argument<1>::type;

			int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());

			if(idx_return_type == IDX_CLASS_VOID_C){

				*f=((void *)(new std::function<void (tParam1,tParam2)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2){

						vector<tStackElement> args={
								 var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
								,var2stk(p2,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name()))

						};

						CURRENT_VM->execute(
										fun_obj,
										calling_obj,
										args);
					}

				)));
			}
			else{
				*f=((void *)(new std::function<tReturn (tParam1,tParam2)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2){

							tReturn ret_value;
							string error;

							vector<tStackElement> args={
									 var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
									,var2stk(p2,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name()))
							};


							tStackElement *stk = CURRENT_VM->execute(
														fun_obj,
														calling_obj,
														args);

							if(!stk2var(stk,idx_return_type, typeid(tReturn).name(), (void **)(&ret_value),error)){
								zs_print_error_cr("error converting result value: %s",error.c_str());
							}
							return ret_value;
					}
				)));
			}
			//return NULL;
		}

		// template when parameters argIdx == 3
		template <typename _T, typename... ArgTypes>
		static auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
			-> typename std::enable_if<(sizeof...(ArgTypes) == 3)>::type
		{
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;
			using tParam2 = typename _T::template argument<1>::type;
			using tParam3 = typename _T::template argument<2>::type;

			int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());


			if(idx_return_type == IDX_CLASS_VOID_C){

				*f=((void *)(new std::function<void (tParam1,tParam2,tParam3)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3){

						vector<tStackElement> args={
								 var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
								,var2stk(p2,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name()))
								,var2stk(p3,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name()))


						};

						CURRENT_VM->execute(
											fun_obj,
											calling_obj,
											args);
					}

				)));
			}
			else{
				*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3){
							tReturn ret_value;
							string error;

							vector<tStackElement> args={
									 var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
									,var2stk(p2,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name()))
									,var2stk(p3,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name()))


							};

							tStackElement *stk = CURRENT_VM->execute(
														fun_obj,
														calling_obj,
														args);

							if(!stk2var(stk,idx_return_type, typeid(tReturn).name(), (void **)(&ret_value),error)){
								zs_print_error_cr("error converting result value: %s",error.c_str());
							}
							return ret_value;
					}
				)));
			}
		}

		// template when parameters argIdx == 4
		template <typename _T, typename... ArgTypes>
		static auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
			-> typename std::enable_if<(sizeof...(ArgTypes) == 4)>::type
		{
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;
			using tParam2 = typename _T::template argument<1>::type;
			using tParam3 = typename _T::template argument<2>::type;
			using tParam4 = typename _T::template argument<3>::type;

			int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());

			if(idx_return_type == IDX_CLASS_VOID_C){

				*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){

						vector<tStackElement> args={
								 var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
								,var2stk(p2,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name()))
								,var2stk(p3,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name()))
								,var2stk(p4,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name()))
						};

						CURRENT_VM->execute(
											fun_obj,
											calling_obj,
											args);
					}

				)));
			}
			else{
				*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3,tParam4)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){
							tReturn ret_value;
							string error;

							vector<tStackElement> args={
									 var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
									,var2stk(p2,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name()))
									,var2stk(p3,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name()))
									,var2stk(p4,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name()))

							};

							tStackElement *stk = CURRENT_VM->execute(
														fun_obj,
														calling_obj,
														args);

							if(!stk2var(stk,idx_return_type, typeid(tReturn).name(), (void **)(&ret_value),error)){
								zs_print_error_cr("error converting result value: %s",error.c_str());
							}
							return ret_value;

					}
				)));
			}
		}


		// template when parameters argIdx == 5
		template <typename _T, typename... ArgTypes>
		static auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
			-> typename std::enable_if<(sizeof...(ArgTypes) == 5)>::type
		{
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;
			using tParam2 = typename _T::template argument<1>::type;
			using tParam3 = typename _T::template argument<2>::type;
			using tParam4 = typename _T::template argument<3>::type;
			using tParam5 = typename _T::template argument<4>::type;

			int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());

			if(idx_return_type == IDX_CLASS_VOID_C){

				*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){

						vector<tStackElement> args={
								 var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
								,var2stk(p2,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name()))
								,var2stk(p3,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name()))
								,var2stk(p4,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name()))
								,var2stk(p5,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam5).name()))
						};

						CURRENT_VM->execute(
										fun_obj,
										calling_obj,
										args);
					}

				)));
			}
			else{
				*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3,tParam4,tParam5)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){

							tReturn ret_value;
							string error;

							vector<tStackElement> args={
									 var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
									,var2stk(p2,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name()))
									,var2stk(p3,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name()))
									,var2stk(p4,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name()))
									,var2stk(p5,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam5).name()))
							};

							tStackElement *stk = CURRENT_VM->execute(
														fun_obj,
														calling_obj,
														args);

							if(!stk2var(stk,idx_return_type, typeid(tReturn).name(), (void **)(&ret_value),error)){
								zs_print_error_cr("error converting result value: %s",error.c_str());
							}
							return ret_value;
					}
				)));
			}
			//return NULL;
		}

		// template when parameters argIdx == 6
		template <typename _T, typename... ArgTypes>
		static auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
			-> typename std::enable_if<(sizeof...(ArgTypes) == 6)>::type
		{
			using tReturn = typename _T::return_type;
			using tParam1 = typename _T::template argument<0>::type;
			using tParam2 = typename _T::template argument<1>::type;
			using tParam3 = typename _T::template argument<2>::type;
			using tParam4 = typename _T::template argument<3>::type;
			using tParam5 = typename _T::template argument<4>::type;
			using tParam6 = typename _T::template argument<5>::type;

			int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());

			if(idx_return_type == IDX_CLASS_VOID_C){

				*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){

						vector<tStackElement> args={
								 var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
								,var2stk(p2,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name()))
								,var2stk(p3,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name()))
								,var2stk(p4,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name()))
								,var2stk(p5,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam5).name()))
								,var2stk(p6,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam6).name()))

						};

						CURRENT_VM->execute(
										fun_obj,
										calling_obj,
										args);
					}

				)));
			}
			else{
				*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
					[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){

							tReturn ret_value;
							string error;

							vector<tStackElement> args={
									 var2stk(p1,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name()))
									,var2stk(p2,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name()))
									,var2stk(p3,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name()))
									,var2stk(p4,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name()))
									,var2stk(p5,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam5).name()))
									,var2stk(p6,CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam6).name()))
							};

							tStackElement *stk = CURRENT_VM->execute(
														fun_obj,
														calling_obj,
														args);

							if(!stk2var(stk,idx_return_type, typeid(tReturn).name(), (void **)(&ret_value),error)){
								zs_print_error_cr("error converting result value: %s",error.c_str());
							}
							return ret_value;

					}
				)));
			}
			//return NULL;
		}


		 template <typename _T, std::size_t... Is>
		 static void bind_script_function_builder_base(void **f, CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj,index_sequence<Is...>)
		{
			 bind_script_function_builder<_T, typename _T::template argument<Is>::type...>(f,calling_obj,fun_obj);
		}


		/**
		 * Main bind function
		 */

		bool getScriptObjectFromFunctionAccess(const string &function_access_expression
														  ,CScriptVariable **calling_obj
														  ,CScriptFunctionObject **fun_obj);


		template <  typename _F>
		static std::function<_F> * bind_function(const string & function_access)
		{
			string return_type;
			vector<string> params;
			vector<string> m_arg;
			int idx_return_type=-1;
			void *ptr;
			CScriptFunctionObject * fun=NULL;
			CScriptVariable *calling_obj=NULL;

			if(CZetScript::getInstance()->getScriptObjectFromFunctionAccess(function_access,&calling_obj,&fun))
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
				bind_script_function_builder_base<Traits3>(&ptr,calling_obj,fun,make_index_sequence<Traits3::arity>{});
				return (std::function<_F> *)ptr;

			}

			return NULL;

		}



		template<typename _T>
		static vector<_T> vscript2vector(CVectorScriptVariable *v_in){
			vector<_T> v_out;
			string typestr = typeid(_T).name();


			if(v_in){
				for(int i = 0; i < v_in->size(); i++){

					tStackElement sv=v_in->m_objVector[i];

					switch(sv.properties & MASK_VAR_PRIMITIVE_TYPES)
					{
						default:
						case STK_PROPERTY_TYPE_UNDEFINED:
						case STK_PROPERTY_TYPE_NULL:
						case STK_PROPERTY_TYPE_NUMBER:
						case STK_PROPERTY_TYPE_BOOLEAN:
						case STK_PROPERTY_TYPE_STRING:
						case STK_PROPERTY_TYPE_FUNCTION:
							zs_print_error_cr("type not implemented to cast. Sorry.");
							return v_out;
							break;
						case STK_PROPERTY_TYPE_INTEGER:
							if((typestr == typeid(int).name())){
								v_out.push_back((intptr_t)sv.stkValue);
							}else{
								zs_print_error_cr("Error trying to cast element on vector<int>");
								return v_out;
							}
							break;

					}
				}
			}


			return v_out;
		}

		template<typename _T>
		static CVectorScriptVariable * vector2vscript(const vector<_T> & v){
			CVectorScriptVariable *vsv = new CVectorScriptVariable();

			for ( unsigned i = 0; i < v.size(); i++){
				tStackElement *stk = vsv->push();
				*stk = var2stk(v[i],CScriptClass::getIdxClassFromIts_C_Type(typeid(_T).name()));
			}

			return vsv;
		}

		//----




		CVirtualMachine * getVirtualMachine();
		CScriptVariable *getMainObject(){return m_mainObject;}



		bool execute();

		bool eval(const string & string, bool execute=true, const char *filename=NULL);
		bool eval_file(const char * filename);

	
		static void destroy();
	};

}



