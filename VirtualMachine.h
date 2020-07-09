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
	#define SET_FLOAT_RETURN(f)   	CURRENT_VM->SetFloatReturnValue(f)
	#define SET_STRING_RETURN(s)  	CURRENT_VM->SetStringReturnValue(s)

	#define NO_PARAMS std::vector<StackElement>{}
	#define ZS_VM_FUNCTION_TYPE std::function<ScriptVar * (const std::vector<ScriptVar *> & param)>

	class ScriptFunction;
	class ZetScript;
	class  VirtualMachine{

	//===================================================================================================
	//
	// POINTER MANAGER ...
	//

		typedef struct {
			PInfoSharedPointerNode first, last;
		}InfoSharedList;

		InfoSharedList zero_shares[MAX_FUNCTION_CALL];
		InfoSharedList shared_var[MAX_FUNCTION_CALL];

		//===================================================================================================

		typedef struct{
			ByteCode byte_code_op;
			const char *str;
		}OpCodeInfo;

		ScriptFunction  *main_function_object;
		ScriptFunction 	**script_functions;
		unsigned				size_vec_script_function_object_node;
		void destroyCache();

		int idx_current_stack;
		int idx_last_statment;
		const ScriptFunction *current_call_c_function;
		ZetScript *zs;
		ScriptFunctionFactory *script_function_factory;
		ScriptClassFactory 	*script_class_factory;




		float f_return_value;
		std::string s_return_value;
		StackElement stk_aux;


		StackElement  ExecuteInternal(
				ScriptFunction *info_function,
				ScriptVar * this_object,
				bool & error,
				StackElement 		  * _ptrStartOp=NULL,
				std::string 		  		  * _ptrStartStr=NULL,
				unsigned char n_args=0,
				Instruction *calling_instruction = NULL);

		bool cancel_execution;
		const char *custom_error;


	public:


	//===================================================================================================
	//
	// POINTER MANAGER ...
	//

		PInfoSharedPointerNode NewSharedPointer(ScriptVar *var_ptr);
		bool SharePointer( PInfoSharedPointerNode _node);
		void UnrefSharedScriptVar( PInfoSharedPointerNode _node, bool remove_if_0=false);
		void RemoveSharedScriptVar( PInfoSharedPointerNode _node);



		//void iniStackVar(unsigned int pos,const StackElement & stk);

		const ScriptFunction * GetCurrent_C_FunctionCall();

		std::string Stk_C_TypeStr(const StackElement & stk_v);

		inline float *SetFloatReturnValue(float f){
			f_return_value = f;
			return &f_return_value;
		}

		inline std::string *SetStringReturnValue(std::string s){
			s_return_value = s;
			return &s_return_value;
		}

		void AddGlobalVar(const StackElement & stk);

		void ClearGlobalVars();

		 StackElement Execute(
					 ScriptFunction *info_function,
					 ScriptVar *this_object,
					 bool & error,
					const std::vector<StackElement> &  argv=NO_PARAMS);



		StackElement *GetLastStackValue();
		StackElement * GetStackElement(unsigned int idx_glb_element);

		//template<typename _T>


		void buildCache();

		void CancelExecution();
		void SetError(const char *str);

		VirtualMachine(ZetScript * _zs);
		~VirtualMachine();



	private:

		struct VM_ScopeInfo{
			short					   index;
			ScriptFunction *ptr_info_function;
			StackElement 		  *ptr_local_var;
			unsigned char properties;
		};

		struct ForeachInfo{
			StackElement 		   *key; // iterator element can be std::string or integer...
			ScriptVar			*ptr; // can be struct or std::vector...
			unsigned int 		   idx_current;

		};

		char		str_aux[8192];
		float 		f_aux_value1,f_aux_value2;
		 std::string 	aux_string,symbol_to_find,error_str;
		 ForeachInfo stk_foreach[VM_MAX_FOREACH];
		 ForeachInfo *current_foreach;



		 std::string     aux_string_param[MAX_N_ARGS]; // for std::string params...

		 VM_ScopeInfo		*current_scope_info_ptr;
		 VM_ScopeInfo		scope_info[VM_MAX_SCOPES];
		 VM_ScopeInfo		*max_scope_info;


		std::string 				stk_string[VM_MAX_AUX_STRINGS]; // aux values for std::string ...
		std::string              *ptr_last_str;
		std::string              *ptr_current_str;

		 StackElement     stack[VM_LOCAL_VAR_MAX_STACK];
		 int n_globals;

		 // global vars show be initialized to stack array taking the difference (the registered variables on the main function) - global_vars ...
		 std::vector<StackElement> global_var;
		StackElement *stk_current_data;


		 StackElement  Call_C_Function(
				 intptr_t fun_ptr,
				 const ScriptFunction *irfs,
				 bool & error,
				 StackElement *ptrArg,
				 unsigned char n_args,
				 Instruction *ins,
				 ScriptVar  * this_object);

		/**
		 * Reserve for N vars. Return base pointer.
		 */

		const char * IndexInstructionVarTypeToStr(StackElement * index);
		inline void  RemoveEmptySharedPointers(int idx_current_stack,void *ptr_callc_result);
		inline ScriptFunction *  FindFunction(
									ScriptVar *calling_object
									,ScriptFunction *info_function
									,Instruction *instruction
									,Instruction * call_ale_instruction

				 	 	 	 	 	,std::vector<FunctionSymbol> *function_symbol
									,std::vector<ScriptFunction *> *global_functions

									,bool is_constructor
									,const std::string & symbol_to_find


									,StackElement *stk_result_op1
									,StackElement *stk_result_op2
									,StackElement *start_arg
									,unsigned char n_args
									,const char * metamethod_str);
		inline bool AssignStackVar(StackElement *dst_ins, StackElement *src_ins,Instruction *instruction);
		inline bool PopScopeCall(int idx_stack,void * ptr_callc_result, unsigned char properties);

		inline bool ApplyMetamethod(
										ScriptVar *calling_object
										,ScriptFunction *info_function
										,Instruction *instruction
										,const char *op_code_str
										,ByteCodeMetamethod op_code_methamethod
										,StackElement *stk_result_op1
										,StackElement *stk_result_op2

									);

		void 				DoStackDump();

	};

}


#include "VirtualMachine.inc"
