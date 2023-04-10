/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	bool vm_create_shared_script_object(
		VirtualMachine *_vm
		,ScriptObject *_obj
		,VM_ScopeBlock *_vm_scope_block
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;

		if(_obj->properties  & SCRIPT_OBJECT_PROPERTY_CONSTANT){
			ZS_THROW_EXCEPTION("Internal : try to create shared constant script object");
		}

		// if vm block NULL set current block scope by default
		if(_vm_scope_block == NULL){
			_vm_scope_block=ZS_VM_CURRENT_SCOPE_BLOCK;
		}

		if(_obj->shared_pointer == NULL){
			InfoSharedPointerNode *_node = (InfoSharedPointerNode *)ZS_MALLOC(sizeof(InfoSharedPointerNode));
			// init
			_node->previous=NULL;
			_node->next=NULL;
			_node->data.n_shares=0;
			_node->data.ptr_script_object_shared=_obj;
			_node->data.vm_scope_block_where_created=_vm_scope_block;//ZS_VM_CURRENT_SCOPE_BLOCK;//data->vm_idx_call; // it saves the zeros nodes where was set

			// insert node into shared nodes ...
			if(!vm_insert_shared_node(_vm,&_node->data.vm_scope_block_where_created->unreferenced_objects,_node)){
				return false;
			}
			_obj->shared_pointer=_node;
			return true;
		}else{
			ZS_THROW_EXCEPTION("Internal : shared ptr already has a shared pointer data");
		}

		return false;

	}
	bool vm_share_script_object(
		VirtualMachine *_vm
		,ScriptObject *_obj
	){
		InfoSharedPointerNode *_node=_obj->shared_pointer;

		if(_obj->properties  & SCRIPT_OBJECT_PROPERTY_CONSTANT){
			ZS_THROW_EXCEPTION("Internal : try to share constant script object");
		}

		unsigned short *n_shares = &_node->data.n_shares;

		bool move_to_shared_list=*n_shares==0;

		/*if(*n_shares >= VM_VARIABLE_SHARES_MAX){
			ZS_VM_SET_USER_ERROR(vm,"MAX SHARED VARIABLES (Max. %i)",VM_VARIABLE_SHARES_MAX);
			return false;
		}*/

		(*n_shares)++;

		if(move_to_shared_list){

			// Mov to shared pointer...
			if(!vm_deattach_shared_node(_vm,&_node->data.vm_scope_block_where_created->unreferenced_objects,_node)){
				return false;
			}
			// update current stack due different levels from functions!
			//_node->currentStack=idx_current_call;
			/*if(!vm_insert_shared_node(vm,&data->shared_vars,_node)){
				return false;
			}*/

			ZS_LOG_DEBUG("Share pointer %i:%p",_node->data.ptr_script_object_shared->idx_script_type,_node->data.ptr_script_object_shared);

		}
		return true;
	}

	bool vm_unref_shared_script_object(
			VirtualMachine 	*	_vm
			, ScriptObject 	*	_obj
			,VM_ScopeBlock 	*	_scope_block
	){
		if(_obj->properties  & SCRIPT_OBJECT_PROPERTY_CONSTANT){
			ZS_THROW_EXCEPTION("Internal : try to unref constant script object");
		}

		InfoSharedPointerNode *shared_pointer=_obj->shared_pointer;
		if(shared_pointer==NULL){
			ZS_VM_SET_USER_ERRORF(_vm,"shared ptr not registered");
			return false;
		}

		if(shared_pointer->data.n_shares==0){
			// since objects can have cyclic references unref can reach twice or more 0 (it has to check more cases)
			// we return true
			fprintf(stderr,"WARNING: Shared pointer already deattached\n");
			return false;
		}

		shared_pointer->data.n_shares--;

		if(shared_pointer->data.n_shares==0){

			if(_scope_block==NULL){
				delete shared_pointer->data.ptr_script_object_shared; // it deletes shared_script_object
				free(shared_pointer);
			}else{
				InfoSharedList *unreferenced_objects = &_scope_block->unreferenced_objects;

				// insert to zero shares vector to remove automatically on ending scope
				if(vm_insert_shared_node(_vm,unreferenced_objects,shared_pointer)==false){
					return false;
				}
			}
		}

		return true;
	}

	bool vm_deattach_shared_node(VirtualMachine *vm, InfoSharedList * list, InfoSharedPointerNode *_node){

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

	void vm_remove_empty_shared_pointers(VirtualMachine *vm,VM_ScopeBlock *scope_block){
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

}
