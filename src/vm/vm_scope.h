/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define ZS_VM_PUSH_SCOPE(_scope)\
	ZS_VM_CURRENT_SCOPE_FUNCTION->current_scope_block->scope=(ScriptScope *)_scope;\
	ZS_VM_CURRENT_SCOPE_FUNCTION->current_scope_block++;\

namespace zetscript{

	struct VM_ScopeBlock{
		ScriptScope				*scope;
		InfoSharedList		unreferenced_objects;
	};


	struct VM_ScopeFunction{
		VM_ScopeBlock		scope_block[ZS_SCOPE_MAX_INNER_SCOPES_FUNCTION];
		VM_ScopeBlock		*first_scope_block;
		VM_ScopeBlock		*current_scope_block;
		StackElement 		*stk_local_vars;
		ScriptFunction		*calling_function;

		VM_ScopeFunction(){
			memset(scope_block,0,sizeof(scope_block));
			stk_local_vars=NULL;
			current_scope_block=NULL;
			first_scope_block=scope_block;
			calling_function=NULL;
		}
	};

}
