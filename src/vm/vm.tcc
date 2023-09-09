/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{

	inline void vm_pop_scope(VirtualMachine *vm)
	{\
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		VM_ScopeBlock *scope_block=--ZS_VM_CURRENT_SCOPE_FUNCTION->current_scope_block;\
		Scope *scope=scope_block->scope;\
		StackElement         * stk_local_vars	=ZS_VM_CURRENT_SCOPE_FUNCTION->stk_local_vars;\
		zs_vector<Symbol *> *scope_symbols=scope->symbol_variables;\
		int count=(int)scope_symbols->size();\

		if(count > 0 ){
			StackElement *stk_local_var=stk_local_vars+scope_symbols->data()[0]->idx_position;\
			while(count--){\
				if((stk_local_var->properties & ZS_STK_PROPERTY_SCRIPT_OBJECT)){\
					ScriptObject *so=(ScriptObject *)(stk_local_var->value);\
					if(so != NULL && so->shared_pointer!=NULL){\
						 vm_unref_shared_script_object(vm,so,NULL);\
					}\
				}\
				ZS_STK_SET_UNDEFINED(stk_local_var);\
				stk_local_var++;\
			}\
		}\
		vm_remove_empty_shared_pointers(vm,scope_block);\
	}

}
