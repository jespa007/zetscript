#pragma once


namespace ZetScript{


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

	#define NO_PARAMS std::vector<StackElement>{}
	#define ZS_VM_FUNCTION_TYPE std::function<CScriptVar * (const std::vector<CScriptVar *> & param)>

	class ScriptFunction;
	class ZetScript;
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

		//===================================================================================================

		typedef struct{
			OP_CODE op_code;
			const char *str;
		}OpCodeInfo;

		ScriptFunction  *main_function_object;
		ScriptFunction 	**vec_script_function_node;
		unsigned				size_vec_script_function_object_node;
		void destroyCache();

		int idxCurrentStack;
		int idx_laststatment;
		const ScriptFunction *current_call_c_function;
		ZetScript *zs;
		ScriptFunctionFactory *script_function_factory;
		ScriptClassFactory 	*script_class_factory;




		float f_return_value;
		std::string s_return_value;
		StackElement stk_aux;


		StackElement  execute_internal(
				ScriptFunction *info_function,
				CScriptVar * this_object,
				bool & error,
				StackElement 		  * _ptrStartOp=NULL,
				std::string 		  		  * _ptrStartStr=NULL,
				unsigned char n_args=0,
				ByteCode *calling_instruction = NULL);

		bool cancel_execution;
		const char *custom_error;


	public:


	//===================================================================================================
	//
	// POINTER MANAGER ...
	//

		PInfoSharedPointerNode newSharedPointer(CScriptVar *var_ptr);
		bool sharePointer( PInfoSharedPointerNode _node);
		void unrefSharedScriptVar( PInfoSharedPointerNode _node, bool remove_if_0=false);
		void removefSharedScriptVar( PInfoSharedPointerNode _node);



		//void iniStackVar(unsigned int pos,const StackElement & stk);

		const ScriptFunction * getCurrent_C_FunctionCall();

		std::string stk_C_TypeStr(const StackElement & stk_v);

		inline float *setFloatReturnValue(float f){
			f_return_value = f;
			return &f_return_value;
		}

		inline std::string *setStringReturnValue(std::string s){
			s_return_value = s;
			return &s_return_value;
		}

		void addGlobalVar(const StackElement & stk);

		void clearGlobalVars();

		 StackElement execute(
					 ScriptFunction *info_function,
					 CScriptVar *this_object,
					 bool & error,
					const std::vector<StackElement> &  argv=NO_PARAMS);



		StackElement *getLastStackValue();
		StackElement * getStackElement(unsigned int idx_glb_element);

		//template<typename _T>


		void buildCache();

		void cancelExecution();
		void setError(const char *str);

		CVirtualMachine(ZetScript * _zs);
		~CVirtualMachine();



	private:

		struct tVM_ScopeInfo{
			short					   index;
			ScriptFunction *ptr_info_function;
			StackElement 		  *ptr_local_var;
			unsigned char properties;
		};

		struct tForeachInfo{
			StackElement 		   *key; // iterator element can be std::string or integer...
			CScriptVar			*ptr; // can be struct or std::vector...
			unsigned int 		   idx_current;

		};

		char		str_aux[8192];
		float 		f_aux_value1,f_aux_value2;
		 std::string 	aux_string,symbol_to_find,error_str;
		 tForeachInfo stkForeach[VM_MAX_FOREACH];
		 tForeachInfo *current_foreach;



		 std::string     aux_string_param[MAX_N_ARGS]; // for std::string params...

		 tVM_ScopeInfo		*current_scope_info_ptr;
		 tVM_ScopeInfo		scope_info[VM_MAX_SCOPES];
		 tVM_ScopeInfo		*MAX_SCOPE_INFO;


		std::string 				stkString[VM_MAX_AUX_STRINGS]; // aux values for std::string ...
		std::string              *ptrLastStr;
		std::string              *ptrCurrentStr;

		 StackElement     stack[VM_LOCAL_VAR_MAX_STACK];
		 int n_globals;

		 // global vars show be initialized to stack array taking the difference (the registered variables on the main function) - global_vars ...
		 std::vector<StackElement> global_var;
		StackElement *stkCurrentData;


		 StackElement  call_C_function(
				 intptr_t fun_ptr,
				 const ScriptFunction *irfs,
				 bool & error,
				 StackElement *ptrArg,
				 unsigned char n_args,
				 ByteCode *ins,
				 CScriptVar  * this_object);

		/**
		 * Reserve for N vars. Return base pointer.
		 */

		const char * STR_GET_TYPE_VAR_INDEX_INSTRUCTION(StackElement * index);
		inline void  REMOVE_0_SHARED_POINTERS(int idxCurrentStack,void *ptr_callc_result);
		inline ScriptFunction *  FIND_FUNCTION(
									CScriptVar *calling_object
									,ScriptFunction *info_function
									,ByteCode *instruction
									,ByteCode * callAleInstruction

				 	 	 	 	 	,std::vector<FunctionSymbol> *m_functionSymbol
									,std::vector<ScriptFunction *> *vec_global_functions

									,bool is_constructor
									,const std::string & symbol_to_find


									,StackElement *stkResultOp1
									,StackElement *stkResultOp2
									,StackElement *startArg
									,unsigned char n_args
									,const char * metamethod_str);
		inline bool ASSIGN_STACK_VAR(StackElement *dst_ins, StackElement *src_ins,ByteCode *instruction);
		inline bool POP_SCOPE_CALL(int idx_stack,void * ptr_callc_result, unsigned char properties);

		inline bool APPLY_METAMETHOD(
										CScriptVar *calling_object
										,ScriptFunction *info_function
										,ByteCode *instruction
										,const char *__OVERR_OP__
										,METAMETHOD_OP_CODE __METAMETHOD__
										,StackElement *stkResultOp1
										,StackElement *stkResultOp2

									);

		void 				stackDumped();

	};

}


#include "CVirtualMachine.inc"
