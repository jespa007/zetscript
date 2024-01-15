/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{

	inline bool vm_object_in_objects_to_return(
			VirtualMachine *_vm
			,StackElement *_stk_start
			,ScriptObject *_check_so
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		// check stack elements to return
		//ScriptObject *so_aux=NULL;
		if(_stk_start!=NULL){
			for(StackElement *stk_var=data->vm_stk_current-1;stk_var>=_stk_start;stk_var--){
				StackElement *stk_result_op1=stk_var;

				if(
							(stk_result_op1->properties & STACK_ELEMENT_PROPERTY_OBJECT)
				){
					if(_check_so == (ScriptObject *)stk_result_op1->value){
						return true;
					}
				}
			}
		}

		return false;
	}

	inline void vm_pop_scope(VirtualMachine *_vm,StackElement *_stk_start=NULL)
	{\
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		VM_ScopeBlock *scope_block=--ZS_VM_CURRENT_SCOPE_FUNCTION->current_scope_block;\
		ScriptScope *scope=scope_block->scope;\
		StackElement         * stk_local_vars	=ZS_VM_CURRENT_SCOPE_FUNCTION->stk_local_vars;\
		Vector<Symbol *> *scope_symbols=scope->symbol_variables;\
		int count=(int)scope_symbols->length();\

		if(count > 0 ){
			StackElement *stk_local_var=stk_local_vars+scope_symbols->data()[0]->idx_position;\
			while(count--){\
				if((stk_local_var->properties & STACK_ELEMENT_PROPERTY_OBJECT)){\
					ScriptObject *so=(ScriptObject *)(stk_local_var->value);\
					if(so != NULL && so->shared_pointer!=NULL){\


						if((so->shared_pointer->data.n_shares > 1) || (vm_object_in_objects_to_return(_vm,_stk_start,so) == false)){
							vm_unref_shared_object(_vm,so,NULL);//so->shared_pointer->data.vm_scope_block_where_created);
						}else{
							// do not unreferences shared pointer, instead deataches and set shared pointer to NULL because it will be
							// managed as the return values after call the function
							if(so->shared_pointer->data.n_shares<=1){

								// deatch from list and set it NULL
								if(so->shared_pointer->data.n_shares == 0){
									vm_deattach_shared_node(
											_vm
											,&so->shared_pointer->data.vm_scope_block_where_created->unreferenced_objects
											,so->shared_pointer
									);
								}
								free(so->shared_pointer);
								so->shared_pointer=NULL;
							}
						}
					}\
				}\
				STACK_ELEMENT_SET_UNDEFINED(stk_local_var);\
				stk_local_var++;\
			}\
		}\
		vm_remove_empty_shared_pointers(_vm,scope_block,_stk_start);\
	}
}
