#pragma once

#define VM_STACK_LOCAL_VAR_MAX				256
#define VM_SCOPE_MAX						64
#define MAX_FUNCTION_CALL 					128
#define MAX_SHARES_VARIABLE 				256


#define SET_INTEGER_RETURN(i)	CURRENT_VM->setIntegerReturnValue(i)
#define SET_BOOLEAN_RETURN(b) 	CURRENT_VM->setBooleanReturnValue(b)
#define SET_FLOAT_RETURN(f)   	CURRENT_VM->setFloatReturnValue(f)

#define NO_PARAMS std::vector<StackElement>{}
#define ZS_VM_FUNCTION_TYPE std::function<ScriptObjectObject * (const std::vector<ScriptObjectObject *> & param)>

#define VM_EXECUTE(vm,o,f,stk,n)	vm_execute(vm,o,f,stk,n,__FILE__,__LINE__)
#define VM_ERROR(s,...)				data->vm_error=true;data->vm_error_str=ZS_LOG_FILE_LINE_STR(SFI_GET_FILE(calling_function,instruction),SFI_GET_LINE(calling_function,instruction))+zetscript::zs_strutils::format(s, ##__VA_ARGS__);
#define VM_ERROR_AND_RET(s,...)		data->vm_error=true;data->vm_error_str=ZS_LOG_FILE_LINE_STR(SFI_GET_FILE(calling_function,instruction),SFI_GET_LINE(calling_function,instruction))+zetscript::zs_strutils::format(s, ##__VA_ARGS__);return;
#define VM_STOP_EXECUTE(s,...)		data->vm_error=true;data->vm_error_str=ZS_LOG_FILE_LINE_STR(SFI_GET_FILE(calling_function,instruction),SFI_GET_LINE(calling_function,instruction))+zetscript::zs_strutils::format(s, ##__VA_ARGS__);goto lbl_exit_function;
#define VM_SET_USER_ERROR(vm,s,...)	vm_set_error_file_line(vm,__FILE__,__LINE__, s, ##__VA_ARGS__)


namespace zetscript{

	struct  VirtualMachine{
		void *data;
	};

	VirtualMachine *vm_new(ZetScript *zs);


	/*void  vm_call_function_script(
			VirtualMachine		* 	vm,
			ScriptObject		* 	this_object,
			ScriptFunction 		*	info_function,
			StackElement 		* 	_stk_start_args,
			//std::string 		* _ptrStartStr=NULL,
			unsigned char 		n_args=0
	);


	 void  vm_call_function_native(
		 VirtualMachine		* 	vm,
		 const ScriptFunction *calling_function,
		 StackElement *stk_arg_calling_function,
		 unsigned char n_args,
		 Instruction *ins,
		 ScriptObject  * this_object
	);*/

	/**
	 * Reserve for N vars. Return base pointer.
	 */

	//const char * toString(StackElement * index);

	void 			vm_push_stack_element(VirtualMachine *vm, StackElement stk);
	bool 			vm_share_pointer(VirtualMachine *vm,ScriptObject *_obj);
	bool 			vm_create_shared_pointer(VirtualMachine *vm,ScriptObject *_obj);
	inline bool 	vm_unref_shared_script_object_and_remove_if_zero(VirtualMachine *vm,ScriptObject **so);
	void 			vm_destroy_life_time_object(VirtualMachine *vm,ScriptObject *script_object);
	bool 			vm_set_stack_element_at(VirtualMachine *vm,unsigned int idx, StackElement stk);

	void 			vm_set_error_file_line(VirtualMachine *vm, const char *file, int line, const char *in_txt,...);
	std::string 	vm_get_error(VirtualMachine *vm);

	StackElement * 	vm_get_top_stack_element_from_stack(VirtualMachine *vm);
	StackElement *	vm_get_stack_elements(VirtualMachine *vm);
	StackElement * 	vm_get_stack_element_at(VirtualMachine *vm,unsigned int idx_glb_element);
	StackElement vm_execute(
		VirtualMachine 	*	vm
		,ScriptObject 	*	this_object
		,ScriptFunction *	calling_function
		,StackElement 	*  	stk_params=NULL
		,unsigned char		n_stk_params=0
		,const char 	*	file=NULL
		,int 				line=-1
	);
	//std::string  convertStackElementVarTypeToStr(StackElement stk_v)


	/*inline bool vm_insert_shared_node							(VirtualMachine		* 	vm,InfoSharedList * list, InfoSharedPointerNode *_node);
	inline bool vm_deattach_shared_Node							(VirtualMachine		* 	vm,InfoSharedList * list, InfoSharedPointerNode *_node);
	inline bool vm_decrement_shared_nodes_and_dettach_if_zero	(VirtualMachine		* 	vm,InfoSharedPointerNode *_node, bool & is_dettached);
	void 		vm_insert_life_time_object						(VirtualMachine 	* 	vm,const char *file, int line, ScriptObject *script_object);


	inline ScriptFunction *  vm_find_function(
		VirtualMachine		* 	vm
		,ScriptObject *calling_object
		,ScriptFunction *info_function
		,Instruction * instruction
		,bool is_constructor
		,void *stk_elements_builtin_ptr // can be **stack_element from ScriptObjectObject stk_properties/metamethods or can be *StackElement from global -i.e vm_stack-)...
		,int stk_elements_builtin_len // length of stk_builtin_elements
		,const std::string & symbol_to_find
		,StackElement *stk_arg
		,unsigned char n_args
	);


	inline bool vm_apply_metamethod_primitive(
		VirtualMachine		* 	vm
		,ScriptFunction *info_function
		,Instruction *instruction
		,ByteCodeMetamethod op_code_methamethod
		,StackElement *stk_result_op1
		,StackElement *stk_result_op2
		, bool & error
	);

	inline bool vm_apply_metamethod(
		 VirtualMachine		* 	vm
		,ScriptFunction *info_function
		,Instruction *instruction
		,ByteCodeMetamethod op_code_methamethod
		,StackElement *stk_result_op1
		,StackElement *stk_result_op2
	);

	//inline StackElement performAddString(StackElement *stk_result_op1,StackElement *stk_result_op2);
	//inline StackElement performSubString(StackElement *stk_result_op1,StackElement *stk_result_op2);

	void 				vm_do_stack_dump(VirtualMachine		* 	vm);*/



}


#include "vm.tcc"

