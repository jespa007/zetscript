/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#ifdef  __ZS_VERBOSE_MESSAGE__

#define print_vm_cr ZS_LOG_INFO
#else
#define print_vm_cr(s,...)
#endif

namespace zetscript{

	struct VM_Foreach{
		StackElement 		   	*key; // iterator element can be String or integer...
		ObjectScriptObject		*ptr; // can be struct or Vector...
		unsigned int 		   	idx_current;
	};

	void vm_reset_stack(VirtualMachine *_vm, StackElement *_reset_stack){
		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		data->vm_stk_current=_reset_stack;

		VM_ScopeBlock *scope_block=ZS_VM_CURRENT_SCOPE_FUNCTION->current_scope_block-1;

		vm_remove_empty_shared_pointers(_vm,scope_block);
	}

	void vm_do_stack_dump(
			VirtualMachine *_vm
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		// derefer all variables in all scopes (except main )...
		while(data->vm_current_scope_function > ZS_VM_SCOPE_FUNCTION_MAIN){
			while(
					(ZS_VM_CURRENT_SCOPE_FUNCTION->current_scope_block > ZS_VM_CURRENT_SCOPE_FUNCTION->first_scope_block)

			){
				vm_pop_scope(_vm);
			}

			--data->vm_current_scope_function;
		}

		data->vm_current_scope_function =  ZS_VM_SCOPE_FUNCTION_MAIN;
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

	void vm_init(
		VirtualMachine *_vm
		,ZetScript *_zs
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;

		// script type factory should be created and initialized
		data->script_function_factory=_zs->getScriptFunctionsFactory();
		data->script_types_factory=_zs->getScriptTypesFactory();
		data->scope_factory = _zs->getScriptScopesFactory();
		data->main_function_object = ZS_MAIN_FUNCTION(data);
		data->main_class_object = ZS_TYPE_MAIN(data->script_types_factory);
	}

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

	String vm_get_error(
			VirtualMachine *vm
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->vm_error_description;
	}

	void vm_insert_lifetime_object(
			VirtualMachine *_vm
			, const char *_file
			, int _line
			, ScriptObject *_object
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		InfoLifetimeObject *info = (InfoLifetimeObject *)ZS_MALLOC(sizeof(InfoLifetimeObject));

		info->file=_file;
		info->line=_line;
		info->object=_object;

		if(_object->shared_pointer != NULL){
			if(_object->shared_pointer->data.n_shares > 0){
				_object->shared_pointer->data.n_shares++;
			}
		}

		data->lifetime_object.push(info);
	}

	int vm_find_lifetime_object(
			VirtualMachine *_vm
			,ScriptObject *_object
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		for(int i=0; i < data->lifetime_object.length(); i++){
			InfoLifetimeObject *info=data->lifetime_object.get(i);
			if(info->object==_object){
				return i;
			}
		}

		return ZS_UNDEFINED_IDX;
	}

	void vm_unref_lifetime_object(
			VirtualMachine *_vm
			,ScriptObject *_object
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		int idx = ZS_UNDEFINED_IDX;
		if((idx=vm_find_lifetime_object(_vm,_object))==ZS_UNDEFINED_IDX){
			ZS_THROW_RUNTIME_ERRORF("Cannot find stack element lifetime");
		}
		InfoLifetimeObject *info=data->lifetime_object.get(idx);
		data->lifetime_object.erase(idx);

		if(info->object->shared_pointer!=NULL){
			vm_unref_shared_object(_vm,_object,NULL);
		}else{
			delete _object;
		}
		free(info);
	}

	StackElement *vm_get_stack_elements(VirtualMachine *_vm){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		return data->vm_stack;
	}

	void vm_push_stack_element(VirtualMachine *_vm, StackElement _stk){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		if(((data->vm_stk_current-data->vm_stack)+1)>=ZS_VM_STACK_MAX){

			vm_set_error(_vm,"Error MAXIMUM stack size reached");
			return;
		}
		*data->vm_stk_current++=_stk;
	}

	bool vm_set_stack_element_at(VirtualMachine *_vm,unsigned int _idx, StackElement _stk){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		if(_idx >= ZS_VM_STACK_MAX){
			ZS_VM_SET_USER_ERRORF(_vm,"setStackElementByKeyName: out of bounds");
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
		if(_idx_glb_element < (unsigned)data->main_function_object->local_variables->length()){
			return &data->vm_stack[_idx_glb_element];
		}else{
			ZS_VM_SET_USER_ERRORF(_vm,"getStackElementByKeyName: out of bounds");
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
		 ,ScriptFunction 	*	_script_function
		 ,StackElement 		*  	_stk_params
		 ,unsigned	char  		_n_stk_params
		 ,unsigned short		_properties
		 ,const char 		*	_file
		 ,int 					_line
	){
		ScriptObject *deref_native_args[ZS_MAX_NATIVE_FUNCTION_ARGS];
		int n_deref_native_args=0;
		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		int n_stk_params=_n_stk_params;

		if(_script_function->instructions == NULL){ // no instructions do not execute
			return k_stk_undefined;
		}

		// in case user binds script function and calls directy from C code, idx_call will 0 so we have
		// to prevent conflict with first call that is reserved by main_function
		bool first_script_call_from_c=false;
		StackElement stk_return=k_stk_undefined;
		StackElement *stk_start=NULL;

		if(data->vm_current_scope_function == ZS_VM_SCOPE_FUNCTION_MAIN){
			vm_reset_error_vars(_vm);
		}

		// calling main funcion
		if(
			_script_function->id==ZS_IDX_FUNCTION_MAIN
		){ // set stack and Init vars for first call...


			if(data->vm_current_scope_function != ZS_VM_SCOPE_FUNCTION_MAIN){
				ZS_THROW_RUNTIME_ERROR("Internal: vm_idx_call != 0 (%i)",ZS_IDX_VM_CURRENT_SCOPE_FUNCTION);
			}

			stk_start=data->vm_stack;
			n_stk_params=data->main_function_object->local_variables->length();

		}else{ // Not main function -> allow params for other functions
			// push param stack elements...
			stk_start=data->vm_stk_current;
			StackElement *min_stk=&data->vm_stack[data->main_function_object->local_variables->length()];
			first_script_call_from_c=false;

			if(data->vm_current_scope_function == ZS_VM_SCOPE_FUNCTION_MAIN){

				if((_properties & ZS_VM_PROPERTY_CALL_FROM_NATIVE)==0){
					ZS_THROW_RUNTIME_ERRORF("Internal: expected first call function from C");
				}

				first_script_call_from_c=true;
				data->vm_current_scope_function=ZS_VM_SCOPE_FUNCTION_FIRST; // assign first idx call as 1
				stk_start=min_stk;
			}

			if(stk_start<min_stk){ // control: not overwrite global symbols
				// this could happen when you call script function from native c++, so this control is needed
				ZS_THROW_RUNTIME_ERRORF("Internal: stk_start < min_stk");
			}

			StackElement *stk_it=stk_start;
			for(int i = 0; i < n_stk_params; i++){
				if(_stk_params[i].properties & STACK_ELEMENT_PROPERTY_OBJECT){
					ScriptObject *so=(ScriptObject *)_stk_params[i].value;

					if(so->shared_pointer==NULL){
						vm_create_shared_object(_vm,so);
					}else if((so->shared_pointer->data.n_shares==0) && (_properties & ZS_VM_PROPERTY_CALL_FROM_NATIVE)){
						// share object to avoid be deallocated ...
						vm_share_object(_vm,so);

						// ... and put in the list to deref after execute
						deref_native_args[n_deref_native_args++]=so;

					}

					vm_share_object(_vm,so);

				}
				*stk_it++=_stk_params[i];
			}
		}

		// byte code executing starts here. Later script function can call c++ function, but once in c++ function is not possible by now call script function again.
		vm_execute_script_function(
			_vm,
			_this_object,
			_script_function,
			stk_start
		);

		// TODO: Eval
		/*if(data->vm_current_scope_function == ZS_VM_SCOPE_FUNCTION_MAIN){
			vm_deref_cyclic_references(_vm);
		}*/

		// any empty shared pointers after call main
		if(data->vm_current_scope_function == ZS_VM_SCOPE_FUNCTION_MAIN){
			vm_remove_empty_shared_pointers(_vm
					,vm_get_scope_block_main(_vm)
			);
		}

		// get number return elements
		if(data->vm_error){
			// set error as false
			// it was error so reset stack and stop execution ...
			vm_do_stack_dump(_vm);

			String total_error=data->vm_error_description;
			total_error.append(data->vm_error_callstack_str);

			throw_exception_file_line(data->vm_error_file.toConstChar(),data->vm_error_line,total_error.toConstChar());
		}else{
			int n_returned_arguments_from_function=data->vm_stk_current-(stk_start+(int)_script_function->local_variables->length());

			if(n_returned_arguments_from_function > 0){

				// get first...
				StackElement *ptr_stk_return=(data->vm_stk_current-n_returned_arguments_from_function);
				stk_return = ptr_stk_return[0];

				// if object add into lifetime till user delete it
				if(stk_return.properties & STACK_ELEMENT_PROPERTY_OBJECT){
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
					/*StackElement stk_ret=ptr_stk_return[i];
					if(stk_ret.properties & STACK_ELEMENT_PROPERTY_OBJECT){
						delete (ScriptObject *)stk_ret.value;
					}*/
					// deinit vm variable...
					ptr_stk_return[i].setUndefined();
				}
			}
		}

		// unref params passed through from native function if any
		for(int i=0; i < n_deref_native_args; i++){
			vm_unref_shared_object(_vm,deref_native_args[i],deref_native_args[i]->shared_pointer->data.vm_scope_block_where_created);
		}

		// Important restore stk!
		data->vm_stk_current=stk_start;

		// restore idx_call as 0 if first call was a script function called from C
		if(first_script_call_from_c==true){
			data->vm_current_scope_function=ZS_VM_SCOPE_FUNCTION_MAIN;
		}

		return stk_return;
	}

	void vm_delete(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(data->lifetime_object.length()>0){
			String created_at="";
			String end="";
			bool some_registers_without_file_line=false;

			String error="\n\nSome lifetime objects returned by virtual machine were not unreferenced:\n\n";
			for(int i=0; i < data->lifetime_object.length();i++ ){
				InfoLifetimeObject *info=data->lifetime_object.get(i);
				created_at="";
				end="";

				if(info->file != 0 && *info->file!=0){
					created_at=String::format(" at " ZS_FORMAT_FILE_LINE,Path::getFilename(info->file).toConstChar(),info->line);
				}else{
					created_at=" at [??:??]";
					some_registers_without_file_line=true;
				}
				error+=String::format("* Returned lifetime object%s was not unreferenced. %s \n",created_at.toConstChar(),end.toConstChar());

			}

			if(some_registers_without_file_line==true){
				error+="\n\nSome lifetimes objects were returned/created from unknown file/line. Tip: Pass pass the file and line to all 'eval' function that it's calling from c++ in order to give you a clue where the 'lifetime object' was returned/created. For example,\n\n\n"
						"\tStackElement var_from_script=zs->eval(\"return new ScriptObject()\",__FILE__,__LINE__)\n";
			}

			error+="\n\nLifetime objects returned by virtual machine must be unreferenced by calling 'unrefLifetimeObject' \n\n";

			error+="\n\n";

			fprintf(stderr,"%s",error.toConstChar());

		}

		delete data;
		free(vm);
	}

	void vm_log_container_slot(
		VirtualMachine* _vm
		, ContainerSlot* _container_slot
		, ContainerScriptObject* _src_container_ref
	) {

		VirtualMachineData* data = (VirtualMachineData*)_vm->data;

		StackElement* stk_obj = NULL;
		ContainerScriptObject* dst_container_ref = _container_slot->getDstContainerRef();
		zs_int dst_container_slot_id = _container_slot->getIdSlot();

		// More tests would be needed see issue #336
		if (dst_container_ref->script_type_id == SCRIPT_TYPE_ID_ARRAY_SCRIPT_OBJECT) {
			stk_obj = ((ArrayScriptObject*)dst_container_ref)->getStackElementByIndex((int)dst_container_slot_id);

			printf("\nAssing object %p type '%s' TO  vector %p slot '%i' type '%s'. Last value type '%s'\n"
				, (void*)_src_container_ref
				, _src_container_ref->getScriptType()->name.toConstChar()
				, (void*)dst_container_ref
				, (int)dst_container_slot_id
				, dst_container_ref->getScriptType()->name.toConstChar()
				, data->zs->stackElementToStringTypeOf( stk_obj).toConstChar()

			);

		}
		else {
			// object
			stk_obj = dst_container_ref->getStackElementByKeyName((const char*)dst_container_slot_id);
			printf("\nAssing object %p type '%s' TO  object %p slot '%s' type '%s'. Last value type '%s'\n"
				, (void*)_src_container_ref
				, _src_container_ref->getScriptType()->name.toConstChar()
				, (void*)dst_container_ref
				, (const char*)dst_container_slot_id
				, dst_container_ref->getScriptType()->name.toConstChar()
				, data->zs->stackElementToStringTypeOf( stk_obj).toConstChar()

			);

		}
	
	}

	void vm_assign_container_slot(
			VirtualMachine *_vm
			, ContainerSlot *_container_slot
			, ContainerScriptObject *_src_container_ref
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;

		StackElement *stk_dst=_container_slot->getPtrStackElement();

#ifdef __ZS_LOG_CONTAINER_SLOT__
		vm_log_container_slot(_vm, _container_slot, _src_container_ref);
#endif

		// finally adds to container slot hierarchy
		_src_container_ref->addContainerSlot(_container_slot);

		// do the assigment
		stk_dst->value=(zs_int)_container_slot;
		stk_dst->properties=STACK_ELEMENT_PROPERTY_CONTAINER_SLOT;

		// add ref into map of pointers
		data->cyclic_container_instances.set((zs_int)_src_container_ref,(zs_int)_src_container_ref);

	}

	void vm_remove_container_instance_cyclic_references_map(VirtualMachine *_vm,ContainerScriptObject *_csso){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		data->cyclic_container_instances.erase((zs_int)_csso);
	}

	void vm_count_cyclic_references(ContainerScriptObject *_current,ContainerScriptObject *_container_to_check, Vector<ListNode<ContainerSlot *> *> *_slots){
		ListNode<ContainerSlot *> *first_node=NULL,* current_node=NULL;

		current_node=first_node=_current->getListContainerSlotsRef()->first;

		if(current_node != NULL){
			do{
				if(_container_to_check==current_node->data->getSrcContainerRef()){
					_slots->push(current_node);//_count++;
				}
				current_node=current_node->next;
			}while(current_node!=first_node);

		}
	}

	MapInt *vm_get_cyclic_container_instances(VirtualMachine *_vm){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		return &data->cyclic_container_instances;
	}

	void vm_deref_cyclic_references(VirtualMachine *_vm){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		Vector<ListNode<ContainerSlot *>  *> slots;

		for(size_t it=0; it < data->cyclic_container_instances.count();){
			// count reference number
			ContainerScriptObject *cso=(ContainerScriptObject *)(data->cyclic_container_instances.getValueByIdx(it));
			bool cyclic_container_instances_element=false;

			vm_count_cyclic_references(cso,cso,&slots);
#ifdef __ZS_LOG_CONTAINER_SLOT__
			printf("Counting reference for container instance %p. Cyclic counts %i Shares counts: %i. Can be removed (cyclic == n_shares): %s\n"
				,(void *)cso
				,slots.count
				,cso->shared_pointer->data.n_shares
				,slots.count==cso->shared_pointer->data.n_shares?"true":"false"
			);
#endif

			// if n_shares == shared_pointers
			if(slots.length()>0){
				if(slots.length()==cso->shared_pointer->data.n_shares){
					cyclic_container_instances_element=true;
					auto container_slots_intance=cso->getListContainerSlotsRef();

					for(int i=0; i < slots.length();i++){

						// for each container slot.
						// 1. dettach from container slot list
						auto container_slot_node=slots.get(i)->data->getContainerSlotNode();
						container_slots_intance->remove(container_slot_node);
						vm_unref_shared_object(_vm,cso,ZS_VM_MAIN_SCOPE_BLOCK);

						// 2. back container slot as undefined
						//
						// delete container slot
						auto stk=slots.get(i)->data->getPtrStackElement();

						if((stk->properties & STACK_ELEMENT_PROPERTY_CONTAINER_SLOT)==0){
							ZS_THROW_RUNTIME_ERRORF("stk container is not container slot");
						}

						ContainerSlot::deleteContainerSlot((ContainerSlot *)stk->value,false);

						// and set as undefined
						*stk=k_stk_undefined;

					}
				}
			}

			if(cyclic_container_instances_element==true){
				data->cyclic_container_instances.erase((zs_int)cso);
			}else{
				it++;
			}

			slots.clear();
		}


	}

	VM_ScopeBlock *vm_get_main_scope_block(
			VirtualMachine *_vm
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		return ZS_VM_MAIN_SCOPE_BLOCK;
	}


}

#include "vm_script_function.cpp"
#include "vm_native_function.cpp"
#include "vm_share_manager.cpp"

