/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "vm_share_manager.h"
#include "vm_pop_stk.h"
#include "vm_push_stk.h"
#include "vm_scope.h"


#define ZS_VM_STACK_MAX						150
#define ZS_VM_FUNCTION_CALL_MAX 			100

namespace zetscript{

	struct  VirtualMachine{
		void *data;
	};

	VirtualMachine *vm_new(ScriptEngine *zs);

	void			vm_init(VirtualMachine *vm, ScriptEngine *_zs);
	void 			vm_push_stack_element(VirtualMachine *vm, StackElement stk);

	void 			vm_unref_lifetime_object(VirtualMachine *vm,ScriptObject *object);
	bool 			vm_set_stack_element_at(VirtualMachine *vm,unsigned int idx, StackElement stk);

	VM_ScopeBlock  *vm_get_scope_block_main(VirtualMachine *vm);

	void 			vm_set_error(VirtualMachine *vm, const char *_str_error,...);
	void 			vm_set_file_line_error(VirtualMachine *vm, const char *file, int line, const char *in_txt,...);
	String 		vm_get_error(VirtualMachine *vm);
	
	bool 			vm_it_has_error(
		VirtualMachine 	*	_vm
	);

	StackElement *	vm_get_stack_elements(
		VirtualMachine 	*	_vm
	);

	StackElement *	vm_get_current_stack_element(
		VirtualMachine 	*	_vm
	);

	void 			vm_reset_error_vars(
		VirtualMachine *vm
	);

	StackElement * 	vm_get_stack_element_at(
		VirtualMachine *vm
		,unsigned idx_glb_element
	);

	StackElement vm_execute(
		VirtualMachine 	*	_vm
		,ScriptObject 	*	_this_object
		,ScriptFunction *	_script_function
		,StackElement 	*  	_stk_params=NULL
		,unsigned char		_n_stk_params=0
		,unsigned short 	_properties=0
		,const char 	*	_file=""
		,int 				_line=-1
	);

	void vm_execute_script_function(
			VirtualMachine			* _vm,
			ScriptObject			* _this_object,
			ScriptFunction 			* _script_function,
			StackElement 		  	* _stk_local_var
	);

	void  vm_execute_native_function(
		VirtualMachine 			*	_vm,
		const ScriptFunction 	*	_script_function,
		Instruction 			*	_instruction,
		ScriptObject  			* 	_this_object,
		const ScriptFunction 	*	_c_function,
		StackElement 			*	_stk_arg_c_function,
		unsigned char 				_n_args
	);

	void vm_execute_script_function(
		VirtualMachine 		*	_vm,
		ScriptObject		* 	_this_object,
		ScriptFunction 		* 	_script_function,
		StackElement 		*	_stk_local_var
	);

	VM_ScopeBlock *vm_get_main_scope_block(
			VirtualMachine *_vm
	);

	void vm_delete(VirtualMachine *vm);
}

#include "vm_common.h"
#include "vm.tcc"
