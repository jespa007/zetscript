/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define VM_PUSH_SCOPE(_scope)\
	(data->vm_current_scope_function->current_scope_block)->scope=(Scope *)_scope;\
	data->vm_current_scope_function->current_scope_block++;\

// defer all local vars
#define VM_POP_SCOPE \
{\
	VM_ScopeBlock *scope_block=--data->vm_current_scope_function->current_scope_block;\
	Scope *scope=scope_block->scope;\
	StackElement         * stk_local_vars	=data->vm_current_scope_function->stk_local_vars;\
	zs_vector *scope_symbols=scope->symbol_variables;\
	int count=scope_symbols->count;\
	if(count > 0){\
		StackElement *stk_local_var=stk_local_vars+((Symbol *)scope_symbols->items[0])->idx_position;\
		while(count--){\
			if((stk_local_var->properties & STK_PROPERTY_SCRIPT_OBJECT)){\
				ScriptObject *so=(ScriptObject *)(stk_local_var->value);\
				if(so != NULL && so->shared_pointer!=NULL){\
					 vm_unref_shared_script_object(vm,so,NULL);\
				}\
			}\
			STK_SET_UNDEFINED(stk_local_var);\
			stk_local_var++;\
		}\
	}\
	vm_remove_empty_shared_pointers(vm,scope_block);\
}

namespace zetscript{

	struct VM_ScopeBlock{
		Scope				*scope;
		InfoSharedList		scope_block;
	};


	struct VM_ScopeFunction{
		VM_ScopeBlock		scope_block[MAX_INNER_SCOPES_FUNCTION];
		VM_ScopeBlock		*first_scope_block;
		VM_ScopeBlock		*current_scope_block;
		StackElement 		*stk_local_vars;

		VM_ScopeFunction(){
			memset(scope_block,0,sizeof(scope_block));
			stk_local_vars=NULL;
			current_scope_block=NULL;
			first_scope_block=scope_block;
		}
	};

}
