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

#define VM_EXECUTE(vm,o,f,stk,n)	vm_execute(vm,o,f,stk,n,0,__FILE__,__LINE__)
#define VM_ERROR(s,...)				data->vm_error=true;data->vm_error_str=ZS_LOG_FILE_LINE_STR(SFI_GET_FILE(calling_function,instruction),SFI_GET_LINE(calling_function,instruction))+zetscript::zs_strutils::format(s, ##__VA_ARGS__);
#define VM_ERROR_AND_RET(s,...)		data->vm_error=true;data->vm_error_str=ZS_LOG_FILE_LINE_STR(SFI_GET_FILE(calling_function,instruction),SFI_GET_LINE(calling_function,instruction))+zetscript::zs_strutils::format(s, ##__VA_ARGS__);return;
#define VM_STOP_EXECUTE(s,...)		data->vm_error=true;data->vm_error_str=ZS_LOG_FILE_LINE_STR(SFI_GET_FILE(calling_function,instruction),SFI_GET_LINE(calling_function,instruction))+zetscript::zs_strutils::format(s, ##__VA_ARGS__);goto lbl_exit_function;
#define VM_SET_USER_ERROR(vm,s,...)	vm_set_error_file_line(vm,__FILE__,__LINE__, s, ##__VA_ARGS__)


#define VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE	0x1


namespace zetscript{

	struct  VirtualMachine{
		void *data;
	};

	VirtualMachine *vm_new(ZetScript *zs);


	/**
	 * Reserve for N vars. Return base pointer.
	 */

	void			vm_init(VirtualMachine *vm, ZetScript *_zs);
	void 			vm_push_stack_element(VirtualMachine *vm, StackElement stk);
	bool 			vm_share_pointer(VirtualMachine *vm,ScriptObject *_obj);
	bool 			vm_create_shared_pointer(VirtualMachine *vm,ScriptObject *_obj);
	inline bool 	vm_unref_shared_script_object_and_remove_if_zero(VirtualMachine *vm,ScriptObject **so);
	void 			vm_unref_lifetime_object(VirtualMachine *vm,ScriptObject *script_object);
	bool 			vm_set_stack_element_at(VirtualMachine *vm,unsigned int idx, StackElement stk);


	void 			vm_set_error(VirtualMachine *vm,const std::string & str);
	void 			vm_set_error_file_line(VirtualMachine *vm, const char *file, int line, const char *in_txt,...);
	std::string 	vm_get_error(VirtualMachine *vm);
	
	bool 			vm_it_has_error(VirtualMachine *vm);

	//StackElement * 	vm_get_top_stack_element_from_stack(VirtualMachine *vm);
	StackElement *	vm_get_stack_elements(VirtualMachine *vm);
	StackElement *	vm_get_current_stack_element(VirtualMachine *vm);
	StackElement * 	vm_get_stack_element_at(VirtualMachine *vm,unsigned int idx_glb_element);
	StackElement vm_execute(
		VirtualMachine 	*	vm
		,ScriptObject 	*	this_object
		,ScriptFunction *	calling_function
		,StackElement 	*  	stk_params=NULL
		,unsigned char		n_stk_params=0
		,unsigned short 	properties=0
		,const char 	*	file=""
		,int 				line=-1
	);

	void vm_delete(VirtualMachine *vm);
}


#include "vm.tcc"
