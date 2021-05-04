#define IDX_CALL_STACK_MAIN 1

#define PUSH_NULL \
STK_SET_NULL(data->stk_vm_current++); \

#define PUSH_BOOLEAN(init_value) \
data->stk_vm_current->value=(void *)((zs_int)(init_value)); \
data->stk_vm_current->properties=STK_PROPERTY_BOOL; \
data->stk_vm_current++;


#define PUSH_INTEGER(init_value) \
data->stk_vm_current->value=(void *)((zs_int)(init_value)); \
data->stk_vm_current->properties=STK_PROPERTY_ZS_INT; \
data->stk_vm_current++;

#define PUSH_OBJECT(obj_value) \
data->stk_vm_current->value=(void *)((zs_int)(obj_value)); \
data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT; \
data->stk_vm_current++;


#define PUSH_FLOAT(init_value) \
{\
	zs_float aux=(zs_float)(init_value); \
	ZS_FLOAT_COPY(&data->stk_vm_current->value,&aux); \
	data->stk_vm_current->properties=STK_PROPERTY_ZS_FLOAT; \
	data->stk_vm_current++; \
}

#define PUSH_FUNCTION(ref) \
data->stk_vm_current->value=(void *)ref; \
data->stk_vm_current->properties=STK_PROPERTY_FUNCTION; \
data->stk_vm_current++;

#define PUSH_CLASS(ref) \
data->stk_vm_current->value=(void *)ref; \
data->stk_vm_current->properties=STK_PROPERTY_CLASS; \
data->stk_vm_current++;

// explains whether stk is this or not. Warning should be given as value and not as ptr
#define STK_IS_THIS(stk) (this_object != NULL && (stk)->value == this_object)

#define PRINT_DUAL_ERROR_OP(c)\
std::string var_type1=stk_result_op1->typeStr(),\
	   var_type2=stk_result_op2->typeStr();\
\
	VM_ERROR("cannot perform operator \"%s\" %s \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
		var_type1.c_str(),\
		c,\
		var_type2.c_str());\

#define PRINT_ERROR_OP(c)\
	std::string var_type1=stk_result_op1->typeStr();\
\
VM_ERROR("cannot perform preoperator %s\"%s\". Check whether op1 implements the metamethod",\
	c,\
	var_type1.c_str());\
	return NULL;


// defer all local vars
#define POP_VM_SCOPE()\
 if(data->vm_scope>=data->vm_current_scope){\
	VM_SET_USER_ERROR(vm,"internal error: trying to pop at the bottom");\
}\
{\
	VM_Scope *vm_check_scope=(data->vm_current_scope-1);\
	StackElement         * stk_local_vars	=vm_check_scope->stk_local_vars;\
	zs_vector *scope_symbols=vm_check_scope->scope->registered_symbols;\
	zs_int *symbols					=scope_symbols->items;\
	for(int i = scope_symbols->count-1; i >=0 ; --i){\
		StackElement *stk_local_var =stk_local_vars+((Symbol *)(symbols[i]))->idx_position;\
		if((stk_local_var->properties & STK_PROPERTY_SCRIPT_OBJECT)==STK_PROPERTY_SCRIPT_OBJECT){\
			ScriptObject *so=(ScriptObject *)(stk_local_var->value);\
			if(so != NULL){\
				if(vm_unref_shared_script_object(vm,so,data->vm_idx_call)==false){\
					return;\
				}\
			}\
		}\
		STK_SET_NULL(stk_local_var);\
	}\
	--data->vm_current_scope;\
}
	


namespace zetscript{

	class ZetScript;

	typedef struct{
		const char *file;
		int line;
		ScriptObject * script_object;
	}InfoLifetimeObject;


	struct VM_Scope{
		Scope				*scope;
		ScriptFunction 		*script_function;
		StackElement 		*stk_local_vars;
		unsigned char 		properties;
	};


	struct VirtualMachineData{
		//char				str_aux[8192];
		//===================================================================================================
		//
		// POINTER MANAGER ...
		//

		InfoSharedList zero_shares[MAX_FUNCTION_CALL]; // each function contains the number of local scriptvars vars that should or not removed.
		InfoSharedList shared_vars; // global vector

		//===================================================================================================

		 bool				vm_error;
		 std::string 		vm_error_str;
		 std::string 		vm_error_callstack_str;
		 VM_Scope			*vm_current_scope;
		 VM_Scope			vm_scope[VM_SCOPE_MAX];
		 VM_Scope			*vm_scope_max;

		 StackElement     					vm_stack[VM_STACK_LOCAL_VAR_MAX];
		 std::map<void *,InfoLifetimeObject *>	lifetime_object;

		 // global vars show be initialized to stack array taking the difference (the registered variables on the main function) - global_vars ...
		StackElement *stk_vm_current;
		ScriptFunction  *main_function_object;
		ScriptClass *main_class_object;

		int vm_idx_call;
		int idx_last_statment;
		const ScriptFunction *current_call_c_function;
		ZetScript *zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptClassFactory 		*script_class_factory;
		ScopeFactory 			*scope_factory;

		StackElement stk_aux;

		VirtualMachineData(ZetScript *_zs){
			memset(&zero_shares,0,sizeof(zero_shares));
			memset(&shared_vars,0,sizeof(shared_vars));
			memset(&vm_stack,0,sizeof(vm_stack));

			vm_idx_call=0;
			stk_vm_current=NULL;
			vm_current_scope = vm_scope;

			idx_last_statment=0;

			vm_scope_max = &vm_scope[VM_SCOPE_MAX-1];

			main_function_object = NULL;
			main_class_object=NULL;

			current_call_c_function = NULL;
			zs=_zs;

			script_function_factory=NULL;
			script_class_factory=NULL;
			scope_factory=NULL;
			vm_error=false;
			vm_error_str="";
		}
	};

	void vm_call_function_script(
				VirtualMachine			* vm,
				ScriptObject			* this_object,
				ScriptFunction 			* calling_function,
				StackElement 		  	* _stk_local_var,
				unsigned char 			n_args
	);

	void  vm_call_function_native(
		VirtualMachine *vm,
		ScriptObject  * this_object,
		const ScriptFunction *calling_function,
		StackElement *stk_arg_calling_function,
		unsigned char n_args,
		Instruction *instruction
	);

	inline bool  vm_insert_shared_node(VirtualMachine *vm, InfoSharedList * list, InfoSharedPointerNode *_node){
		if(_node->next != NULL || _node->previous!=NULL) {
			VM_SET_USER_ERROR(vm," Internal error expected node not in list");
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
			VM_SET_USER_ERROR(vm," Internal error: An already deattached node");
			return false;
		}

		if((_node->previous == _node) && (_node->next == _node)){ // 1 single node...
			list->last=list->first=NULL;
		}
		else{ // dettach and attach next...
			// [1]<->[2]<-> ...[P]<->[C]<->[N]...[M-1]<->[M]
			//InfoSharedPointerNode *aux_node=_node;
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


		if(current != NULL){
			bool finish=false;
			do{
				next_node=current->next;
				finish=next_node ==list->first;

				if(!vm_deattach_shared_node(vm,list,current)){
					return;
				}

				//ZS_LOG_DEBUG("Deallocating %i:%p",current->data.ptr_script_object_shared->idx_script_class,current->data.ptr_script_object_shared);
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
				//ZS_LOG_DEBUG("Deallocating %i:%p",_node->data.ptr_script_object_shared->idx_script_class,_node->data.ptr_script_object_shared);
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
					shared_pointer->data.zero_shares=&data->zero_shares[idx_current_call];
					if(!vm_insert_shared_node(vm,shared_pointer->data.zero_shares,shared_pointer)){ // insert to zero shares vector to remove automatically on ending scope
						return false;
					}
				}
				return true;
			}
		}else{
			VM_SET_USER_ERROR(vm,"shared ptr not registered");
		}

		return false;
	}

	inline ScriptFunction * vm_find_function(
			VirtualMachine *vm
			,ScriptObject *calling_object
			,ScriptFunction *calling_function
			,Instruction * instruction // call instruction
			,bool is_constructor
			,void *stk_elements_builtin_ptr // vector of properties
			,int stk_elements_builtin_len // vector of properties
			,const std::string & symbol_to_find
			,StackElement *stk_arg
			,unsigned char n_args
		) {

		// by default search over global functions...
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		ScriptFunction * ptr_function_found=NULL;
		std::string aux_string;
		bool stk_element_are_vector_element_ptr=stk_elements_builtin_ptr!=data->vm_stack;

		for(int i = stk_elements_builtin_len-1; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
			StackElement *stk_element=NULL;
			ScriptFunction *irfs = NULL;
			int this_as_first_parameter=0;

			if(stk_element_are_vector_element_ptr){
				stk_element=(StackElement *)(((zs_int *)stk_elements_builtin_ptr)[i]);//(StackElement *)list_symbols->items[i];
			}else{
				stk_element=&((StackElement *)stk_elements_builtin_ptr)[i];
			}

			if((stk_element->properties & STK_PROPERTY_FUNCTION) == 0){
				continue;
			}


			if(stk_element->properties & STK_PROPERTY_MEMBER_FUNCTION ){
				StackMemberFunction *fm=(StackMemberFunction *)stk_element->value;
				irfs=fm->so_function;
				this_as_first_parameter=1;
			}else{
				irfs = (ScriptFunction *)stk_element->value;
			}
			aux_string=irfs->symbol.name;

			if((aux_string == symbol_to_find && irfs->params->count == (n_args+this_as_first_parameter))){
				if((irfs->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF)){ /* C! Must match all args...*/
					bool all_check=true; /*  check arguments types ... */
					int idx_type=-1;
					int arg_idx_type=-1;
					for( unsigned k = 0; k < n_args && all_check;k++){
						StackElement *current_arg=&stk_arg[k];
						arg_idx_type=((ScriptFunctionArg *)irfs->params->items[k+this_as_first_parameter])->idx_type;

						if(arg_idx_type!=IDX_BUILTIN_TYPE_STACK_ELEMENT){

							//unsigned short var_type = GET_STK_PROPERTY_TYPES(current_arg->properties);

							switch(current_arg->properties & STK_PROPERTY_TYPES){
								default:
									aux_string="unknow";
									all_check=false;
									break;
								case STK_PROPERTY_ZS_INT:
									idx_type=IDX_BUILTIN_TYPE_ZS_INT_PTR_C;
									all_check=
											arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_PTR_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_PTR_C;
									break;
								case STK_PROPERTY_ZS_FLOAT:
									idx_type=IDX_BUILTIN_TYPE_FLOAT_PTR_C;
									all_check=arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_PTR_C
											||arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_C
											||arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_PTR_C
										    ||arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_C;
									break;
								case STK_PROPERTY_BOOL:
									idx_type=IDX_BUILTIN_TYPE_BOOL_PTR_C;
									all_check=
											arg_idx_type==IDX_BUILTIN_TYPE_BOOL_PTR_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_BOOL_C;

									break;
								case STK_PROPERTY_NULL:
									all_check=false;
									break;
								case STK_PROPERTY_SCRIPT_OBJECT:

									if(STK_IS_SCRIPT_OBJECT_STRING(current_arg)){
										idx_type=IDX_BUILTIN_TYPE_STRING_PTR_C;

										all_check =
											(	arg_idx_type==IDX_BUILTIN_TYPE_STRING_PTR_C && current_arg->value!=0)
										  ||	arg_idx_type==IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C;
									}else if(STK_IS_SCRIPT_OBJECT_CLASS(current_arg)){
										ScriptObjectClass *var_object_class=((ScriptObjectClass *)current_arg->value);
										aux_string=var_object_class->getClassName();
										if(arg_idx_type==idx_type){
											all_check=true;
										}
									}else{
										ScriptObject *var_object = NULL;
										var_object=((ScriptObject *)current_arg->value);
										aux_string=var_object->getClassName();
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
			std::string str_candidates="";
			std::string args_str = "";
			/* get arguments... */

			for( unsigned k = 0; k < n_args;k++){
				StackElement *current_arg=&stk_arg[k];

				if(k>0){
					args_str+=",";
				}
				//unsigned short var_type = GET_STK_PROPERTY_TYPES(current_arg->properties);

				switch(current_arg->properties & STK_PROPERTY_TYPES){

				default:
					aux_string="unknow";
					break;
				case STK_PROPERTY_ZS_INT:
					aux_string=k_str_zs_int_type;
					break;
				case STK_PROPERTY_ZS_FLOAT:
					aux_string=k_str_float_type;
					break;
				case STK_PROPERTY_BOOL:
					aux_string=k_str_bool_type;
					break;
				case STK_PROPERTY_NULL:
					aux_string="undefined";
					break;
				case STK_PROPERTY_SCRIPT_OBJECT:
					if(STK_IS_SCRIPT_OBJECT_STRING(current_arg)){
						aux_string=k_str_string_type_ptr;
						if(current_arg->value==0){ /* is constant char */
							aux_string=	k_str_const_char_type_ptr;
						}
					}else if(STK_IS_SCRIPT_OBJECT_CLASS(current_arg)){
						aux_string = ((ScriptObjectClass *)current_arg->value)->getNativePointerClassName();
					}else{ // object
						aux_string = ((ScriptObject *)current_arg->value)->getClassName();
					}
					break;
				}
				args_str+=zs_rtti::demangle(aux_string);

				if(current_arg->properties == STK_PROPERTY_ZS_INT
				||current_arg->properties == STK_PROPERTY_ZS_FLOAT
				||current_arg->properties == STK_PROPERTY_BOOL
				){
					args_str+=" [*] ";
				}
			}

			for(int i = stk_elements_builtin_len-1; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
				StackElement *stk_element=NULL;
				ScriptFunction *irfs=NULL;

				if(stk_element_are_vector_element_ptr){
					stk_element=(StackElement *)(((zs_int *)stk_elements_builtin_ptr)[i]);//(StackElement *)list_symbols->items[i];
				}else{
					stk_element=&((StackElement *)stk_elements_builtin_ptr)[i];
				}
				if((stk_element->properties & STK_PROPERTY_FUNCTION)== 0){
					continue;
				}

				if(stk_element->properties & STK_PROPERTY_MEMBER_FUNCTION ){
					StackMemberFunction *fm=(StackMemberFunction *)stk_element->value;
					irfs=fm->so_function;
				}else{
					irfs = (ScriptFunction *)stk_element->value;
				}


				if(irfs->symbol.name == symbol_to_find){

					if(n_candidates == 0){
						str_candidates+="\tPossible candidates are:\n\n";
					}
					str_candidates+="\t\t-"+(calling_object==NULL?""
							:calling_object->idx_script_class!=IDX_BUILTIN_TYPE_MAIN?(calling_object->getClassName()+"::")
							:"")+irfs->symbol.name+"(";

					for(unsigned a = 0; a < irfs->params->count; a++){
						if(a>0){
							str_candidates+=",";
						}

						if(irfs->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF){
							str_candidates+=zs_rtti::demangle(
									GET_IDX_2_CLASS_C_STR(data,((ScriptFunctionArg *)irfs->params->items[a])->idx_type
							));
						}else{ /* typic var ... */
							str_candidates+="arg"+zs_strutils::zs_int_to_str(a+1);
						}
					}
					str_candidates+=");\n";
					n_candidates++;
				}
			}

			if(n_candidates == 0){
				VM_ERROR("Cannot find %s \"%s%s(%s)\".\n\n",
						is_constructor ? "constructor":"function",
						calling_object==NULL?"":calling_object->idx_script_class!=IDX_BUILTIN_TYPE_MAIN?(calling_object->getClassName()+"::").c_str():"",
								calling_function->getInstructionSymbolName(instruction),
						args_str.c_str()
				);

				return NULL;
			}
			else{
				VM_ERROR("Cannot match %s \"%s%s(%s)\" .\n\n%s",
					is_constructor ? "constructor":"function",
					calling_object==NULL?"":calling_object->idx_script_class!=IDX_BUILTIN_TYPE_MAIN?(calling_object->getClassName()+"::").c_str():"",
							calling_function->getInstructionSymbolName(instruction),
					args_str.c_str(),
					str_candidates.c_str());
				return NULL;
			}
		}

		return ptr_function_found;
	}

	inline bool vm_apply_metamethod_primitive(
			VirtualMachine *vm
			,ScriptFunction *calling_function
			,Instruction *instruction
			,ByteCodeMetamethod byte_code_metamethod
			,StackElement *stk_result_op1
			,StackElement *stk_result_op2
			, bool & error
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;

		if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op1)){
			stk_result_op1=((ScriptObjectVarRef *)stk_result_op1->value)->getStackElementPtr();
		}

		if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op2)){
			stk_result_op2=((ScriptObjectVarRef *)stk_result_op2->value)->getStackElementPtr();
		}
		//error=true;
		switch(byte_code_metamethod){
		case ByteCodeMetamethod::BYTE_CODE_METAMETHOD_ADD:
			if(
				STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)
					||
				STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)
			){
				ScriptObjectString *so_string=ScriptObjectString::newScriptObjectStringAddStk(data->zs,stk_result_op1,stk_result_op2);
				vm_create_shared_pointer(vm,so_string);
				PUSH_OBJECT(so_string);
				return true;
			}
			else if(
				STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op1)
					&&
				STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)
			){
				ScriptObjectVector *so_vector=ScriptObjectVector::newScriptObjectVectorAdd(
						data->zs
						,(ScriptObjectVector *)stk_result_op1->value
						,(ScriptObjectVector *)stk_result_op2->value
				);
				vm_create_shared_pointer(vm,so_vector);
				PUSH_OBJECT(so_vector);

				return true;
			}
			else{ // try object
				ScriptObject *obj1=NULL;
				ScriptObject *obj2=NULL;

				if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
					obj1=(ScriptObject *)stk_result_op1->value;
				}

				if(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT){
					obj2=(ScriptObject *)stk_result_op2->value;
				}


				if(
					obj1->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT
					&&
					obj2->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT
				){
					ScriptObjectObject *so_object=ScriptObjectObject::concat(
							data->zs
							,(ScriptObjectObject *)obj1
							,(ScriptObjectObject *)obj2
					);
					vm_create_shared_pointer(vm,so_object);
					PUSH_OBJECT(so_object);
					return true;
				}
			}
			break;
		default:
			break;
		}

		return false;
	}

	inline bool vm_apply_metamethod(
			VirtualMachine *vm
		 ,ScriptFunction *calling_function
		,Instruction *instruction
		,ByteCodeMetamethod byte_code_metamethod
		,StackElement *stk_result_op1
		,StackElement *stk_result_op2
	
	) {
		bool error=false;
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		//std::string str_symbol_to_find="";
		std::string str_stk_result_op1_full_definition="";
		std::string str_stk_result_op2_full_definition="";
		//ScriptObjectClass *calling_object=NULL;
		ScriptClass *script_class_aux=NULL;
		StackElement *stk_vm_current_backup,*stk_args;
		int stk_element_len=0;
		ScriptFunction *ptr_function_found=NULL;
		StackElement ret_obj;
		const char *byte_code_metamethod_operator_str=byte_code_metamethod_to_operator_str(byte_code_metamethod);
		const char *str_symbol_metamethod=byte_code_metamethod_to_symbol_str(byte_code_metamethod);
		//zs_vector *stk_builtin_elements=NULL;
		std::string error_found="";
		zs_vector * list_props=NULL;
		ScriptObject *script_object=NULL;
		std::string class_name_object_found="";
		ScriptObjectClass *one_param_object_class = NULL;
		int n_stk_args=byte_code_get_num_arguments_static_metamethod(byte_code_metamethod);
		StackElement *stk_return=NULL;
		int n_returned_arguments_from_function=0;
		//unsigned char n_args=instruction->value_op1; // number arguments will pass to this function
		//StackElement *stk_start_arg_call=(stk_vm_current-n_args);
		ret_obj.setUndefined();

		// init stk
		stk_vm_current_backup=stk_args=data->stk_vm_current;

		if(stk_result_op1->properties & STK_PROPERTY_PTR_STK){
			stk_result_op1 = (StackElement *)(stk_result_op1->value);
		}

		if(
				// allowed classes that accepts metamethods
				STK_IS_SCRIPT_OBJECT_CLASS(stk_result_op1)
				|| STK_IS_SCRIPT_OBJECT_STRING_ITERATOR(stk_result_op1)
				|| STK_IS_SCRIPT_OBJECT_VECTOR_ITERATOR(stk_result_op1)
				|| STK_IS_SCRIPT_OBJECT_OBJECT_ITERATOR(stk_result_op1)
		){
			script_object = (ScriptObjectClass *)(stk_result_op1->value);
		}else if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject * script_object_found=(ScriptObject *)stk_result_op1->value;
			class_name_object_found=script_object_found->getClassName();
		}


		// only C refs can check 2nd param
		if(script_object == NULL && stk_result_op2 != NULL) { // script null

			if(((stk_result_op2->properties & STK_PROPERTY_PTR_STK))){
				stk_result_op2 = (StackElement *)(stk_result_op2->value);
			}

			// allowed classes that accepts metamethods
			if(			STK_IS_SCRIPT_OBJECT_CLASS(stk_result_op2)
					|| STK_IS_SCRIPT_OBJECT_STRING_ITERATOR(stk_result_op2)
					|| STK_IS_SCRIPT_OBJECT_VECTOR_ITERATOR(stk_result_op2)
					|| STK_IS_SCRIPT_OBJECT_OBJECT_ITERATOR(stk_result_op2)

					){
				script_object = (ScriptObject *)(stk_result_op2->value);
			}else if(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT){
					ScriptObject *script_object_found=(ScriptObject *)stk_result_op2->value;
					class_name_object_found=script_object_found->getClassName();
			}
		}

		if(stk_result_op2 != NULL){
			if(vm_apply_metamethod_primitive(
						vm
						 ,calling_function
						,instruction
						,byte_code_metamethod
						,stk_result_op1
						,stk_result_op2
						, error
				)){
				return true;
			}

			if(error == true){
				return false;
			}
		}


		if(script_object == NULL){ // cannot perform operation
			if(class_name_object_found.empty()){ // not any object found
				// Because script elements can return "null" due undefined properties, do not show any error to not confuse.
				// If is an internal error, fix!
			}else{
				error_found=zs_strutils::format("Class \"%s\" is not allowed to have metamethods",class_name_object_found.c_str());
			}
			goto apply_metamethod_error;
		}

		if(script_object->isNativeObject()){ // because isNativeObject it can have more than one setter
			list_props=script_object->getStkBuiltinListElements();//getFunctions();

			if((ptr_function_found = vm_find_function(
				vm
				,NULL
				,calling_function
				,instruction
				,false
				,(void *)list_props->items
				,list_props->count
				,str_symbol_metamethod
				,stk_args+1
				,n_stk_args-1
			)) == NULL){
				error_found=zs_strutils::format("Operator metamethod \"%s (aka %s)\" it's not implemented or it cannot find appropriate arguments for calling function",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}


		}else{ // get first item...
			StackElement * stk = script_object->getProperty(str_symbol_metamethod,NULL);

			if(stk == NULL){
				error_found=zs_strutils::format("Operator metamethod \"%s (aka %s)\" is not implemented",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}

			if((stk->properties & STK_PROPERTY_FUNCTION)==0){
				error_found=zs_strutils::format("Operator metamethod \"%s (aka %s)\" is not a function",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}

			ptr_function_found=((StackMemberFunction *)stk->value)->so_function;

		}

		if((ptr_function_found->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) == 0){
			vm_call_function_script(
				vm
				,script_object
				,ptr_function_found
				,stk_args
				,n_stk_args
			);
		}else{ //
			vm_call_function_native(
					vm
					,script_object
					,ptr_function_found
					,stk_args
					,n_stk_args
					,instruction

			);
		}

		stk_return=(stk_args+ptr_function_found->registered_symbols->count );
		n_returned_arguments_from_function=data->stk_vm_current-stk_return;


		if(n_returned_arguments_from_function == 0){
			switch(byte_code_metamethod){
			case BYTE_CODE_METAMETHOD_PREVIOUS:
			case BYTE_CODE_METAMETHOD_NEXT:
				ret_obj.value=script_object;
				ret_obj.properties=STK_PROPERTY_SCRIPT_OBJECT;
				break;
			}

		}else{

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
		}



		// reset stack...
		data->stk_vm_current=stk_vm_current_backup;

		*data->stk_vm_current++ = ret_obj;
		return true;

apply_metamethod_error:


		if(n_stk_args==1){
			VM_ERROR("cannot perform operation \"%s %s\". %s"
				,byte_code_metamethod_to_operator_str(byte_code_metamethod)
				,stk_result_op1->toString().c_str()
				,error_found.c_str()
				);
		}else{
			VM_ERROR("cannot perform operation \"%s %s %s\". %s"
				,stk_result_op1->toString().c_str()
				,byte_code_metamethod_to_operator_str(byte_code_metamethod)
				,stk_result_op2->toString().c_str()
				,error_found.c_str()
				);
		}

		return false;
	}
}
