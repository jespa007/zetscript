/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define IDX_CALL_STACK_MAIN 1


// explains whether stk is this or not. Warning should be given as value and not as ptr
#define IS_STK_THIS(stk) (this_object != NULL && (stk)->value == (zs_int)(this_object))

#define PRINT_DUAL_ERROR_OP(c)\
zs_string var_type1=stk_to_typeof_str(data->zs,stk_result_op1),\
	   var_type2=stk_to_typeof_str(data->zs,stk_result_op2);\
\
	VM_ERROR("cannot perform operator '%s' %s '%s'. Check whether op1 and op2 are same type, or class implements the metamethod",\
		var_type1.c_str(),\
		c,\
		var_type2.c_str());\

#define PRINT_ERROR_OP(c)\
	zs_string var_type1=stk_to_typeof_str(data->zs,stk_result_op1);\
\
VM_ERROR("cannot perform preoperator %s'%s'. Check whether op1 implements the metamethod",\
	c,\
	var_type1.c_str());\
	return NULL;

#define CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_return, n_return,with_share)\
	for(int i=0; i < n_return; i++){\
		StackElement *stk_ret = stk_return+i;\
		if(stk_ret->properties & STK_PROPERTY_SCRIPT_OBJECT){\
			ScriptObject *sv=(ScriptObject *)stk_ret->value;\
			if(sv->shared_pointer == NULL){\
				if(!vm_create_shared_pointer(vm,sv)){\
					goto lbl_exit_function;\
				}\
				PRAGMA_PUSH\
				PRAGMA_DISABLE_WARNING(4127)\
				if(with_share==true){\
					PRAGMA_POP\
					if(!vm_share_pointer(vm,sv)){\
						goto lbl_exit_function;\
					}\
				}\
			}\
		}\
	}

#define VM_INNER_CALL_ONLY_RETURN(so,sf,name,reset) VM_INNER_CALL(so,sf,name,reset,0,true)


#define VM_INNER_CALL(so,sf,name,reset,n_args,goto_on_error)\
{\
	StackElement *stk_def_afun_start=data->stk_vm_current;\
	int n_returned_args_afun=0;\
	if(((ScriptFunction *)sf)->properties & FUNCTION_PROPERTY_C_OBJECT_REF){\
		vm_call_function_native(\
				vm\
				,so\
				,((ScriptFunction *)sf)\
				,calling_function\
				,instruction\
				,stk_def_afun_start\
				,n_args\
		);\
	}else{\
		vm_call_function_script(\
			vm\
			,so\
			,((ScriptFunction *)sf)\
			,stk_def_afun_start\
		);\
	}\
	if(data->vm_error == true){ \
		data->vm_error_callstack_str+=zs_strutils::format(\
			"\nat %s (file:%s line:%i)" /* TODO: get full symbol ? */ \
			, name \
			,SFI_GET_FILE(calling_function,instruction)\
			,SFI_GET_LINE(calling_function,instruction)\
		);\
		if(goto_on_error){\
			goto lbl_exit_function;\
		}\
	}\
	n_returned_args_afun=data->stk_vm_current-stk_def_afun_start;\
	/* we share pointer (true second arg) to not remove on pop in calling return */\
	CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_def_afun_start,n_returned_args_afun,true) \
	/* reset stack */\
	if(reset){\
		data->stk_vm_current=stk_def_afun_start; \
	}\
}

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

		InfoSharedList zero_shares[VM_FUNCTION_CALL_MAX]; // each function contains the number of local scriptvars vars that should or not removed.
		InfoSharedList shared_vars; // global vector

		//===================================================================================================

		 bool				vm_error;
		 zs_string 			vm_error_str;
		 zs_string 			vm_error_callstack_str;
		 VM_ScopeFunction	*vm_current_scope_function;
		 VM_ScopeFunction	vm_scope_function[VM_FUNCTION_CALL_MAX];

		 StackElement     	vm_stack[VM_STACK_MAX];
		 zs_vector			lifetime_object;

		 // global vars show be initialized to stack array taking the difference (the registered variables on the main function) - global_vars ...
		StackElement *stk_vm_current;
		ScriptFunction  *main_function_object;
		ScriptType *main_class_object;

		int vm_idx_call;
		int idx_last_statment;
		const ScriptFunction *current_call_c_function;
		ZetScript *zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptTypeFactory 		*script_type_factory;
		ScopeFactory 			*scope_factory;

		StackElement stk_aux;

		VirtualMachineData(ZetScript *_zs){
			memset(&zero_shares,0,sizeof(zero_shares));
			memset(&shared_vars,0,sizeof(shared_vars));
			memset(&vm_stack,0,sizeof(vm_stack));

			vm_idx_call=0;
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
			vm_error_str="";
		}

	};

	//-----------------------------------------
	//
	// PROTOTIPES
	//

	void vm_call_function_script(
				VirtualMachine			* vm,
				ScriptObject			* this_object,
				ScriptFunction 			* calling_function,
				StackElement 		  	* _stk_local_var
	);

	void  vm_call_function_native(
		VirtualMachine *vm,
		ScriptObject  * this_object,
		const ScriptFunction *c_function,
		const ScriptFunction *calling_function,
		Instruction *instruction,
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


	inline void vm_remove_empty_shared_pointers(VirtualMachine *vm,int idx_call_stack){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(idx_call_stack<=0){
			return;
		}

		InfoSharedList *list=&data->zero_shares[idx_call_stack];
		InfoSharedPointerNode *next_node=NULL,*current=list->first;
		bool check_empty_shared_pointers=false;

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

	inline bool vm_decrement_shared_nodes_and_dettach_if_zero(VirtualMachine *vm,InfoSharedPointerNode *_node, bool & is_dettached){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		is_dettached=false;
		unsigned short *n_shares = &_node->data.n_shares;
		if(*n_shares > 0){ // not zero
			if(--(*n_shares)==0){ // mov back to 0s shares (candidate to be deleted on GC check)

				if(!vm_deattach_shared_node(vm,&data->shared_vars,_node)){
					return false;
				}

				is_dettached=true;
			}
		}

		return true;
	}

	inline bool vm_unref_shared_script_object_and_remove_if_zero(VirtualMachine *vm,ScriptObject **so){

		InfoSharedPointerNode *_node=(*so)->shared_pointer;
		bool is_dettached=false;

		if(vm_decrement_shared_nodes_and_dettach_if_zero(vm,_node,is_dettached)){

			if(is_dettached == true){
				delete _node->data.ptr_script_object_shared; // it deletes shared_script_object
				free(_node);
			}
			return true;
		}

		return false;
	}


	inline bool vm_unref_shared_script_object(VirtualMachine *vm,ScriptObject *_obj, int idx_current_call){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		InfoSharedPointerNode *shared_pointer=_obj->shared_pointer;
		if(shared_pointer!=NULL){
			bool is_dettached=false;
			if(vm_decrement_shared_nodes_and_dettach_if_zero(vm,shared_pointer,is_dettached)){
				if(is_dettached){
					shared_pointer->data.created_idx_call=idx_current_call;
					if(!vm_insert_shared_node(vm,&data->zero_shares[shared_pointer->data.created_idx_call],shared_pointer)){ // insert to zero shares vector to remove automatically on ending scope
						return false;
					}
				}
				return true;
			}
		}else{
			VM_SET_USER_ERRORF(vm,"shared ptr not registered");
		}

		return false;
	}


#define EXTRACT_FUNCTION_INFO\
	{ /* get elements from class */ \
		Symbol *symbol = (Symbol *)(*(stk_elements_builtin_ptr+i));\
		if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){ \
			irfs = (ScriptFunction *)symbol->ref_ptr;\
			if(irfs->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION ){\
				start_param=2;\
			}\
		}\
	}\
	if(irfs==NULL) continue;

	inline ScriptFunction * vm_find_function(
			VirtualMachine *vm
			,ScriptType *class_obj // if NULL is MainClass
			,ScriptFunction *calling_function
			,Instruction * instruction // call instruction
			,bool is_constructor
			//,void *stk_elements_builtin_ptr // vector of properties
			//,int stk_elements_builtin_len // vector of properties
			,const zs_string & symbol_to_find
			,StackElement *stk_arg
			,unsigned char n_args
		) {

		// by default search over global functions...
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		ScriptFunction * ptr_function_found=NULL;
		zs_string aux_string;
		int start_param=0;

		bool is_set_attrib_metamethod=zs_strutils::starts_with(symbol_to_find,"_set@");

		zs_int *stk_elements_builtin_ptr= data->main_function_object->function_scope->symbol_functions->items;// vector of symbols
		int stk_elements_builtin_len=  data->main_function_object->function_scope->symbol_functions->count;// vector of symbols

		if(class_obj != NULL){
			stk_elements_builtin_ptr=class_obj->class_scope->symbol_functions->items;
			stk_elements_builtin_len=class_obj->class_scope->symbol_functions->count;

		}

		for(int i = stk_elements_builtin_len-1; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
			ScriptFunction *irfs = NULL;
			start_param=1;

			EXTRACT_FUNCTION_INFO

			aux_string=irfs->function_name;

			bool symbol_equals=
								aux_string == symbol_to_find
								|| (is_set_attrib_metamethod && zs_strutils::starts_with(aux_string,"_set@"));

			if((symbol_equals && ((int)irfs->params_len == (n_args+start_param)))){
				if((irfs->properties & FUNCTION_PROPERTY_C_OBJECT_REF)){ /* C! Must match all args...*/
					bool all_check=true; /*  check arguments types ... */
					int idx_type=-1;
					int arg_idx_type=-1;

					for( unsigned k = 0; k < n_args && all_check;k++){ // ignore first parameter due expects zetscript
						StackElement *current_arg=&stk_arg[k];
						arg_idx_type=irfs->params[k+start_param].idx_type;

						if(arg_idx_type!=IDX_TYPE_STACK_ELEMENT
								 /*&&
						((current_arg->properties & STK_PROPERTY_PTR_STK) == 0)*/
								){

							//unsigned short var_type = GET_STK_PROPERTY_TYPES(current_arg->properties);
							if(current_arg->properties & STK_PROPERTY_PTR_STK){
								current_arg=(StackElement *)current_arg->value;
							}


							switch(current_arg->properties & STK_PROPERTY_TYPES){
								default:
									aux_string="unknow";
									all_check=false;
									break;
								case STK_PROPERTY_ZS_INT:
									idx_type=IDX_TYPE_ZS_INT_PTR_C;
									all_check=
											arg_idx_type==IDX_TYPE_ZS_INT_PTR_C
										  ||arg_idx_type==IDX_TYPE_ZS_INT_C
										  ||arg_idx_type==IDX_TYPE_ZS_FLOAT_PTR_C;
									break;
								case STK_PROPERTY_ZS_FLOAT:
									idx_type=IDX_TYPE_ZS_FLOAT_PTR_C;
									all_check=arg_idx_type==IDX_TYPE_ZS_FLOAT_PTR_C
											||arg_idx_type==IDX_TYPE_ZS_FLOAT_C
											||arg_idx_type==IDX_TYPE_ZS_INT_PTR_C
										    ||arg_idx_type==IDX_TYPE_ZS_INT_C;
									break;
								case STK_PROPERTY_BOOL:
									idx_type=IDX_TYPE_BOOL_PTR_C;
									break;
								case STK_PROPERTY_FUNCTION|STK_PROPERTY_MEMBER_FUNCTION:
									all_check=
												arg_idx_type==IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER;
									idx_type=IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER;
									break;
								case STK_PROPERTY_FUNCTION:
									idx_type=IDX_TYPE_FUNCTION;
									all_check=
												arg_idx_type==IDX_TYPE_FUNCTION;
									break;
								/*case STK_PROPERTY_NULL:
									all_check=false;
									break;*/
								case STK_PROPERTY_SCRIPT_OBJECT:

									if(STK_IS_SCRIPT_OBJECT_STRING(current_arg)){
										idx_type=IDX_TYPE_STRING_PTR_C;

										all_check =
											(	arg_idx_type==IDX_TYPE_STRING_PTR_C && current_arg->value!=0)
										  ||	arg_idx_type==IDX_TYPE_CONST_CHAR_PTR_C;
									}else if(STK_IS_SCRIPT_OBJECT_CLASS(current_arg)){
										ScriptObjectClass *var_object_class=((ScriptObjectClass *)current_arg->value);
										aux_string=var_object_class->getTypeName();
										if(arg_idx_type==idx_type){
											all_check=true;
										}
									}else{
										ScriptObject *var_object = NULL;
										var_object=((ScriptObject *)current_arg->value);
										aux_string=var_object->getTypeName();
										if(arg_idx_type==idx_type){
											all_check=true;
										}
									}
									break;
							}
						}
					}

					if(all_check){ /* we found the right function (set it up!) ... */
						ptr_function_found = irfs;
					}
				}else{ /* type script function  ... */
					ptr_function_found = irfs;
				}
			}
		}

		if(ptr_function_found == NULL){
			int n_candidates=0;
			zs_string str_candidates="";
			zs_string args_str = "";
			//int arg_idx_type=-1;
			/* get arguments... */
			for( unsigned k = 0; k < n_args;k++){
				StackElement *current_arg=&stk_arg[k];

				if(k>0){
					args_str.append(",");
				}
				//unsigned short var_type = GET_STK_PROPERTY_TYPES(current_arg->properties);
				if(current_arg->properties & STK_PROPERTY_PTR_STK){
					aux_string="StackElement";
				}
				else{

					switch(current_arg->properties & STK_PROPERTY_TYPES){

					default:
						aux_string="unknow";
						break;
					case STK_PROPERTY_ZS_INT:
						aux_string=k_str_zs_int_type;
						break;
					case STK_PROPERTY_ZS_FLOAT:
						aux_string=k_str_zs_float_type;
						break;
					case STK_PROPERTY_BOOL:
						aux_string=k_str_bool_type;
						break;
					case STK_PROPERTY_NULL:
						aux_string="null";
						break;
					case STK_PROPERTY_SCRIPT_OBJECT:
						if(STK_IS_SCRIPT_OBJECT_STRING(current_arg)){
							aux_string=k_str_zs_string_type_ptr;
							if(current_arg->value==0){ /* is constant char */
								aux_string=	k_str_const_char_type_ptr;
							}
						}else if(STK_IS_SCRIPT_OBJECT_CLASS(current_arg)){
							aux_string = ((ScriptObjectClass *)current_arg->value)->getTypeNamePtr();
						}else{ // object
							aux_string = ((ScriptObject *)current_arg->value)->getTypeName();
						}
						break;
					}
				}

				args_str.append(zs_rtti::demangle(aux_string.c_str()));

				if(current_arg->properties == STK_PROPERTY_ZS_INT
				||current_arg->properties == STK_PROPERTY_ZS_FLOAT
				||current_arg->properties == STK_PROPERTY_BOOL
				){
					args_str.append(" [*] ");
				}
			}

			for(int i = stk_elements_builtin_len-1; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
				start_param=1;
				ScriptFunction *irfs=NULL;

				EXTRACT_FUNCTION_INFO


				if(irfs->function_name == symbol_to_find){

					if(n_candidates == 0){
						str_candidates.append("\tPossible candidates are:\n\n");
					}
					str_candidates.append("\t\t-");

					// class if not mail
					if(class_obj!=NULL && class_obj->idx_type!=IDX_TYPE_CLASS_MAIN){
						str_candidates.append(class_obj->type_name.c_str());
						str_candidates.append("::");
					}

					// function name
					str_candidates.append(irfs->function_name.c_str());
					str_candidates.append("(");


					for(int a = 0; a < irfs->params_len; a++){
						if(a>0){
							str_candidates.append(",");
						}

						if(irfs->properties & FUNCTION_PROPERTY_C_OBJECT_REF){
							str_candidates+=zs_rtti::demangle(
								GET_IDX_2_CLASS_C_STR(data,irfs->params[a].idx_type)
							);
						}else{ /* typic var ... */
							str_candidates.append("arg");
							str_candidates.append(zs_strutils::zs_int_to_str(a+1));
						}
					}
					str_candidates.append(");\n");
					n_candidates++;
				}
			}

			if(n_candidates == 0){
				VM_ERROR("Cannot find %s '%s%s(%s)'.\n\n",
						is_constructor ? "constructor":"function",
								class_obj==NULL?"":class_obj->idx_type!=IDX_TYPE_CLASS_MAIN?(class_obj->type_name+"::").c_str():"",
								symbol_to_find.c_str(),//calling_function->getInstructionSymbolName(instruction),
						args_str.c_str()
				);

				return NULL;
			}
			else{
				VM_ERROR("Cannot match %s '%s%s(%s)' .\n\n%s",
					is_constructor ? "constructor":"function",
							class_obj==NULL?"":class_obj->idx_type!=IDX_TYPE_CLASS_MAIN?(class_obj->type_name+"::").c_str():"",
									symbol_to_find.c_str(),//calling_function->getInstructionSymbolName(instruction),
					args_str.c_str(),
					str_candidates.c_str());
				return NULL;
			}
		}

		return ptr_function_found;
	}

	inline bool vm_apply_static_metamethod(
		VirtualMachine *vm
		,ScriptFunction *calling_function
		,Instruction *instruction
		,ByteCodeMetamethod byte_code_metamethod
		,StackElement *stk_result_op1
		,StackElement *stk_result_op2
	
	) {
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		zs_string str_stk_result_op1_full_definition="";
		zs_string str_stk_result_op2_full_definition="";
		StackElement *stk_vm_current_backup,*stk_args;
		//int stk_element_len=0;
		ScriptFunction *ptr_function_found=NULL;
		StackElement ret_obj;
		const char *byte_code_metamethod_operator_str=byte_code_metamethod_to_operator_str(byte_code_metamethod);
		const char *str_symbol_metamethod=byte_code_metamethod_to_symbol_str(byte_code_metamethod);
		zs_string error_found="";
		ScriptObject *script_object=NULL;
		zs_string class_name_object_found="";
		int n_stk_args=byte_code_metamethod_get_num_arguments(byte_code_metamethod);
		StackElement *stk_return=NULL;
		int n_returned_arguments_from_function=0;

		ret_obj.setUndefined();

		// init stk
		stk_vm_current_backup=data->stk_vm_current;
		stk_args=data->stk_vm_current;

		if(stk_result_op1->properties & STK_PROPERTY_PTR_STK){
			stk_result_op1 = (StackElement *)(stk_result_op1->value);
		}

		if(stk_result_op2->properties & STK_PROPERTY_PTR_STK){
			stk_result_op2 = (StackElement *)(stk_result_op2->value);
		}

		if(
			// allowed classes that accepts metamethods
			STK_IS_SCRIPT_OBJECT_CLASS(stk_result_op1)
			|| STK_IS_SCRIPT_OBJECT_ITERATOR_STRING(stk_result_op1)
			|| STK_IS_SCRIPT_OBJECT_ITERATOR_VECTOR(stk_result_op1)
			|| STK_IS_SCRIPT_OBJECT_ITERATOR_OBJECT(stk_result_op1)
		){
			script_object = (ScriptObjectClass *)(stk_result_op1->value);
		}else if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject * script_object_found=(ScriptObject *)stk_result_op1->value;
			class_name_object_found=script_object_found->getTypeName();
		}

		// only C refs can check 2nd param
		if(script_object == NULL && stk_result_op2 != NULL) { // script null

			if(((stk_result_op2->properties & STK_PROPERTY_PTR_STK))){
				stk_result_op2 = (StackElement *)(stk_result_op2->value);
			}

			// allowed classes that accepts metamethods
			if(			STK_IS_SCRIPT_OBJECT_CLASS(stk_result_op2)
					|| STK_IS_SCRIPT_OBJECT_ITERATOR_STRING(stk_result_op2)
					|| STK_IS_SCRIPT_OBJECT_ITERATOR_VECTOR(stk_result_op2)
					|| STK_IS_SCRIPT_OBJECT_ITERATOR_OBJECT(stk_result_op2)

					){
				script_object = (ScriptObject *)(stk_result_op2->value);
			}else if(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT){
					ScriptObject *script_object_found=(ScriptObject *)stk_result_op2->value;
					class_name_object_found=script_object_found->getTypeName();
			}
		}

		if(script_object == NULL){ // cannot perform operation
			if(class_name_object_found.empty()){ // not any object found
				// Because script elements can return "null" due undefined properties, do not show any error to not confuse.
				// If is an internal error, fix!
			}else{
				if(instruction->byte_code == BYTE_CODE_JE_CASE){
					error_found=zs_strutils::format("Unable to perform '==' operator for case conditional");
				}else{
					error_found=zs_strutils::format("Type '%s' does not implements metamethod '%s'"
						,class_name_object_found.c_str()
						,byte_code_metamethod_to_symbol_str(byte_code_metamethod)
					);
				}
			}
			goto apply_metamethod_error;
		}

		//------------------------------------
		// push stk results...
		*data->stk_vm_current++=*stk_result_op1;
		if(n_stk_args==2){
			*data->stk_vm_current++=*stk_result_op2;
		}
		//------------------------------------

		if(ptr_function_found == NULL){

			if(script_object->isNativeObject()){ // because isNativeObject it can have more than one setter
				if((ptr_function_found = vm_find_function(
					vm
					,data->script_type_factory->getScriptType(script_object->idx_type)
					,calling_function
					,instruction
					,false
					,str_symbol_metamethod
					,stk_args
					,n_stk_args
				)) == NULL){
					error_found=zs_strutils::format("Operator metamethod '%s (aka %s)' it's not implemented or it cannot find appropriate arguments for calling function",str_symbol_metamethod,byte_code_metamethod_operator_str);
					goto apply_metamethod_error;
				}


			}else{ // get first item...
				ScriptType *sc=script_object->getScriptType();
				Symbol * symbol = sc->getSymbolMemberFunction(str_symbol_metamethod);

				if(symbol == NULL){
					error_found=zs_strutils::format("Operator metamethod '%s (aka %s)' is not implemented",str_symbol_metamethod,byte_code_metamethod_operator_str);
					goto apply_metamethod_error;
				}

				ScriptFunction *sf=(ScriptFunction *)symbol->ref_ptr;

				if((sf->properties & FUNCTION_PROPERTY_STATIC) == 0){
					error_found=zs_strutils::format("Operator metamethod '%s (aka %s)' is not a function",str_symbol_metamethod,byte_code_metamethod_operator_str);
					goto apply_metamethod_error;
				}

				ptr_function_found=(ScriptFunction *)symbol->ref_ptr;

			}
		}

		if((ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){
			// we have to share any object to avoid be removed on function exit
			if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
				if(vm_share_pointer(vm,(ScriptObject *)stk_result_op1->value)==false){
					goto apply_metamethod_error;
				}
			}
			if(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT){
				if(vm_share_pointer(vm,(ScriptObject *)stk_result_op2->value)==false){
					goto apply_metamethod_error;
				}
			}

			vm_call_function_script(
				vm
				,script_object
				,ptr_function_found
				,stk_args
			);
		}else{ //
			vm_call_function_native(
					vm
					,ptr_function_found->properties & FUNCTION_PROPERTY_STATIC ? NULL:script_object
					,ptr_function_found
					,calling_function
					,instruction
					,stk_args
					,n_stk_args
			);
		}

		stk_return=(stk_args+ptr_function_found->function_scope->symbol_variables->count );
		n_returned_arguments_from_function=data->stk_vm_current-stk_return;

		// setup all returned variables from function
		for(int i=0; i < n_returned_arguments_from_function; i++){

			StackElement *stk_ret = --data->stk_vm_current;

			// if a scriptvar --> init shared
			if(stk_ret->properties & STK_PROPERTY_SCRIPT_OBJECT){
				ScriptObject *sv=(ScriptObject *)stk_ret->value;

				// Auto destroy always C when ref == 0
				((ScriptObjectClass *)(stk_ret->value))->deleteNativeObjectOnDestroy(true);

				if(sv->shared_pointer == NULL){ // if return this, it holds ptr_shared_pointer
					if(!vm_create_shared_pointer(vm,sv)){
						return false;
					}
				}
			}
			// ... and push result if not function constructor...
		}

		ret_obj=stk_return[0];


		// reset stack...
		data->stk_vm_current=stk_vm_current_backup;

		*data->stk_vm_current++ = ret_obj;

		return data->vm_error == false;

apply_metamethod_error:


		if(stk_result_op1!=NULL && stk_result_op2!=NULL){
			VM_ERROR("Metamethod operation '%s' (aka %s) failed performing operation by types '%s' %s '%s': %s"
				,byte_code_metamethod_to_operator_str(byte_code_metamethod)
				,byte_code_metamethod_to_symbol_str(byte_code_metamethod)
				,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
				,byte_code_metamethod_to_operator_str(byte_code_metamethod)
				,stk_to_typeof_str(data->zs,stk_result_op2).c_str()
				,error_found.c_str()
			);

		}else if(stk_result_op1!=NULL){
			VM_ERROR("cannot perform operation '%s %s'. %s"
				,byte_code_metamethod_to_operator_str(byte_code_metamethod)
				,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
				,error_found.c_str()
			);

		}else{
			VM_ERROR("cannot perform operation '%s'. %s"
				,byte_code_metamethod_to_operator_str(byte_code_metamethod)
				,error_found.c_str()
			);
		}
		return false;
	}

	inline void vm_iterator_init(VirtualMachine *vm
			 ,ScriptFunction *calling_function
			,Instruction *instruction
			,StackElement *stk_result_op1
			,StackElement *stk_result_op2){

		// stk_op1 expects to be stk
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		//ScriptFunction *sf_iter=NULL;
		Symbol *symbol_iter;

		// stk_op2 expects to be obj with container

		if((stk_result_op2->properties & STK_PROPERTY_PTR_STK) == 0){
			VM_ERROR("internal: Expected stk",0);
			return;
		}

		if((stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT) == 0){
			//VM_ERROR("internal: Expected object");
			if((data->stk_vm_current->properties & STK_PROPERTY_SCRIPT_OBJECT) == 0){
				VM_ERROR("Variable '%s' it doesn't implements iterator",SFI_GET_SYMBOL_NAME(calling_function,instruction));
				return;
			}
		}

		stk_result_op2 = (StackElement *)(stk_result_op2->value);
		ScriptObject *obj=(ScriptObject *)stk_result_op1->value;
		ScriptType *sc=obj->getScriptType();

		symbol_iter=sc->getSymbolMemberFunction("iter");

		if(symbol_iter != NULL){

			ScriptFunction *so_function=(ScriptFunction *)symbol_iter->ref_ptr;
			ScriptObject *so_object=obj;

			int n_args=0;

			// ScriptObjectObject uses static method in order to call iter (i.e Object::iter(o)
			if((symbol_iter->properties & SYMBOL_PROPERTY_STATIC)!= 0){ //is static
				n_args=1;

				// only stores and not increment (++ ) in order to start the stk arg
				*data->stk_vm_current={(intptr_t)so_object,STK_PROPERTY_SCRIPT_OBJECT};
				so_object=NULL;
			}

			VM_INNER_CALL(
					so_object
					,so_function
					,"iter"
					,true
					,n_args
					,false
			);

			if(data->vm_error){
				return;
			}

			// ok stk_vm_current holds the iter object
			if((data->stk_vm_current->properties & STK_PROPERTY_SCRIPT_OBJECT) == false){
				VM_ERROR("Expected IteratorObject returned by 'iter' but it was '%s'"
						,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str());
				return;
			}

			obj=(ScriptObject *)data->stk_vm_current->value;
			sc=obj->getScriptType();

			// check all functions...
			if(sc->getSymbolMemberFunction("get")==NULL){
				VM_ERROR("IteratorObject '%s' does not implement 'get' function",obj->getTypeName().c_str());
				return;
			}

			if(sc->getSymbolMemberFunction("_post_inc")==NULL){
				VM_ERROR("IteratorObject '%s' does not implement '_post_inc' function",obj->getTypeName().c_str());
				return;
			}

			if(sc->getSymbolMemberFunction("end")==NULL){
				VM_ERROR("IteratorObject '%s' does not implement 'end' function",obj->getTypeName().c_str());
				return;
			}

			// everything allright store and share pointer
			*stk_result_op2=*data->stk_vm_current;
		}
		else{
			VM_ERROR("Object not implements 'iter' ",obj->getTypeName().c_str());
		}

		// get iterator...
lbl_exit_function:

		return;

	}

	inline bool vm_perform_in_operator(
			VirtualMachine *vm
			 ,ScriptFunction *calling_function
			,Instruction *instruction
			, StackElement *stk_result_op1
			, StackElement *stk_result_op2){
		zs_string error="";
		VirtualMachineData *data=(VirtualMachineData *)vm->data;

		if(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so_aux=(ScriptObject *)stk_result_op2->value;

			switch(so_aux->idx_type){
			case IDX_TYPE_SCRIPT_OBJECT_STRING: // check whether 'char' or 'string' exists
			if(stk_result_op1->properties & STK_PROPERTY_ZS_INT){
				VM_PUSH_STK_BOOLEAN(
					ScriptObjectStringWrap_contains(
						data->zs
						,((ScriptObjectString *)so_aux)
						,(zs_int)stk_result_op1->value
					)
				);
			}else if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)){
				zs_string str_op1=((ScriptObjectString *)stk_result_op1->value)->toString();
				VM_PUSH_STK_BOOLEAN(
					ScriptObjectStringWrap_contains(
						data->zs
						,(ScriptObjectString *)so_aux
						,&str_op1)
				);
			}else{
				error="operand is not 'zs_int' or 'ScriptObjectString' type";
			}
			break;
			case IDX_TYPE_SCRIPT_OBJECT_VECTOR: // check whether value exists...
			//PUSH_STK_BOOLEAN(((ScriptObjectVector *)so_aux)->exists(stk_result_op1));
				VM_PUSH_STK_BOOLEAN(
				ScriptObjectVectorWrap_contains(
					data->zs,(ScriptObjectVector *)so_aux,stk_result_op1
				)
			);
			break;
			case IDX_TYPE_SCRIPT_OBJECT_OBJECT: // check key value exists...
			 if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
				zs_string str_op1=((ScriptObjectString *)stk_result_op1->value)->toString();
				VM_PUSH_STK_BOOLEAN(
					ScriptObjectObjectWrap_contains(
						data->zs,(ScriptObjectObject *)so_aux,&str_op1
					)
				);

			 }else{
			 }
			break;
			default:
				// TODO:
				if(vm_apply_static_metamethod(
						vm,
						calling_function,
						instruction,
						BYTE_CODE_METAMETHOD_IN,
						stk_result_op2,
						stk_result_op1
				)==false){
					return false;
				}
			break;
			}
		}else{
			error="second operand is not iterable or not implements _in metamethod";

		}

		if(error.empty()==false){
			VM_ERROR("Cannot perform operation 'in' because %s",error.c_str());
			return false;
		}

		return true;
	}
}

