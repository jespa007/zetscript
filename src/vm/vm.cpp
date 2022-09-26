/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	struct VM_Foreach{
		StackElement 		   	*key; // iterator element can be zs_string or integer...
		ObjectScriptObject		*ptr; // can be struct or zs_vector...
		unsigned int 		   	idx_current;
	};

	void vm_do_stack_dump(
			VirtualMachine *_vm
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		// derefer all variables in all scopes (except main )...
		while(data->vm_current_scope_function > VM_SCOPE_FUNCTION_MAIN){
			while(
					(VM_CURRENT_SCOPE_FUNCTION->current_scope_block > VM_CURRENT_SCOPE_FUNCTION->first_scope_block)

			){
				vm_pop_scope(_vm);
			}

			--data->vm_current_scope_function;
		}

		data->vm_current_scope_function =  VM_SCOPE_FUNCTION_MAIN;
		vm_remove_empty_shared_pointers(_vm,vm_get_scope_block_main(_vm));
	}

	VirtualMachine *vm_new(ZetScript *_zs){

		VirtualMachine *vm = (VirtualMachine *)ZS_MALLOC(sizeof(VirtualMachine));
		VirtualMachineData *data = new VirtualMachineData(_zs);
		vm->data=data;

		//-----------------------------------------------------------
		// set memory manager
		return vm;
	}

	#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

	#define print_vm_cr ZS_LOG_INFO
	#else
	#define print_vm_cr(s,...)
	#endif


	void vm_init(
		VirtualMachine *_vm
		,ZetScript *_zs
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;

		// script type factory should be created and initialized
		data->script_function_factory=_zs->getScriptFunctionFactory();
		data->script_type_factory=_zs->getScriptTypeFactory();
		data->scope_factory = _zs->getScopeFactory();
		data->main_function_object = MAIN_FUNCTION(data);
		data->main_class_object = SCRIPT_TYPE_MAIN(data->script_type_factory);
	}

	//============================================================================================================================================
	// POINTER MANANAGER
	bool vm_create_shared_script_object(
		VirtualMachine *_vm
		,ScriptObject *_obj
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		if(_obj->shared_pointer == NULL){
			InfoSharedPointerNode *_node = (InfoSharedPointerNode *)ZS_MALLOC(sizeof(InfoSharedPointerNode));
			// init
			_node->previous=NULL;
			_node->next=NULL;
			_node->data.n_shares=0;
			_node->data.ptr_script_object_shared=_obj;
			_node->data.created_scope_block=VM_CURRENT_SCOPE_BLOCK;//data->vm_idx_call; // it saves the zeros nodes where was set

			// insert node into shared nodes ...
			if(!vm_insert_shared_node(_vm,&_node->data.created_scope_block->unreferenced_objects,_node)){
				return false;
			}
			_obj->shared_pointer=_node;
			return true;
		}else{
			VM_SET_USER_ERRORF(_vm," shared ptr already has a shared pointer data");
		}

		return false;

	}

	bool vm_share_script_object(
		VirtualMachine *_vm
		,ScriptObject *_obj
	){
		InfoSharedPointerNode *_node=_obj->shared_pointer;

		unsigned short *n_shares = &_node->data.n_shares;

		bool move_to_shared_list=*n_shares==0;

		/*if(*n_shares >= VM_VARIABLE_SHARES_MAX){
			VM_SET_USER_ERROR(vm,"MAX SHARED VARIABLES (Max. %i)",VM_VARIABLE_SHARES_MAX);
			return false;
		}*/

		(*n_shares)++;

		if(move_to_shared_list){

			// Mov to shared pointer...
			if(!vm_deattach_shared_node(_vm,&_node->data.created_scope_block->unreferenced_objects,_node)){
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
	// POINTER MANANAGER
	//============================================================================================================================================


	const ScriptFunction * vm_get_current_native_function_call(
			VirtualMachine *_vm
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		return data->current_call_c_function;
	}

	bool vm_it_has_error(
			VirtualMachine *vm
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->vm_error;
	}

	void vm_set_error(
			VirtualMachine *_vm
			, const char *_str_error
			,...
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;

		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,_str_error);

		data->vm_error = true;
		data->vm_error_description=out_txt;
		data->vm_error_file="";
		data->vm_error_line=-1;
	}

	void vm_set_file_line_error(
			VirtualMachine *_vm
			, const char *_file
			, int _line
			, const char *_str_error
			,...
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;

		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,_str_error);
		vm_set_error(_vm,out_txt);

		data->vm_error_file=_file==NULL?"":_file;
		data->vm_error_line=_line;
	}

	zs_string vm_get_error(
			VirtualMachine *vm
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->vm_error_description;
	}

	void vm_insert_lifetime_object(
			VirtualMachine *_vm
			, const char *_file
			, int _line
			, ScriptObject *_script_object
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		InfoLifetimeObject *info = (InfoLifetimeObject *)ZS_MALLOC(sizeof(InfoLifetimeObject));

		info->file=_file;
		info->line=_line;
		info->script_object=_script_object;

		if(_script_object->shared_pointer != NULL){
			if(_script_object->shared_pointer->data.n_shares > 0){
				_script_object->shared_pointer->data.n_shares++;
			}
		}

		data->lifetime_object.push_back(info);
	}

	int vm_find_lifetime_object(
			VirtualMachine *_vm
			,ScriptObject *_script_object
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		for(int i=0; i < data->lifetime_object.size(); i++){
			InfoLifetimeObject *info=(InfoLifetimeObject *)data->lifetime_object.items[i];
			if(info->script_object==_script_object){
				return i;
			}
		}

		return ZS_IDX_UNDEFINED;
	}

	void vm_unref_lifetime_object(
			VirtualMachine *_vm
			,ScriptObject *_script_object
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		int idx = ZS_IDX_UNDEFINED;
		if((idx=vm_find_lifetime_object(_vm,_script_object))==ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERRORF("Cannot find stack element lifetime");
		}
		InfoLifetimeObject *info=(InfoLifetimeObject *)data->lifetime_object.items[idx];
		data->lifetime_object.erase(idx);

		if(info->script_object->shared_pointer!=NULL){
			vm_unref_shared_script_object(_vm,_script_object,NULL);
		}else{
			delete _script_object;
		}
		free(info);
	}

	StackElement *vm_get_stack_elements(VirtualMachine *_vm){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		return data->vm_stack;
	}

	void vm_push_stack_element(VirtualMachine *_vm, StackElement _stk){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		if(((data->vm_stk_current-data->vm_stack)+1)>=VM_STACK_MAX){

			vm_set_error(_vm,"Error MAXIMUM stack size reached");
			return;
		}
		*data->vm_stk_current++=_stk;
	}

	bool vm_set_stack_element_at(VirtualMachine *_vm,unsigned int _idx, StackElement _stk){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		if(_idx >= VM_STACK_MAX){
			VM_SET_USER_ERRORF(_vm,"setStackElement: out of bounds");
			return false;
		}

		data->vm_stack[_idx]=_stk;
		return true;
	}

	VM_ScopeBlock  *vm_get_scope_block_main(VirtualMachine *_vm){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		return &data->vm_scope_function[0].scope_block[0];
	}

	StackElement *vm_get_current_stack_element(VirtualMachine *_vm){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		return data->vm_stk_current;
	}

	StackElement * vm_get_stack_element_at(VirtualMachine *_vm, unsigned _idx_glb_element){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		if(_idx_glb_element < (unsigned)data->main_function_object->local_variables->size()){
			return &data->vm_stack[_idx_glb_element];
		}else{
			VM_SET_USER_ERRORF(_vm,"getStackElement: out of bounds");
		}
		return NULL;
	}

	void vm_reset_error_vars(VirtualMachine *_vm){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;

		data->vm_error_max_stack_reached=false;
		data->vm_error="";
		data->vm_error_callstack_str="";
		data->vm_error=false;
		data->vm_error_line=-1;
	}

	StackElement vm_execute(
		VirtualMachine 		*	_vm
		 ,ScriptObject 		*	_this_object
		 ,ScriptFunction 	*	_calling_function
		 ,StackElement 		*  	_stk_params
		 ,unsigned	char  		_n_stk_params
		 ,unsigned short		_properties
		 ,const char 		*	_file
		 ,int 					_line
	){
		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		int n_stk_params=_n_stk_params;

		if(_calling_function->instructions == NULL){ // no instructions do not execute
			return k_stk_undefined;
		}

		// in case user binds script function and calls directy from C code, idx_call will 0 so we have
		// to prevent conflict with first call that is reserved by main_function
		bool first_script_call_from_c=false;
		StackElement stk_return=k_stk_undefined;
		StackElement *stk_start=NULL;

		if(
			_calling_function->idx_script_function==IDX_SCRIPT_FUNCTION_MAIN
			&& ((_properties & VM_PROPERTY_CALL_FROM_NATIVE) == 0)
		){ // set stack and Init vars for first call...


			if(data->vm_current_scope_function != VM_SCOPE_FUNCTION_MAIN){
				THROW_RUNTIME_ERROR("Internal: vm_idx_call != 0 (%i)",IDX_VM_CURRENT_SCOPE_FUNCTION);
			}

			vm_reset_error_vars(_vm);

			stk_start=data->vm_stack;
			n_stk_params=data->main_function_object->local_variables->size();

		}else{ // Not main function -> allow params for other functions
			// push param stack elements...
			stk_start=data->vm_stk_current;
			StackElement *min_stk=&data->vm_stack[data->main_function_object->local_variables->size()];
			first_script_call_from_c=false;

			if(data->vm_current_scope_function == VM_SCOPE_FUNCTION_MAIN){

				if((_properties & VM_PROPERTY_CALL_FROM_NATIVE)==0){
					THROW_RUNTIME_ERRORF("Internal: expected first call function from C");
				}
				//vm_reset_error_vars(vm);

				first_script_call_from_c=true;
				data->vm_current_scope_function=VM_SCOPE_FUNCTION_FIRST; // assign first idx call as 1
				stk_start=min_stk;
			}

			if(stk_start<min_stk){ // control: not overwrite global symbols
				// this could happen when you call script function from native c++, so this control is needed
				THROW_RUNTIME_ERRORF("Internal: stk_start < min_stk");
			}

			StackElement *stk_it=stk_start;
			for(int i = 0; i < n_stk_params; i++){
				if(_stk_params[i].properties & STK_PROPERTY_SCRIPT_OBJECT){
					ScriptObject *so=(ScriptObject *)_stk_params[i].value;

					if(so->shared_pointer==NULL){
						if(vm_create_shared_script_object(_vm,so) == false){
							THROW_RUNTIME_ERRORF("Cannot create shared pointer");
						}
					}

					if(vm_share_script_object(_vm,so) == false){
						THROW_RUNTIME_ERRORF("Cannot shared pointer");
					}

				}
				*stk_it++=_stk_params[i];
			}
		}

		// byte code executing starts here. Later script function can call c++ function, but once in c++ function is not possible by now call script function again.
		vm_execute_script_function(
			_vm,
			_this_object,
			_calling_function,
			stk_start
		);

		// remove empty shared pointers
		if(data->vm_current_scope_function == VM_SCOPE_FUNCTION_MAIN){
			vm_remove_empty_shared_pointers(_vm
					,vm_get_scope_block_main(_vm)
			);
		}

		// get number return elements
		if(data->vm_error){
			// set error as false
			// it was error so reset stack and stop execution ...
			vm_do_stack_dump(_vm);

			zs_string total_error=data->vm_error_description;
			total_error.append(data->vm_error_callstack_str);

			throw_exception_file_line(data->vm_error_file.c_str(),data->vm_error_line,total_error.c_str());
		}else{
			int n_returned_arguments_from_function=data->vm_stk_current-(stk_start+(int)_calling_function->local_variables->size());

			if(n_returned_arguments_from_function > 0){

				// get first...
				StackElement *ptr_stk_return=(data->vm_stk_current-n_returned_arguments_from_function);
				stk_return = ptr_stk_return[0];

				// if object add into lifetime till user delete it
				if(stk_return.properties & STK_PROPERTY_SCRIPT_OBJECT){
					// add generated
					vm_insert_lifetime_object(
						_vm
						,_file
						,_line
						,(ObjectScriptObject *)stk_return.value
					);
				}

				// deinit vm variable...

				ptr_stk_return->setUndefined();
				// deallocate all returned variables from 1
				for(int i=1; i < n_returned_arguments_from_function; i++){
					StackElement stk_ret=ptr_stk_return[i];
					if(stk_ret.properties & STK_PROPERTY_SCRIPT_OBJECT){
						delete (ScriptObject *)stk_ret.value;
					}
					// deinit vm variable...
					ptr_stk_return[i].setUndefined();
				}
			}
		}

		// Important restore stk!
		data->vm_stk_current=stk_start;

		// restore idx_call as 0 if first call was a script function called from C
		if(first_script_call_from_c==true){
			data->vm_current_scope_function=VM_SCOPE_FUNCTION_MAIN;
		}

		if(data->vm_current_scope_function == VM_SCOPE_FUNCTION_MAIN){
			//vm_check_cyclic_references(_vm);
		}

		return stk_return;
	}

	bool vm_unref_shared_script_object(
			VirtualMachine 	*	_vm
			, ScriptObject 	*	_obj
			,VM_ScopeBlock 	*	_scope_block
	){
		InfoSharedPointerNode *shared_pointer=_obj->shared_pointer;
		if(shared_pointer==NULL){
			VM_SET_USER_ERRORF(_vm,"shared ptr not registered");
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

	void vm_delete(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(data->lifetime_object.size()>0){
			zs_string created_at="";
			zs_string end="";
			bool some_registers_without_file_line=false;

			zs_string error="\n\nSome lifetime objects returned by virtual machine were not unreferenced:\n\n";
			for(int i=0; i < data->lifetime_object.size();i++ ){
				InfoLifetimeObject *info=(InfoLifetimeObject *)data->lifetime_object.items[i];
				created_at="";
				end="";

				if(info->file != 0 && *info->file!=0){
					created_at=zs_strutils::format(" at [%s:%i]",zs_path::get_filename(info->file).c_str(),info->line);
				}else{
					created_at=" at [??:??]";
					some_registers_without_file_line=true;
				}
				error+=zs_strutils::format("* Returned lifetime object%s was not unreferenced. %s \n",created_at.c_str(),end.c_str());

			}

			if(some_registers_without_file_line==true){
				error+="\n\nSome lifetimes objects were returned/created from unknown file/line. Tip: Pass pass the file and line to all 'eval' function that it's calling from c++ in order to give you a clue where the 'lifetime object' was returned/created. For example,\n\n\n"
						"\tStackElement var_from_script=zs->eval(\"return new Object()\",__FILE__,__LINE__)\n";
			}

			error+="\n\nLifetime objects returned by virtual machine must be unreferenced by calling 'unrefLifetimeObject' \n\n";

			error+="\n\n";

			fprintf(stderr,"%s",error.c_str());

		}

		delete data;
		free(vm);
	}

	void vm_assign_container_slot(
			VirtualMachine *_vm
			, ContainerSlot *_container_slot
			, ContainerScriptObject *_src_container_ref
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;

		ContainerScriptObject *dst_container_ref=_container_slot->getDstContainerRef();
		zs_int dst_container_slot_id=_container_slot->getIdSlot();
		StackElement *stk_dst=_container_slot->getPtrStackElement();

		StackElement *stk_obj=NULL;

		// More tests would be needed see issue #336
		if(dst_container_ref->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_VECTOR){
			stk_obj=((VectorScriptObject *)dst_container_ref)->getUserElementAt((int)dst_container_slot_id);
			printf("\nAssing object %p type '%s' TO  vector %p slot '%i' type '%s'. Last value type '%s'\n"
					,(void *)_src_container_ref
					,_src_container_ref->getScriptType()->str_script_type.c_str()
					,(void *)dst_container_ref
					,(int)dst_container_slot_id
					,dst_container_ref->getScriptType()->str_script_type.c_str()
					,stk_to_typeof_str(data->zs,stk_obj).c_str()

			);

		}else{
			// object
			stk_obj=dst_container_ref->getProperty((const char *)dst_container_slot_id);
			printf("\nAssing object %p type '%s' TO  object %p slot '%s' type '%s'. Last value type '%s'\n"
					,(void *)_src_container_ref
					,_src_container_ref->getScriptType()->str_script_type.c_str()
					,(void *)dst_container_ref
					,(const char *)dst_container_slot_id
					,dst_container_ref->getScriptType()->str_script_type.c_str()
					,stk_to_typeof_str(data->zs,stk_obj).c_str()

			);

		}

		// finally adds to container slot hierarchy
		_src_container_ref->addContainerSlot(_container_slot);

		// do the assigment
		stk_dst->value=(zs_int)_container_slot;
		stk_dst->properties=STK_PROPERTY_CONTAINER_SLOT;

		// add ref into map of pointers
		data->containers_with_container_slots.set((zs_int)_src_container_ref,(zs_int)_src_container_ref);

	}

	void vm_remove_container_for_cyclic_references_checking(VirtualMachine *_vm,ContainerScriptObject *_csso){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		data->containers_with_container_slots.erase((zs_int)_csso);
	}

	void vm_count_cyclic_references(ContainerScriptObject *_current,ContainerScriptObject *_container_to_check, int & _count){
		zs_list_node<ContainerSlot *> *first_node=NULL,* current_node=NULL;

		current_node=first_node=_current->getListContainerSlotsRef()->first;

		if(current_node != NULL){
			do{
				if(_container_to_check==current_node->data->getSrcContainerRef()){
					_count++;
				}
				current_node=current_node->next;
			}while(current_node!=first_node);

		}
	}

	void vm_check_cyclic_references(VirtualMachine *_vm){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		for(auto it=data->containers_with_container_slots.begin();!it.end();it.next()){
			// count reference number
			ContainerScriptObject *cso=(ContainerScriptObject *)it.value;
			int count=0;

			vm_count_cyclic_references(cso,cso,count);
			printf("Counting reference for container instance %p. Cyclic counts %i Shares counts: %i. Can be removed (cyclic == n_shares): %s\n"
					,(void *)cso
					,count
					,cso->shared_pointer->data.n_shares
					,count==cso->shared_pointer->data.n_shares?"true":"false"

				);
		}
	}

}


#include "vm_script_function.cpp"
#include "vm_native_function.cpp"


