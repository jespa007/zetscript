/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "vm_pop_stk.h"
#include "vm_push_stk.h"
#include "vm_operation.h"
#include "vm_operation_set.h"
#include "vm_set.h"
#include "vm_scope.h"

#define VM_STACK_MAX					150
#define VM_FUNCTION_CALL_MAX 			100
//#define VM_VARIABLE_SHARES_MAX 			10000

#define VM_PROPERTY_CALL_FROM_NATIVE	0x1

#define SET_INTEGER_RETURN(i)			CURRENT_VM->setIntegerReturnValue(i)
#define SET_BOOLEAN_RETURN(b) 			CURRENT_VM->setBooleanReturnValue(b)
#define SET_FLOAT_RETURN(f)   			CURRENT_VM->setFloatReturnValue(f)

#define NO_PARAMS zs_vector<StackElement>{}
#define ZS_VM_FUNCTION_TYPE std::function<ScriptObjectObject * (const zs_vector<ScriptObjectObject *> & param)>

#define VM_EXECUTE(vm,o,f,stk,n)		vm_execute(vm,o,f,stk,n,0,__FILE__,__LINE__)

#define VM_ERROR(s,...)	\
	data->vm_error=true;\
	data->vm_error_file=SFI_GET_FILE(calling_function,instruction);\
	data->vm_error_line=SFI_GET_LINE(calling_function,instruction);\
	data->vm_error_str=ZS_LOG_FILE_LINE_STR(data->vm_error_file.c_str(),data->vm_error_line)+zetscript::zs_strutils::format(s, __VA_ARGS__);

#define VM_ERRORF(s)					VM_ERROR(s,NULL)

#define VM_ERROR_AND_RET(s,...)	\
	data->vm_error=true;\
	data->vm_error_file=SFI_GET_FILE(calling_function,instruction);\
	data->vm_error_line=SFI_GET_LINE(calling_function,instruction);\
	data->vm_error_str=ZS_LOG_FILE_LINE_STR(data->vm_error_file.c_str(),data->vm_error_line)+zetscript::zs_strutils::format(s, __VA_ARGS__);\
	return;

#define VM_ERROR_AND_RETF(s)			VM_ERROR_AND_RET(s,NULL)

#define VM_STOP_EXECUTE(s,...)	\
	data->vm_error=true;\
	data->vm_error_file=SFI_GET_FILE(calling_function,instruction);\
	data->vm_error_line=SFI_GET_LINE(calling_function,instruction);\
	data->vm_error_str=ZS_LOG_FILE_LINE_STR(data->vm_error_file.c_str(),data->vm_error_line)+zetscript::zs_strutils::format(s, __VA_ARGS__);\
	goto lbl_exit_function;

#define VM_STOP_EXECUTEF(s)				VM_STOP_EXECUTE(s,NULL)

#define VM_SET_USER_ERROR(vm,s,...)		vm_set_error_file_line(vm,__FILE__,__LINE__, s, __VA_ARGS__)
#define VM_SET_USER_ERRORF(vm,s)		VM_SET_USER_ERROR(vm,s,NULL)

#define IDX_VM_CURRENT_SCOPE_FUNCTION 	(data->vm_current_scope_function-data->vm_scope_function)
#define VM_SCOPE_FUNCTION_MAIN 			(data->vm_scope_function+0)
#define VM_SCOPE_FUNCTION_FIRST 		(data->vm_scope_function+1)
#define VM_CURRENT_SCOPE_FUNCTION		(data->vm_current_scope_function-1)
#define VM_CURRENT_SCOPE_BLOCK			(VM_CURRENT_SCOPE_FUNCTION->current_scope_block-1)


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
	bool 			vm_share_script_object(VirtualMachine *vm,ScriptObject *_obj);
	bool 			vm_create_shared_script_object(VirtualMachine *vm, VM_ScopeBlock *_scope_block,ScriptObject *_obj);
	//inline bool 	vm_unref_shared_script_object_and_remove_if_zero(VirtualMachine *vm,ScriptObject **so);
	void 			vm_unref_lifetime_object(VirtualMachine *vm,ScriptObject *script_object);
	bool 			vm_set_stack_element_at(VirtualMachine *vm,unsigned int idx, StackElement stk);
	//int				vm_get_idx_call(VirtualMachine *vm);
	VM_ScopeBlock  *vm_get_scope_block_main(VirtualMachine *vm);

	void 			vm_set_error(VirtualMachine *vm,const zs_string & str);
	void 			vm_set_error_file_line(VirtualMachine *vm, const char *file, int line, const char *in_txt,...);
	zs_string 		vm_get_error(VirtualMachine *vm);
	
	bool 			vm_it_has_error(VirtualMachine *vm);


	//StackElement * 	vm_get_top_stack_element_from_stack(VirtualMachine *vm);
	StackElement *	vm_get_stack_elements(VirtualMachine *vm);
	StackElement *	vm_get_current_stack_element(VirtualMachine *vm);
	void 			vm_reset_error_vars(VirtualMachine *vm);
	StackElement * 	vm_get_stack_element_at(VirtualMachine *vm,int idx_glb_element);
	StackElement vm_execute(
		VirtualMachine 	*	_vm
		,ScriptObject 	*	_this_object
		,ScriptFunction *	_calling_function
		,StackElement 	*  	_stk_params=NULL
		,unsigned char		_n_stk_params=0
		,unsigned short 	_properties=0
		,const char 	*	_file=""
		,int 				_line=-1
	);

	void vm_delete(VirtualMachine *vm);
}


//#include "vm.tcc"


