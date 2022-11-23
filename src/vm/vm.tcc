/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{
	inline bool vm_deattach_shared_node(VirtualMachine *vm, InfoSharedList * list, InfoSharedPointerNode *_node){

		if(list == NULL) return true;

		if(_node->next == NULL || _node->previous == NULL){
			ZS_VM_SET_USER_ERRORF(vm," Internal error: An already deattached node");
			return false;
		}

		if((_node->previous == _node) && (_node->next == _node)){ // 1 single node...
			list->last=list->first=NULL;
		}
		else{ // dettach and attach next...
			// [1]<->[2]<-> ...[P]<->[C]<->[N]...[M-1]<->[M]
			if(_node == list->first){
				list->first=_node->next;
			}
			else if(_node == list->last){
				list->last=_node->previous;
			}
			_node->previous->next=_node->next;
			_node->next->previous=_node->previous;

		}
		_node->previous = _node->next = NULL;
		return true;
	}

	inline void vm_remove_empty_shared_pointers(VirtualMachine *vm,VM_ScopeBlock *scope_block){
		InfoSharedList *list=&scope_block->unreferenced_objects;//&data->zero_shares[idx_call_stack];
		InfoSharedPointerNode *next_node=NULL,*current=list->first;
		//bool check_empty_shared_pointers=false;

		if(current != NULL){
			bool finish=false;
			do{
				next_node=current->next;
				finish=next_node ==list->first;

				if(!vm_deattach_shared_node(vm,list,current)){
					return;
				}

				delete current->data.ptr_script_object_shared;
				current->data.ptr_script_object_shared=NULL;
				free(current);

				current=next_node;

			}while(!finish);
		}
	}

	inline void vm_pop_scope(VirtualMachine *vm)
	{\
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		VM_ScopeBlock *scope_block=--ZS_VM_CURRENT_SCOPE_FUNCTION->current_scope_block;\
		Scope *scope=scope_block->scope;\
		StackElement         * stk_local_vars	=ZS_VM_CURRENT_SCOPE_FUNCTION->stk_local_vars;\
		zs_vector<Symbol *> *scope_symbols=scope->symbol_variables;\
		int count=(int)scope_symbols->size();\

		if(count > 0 ){
			StackElement *stk_local_var=stk_local_vars+scope_symbols->items[0]->idx_position;\
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

}
