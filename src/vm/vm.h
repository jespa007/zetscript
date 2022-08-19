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

	void 			vm_set_error(VirtualMachine *vm, const char *_str_error,...);
	void 			vm_set_file_line_error(VirtualMachine *vm, const char *file, int line, const char *in_txt,...);
	std::string 		vm_get_error(VirtualMachine *vm);
	
	bool 			vm_it_has_error(VirtualMachine *vm);


	//StackElement * 	vm_get_top_stack_element_from_stack(VirtualMachine *vm);
	StackElement *	vm_get_stack_elements(VirtualMachine *vm);
	StackElement *	vm_get_current_stack_element(VirtualMachine *vm);
	void 			vm_reset_error_vars(VirtualMachine *vm);
	StackElement * 	vm_get_stack_element_at(VirtualMachine *vm,unsigned idx_glb_element);
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

	void vm_execute_function_script(
			VirtualMachine			* vm,
			ScriptObject			* this_object,
			ScriptFunction 			* calling_function,
			StackElement 		  	* _stk_local_var
	);

	bool vm_unref_shared_script_object(
		VirtualMachine *vm
		, ScriptObject *_obj
		,VM_ScopeBlock *_scope_block
	);

	void vm_delete(VirtualMachine *vm);
}


//#include "vm.tcc"


