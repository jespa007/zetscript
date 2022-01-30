/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define VM_PUSH_SCOPE(_scope)\
	 *data->vm_current_scope_function->scope_current++=(Scope *)_scope;

// defer all local vars
#define VM_POP_SCOPE()\
{\
	Scope *scope=*(data->vm_current_scope_function->scope_current-1);\
	StackElement         * stk_local_vars	=data->vm_current_scope_function->stk_local_vars;\
	zs_vector *scope_symbols=scope->symbol_variables;\
	int count=scope_symbols->count;\
	StackElement *stk_local_var=stk_local_vars+((Symbol *)scope_symbols->items[0])->idx_position;\
	while(count--){\
		if((stk_local_var->properties & STK_PROPERTY_SCRIPT_OBJECT)){\
			ScriptObject *so=(ScriptObject *)(stk_local_var->value);\
			if(so != NULL && so->shared_pointer!=NULL){\
				if(vm_unref_shared_script_object(vm,so,data->vm_idx_call)==false){\
					return;\
				}\
			}\
		}\
		STK_SET_NULL(stk_local_var);\
		stk_local_var++;\
	}\
	--data->vm_current_scope_function->scope_current;\
}

namespace zetscript{


	struct VM_ScopeFunction{
		Scope				*scope[MAX_INNER_SCOPES_FUNCTION];
		Scope				**scope_current;
		StackElement 		*stk_local_vars;
	};

}
