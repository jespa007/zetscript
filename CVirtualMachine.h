#pragma once


namespace zetscript{


	//#define MAX_PER_TYPE_OPERATIONS 32

	#define VM_LOCAL_VAR_MAX_STACK				256
	#define VM_MAX_SCOPES						64
	#define VM_MAX_FOREACH						64
	#define MAX_FUNCTION_CALL 					128
	#define VM_MAX_AUX_STRINGS					128
	#define MAX_SHARES_VARIABLE 				64


	#define SET_INTEGER_RETURN(i)	CURRENT_VM->setIntegerReturnValue(i)
	#define SET_BOOLEAN_RETURN(b) 	CURRENT_VM->setBooleanReturnValue(b)
	#define SET_FLOAT_RETURN(f)   	CURRENT_VM->setFloatReturnValue(f)
	#define SET_STRING_RETURN(s)  	CURRENT_VM->setStringReturnValue(s)

	#define NO_PARAMS vector<tStackElement>{}
	#define ZS_VM_FUNCTION_TYPE std::function<CScriptVariable * (const vector<CScriptVariable *> & param)>




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

		CScriptFunction  *main_function_object;
		CScriptFunction 	**vec_script_function_node;
		unsigned				size_vec_script_function_object_node;
		CASTNode 				**vec_ast_node;
		unsigned  				size_vec_ast_node;
		void destroyCache();

		int idxCurrentStack;
		int idx_laststatment;
		int current_ast_node_call_c_function;

	//===================================================================================================


		float f_return_value;
		string s_return_value;
		tStackElement stk_aux;

		tStackElement  execute_internal(
				CScriptFunction *info_function,
				CScriptVariable * this_object,
				bool & error,
				tStackElement 		  * _ptrStartOp=NULL,
				string 		  		  * _ptrStartStr=NULL,
				unsigned char n_args=0,
				int idxAstNode = ZS_UNDEFINED_IDX);

		bool cancel_execution;
		const char *custom_error;


	public:


	//===================================================================================================
	//
	// POINTER MANAGER ...
	//

		PInfoSharedPointerNode newSharedPointer(CScriptVariable *var_ptr);
		bool sharePointer( PInfoSharedPointerNode _node);
		void unrefSharedScriptVar( PInfoSharedPointerNode _node, bool remove_if_0=false);
		void removefSharedScriptVar( PInfoSharedPointerNode _node);

	//===================================================================================================

		void iniStackVar(unsigned int pos,const tStackElement & stk);
		void clearGlobals();
		int getCurrentAstNodeCall_C_Function();

		string stk_C_TypeStr(const tStackElement & stk_v);

		inline float *setFloatReturnValue(float f){
			f_return_value = f;
			return &f_return_value;
		}

		inline string *setStringReturnValue(string s){
			s_return_value = s;
			return &s_return_value;
		}


		ZETSCRIPT_MODULE_EXPORT tStackElement execute(
					 CScriptFunction *info_function,
					 CScriptVariable *this_object,
					 bool & error,
					const vector<tStackElement> &  argv=NO_PARAMS);



		tStackElement *getLastStackValue();
		tStackElement * getStackElement(unsigned int idx_glb_element);

		//template<typename _T>


		void buildCache();

		void cancelExecution();
		void setError(const char *str);

		CVirtualMachine();
		~CVirtualMachine();



	private:

		struct tVM_ScopeInfo{
			short					   index;
			CScriptFunction *ptr_info_function;
			tStackElement 		  *ptr_local_var;
			unsigned char properties;
		};

		struct tForeachInfo{
			tStackElement 		   *key; // iterator element can be string or integer...
			CScriptVariable			*ptr; // can be struct or vector...
			unsigned int 		   idx_current;

		};

		char		str_aux[8192];
		float 		f_aux_value1,f_aux_value2;
		 string 	aux_string,symbol_to_find,error_str;
		 tForeachInfo stkForeach[VM_MAX_FOREACH];
		 tForeachInfo *current_foreach;



		 string     aux_string_param[MAX_N_ARGS]; // for string params...

		 tVM_ScopeInfo		*current_scope_info_ptr;
		 tVM_ScopeInfo		scope_info[VM_MAX_SCOPES];
		 tVM_ScopeInfo		*MAX_SCOPE_INFO;


		string 				stkString[VM_MAX_AUX_STRINGS]; // aux values for string ...
		string              *ptrLastStr;
		string              *ptrCurrentStr;

		 tStackElement     stack[VM_LOCAL_VAR_MAX_STACK];
		tStackElement *ptrCurrentOp;


		 tStackElement  call_C_function(
				 intptr_t fun_ptr,
				 const CScriptFunction *irfs,
				 bool & error,
				 tStackElement *ptrArg,
				 unsigned char n_args,
				 int idxAstNode);

		/**
		 * Reserve for N vars. Return base pointer.
		 */

		const char * STR_GET_TYPE_VAR_INDEX_INSTRUCTION(tStackElement * index);
		inline void  REMOVE_0_SHARED_POINTERS(int idxCurrentStack,void *ptr_callc_result);
		inline CScriptFunction *  FIND_FUNCTION(vector<tFunctionSymbol> *m_functionSymbol
									,vector<int> *vec_global_functions
									,tInstruction * iao
									,bool is_constructor
									,const string & symbol_to_find
									,CScriptVariable *calling_object
									,tInstruction *instruction
									,tStackElement *ptrResultInstructionOp1
									,tStackElement *ptrResultInstructionOp2
									,tStackElement *startArg
									,unsigned char n_args
									,const char * metamethod_str);
		inline bool ASSIGN_STACK_VAR(tStackElement *dst_ins, tStackElement *src_ins,tInstruction *instruction);
		inline bool POP_SCOPE_CALL(int idx_stack,void * ptr_callc_result, unsigned char properties);

		inline bool APPLY_METAMETHOD(
											const char *__OVERR_OP__
											,METAMETHOD_OPERATOR __METAMETHOD__
											,CScriptVariable *calling_object
											,tInstruction *instruction
											,tStackElement *ptrResultInstructionOp1
											,tStackElement *ptrResultInstructionOp2

											);

		void 				stackDumped();

	};

}


#include "CVirtualMachine.inc"
