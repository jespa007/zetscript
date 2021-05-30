


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
std::string var_type1=stk_result_op1->typeOf(),\
	   var_type2=stk_result_op2->typeOf();\
\
	VM_ERROR("cannot perform operator \"%s\" %s \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
		var_type1.c_str(),\
		c,\
		var_type2.c_str());\

#define PRINT_ERROR_OP(c)\
	std::string var_type1=stk_result_op1->typeOf();\
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

#define CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_return, n_return,with_share)\
	for(int i=0; i < n_return; i++){\
		StackElement *stk_ret = stk_return+i;\
		if(stk_ret->properties & STK_PROPERTY_SCRIPT_OBJECT){\
			ScriptObject *sv=(ScriptObject *)stk_ret->value;\
			if(sv->shared_pointer == NULL){\
				if(!vm_create_shared_pointer(vm,sv)){\
					goto lbl_exit_function;\
				}\
				if(with_share==true){\
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
if(((ScriptFunction *)sf)->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF){\
	vm_call_function_native(\
			vm\
			,so\
			,((ScriptFunction *)sf)\
			,stk_def_afun_start\
			,n_args\
			,calling_function\
			,instruction\
	);\
}else{\
	vm_call_function_script(\
		vm\
		,so\
		,((ScriptFunction *)sf)\
		,stk_def_afun_start\
		,n_args\
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

	//-----------------------------------------
	//
	// PROTOTIPES
	//

	void vm_call_function_script(
				VirtualMachine			* vm,
				ScriptObject			* this_object,
				ScriptFunction 			* calling_function,
				StackElement 		  	* _stk_local_var,
				unsigned char 			n_args=0
	);

	void  vm_call_function_native(
		VirtualMachine *vm,
		ScriptObject  * this_object,
		const ScriptFunction *c_function,
		StackElement *stk_arg_c_function,
		unsigned char n_args,
		const ScriptFunction *calling_function,
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


#define EXTRACT_FUNCTION_INFO\
	if(class_obj!=NULL){ /* get elements from class */ \
		Symbol *symbol = (Symbol *)(((zs_int *)stk_elements_builtin_ptr)[i]);\
		if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){ \
			irfs = (ScriptFunction *)symbol->ref_ptr;\
			if(symbol->properties & SYMBOL_PROPERTY_MEMBER_FUNCTION ){\
				this_as_first_parameter=1;\
			}\
		}\
	}else{ \
		StackElement *stk_element=&((StackElement *)stk_elements_builtin_ptr)[i]; \
		if(stk_element->properties & STK_PROPERTY_FUNCTION){\
			if(stk_element->properties & STK_PROPERTY_MEMBER_FUNCTION ){\
				StackMemberFunction *fm=(StackMemberFunction *)stk_element->value;\
				irfs=fm->so_function;\
				this_as_first_parameter=1;\
			}else{\
				irfs = (ScriptFunction *)stk_element->value;\
			}\
		}\
	}\
	if(irfs==NULL) continue;

	inline ScriptFunction * vm_find_function(
			VirtualMachine *vm
			,ScriptClass *class_obj // if NULL is MainClass
			,ScriptFunction *calling_function
			,Instruction * instruction // call instruction
			,bool is_constructor
			//,void *stk_elements_builtin_ptr // vector of properties
			//,int stk_elements_builtin_len // vector of properties
			,const std::string & symbol_to_find
			,StackElement *stk_arg
			,unsigned char n_args
		) {

		// by default search over global functions...
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		ScriptFunction * ptr_function_found=NULL;
		std::string aux_string;

		void *stk_elements_builtin_ptr= data->vm_stack;// vector of properties
		int stk_elements_builtin_len=  data->main_function_object->registered_symbols->count;// vector of properties

		if(class_obj != NULL){
			stk_elements_builtin_ptr=class_obj->symbol_members->items;
			stk_elements_builtin_len=class_obj->symbol_members->count;

		}
		bool stk_element_are_vector_element_ptr=stk_elements_builtin_ptr!=data->vm_stack;

		for(int i = stk_elements_builtin_len-1; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
			ScriptFunction *irfs = NULL;
			int this_as_first_parameter=0;

			EXTRACT_FUNCTION_INFO

			aux_string=irfs->symbol.name;

			if((aux_string == symbol_to_find && irfs->params->count == (n_args+this_as_first_parameter))){
				if((irfs->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF)){ /* C! Must match all args...*/
					bool all_check=true; /*  check arguments types ... */
					int idx_type=-1;
					int arg_idx_type=-1;
					for( unsigned k = 0; k < n_args && all_check;k++){
						StackElement *current_arg=&stk_arg[k];
						arg_idx_type=((ScriptFunctionArg *)irfs->params->items[k+this_as_first_parameter])->idx_type;

						if(arg_idx_type!=IDX_BUILTIN_TYPE_STACK_ELEMENT
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
								/*case STK_PROPERTY_NULL:
									all_check=false;
									break;*/
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
			int arg_idx_type=-1;
			/* get arguments... */

			for( unsigned k = 0; k < n_args;k++){
				StackElement *current_arg=&stk_arg[k];

				if(k>0){
					args_str+=",";
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
						aux_string=k_str_float_type;
						break;
					case STK_PROPERTY_BOOL:
						aux_string=k_str_bool_type;
						break;
					case STK_PROPERTY_NULL:
						aux_string="null";
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
				int this_as_first_parameter=0;
				ScriptFunction *irfs=NULL;

				EXTRACT_FUNCTION_INFO


				if(irfs->symbol.name == symbol_to_find){

					if(n_candidates == 0){
						str_candidates+="\tPossible candidates are:\n\n";
					}
					str_candidates+="\t\t-"+(class_obj==NULL?""
							:class_obj->idx_class!=IDX_BUILTIN_TYPE_MAIN?(class_obj->symbol_class.name+"::")
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
								class_obj==NULL?"":class_obj->idx_class!=IDX_BUILTIN_TYPE_MAIN?(class_obj->symbol_class.name+"::").c_str():"",
								symbol_to_find.c_str(),//calling_function->getInstructionSymbolName(instruction),
						args_str.c_str()
				);

				return NULL;
			}
			else{
				VM_ERROR("Cannot match %s \"%s%s(%s)\" .\n\n%s",
					is_constructor ? "constructor":"function",
							class_obj==NULL?"":class_obj->idx_class!=IDX_BUILTIN_TYPE_MAIN?(class_obj->symbol_class.name+"::").c_str():"",
									symbol_to_find.c_str(),//calling_function->getInstructionSymbolName(instruction),
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

				if(
						(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT)
					&&	(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT)
				){
					ScriptObject *obj1=(ScriptObject *)stk_result_op1->value;
					ScriptObject *obj2=(ScriptObject *)stk_result_op2->value;

					if(   obj1->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT
					   && obj2->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT
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
		stk_vm_current_backup=data->stk_vm_current;
		stk_args=data->stk_vm_current;

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
			//list_props=script_object->getStkBuiltinListElements();//getFunctions();

			if((ptr_function_found = vm_find_function(
				vm
				,data->script_class_factory->getScriptClass(script_object->idx_script_class)
				,calling_function
				,instruction
				,false
				//,(void *)list_props->items
				//,list_props->count
				,str_symbol_metamethod
				,stk_args // +1 because we include all parameters (include object, i.e 1st parameter)
				,n_stk_args
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
					,stk_args // +1 because we include all parameters (include object, i.e 1st parameter)
					,n_stk_args
					,calling_function
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

		return data->vm_error == false;

apply_metamethod_error:


		if(stk_result_op1!=NULL && stk_result_op2!=NULL){
			VM_ERROR("cannot perform operation \"%s %s %s\". %s"
				,stk_result_op1->toString().c_str()
				,byte_code_metamethod_to_operator_str(byte_code_metamethod)
				,stk_result_op2->toString().c_str()
				,error_found.c_str()
				);

		}else if(stk_result_op1!=NULL){
			VM_ERROR("cannot perform operation \"%s %s\". %s"
				,byte_code_metamethod_to_operator_str(byte_code_metamethod)
				,stk_result_op1->toString().c_str()
				,error_found.c_str()
			);

		}else{
			VM_ERROR("cannot perform operation \"%s\". %s"
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
		ScriptFunction *sf_iter=NULL;
		StackElement *stk_sf_iter;

		// stk_op2 expects to be obj with container

		if((stk_result_op2->properties & STK_PROPERTY_PTR_STK) == false){
			VM_ERROR("internal: Expected stk");
			return;
		}

		if((stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT) == false){
			//VM_ERROR("internal: Expected object");
			if((data->stk_vm_current->properties & STK_PROPERTY_SCRIPT_OBJECT) == false){
				VM_ERROR("Variable '%s' it doesn't implements iterator",SFI_GET_SYMBOL_NAME(calling_function,instruction));
				return;
			}
		}

		stk_result_op2 = (StackElement *)(stk_result_op2->value);
		ScriptObject *obj=(ScriptObject *)stk_result_op1->value;

		stk_sf_iter=obj->getProperty("iter",NULL);

		if(stk_sf_iter != NULL){

			if(stk_sf_iter->properties & (STK_PROPERTY_FUNCTION | STK_PROPERTY_MEMBER_FUNCTION)){
				StackMemberFunction *smf=(StackMemberFunction *)stk_sf_iter->value;
				ScriptObject *so_object=smf->so_object;
				StackElement *stk_start=data->stk_vm_current;
				int n_args=0;

				if(smf->so_function->symbol.properties & SYMBOL_PROPERTY_STATIC){
					n_args=1;

					// only stores and not increment (++ ) in order to start the stk arg
					*data->stk_vm_current={so_object,STK_PROPERTY_SCRIPT_OBJECT};
					so_object=NULL;
				}

				VM_INNER_CALL(
						so_object
						,smf->so_function
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
					VM_ERROR("Expected IteratorObject returned by 'iter' but it was '%s'",data->stk_vm_current->toString().c_str());
					return;
				}

				obj=(ScriptObject *)data->stk_vm_current->value;

				// check all functions...
				if((obj->getProperty("get",NULL))==NULL){
					VM_ERROR("IteratorObject '%s' does not implement 'get' function",obj->getClassName().c_str());
					return;
				}

				if((obj->getProperty("_next",NULL))==NULL){
					VM_ERROR("IteratorObject '%s' does not implement '_next' function",obj->getClassName().c_str());
					return;
				}

				if((obj->getProperty("end",NULL))==NULL){
					VM_ERROR("IteratorObject '%s' does not implement 'end' function",obj->getClassName().c_str());
					return;
				}

				// everything allright store and share pointer
				*stk_result_op2=*data->stk_vm_current;

				//vm_share_pointer(vm,obj);

			}else{
				VM_ERROR("Symbol 'iter' is not a function",obj->getClassName().c_str());
			}
		}
		else{
			VM_ERROR("Object not implements 'iter' ",obj->getClassName().c_str());
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
		std::string error="";
		VirtualMachineData *data=(VirtualMachineData *)vm->data;

		if(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so_aux=(ScriptObject *)stk_result_op2->value;

			switch(so_aux->idx_script_class){
			case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING: // check whether 'char' or 'string' exists
			if(stk_result_op1->properties & STK_PROPERTY_ZS_INT){
				PUSH_BOOLEAN(
					ScriptObjectStringWrap_contains(
						((ScriptObjectString *)so_aux)
						,(zs_int)stk_result_op1->value
					)
				);
			}else if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)){
				std::string str_op1=((ScriptObjectString *)stk_result_op1->value)->toString();
				PUSH_BOOLEAN(
					ScriptObjectStringWrap_contains(
						(ScriptObjectString *)so_aux
						,&str_op1)
				);
			}else{
				error="operand is not 'zs_int' or 'ScriptObjectString' type";
			}
			break;
			case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR: // check whether value exists...
			//PUSH_BOOLEAN(((ScriptObjectVector *)so_aux)->exists(stk_result_op1));
			PUSH_BOOLEAN(
				ScriptObjectVectorWrap_contains(
					(ScriptObjectVector *)so_aux,stk_result_op1
				)
			);
			break;
			case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT: // check key value exists...
			 if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
				std::string str_op1=((ScriptObjectString *)stk_result_op1->value)->toString();
				PUSH_BOOLEAN(
					ScriptObjectObjectWrap_contains(
						(ScriptObjectObject *)so_aux,&str_op1
					)
				);

			 }else{
			 }
			break;
			default:
				// TODO:
				if(vm_apply_metamethod(
						vm,
						calling_function,
						instruction,
						BYTE_CODE_METAMETHOD_IN,
						stk_result_op2,
						stk_result_op1
				)==false){
					return false;
				}
				//PUSH_BOOLEAN(false);
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

