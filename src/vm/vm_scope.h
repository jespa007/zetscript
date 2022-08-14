/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define VM_PUSH_SCOPE(_scope)\
	VM_CURRENT_SCOPE_FUNCTION->current_scope_block->scope=(Scope *)_scope;\
	VM_CURRENT_SCOPE_FUNCTION->current_scope_block++;\

namespace zetscript{

	struct VM_ScopeBlock{
		Scope				*scope;
		InfoSharedList		unreferenced_objects;
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
