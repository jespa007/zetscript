#pragma once


namespace zetscript{

	#include "core/CCompiler.h"
	//#include "core/CScriptClass.h"


	//#define MAX_PER_TYPE_OPERATIONS 32
	#define VM_LOCAL_VAR_MAX_STACK				2048
	#define VM_AUX_VALUES						512


	#define MAX_FUNCTION_CALL 256

	#define MAX_SHARES_VARIABLE 64


	#define SET_INTEGER_RETURN(i)	CURRENT_VM->setIntegerReturnValue(i)
	#define SET_BOOLEAN_RETURN(b) 	CURRENT_VM->setBooleanReturnValue(b)
	#define SET_FLOAT_RETURN(f)   	CURRENT_VM->setFloatReturnValue(f)
	#define SET_STRING_RETURN(s)  	CURRENT_VM->setStringReturnValue(s)

	#define NO_PARAMS vector<zetscript::CScriptVariable*>{}
	#define ZS_VM_FUNCTION_TYPE std::function<CScriptVariable * (const vector<CScriptVariable *> & param)>


	int 									getIdxClassFromIts_C_Type(const string & c_type_str);
	BASIC_CLASS_TYPE 				getIdxPrimitiveFromIts_C_Type(const string & c_type_str);


	class CScriptFunction;
	class  CVirtualMachine{



	//===================================================================================================
	//
	// POINTER MANAGER ...
	//


		typedef struct {
			PInfoSharedPointerNode first, last;
		}tInfoSharedList;


		tInfoSharedList zero_shares[MAX_FUNCTION_CALL];
		tInfoSharedList shared_var[MAX_FUNCTION_CALL];


		int idxCurrentStack;

	//===================================================================================================


		float f_return_value;
		string s_return_value;
		tStackElement stk_aux;

		tStackElement * execute_internal(
				CScriptFunctionObject *info_function,
				CScriptVariable * this_object,
				tStackElement 		  * _ptrStartOp=NULL,
				string 		  		  * _ptrStartStr=NULL,
				unsigned char n_args=0,
				int idxAstNode = ZS_UNDEFINED_IDX);//vector<CScriptVariable *> * argv=NULL,int stk=0);



	public:


	//===================================================================================================
	//
	// POINTER MANAGER ...
	//

		PInfoSharedPointerNode newSharedPointer(CScriptVariable *var_ptr);
		void sharePointer( PInfoSharedPointerNode _node);
		void unrefSharedScriptVar( PInfoSharedPointerNode _node);
		void removefSharedScriptVar( PInfoSharedPointerNode _node);

	//===================================================================================================

		void iniStackVar(unsigned int pos,const tStackElement & stk);
		void clearGlobals();

		string stk_C_TypeStr(const tStackElement & stk_v);

		inline float *setFloatReturnValue(float f){
			f_return_value = f;
			return &f_return_value;
		}

		inline string *setStringReturnValue(string s){
			s_return_value = s;
			return &s_return_value;
		}


		CScriptVariable * execute(
					 CScriptFunctionObject *info_function,
					 CScriptVariable *this_object,
					const vector<CScriptVariable *> &  argv=NO_PARAMS);


		void setCallResult(tStackElement *);
		tStackElement *getLastStackValue();
		tStackElement * getStackElement(unsigned int idx_glb_element);

		CVirtualMachine();
		~CVirtualMachine();



	private:

		struct tVM_ScopeInfo{
			short					   index;
			CScriptFunctionObject *ptr_info_function;
			tStackElement 		  *ptr_local_var;
		};

		char		str_aux[8192];
		float 		f_aux_value1,f_aux_value2;
		 string 	aux_string,symbol_to_find;

		 string     aux_string_param[MAX_N_ARGS]; // for string params...

		 vector<tSymbolInfo> vec_aux_function_symbol;
		 tVM_ScopeInfo		*current_scope_idx;
		 tVM_ScopeInfo		scope_idx[VM_LOCAL_VAR_MAX_STACK];
		tStackElement callc_result;


		string 				stkString[VM_LOCAL_VAR_MAX_STACK]; // aux values for string ...
		string              *ptrLastStr;
		string              *ptrCurrentStr;

		 tStackElement     stack[VM_LOCAL_VAR_MAX_STACK];
		tStackElement *ptrCurrentOp;


		 tStackElement * call_C_function(
				 intptr_t fun_ptr,
				 const CScriptFunctionObject *irfs,
				 tStackElement *ptrArg,
				 unsigned char n_args,
				 int idxAstNode);

		/**
		 * Reserve for N vars. Return base pointer.
		 */

		string STR_GET_TYPE_VAR_INDEX_INSTRUCTION(tStackElement * index);
		void 				stackDumped();

	};

}
