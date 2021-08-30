/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define CALL_GC SHARED_LIST_DESTROY(zero_shares[idx_current_call])

namespace zetscript{

	struct VM_Foreach{
		StackElement 		   	*key; // iterator element can be std::string or integer...
		ScriptObjectObject	*ptr; // can be struct or std::vector...
		unsigned int 		   	idx_current;

	};


	void vm_do_stack_dump(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		// derefer all variables in all scopes (except main )...
		while(data->vm_scope<(data->vm_current_scope)){
			POP_VM_SCOPE();
		}

		vm_remove_empty_shared_pointers(vm,IDX_CALL_STACK_MAIN);
		data->vm_idx_call=0;
	}

	VirtualMachine *vm_new(ZetScript *_zs){

		VirtualMachine *vm = (VirtualMachine *)malloc(sizeof(VirtualMachine));
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


	void vm_init(VirtualMachine *vm,ZetScript *_zs){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;

		// script class factory should be created and initialized
		data->script_function_factory=_zs->getScriptFunctionFactory();
		data->script_class_factory=_zs->getScriptClassFactory();
		data->scope_factory = _zs->getScopeFactory();
		data->main_function_object = MAIN_FUNCTION(data);
		data->main_class_object = SCRIPT_CLASS_MAIN(data);
	}

	//============================================================================================================================================
	// POINTER MANANAGER
	bool vm_create_shared_pointer(VirtualMachine *vm,ScriptObject *_obj){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(_obj->shared_pointer == NULL){
			InfoSharedPointerNode *_node = (InfoSharedPointerNode *)malloc(sizeof(InfoSharedPointerNode));
			// init
			_node->previous=NULL;
			_node->next=NULL;
			_node->data.n_shares=0;
			_node->data.ptr_script_object_shared=_obj;
			_node->data.created_idx_call=data->vm_idx_call; // it saves the zeros nodes where was set

			// insert node into shared nodes ...
			if(!vm_insert_shared_node(vm,&data->zero_shares[data->vm_idx_call],_node)){
				return false;
			}
			_obj->shared_pointer=_node;
			return true;
		}else{
			VM_SET_USER_ERROR(vm," shared ptr already has a shared pointer data");
		}

		return false;

	}

	bool vm_share_pointer(VirtualMachine *vm,ScriptObject *_obj){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		InfoSharedPointerNode *_node=_obj->shared_pointer;

		unsigned short *n_shares = &_node->data.n_shares;

		bool move_to_shared_list=*n_shares==0;

		if(*n_shares >= MAX_SHARES_VARIABLE){
			VM_SET_USER_ERROR(vm,"MAX SHARED VARIABLES (Max. %i)",MAX_SHARES_VARIABLE);
			return false;
		}

		(*n_shares)++;

		if(move_to_shared_list){

			// Mov to shared pointer...
			if(!vm_deattach_shared_node(vm,&data->zero_shares[_node->data.created_idx_call],_node)){
				return false;
			}
			// update current stack due different levels from functions!
			//_node->currentStack=idx_current_call;
			if(!vm_insert_shared_node(vm,&data->shared_vars,_node)){
				return false;
			}

			ZS_LOG_DEBUG("Share pointer %i:%p",_node->data.ptr_script_object_shared->idx_script_class,_node->data.ptr_script_object_shared);

		}
		return true;
	}
	// POINTER MANANAGER
	//============================================================================================================================================


	const ScriptFunction * vm_get_current_native_function_call(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->current_call_c_function;
	}

	void vm_set_error(VirtualMachine *vm,const std::string & str){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		data->vm_error = true;
		data->vm_error_str=str;
	}

	bool vm_it_has_error(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->vm_error;
	}

	void vm_set_error_file_line(VirtualMachine *vm, const char *file, int line, const char *in_txt,...){

		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		vm_set_error(vm,zs_strutils::format("[%s:%i] %s",file,line,out_txt));
	}

	std::string vm_get_error(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->vm_error_str;
	}



	void vm_insert_lifetime_object(VirtualMachine *vm, const char *file, int line, ScriptObject *script_object){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		InfoLifetimeObject *info = (InfoLifetimeObject *)malloc(sizeof(InfoLifetimeObject));

		info->file=file;
		info->line=line;
		info->script_object=script_object;

		if(script_object->shared_pointer != NULL){
			if(script_object->shared_pointer->data.n_shares > 0){
				script_object->shared_pointer->data.n_shares++;
			}
		}

		data->lifetime_object[script_object]=info;
	}

	void vm_unref_lifetime_object(VirtualMachine *vm,ScriptObject *script_object){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(data->lifetime_object.count(script_object)==0){
			THROW_RUNTIME_ERROR("Cannot find stack element lifetime");
		}

		InfoLifetimeObject *info = data->lifetime_object[script_object];

		data->lifetime_object.erase(script_object);

		if(info->script_object->shared_pointer!=NULL){
			vm_unref_shared_script_object_and_remove_if_zero(vm,&script_object);
		}else{
			delete script_object;
		}
		free(info);
	}

	StackElement *vm_get_stack_elements(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->vm_stack;
	}

	void vm_push_stack_element(VirtualMachine *vm, StackElement stk){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(((data->stk_vm_current-data->vm_stack)+1)>=VM_STACK_LOCAL_VAR_MAX){

			vm_set_error(vm,"Error MAXIMUM stack size reached");
			return;
		}
		*data->stk_vm_current++=stk;
	}

	bool vm_set_stack_element_at(VirtualMachine *vm,unsigned int idx, StackElement stk){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(idx >= VM_STACK_LOCAL_VAR_MAX){
			VM_SET_USER_ERROR(vm,"setStackElement: out of bounds");
			return false;
		}

		data->vm_stack[idx]=stk;
		return true;
	}

	StackElement *vm_get_current_stack_element(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->stk_vm_current;
	}

	StackElement * vm_get_stack_element_at(VirtualMachine *vm,unsigned int idx_glb_element){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(idx_glb_element < data->main_function_object->symbol_registered_variables->size()){
			return &data->vm_stack[idx_glb_element];
		}else{
			VM_SET_USER_ERROR(vm,"getStackElement: out of bounds");
		}
		return NULL;
	}

	/*StackElement * 	vm_get_top_stack_element_from_stack(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return (data->stk_vm_current);
	}*/

	StackElement vm_execute(
		VirtualMachine *vm
		 ,ScriptObject 		*	this_object
		 ,ScriptFunction 	*	calling_function
		 ,StackElement 		*  	stk_params
		 ,unsigned	char  		n_stk_params
		 ,unsigned short		properties
		 ,const char *file
		 ,int line
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(calling_function->instructions == NULL){ // no instructions do not execute
			return k_stk_undefined;
		}

		StackElement stk_return=k_stk_undefined;
		StackElement *stk_start=NULL;


		if(
			calling_function->idx_script_function==IDX_SCRIPT_FUNCTION_MAIN
			&& ((properties & VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE) == 0)
		){ // set stack and Init vars for first call...


			if(data->vm_idx_call != 0){
				THROW_RUNTIME_ERROR("Internal: vm_idx_call != 0 (%i)",data->vm_idx_call);
			}


			data->vm_error=false;
			data->vm_error_str="";
			data->vm_error_callstack_str="";

			stk_start=data->vm_stack;
			n_stk_params=data->main_function_object->symbol_registered_variables->size();

			// calls script function from C : preserve stack space for global vars to avoid
			//stk_start=&data->vm_stack[data->main_function_object->registered_symbols->count];
		}else{ // Not main function -> allow params for other functions
			// push param stack elements...
            /*if(data->vm_idx_call == 0){
				data->vm_idx_call=1; // is calling from application set as 1 to make sure it not become conflict with global vars
			}*/
			stk_start=data->stk_vm_current;
			StackElement *min_stk=&data->vm_stack[data->main_function_object->symbol_registered_variables->size()];

			if (properties & VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE){
				data->vm_idx_call = 1;
				stk_start=min_stk;
			}


			if(stk_start<min_stk){ // control: not overwrite global symbols
				// this could happen when you call script function from native c++, so this control is needed
				THROW_RUNTIME_ERROR("Internal: stk_start < min_stk");
			}

			StackElement *stk_aux=stk_start;
			for(unsigned i = 0; i < n_stk_params; i++){
				*stk_aux++=stk_params[i];
			}
		}

		// byte code executing starts here. Later script function can call c++ function, but once in c++ function is not possible by now call script function again.
		vm_call_function_script(
			vm,
			this_object,
			calling_function,
			stk_start,
			n_stk_params);

		// get number return elements
		if(data->vm_error){
			// it was error so reset stack and stop execution ? ...
			vm_do_stack_dump(vm);
			throw std::runtime_error(data->vm_error_str+data->vm_error_callstack_str);
		}else{
			int n_returned_arguments_from_function=data->stk_vm_current-(stk_start+calling_function->symbol_registered_variables->size());

			if(n_returned_arguments_from_function > 0){

				// get first...
				StackElement *ptr_stk_return=(data->stk_vm_current-n_returned_arguments_from_function);
				stk_return = ptr_stk_return[0];

				// if object add into lifetime till user delete it
				if(stk_return.properties & STK_PROPERTY_SCRIPT_OBJECT){
					// add generated
					vm_insert_lifetime_object(vm,file,line,(ScriptObjectObject *)stk_return.value);
				}

				// deinit vm variable...

				ptr_stk_return->setUndefined();
				// deallocate all returned variables from 1
				for(int i=1; i < n_returned_arguments_from_function; i++){
					StackElement stk_aux=ptr_stk_return[i];
					if(stk_aux.properties & STK_PROPERTY_SCRIPT_OBJECT){
						delete (ScriptObject *)stk_aux.value;
					}
					// deinit vm variable...
					ptr_stk_return[i].setUndefined();
				}
			}
		}

		// Important restore stk!
		data->stk_vm_current=stk_start;

		if((properties & VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE)){ // restore idx_call
			data->vm_idx_call=0;
		}

		return stk_return;
	}

	void vm_delete(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(data->lifetime_object.size()>0){
			std::string created_at="";
			std::string end="";

			std::string error="\n\nSome lifetime objects returned by virtual machine were not unreferenced:\n\n";
			for(auto it=data->lifetime_object.begin(); it !=data->lifetime_object.end();it++ ){
				created_at="";
				end="";
				if((it->second->file == 0 || *it->second->file==0)){
					end="Tip: Use ZS_EVAL in order to get file:line where the lifetime object was returned";
				}else{
					created_at=zs_strutils::format(" at [%s:%i]",zs_path::get_filename(it->second->file).c_str(),it->second->line);
				}
				error+=zs_strutils::format("* Returned lifetime object%s was not unreferenced. %s \n",created_at.c_str(),end.c_str());
			}
			error+="\n\nLifetime objects returned by virtual machine must be unreferenced by calling 'unrefLifetimeObject' \n\n";

			error+="\n\n";
			//error+="\nPlease destroy lifetime objects through unrefLifetimeObject() before destroy zetscript to avoid this exception\n";

			fprintf(stderr,"%s",error.c_str());

		}

		delete data;
		free(vm);
	}

}


#include "vm_call_function_native.cpp"
#include "vm_call_function_script.cpp"
