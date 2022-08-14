/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define __STK_VAR_COPY__			stk_aux1

// explains whether stk is this or not. Warning should be given as value and not as ptr
#define PRINT_DUAL_ERROR_OP(c)\
	VM_ERROR("cannot perform operator '%s' %s '%s'. Check whether op1 and op2 are same type, or type implements the metamethod",\
		stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1),\
		c,\
		stk_to_typeof_str(VM_STR_AUX_PARAM_1,data->zs,stk_result_op2));\

#define PRINT_ERROR_OP(c)\
VM_ERROR("cannot perform preoperator %s'%s'. Check whether op1 implements the metamethod",\
	c,\
	stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1));\
	return NULL;

#define CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_return, n_return,with_share)\
	for(int i=0; i < n_return; i++){\
		StackElement *stk_ret = stk_return+i;\
		if(stk_ret->properties & STK_PROPERTY_SCRIPT_OBJECT){\
			ScriptObject *sv=(ScriptObject *)stk_ret->value;\
			if(sv->shared_pointer == NULL){\
				if(!vm_create_shared_script_object(vm,sv)){\
					goto lbl_exit_function;\
				}\
				PRAGMA_PUSH\
				PRAGMA_DISABLE_WARNING(4127)\
				if(with_share==true){\
					PRAGMA_POP\
					if(!vm_share_script_object(vm,sv)){\
						goto lbl_exit_function;\
					}\
				}\
			}\
		}\
	}

//#define VM_INNER_CALL_ONLY_RETURN(_so,_sf,_name,_reset) VM_INNER_CALL(_so,_sf,0,_name,_reset,true)

#define VM_EXTRACT_FUNCTION_INFO\
	{ /* get elements from type */ \
		Symbol *symbol = (Symbol *)(*(stk_elements_builtin_ptr+i));\
		if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){ \
			irfs = (ScriptFunction *)symbol->ref_ptr;\
			if(irfs->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION ){\
				start_param=2;\
			}\
		}\
	}\
	if(irfs==NULL) continue;

#define VM_INNER_CALL(_so_object,_so_function,_n_args,_name)\
	vm_inner_call_internal( \
			vm \
			,calling_function \
			,instruction \
			,_so_object \
			,_so_function \
			,_n_args \
	);\
	if(data->vm_error){\
		goto lbl_exit_function;\
	}\



namespace zetscript{

	class ZetScript;

	typedef struct{
		const char *file;
		int line;
		ScriptObject * script_object;
	}InfoLifetimeObject;



	struct VirtualMachineData{
		//===================================================================================================
		//
		// POINTER MANAGER ...
		//

		//InfoSharedList zero_shares[VM_FUNCTION_CALL_MAX]; // each function contains the number of local scriptvars vars that should or not removed.
		//InfoSharedList shared_vars; // global vector

		//===================================================================================================

		 bool				vm_error,vm_error_max_stack_reached;
		 char 				vm_error_str[1024];
		 char 				vm_error_file[512];
		 char				vm_str_aux[3][512]; // todo str intermediate operations
		 char 				vm_str_metamethod_aux[100];
		 int 				vm_error_line;
		 char 				vm_error_callstack_str[4096];
		 VM_ScopeFunction	*vm_current_scope_function;
		 VM_ScopeFunction	vm_scope_function[VM_FUNCTION_CALL_MAX];


		 StackElement     	vm_stack[VM_STACK_MAX];
		 zs_vector<InfoLifetimeObject *>			lifetime_object;

		 // global vars show be initialized to stack array taking the difference (the registered variables on the main function) - global_vars ...
		StackElement *stk_vm_current;
		ScriptFunction  *main_function_object;
		ScriptType *main_class_object;

		int idx_last_statment;
		const ScriptFunction *current_call_c_function;
		ZetScript *zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptTypeFactory 		*script_type_factory;
		ScopeFactory 			*scope_factory;

		StackElement stk_aux;

		VirtualMachineData(ZetScript *_zs){
			memset(&vm_stack,0,sizeof(vm_stack));

			stk_vm_current=NULL;
			vm_current_scope_function = vm_scope_function;

			idx_last_statment=0;

			main_function_object = NULL;
			main_class_object=NULL;

			current_call_c_function = NULL;
			zs=_zs;

			script_function_factory=NULL;
			script_type_factory=NULL;
			scope_factory=NULL;
			vm_error=false;
			memset(vm_error_str,0,sizeof(vm_error_str));
			memset(vm_error_file, 0, sizeof(vm_error_file));
			vm_error_line=-1;
			vm_error_max_stack_reached=false;

			// each push scope from main is done from block scope 1
			vm_scope_function[0].first_scope_block=&vm_scope_function[0].scope_block[1];
		}

	};

	//-----------------------------------------
	//
	// PROTOTIPES
	//

	inline void vm_inner_call_internal(
			VirtualMachine 	*vm
			,ScriptFunction	* calling_function
			,Instruction	* instruction
			,ScriptObject 	*script_object
			,ScriptFunction *script_function
			,int _n_args

	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		StackElement *stk_def_afun_start=data->stk_vm_current;
		int n_returned_args_afun=0;
		if((script_function)->properties & FUNCTION_PROPERTY_C_OBJECT_REF){
			vm_execute_function_native(
					vm
					,calling_function
					,instruction
					,script_object
					,script_function
					,stk_def_afun_start
					,_n_args
			);
		}else{
			vm_execute_function_script(
				vm
				,script_object
				,script_function
				,stk_def_afun_start
			);
		}
		if(data->vm_error == true){

			sprintf(VM_STR_AUX_PARAM_0\
				,"\nat %s (file:%s line:%i)" /* TODO: get full symbol ? */ \
				, "iter" \
				,SFI_GET_FILE(calling_function,instruction) \
				,SFI_GET_LINE(calling_function,instruction) \
			);\
			strcat(data->vm_error_callstack_str,VM_STR_AUX_PARAM_0);\
			return;
			/*if(goto_on_error){
				goto lbl_exit_function;
			}*/
		}
		n_returned_args_afun=data->stk_vm_current-stk_def_afun_start;
		/* we share pointer (true second arg) to not remove on pop in calling return */
		CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_def_afun_start,n_returned_args_afun,false)

		/* reset stack */
		data->stk_vm_current=stk_def_afun_start;

	lbl_exit_function:

		return;

	}

	void vm_execute_function_script(
				VirtualMachine			* vm,
				ScriptObject			* this_object,
				ScriptFunction 			* calling_function,
				StackElement 		  	* _stk_local_var
	);

	void  vm_execute_function_native(
		VirtualMachine *vm,
		const ScriptFunction *calling_function,
		Instruction *instruction,
		ScriptObject  * this_object,
		const ScriptFunction *c_function,
		StackElement *stk_arg_c_function=NULL,
		unsigned char n_args=0
	);

	inline bool  vm_insert_shared_node(VirtualMachine *vm, InfoSharedList * list, InfoSharedPointerNode *_node){
		if(_node->next != NULL || _node->previous!=NULL) {
			VM_SET_USER_ERRORF(vm," Internal error expected node not in list");
			return false;
		}

		if(list->first == NULL){ /*one  node: trivial ?*/
			_node->previous=_node->next= list->last = list->first =_node;
		}
		else{ /* >1 node add to the end */
			// attach last-previous
			_node->previous=list->last;
			list->last->next=_node;
			list->last=_node;

			// attach next
			_node->next=list->first;
			list->first->previous=_node;
		}
		return true;
	}

	inline bool vm_deattach_shared_node(VirtualMachine *vm, InfoSharedList * list, InfoSharedPointerNode *_node){

		if(list == NULL) return true;

		if(_node->next == NULL || _node->previous == NULL){
			VM_SET_USER_ERRORF(vm," Internal error: An already deattached node");
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

	inline bool vm_unref_shared_script_object(VirtualMachine *vm, ScriptObject *_obj,VM_ScopeBlock *_scope_block){
		InfoSharedPointerNode *shared_pointer=_obj->shared_pointer;
		if(shared_pointer==NULL){
			VM_SET_USER_ERRORF(vm,"shared ptr not registered");
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

			// weak pointer keep shared pointers
			if(_obj->deRefWeakPointer()){
				vm_share_script_object(vm,_obj);
				//shared_pointer->data.n_shares=1; // already weak pointers
				return true;
			}

			if(_scope_block==NULL){
				delete shared_pointer->data.ptr_script_object_shared; // it deletes shared_script_object
				free(shared_pointer);
			}else{
				InfoSharedList *unreferenced_objects = &_scope_block->unreferenced_objects;

				// insert to zero shares vector to remove automatically on ending scope
				if(vm_insert_shared_node(vm,unreferenced_objects,shared_pointer)==false){
					return false;
				}
			}
		}

		return true;
	}

	// defer all local vars
	inline void vm_pop_scope(VirtualMachine *_vm)
	{\
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		VM_ScopeBlock *scope_block=--VM_CURRENT_SCOPE_FUNCTION->current_scope_block;\
		Scope *scope=scope_block->scope;\
		StackElement         * stk_local_vars	=VM_CURRENT_SCOPE_FUNCTION->stk_local_vars;\
		zs_vector<Symbol *> *scope_symbols=scope->symbol_variables;\
		int count=scope_symbols->count;\
		if(count > 0){\
			StackElement *stk_local_var=stk_local_vars+scope_symbols->items[0]->idx_position;\
			while(count--){\
				if((stk_local_var->properties & STK_PROPERTY_SCRIPT_OBJECT)){\
					ScriptObject *so=(ScriptObject *)(stk_local_var->value);\
					if(so != NULL && so->shared_pointer!=NULL){\
						 vm_unref_shared_script_object(_vm,so,NULL);\
					}\
				}\
				STK_SET_UNDEFINED(stk_local_var);\
				stk_local_var++;\
			}\
		}\
		vm_remove_empty_shared_pointers(_vm,scope_block);\
	}

}

