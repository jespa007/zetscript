/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define VM_PUSH_SCOPE(_scope)\
	(data->vm_current_scope_function->scope_current)->scope=(Scope *)_scope;\
	data->vm_current_scope_function->scope_current++;\

// defer all local vars
#define VM_POP_SCOPE(destroy_on_zero_ref)\
{\
	Scope *scope=(data->vm_current_scope_function->scope_current-1)->scope;\
	StackElement         * stk_local_vars	=data->vm_current_scope_function->stk_local_vars;\
	zs_vector *scope_symbols=scope->symbol_variables;\
	int count=scope_symbols->count;\
	if(count > 0){\
		StackElement *stk_local_var=stk_local_vars+((Symbol *)scope_symbols->items[0])->idx_position;\
		while(count--){\
			if((stk_local_var->properties & STK_PROPERTY_SCRIPT_OBJECT)){\
				ScriptObject *so=(ScriptObject *)(stk_local_var->value);\
				if(so != NULL && so->shared_pointer!=NULL){\
					/* printf("[%s] %i\n",((Symbol *)scope_symbols->items[(scope_symbols->count-1)-count])->name.c_str(),so->shared_pointer->data.n_shares);*/\
					destroy_on_zero_ref==true?\
						vm_unref_shared_script_object_and_remove_if_zero(vm,&so)\
					:\
						 vm_unref_shared_script_object(vm,so,data->vm_idx_call);\
				}\
			}\
			STK_SET_UNDEFINED(stk_local_var);\
			stk_local_var++;\
		}\
	}\
	--data->vm_current_scope_function->scope_current;\
}

namespace zetscript{

	struct VM_ScopeBlock{
		Scope				*scope;
		zs_vector			unreferenced_objects;
	};


	struct VM_ScopeFunction{
		VM_ScopeBlock		scope[MAX_INNER_SCOPES_FUNCTION];
		VM_ScopeBlock		*scope_current;
		StackElement 		*stk_local_vars;
	};

}
