#pragma once

namespace zetscript{

	#define VM_STACK_LOCAL_VAR_MAX				256
	#define VM_SCOPE_MAX						64
	#define MAX_FUNCTION_CALL 					128
	#define MAX_SHARES_VARIABLE 				64


	#define SET_INTEGER_RETURN(i)	CURRENT_VM->setIntegerReturnValue(i)
	#define SET_BOOLEAN_RETURN(b) 	CURRENT_VM->setBooleanReturnValue(b)
	#define SET_FLOAT_RETURN(f)   	CURRENT_VM->setFloatReturnValue(f)

	#define NO_PARAMS std::vector<StackElement>{}
	#define ZS_VM_FUNCTION_TYPE std::function<ScriptObject * (const std::vector<ScriptObject *> & param)>

	#define VM_ERROR(script_def,s,...)				error=true;error_str=zetscript::zs_strutils::format(script_def,"[%s:%i] %s",s, ##__VA_ARGS__);
	#define VM_ERROR_AND_RET(script_def,s,...)		error=true;error_str=zetscript::zs_strutils::format(script_def,"[%s:%i] %s",s, ##__VA_ARGS__);return stk_result;


	class ScriptFunction;
	class ZetScript;
	class  VirtualMachine{

	public:

		VirtualMachine(ZetScript *_zs);

		InfoSharedPointerNode *newSharedPointer(ScriptObject *var_ptr);
		void sharePointer( InfoSharedPointerNode *_node);
		void unrefSharedScriptObject( InfoSharedPointerNode *_node, bool remove_if_0=false);
		void removeSharedScriptObject( InfoSharedPointerNode *_node);

		const ScriptFunction * getCurrent_C_FunctionCall();

		void init();

		/*inline float *setFloatReturnValue(float f){
			f_return_value = f;
			return &f_return_value;
		}

		inline std::string *setStringReturnValue(std::string s){
			s_return_value = s;
			return &s_return_value;
		}*/

		void addGlobalVar(const StackElement & stk);

		void clearGlobalVars();

		StackElement execute(
			 ScriptFunction *	script_function
			 ,ScriptObject 	*	this_object
			 ,StackElement 	*  	stk_params=NULL
			 ,unsigned char		n_stk_params=0
		);

		 void setStackElement(unsigned int idx, StackElement stk);

		StackElement *getLastStackValue();
		StackElement * getStackElement(unsigned int idx_glb_element);

		void cancelExecution();
		void setError(const char *str);

		~VirtualMachine();

	private:

		//===================================================================================================
		//
		// POINTER MANAGER ...
		//

		InfoSharedList zero_shares[MAX_FUNCTION_CALL]; // each function contains the number of local scriptvars vars that should or not removed.
		InfoSharedList shared_vars; // global vector

		//===================================================================================================

		typedef struct{
			ByteCode byte_code;
			const char *str;
		}OpCodeInfo;


		struct VM_Scope{
			Scope				*scope;
			ScriptFunction 		*script_function;
			StackElement 		*stk_local_vars;
			unsigned char 		properties;
		};

		struct VM_Foreach{
			StackElement 		   	*key; // iterator element can be std::string or integer...
			ScriptObject				*ptr; // can be struct or std::vector...
			unsigned int 		   	idx_current;

		};

		//char				str_aux[8192];
		 float 				f_aux_value1,f_aux_value2;
		 bool				error;
		 std::string 		error_str;
		 std::string 		error_callstack_str;
		 VM_Scope			*vm_current_scope;
		 VM_Scope			vm_scope[VM_SCOPE_MAX];
		 VM_Scope			*vm_scope_max;

		 StackElement     	vm_stack[VM_STACK_LOCAL_VAR_MAX];

		 // global vars show be initialized to stack array taking the difference (the registered variables on the main function) - global_vars ...
		StackElement *vm_stk_current;
		ScriptFunction  *main_function_object;
		ScriptClass *main_class_object;

		int idx_current_call;
		int idx_last_statment;
		const ScriptFunction *current_call_c_function;
		ZetScript *zs;		ScriptFunctionFactory 	*script_function_factory;
		ScriptClassFactory 		*script_class_factory;
		ScopeFactory 			*scope_factory;


		//float f_return_value;
		//std::string s_return_value;
		StackElement stk_aux;

		bool cancel_execution;
		const char *custom_error;


		StackElement  callFunctionScript(
				ScriptObject 		* 	this_object,
				ScriptFunction 	*	info_function,
				StackElement 	* 	_stk_start_args=NULL,
				//std::string 		  		  * _ptrStartStr=NULL,
				unsigned char 		n_args=0,
				Instruction *calling_instruction = NULL);


		 StackElement  callFunctionNative(
			 const ScriptFunction *calling_function,
			 StackElement *stk_arg_calling_function,
			 unsigned char n_args,
			 Instruction *ins,
			 ScriptObject  * this_object
		);

		/**
		 * Reserve for N vars. Return base pointer.
		 */

		//const char * toString(StackElement * index);
		inline void  removeEmptySharedPointers();
		inline void insertShareNode(InfoSharedList * list, InfoSharedPointerNode *_node);
		inline void deattachShareNode(InfoSharedList * list, InfoSharedPointerNode *_node);
		//std::string  convertStackElementVarTypeToStr(StackElement stk_v)

		inline ScriptFunction *  findFunction(
			ScriptObject *calling_object
			,ScriptFunction *info_function
			,Instruction * instruction
			,bool is_constructor
			,void *stk_elements_ptr // can be **stack_element from ScriptObject stk_properties/metamethods or can be *StackElement from global -i.e vm_stack-)...
			,int stk_elements_len // length of stk_elements
			,const std::string & symbol_to_find
			,StackElement *stk_arg
			,unsigned char n_args
			,StackElement *stk_result_op1=NULL
			,StackElement *stk_result_op2=NULL
			,const char * metamethod_str=NULL
		);


		inline void popVmScope(bool check_empty_shared_pointers=true);

		inline void applyMetamethod(
			 ScriptFunction *info_function
			,Instruction *instruction
			,const char *op_code_str
			,ByteCodeMetamethod op_code_methamethod
			,StackElement *stk_result_op1
			,StackElement *stk_result_op2

		);

		inline StackElement performAddString(StackElement *stk_result_op1,StackElement *stk_result_op2);
		inline StackElement performSubString(StackElement *stk_result_op1,StackElement *stk_result_op2);

		void 				doStackDump();

	};

}


#include "VirtualMachine.tcc"
//#include "VirtualMachine_call_function_script.tcc"
