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

	#define print_vm_cr ZS_PRINT_INFO
	#else
	#define print_vm_cr(s,...)
	#endif


	void vm_init(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		// script class factory should be created and initialized
		data->script_function_factory=data->zs->getScriptFunctionFactory();
		data->script_class_factory=data->zs->getScriptClassFactory();
		data->scope_factory = data->zs->getScopeFactory();
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
			_node->data.zero_shares=&data->zero_shares[data->vm_idx_call]; // it saves the zeros nodes where was set

			// insert node into shared nodes ...
			if(!vm_insert_shared_node(vm,_node->data.zero_shares,_node)){
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
			if(!vm_deattach_shared_node(vm,_node->data.zero_shares,_node)){
				return false;
			}
			// update current stack due different levels from functions!
			//_node->currentStack=idx_current_call;
			if(!vm_insert_shared_node(vm,&data->shared_vars,_node)){
				return false;
			}

			// node is not in list of zero refs anymore...
			_node->data.zero_shares=NULL;
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

	std::string vm_get_error(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->vm_error_str;
	}

	void vm_set_error_file_line(VirtualMachine *vm, const char *file, int line, const char *in_txt,...){

		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		vm_set_error(vm,zs_strutils::format("[%s:%i] %s",file,line,out_txt));
	}

	void vm_insert_life_time_object(VirtualMachine *vm, const char *file, int line, ScriptObject *script_object){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		InfoLifetimeObject *info = (InfoLifetimeObject *)malloc(sizeof(InfoLifetimeObject));

		info->file=file;
		info->line=line;
		info->script_object=script_object;

		data->lifetime_object[script_object]=info;
	}

	void vm_destroy_life_time_object(VirtualMachine *vm,ScriptObject *script_object){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(data->lifetime_object.count(script_object)==0){
			THROW_RUNTIME_ERROR("Cannot find stack element lifetime");
		}

		InfoLifetimeObject *info = data->lifetime_object[script_object];

		data->lifetime_object.erase(script_object);

		delete info->script_object;
		free(info);
	}

	StackElement *vm_get_stack_elements(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->vm_stack;
	}

	void vm_push_stack_element(VirtualMachine *vm, StackElement stk){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
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
		if(idx_glb_element < data->main_function_object->registered_symbols->count){
			return &data->vm_stack[idx_glb_element];
		}else{
			VM_SET_USER_ERROR(vm,"getStackElement: out of bounds");
		}
		return NULL;
	}

	StackElement * 	vm_get_top_stack_element_from_stack(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return (data->stk_vm_current-1);
	}

	StackElement vm_execute(
		VirtualMachine *vm
		 ,ScriptObject 		*	this_object
		 ,ScriptFunction 	*	calling_function
		 ,StackElement 		*  	stk_params
		 ,unsigned	char  		n_stk_params
		 ,const char *file
		 ,int line
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(calling_function->instructions == NULL){ // no instructions do not execute
			return stk_undefined;
		}

		StackElement stk_return=stk_undefined;
		StackElement *stk_start=NULL;


		if(data->vm_idx_call==0){ // set stack and Init vars for first call...

			data->vm_error=false;
			data->vm_error_str="";
			data->vm_error_callstack_str="";

			stk_start=data->vm_stack;

			// calls script function from C : preserve stack space for global vars to avoid
			if(calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN){
				stk_start=&data->vm_stack[data->main_function_object->registered_symbols->count];
			}
		}else{ // Not main function -> allow params for other functions
			// push param stack elements...
			stk_start=data->stk_vm_current;

			for(unsigned i = 0; i < n_stk_params; i++){
				*stk_start++=stk_params[i];
			}
		}

		// byte code executing starts here. Later script function can call c++ function, but once in c++ function is not possible by now call script function again.
		//stk_start=stk_vm_current;

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
			int n_returned_arguments_from_function=data->stk_vm_current-(stk_start+calling_function->registered_symbols->count);

			if(n_returned_arguments_from_function > 0){

				// get first...
				StackElement *ptr_stk_return=(data->stk_vm_current-n_returned_arguments_from_function);
				stk_return = ptr_stk_return[0];

				// if object add into lifetime till user delete it
				if(stk_return.properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
					// add generated
					vm_insert_life_time_object(vm,file,line,(ScriptObjectObject *)stk_return.stk_value);
				}

				// deallocate all returned variables from 1
				for(int i=1; i < n_returned_arguments_from_function; i++){
					StackElement stk_aux=ptr_stk_return[i];
					if(stk_aux.properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
						delete (ScriptObject *)stk_aux.stk_value;
					}
				}
			}
		}

		return stk_return;
	}

	void vm_delete(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(data->lifetime_object.size()>0){

			std::string error="\n\nSome lifetime objects created by virtual machine were not destroyed:\n\n";

			for(auto it=data->lifetime_object.begin(); it !=data->lifetime_object.end();it++ ){
				error+=zs_strutils::format("* Object lifetime created at [%s:%i] was not destroyed \n",zs_path::get_filename(it->second->file).c_str(),it->second->line);
			}

			error+="\n\n";
			//error+="\nPlease destroy lifetime objects through destroyLifetimeObject() before destroy zetscript to avoid this exception\n";

			fprintf(stderr,"%s",error.c_str());

		}

		delete data;
		free(vm);
	}

}


#include "vm_call_function_native.cpp"
#include "vm_call_function_script.cpp"
