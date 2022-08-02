/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define IDX_CALL_STACK_MAIN 1


// explains whether stk is this or not. Warning should be given as value and not as ptr
//#define IS_STK_THIS(stk) (this_object != NULL && (stk)->value == (zs_int)(this_object))

#define PRINT_DUAL_ERROR_OP(c)\
zs_string var_type1=stk_to_typeof_str(data->zs,stk_result_op1),\
	   var_type2=stk_to_typeof_str(data->zs,stk_result_op2);\
\
	VM_ERROR("cannot perform operator '%s' %s '%s'. Check whether op1 and op2 are same type, or type implements the metamethod",\
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

#define VM_INNER_CALL(_so_object,_so_function,_n_args,_name,_reset)\
	vm_inner_call_internal( \
			vm \
			,calling_function \
			,instruction \
			,_so_object \
			,_so_function \
			,_n_args \
			,_reset \
	);\
	if(data->vm_error){\
		sprintf(data->vm_str_aux[0]\
			,"\nat %s (file:%s line:%i)" /* TODO: get full symbol ? */ \
			, "iter" \
			,SFI_GET_FILE(calling_function,instruction) \
			,SFI_GET_LINE(calling_function,instruction) \
		);\
		strcat(data->vm_error_callstack_str,data->vm_str_aux[0]);\
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
		 char 				vm_error_file[1024];
		 char				vm_str_aux[3][1024]; // todo str intermediate operations
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
			//memset(&zero_shares,0,sizeof(zero_shares));
			//memset(&shared_vars,0,sizeof(shared_vars));
			memset(&vm_stack,0,sizeof(vm_stack));

			//vm_idx_call=0;
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
			,unsigned char _n_args
			,bool _reset

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

			return;
			/*if(goto_on_error){
				goto lbl_exit_function;
			}*/
		}
		n_returned_args_afun=data->stk_vm_current-stk_def_afun_start;
		/* we share pointer (true second arg) to not remove on pop in calling return */
		CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_def_afun_start,n_returned_args_afun,false)
		/* reset stack */
		if(_reset){
			data->stk_vm_current=stk_def_afun_start;
		}

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


#define EXTRACT_FUNCTION_INFO\
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

	inline ScriptFunction * vm_find_native_function(
			VirtualMachine *vm
			,ScriptType *class_obj // if NULL is MainClass
			,ScriptFunction *calling_function
			,Instruction * instruction // call instruction
			,bool is_constructor
			,const zs_string & symbol_to_find
			,StackElement *stk_arg
			,unsigned char n_args
		) {

		// by default search over global functions...
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		ScriptFunction * ptr_function_found=NULL;
		zs_string aux_string;
		int start_param=0;

		Symbol ** stk_elements_builtin_ptr= data->main_function_object->scope_script_function->symbol_functions->items;// vector of symbols
		size_t stk_elements_builtin_len=  data->main_function_object->scope_script_function->symbol_functions->count;// vector of symbols

		if(class_obj != NULL){
			stk_elements_builtin_ptr=class_obj->scope_script_type->symbol_functions->items;
			stk_elements_builtin_len=class_obj->scope_script_type->symbol_functions->count;
		}

		for(int i = (int)(stk_elements_builtin_len-1); i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
			ScriptFunction *irfs = NULL;
			start_param=1;

			EXTRACT_FUNCTION_INFO

			aux_string=irfs->name_script_function;

			bool symbol_equals=aux_string == symbol_to_find;

			if((symbol_equals && ((int)irfs->params_len == (n_args+start_param)))){
				// Only check native functions
				if((irfs->properties & FUNCTION_PROPERTY_C_OBJECT_REF)){ /* C! Must match all args...*/
					bool all_check=true; /*  check arguments types ... */
					int arg_idx_script_type=-1;

					for( unsigned k = 0; k < n_args && all_check;k++){ // ignore first parameter due expects zetscript
						StackElement *current_arg=&stk_arg[k];
						arg_idx_script_type=irfs->params[k+start_param].idx_script_type;

						if(arg_idx_script_type!=IDX_TYPE_STACK_ELEMENT){
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
									all_check=
											arg_idx_script_type==IDX_TYPE_ZS_INT_PTR_C
										  ||arg_idx_script_type==IDX_TYPE_ZS_INT_C
										  ||arg_idx_script_type==IDX_TYPE_ZS_FLOAT_PTR_C;
									break;
								case STK_PROPERTY_ZS_FLOAT:
									all_check=arg_idx_script_type==IDX_TYPE_ZS_FLOAT_PTR_C
											||arg_idx_script_type==IDX_TYPE_ZS_FLOAT_C
											||arg_idx_script_type==IDX_TYPE_ZS_INT_PTR_C
										    ||arg_idx_script_type==IDX_TYPE_ZS_INT_C;
									break;
								case STK_PROPERTY_BOOL:
									all_check=arg_idx_script_type==IDX_TYPE_BOOL_PTR_C;
									break;
								case STK_PROPERTY_FUNCTION|STK_PROPERTY_MEMBER_FUNCTION:
									all_check=arg_idx_script_type==IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER;
									break;
								case STK_PROPERTY_FUNCTION:
									all_check=arg_idx_script_type==IDX_TYPE_FUNCTION;
									break;
								// decoment to not allow nulls
								/*case STK_PROPERTY_NULL:
									all_check=false;
									break;*/
								case STK_PROPERTY_SCRIPT_OBJECT:

									if(STK_IS_SCRIPT_OBJECT_STRING(current_arg)){
										all_check=arg_idx_script_type==IDX_TYPE_SCRIPT_OBJECT_STRING; // if string object --> direct

										if(all_check==false){ // try native conversions
											all_check =
												(	arg_idx_script_type==IDX_TYPE_ZS_STRING_PTR_C && current_arg->value!=0)
											  ||	arg_idx_script_type==IDX_TYPE_CONST_CHAR_PTR_C;
										}
									}else{
										ScriptObject *var_object = NULL;
										var_object=((ScriptObject *)current_arg->value);
										aux_string=var_object->getTypeName();
										all_check=var_object->getScriptType()->extendsFrom(arg_idx_script_type);
									}
									break;
							}
						}
					}

					if(all_check){ /* we found the right function (set it up!) ... */
						ptr_function_found = irfs;
					}
				}

			}
		}

		if(ptr_function_found == NULL){
			zs_string class_str=class_obj==NULL?"":class_obj->idx_script_type!=IDX_TYPE_CLASS_MAIN?class_obj->str_script_type:"";
			int n_candidates=0;
			zs_string str_candidates="";
			zs_string function_name_not_found=class_str==""?symbol_to_find.c_str():zs_strutils::format("%s::%s",class_str.c_str(),symbol_to_find.c_str());
			zs_string args_str = "";
			/* get arguments... */
			for( unsigned k = 0; k < n_args;k++){
				StackElement *current_arg=&stk_arg[k];

				if(k>0){
					args_str.append(",");
				}

				//unsigned short var_type = GET_STK_PROPERTY_TYPES(current_arg->properties);
				if(current_arg->properties & STK_PROPERTY_PTR_STK){
					current_arg=(StackElement *)current_arg->value;
				}


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
				case STK_PROPERTY_UNDEFINED:
					aux_string="undefined";
					break;
				case STK_PROPERTY_NULL:
					aux_string="null";
					break;
				case STK_PROPERTY_SCRIPT_OBJECT:
					aux_string = ((ScriptObject *)current_arg->value)->getScriptType()->str_script_type_ptr;
					break;
				}

				args_str.append(zs_rtti::demangle(aux_string.c_str()));

				if(current_arg->properties == STK_PROPERTY_ZS_INT
				||current_arg->properties == STK_PROPERTY_ZS_FLOAT
				||current_arg->properties == STK_PROPERTY_BOOL
				){
					args_str.append(" [*] ");
				}
			}

			for(int i = (int)(stk_elements_builtin_len-1); i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
				start_param=1;
				ScriptFunction *irfs=NULL;

				EXTRACT_FUNCTION_INFO


				if((irfs->name_script_function == symbol_to_find) && (irfs->properties & FUNCTION_PROPERTY_C_OBJECT_REF)){

					if(n_candidates == 0){
						str_candidates.append("\tPossible candidates are:\n\n");
					}
					str_candidates.append("\t\t-");

					// type if not main
					if(class_obj!=NULL && class_obj->idx_script_type!=IDX_TYPE_CLASS_MAIN){
						str_candidates.append(class_obj->str_script_type.c_str());
						str_candidates.append("::");
					}

					// writes script function interface
					// function name
					str_candidates.append(irfs->name_script_function.c_str());
					str_candidates.append("(");

					for(int a = 2; a < irfs->params_len; a++){
						if(a>2){
							str_candidates.append(",");
						}

						str_candidates+=zs_rtti::demangle(
							GET_IDX_2_CLASS_C_STR(data->script_type_factory,irfs->params[a].idx_script_type)
						);
					}

					// writes wrap

					str_candidates.append(")");

					str_candidates.append(" -- BIND FUNCTION AS --> ");

					str_candidates.append(zs_rtti::demangle(
							GET_IDX_2_CLASS_C_STR(data->script_type_factory,irfs->idx_script_type_return)
						)
					);

					str_candidates.append(" (*)");
					str_candidates.append("(");

					for(int a = 0; a < irfs->params_len; a++){
						if(a>0){
							str_candidates.append(",");
						}

						str_candidates.append(zs_rtti::demangle(
							GET_IDX_2_CLASS_C_STR(data->script_type_factory,irfs->params[a].idx_script_type)
							)
						);
					}
					str_candidates.append(")\n");

					n_candidates++;
				}
			}

			if(n_candidates == 0){
				VM_ERROR("Cannot call native %s '%s(%s)'. Function not registered\n\n"
					,is_constructor ? "constructor":class_str==""?"function":"member function"
					,function_name_not_found.c_str()
					,args_str.c_str()
				);

				return NULL;
			}
			else{
				VM_ERROR("Cannot call native %s '%s(%s)'\n\n%s"
					,is_constructor ? "constructor":class_str==""?"function":"member function"
					,function_name_not_found.c_str()
					,args_str.c_str()
					,str_candidates.c_str()
				);
				return NULL;
			}
		}

		return ptr_function_found;
	}

	inline bool vm_call_metamethod(
		VirtualMachine *vm
		,ScriptFunction *calling_function
		,Instruction *instruction
		,ByteCodeMetamethod byte_code_metamethod
		,StackElement *stk_result_op1
		,StackElement *stk_result_op2
		, bool is_static=true
		, bool is_je_case=false
	
	) {
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		zs_string str_stk_result_op1_full_definition="";
		zs_string str_stk_result_op2_full_definition="";
		StackElement *stk_vm_current_backup,*stk_args;
		ScriptFunction *ptr_function_found=NULL;
		StackElement ret_obj;
		const char *byte_code_metamethod_operator_str=byte_code_metamethod_to_operator_str(byte_code_metamethod);
		const char *str_symbol_metamethod=byte_code_metamethod_to_symbol_str(byte_code_metamethod);
		zs_string error_found="";
		ScriptObject *script_object=NULL;
		zs_string str_script_type_object_found="";
		int n_stk_args=is_static?2:1;
		size_t n_stk_local_symbols=0;
		StackElement *stk_return=NULL;
		int n_returned_arguments_from_function=0;

		ret_obj.setUndefined();

		// init stk
		stk_vm_current_backup=data->stk_vm_current;
		stk_args=data->stk_vm_current;

		// op1/op2 should be the object that have the metamethod
		if((stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT)){
			script_object=(ScriptObject *)stk_result_op1->value;
			str_script_type_object_found=script_object->getTypeName();
		}else if ((is_static==true) && (stk_result_op2!=NULL && (stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT))){
			script_object=(ScriptObject *)stk_result_op2->value;
			str_script_type_object_found=script_object->getTypeName();

		}


		if(script_object == NULL){ // cannot perform operation
			if(str_script_type_object_found.empty()){ // not any object found
				// Because script elements can return "null" due undefined properties, do not show any error to not confuse.
				// If is an internal error, fix!
			}else{
				if(instruction->byte_code == BYTE_CODE_JE_CASE){
					error_found=zs_strutils::format("Unable to perform '==' operator for case conditional");
				}else{
					error_found=zs_strutils::format("Type '%s' does not implements metamethod '%s'"
						,str_script_type_object_found.c_str()
						,byte_code_metamethod_to_symbol_str(byte_code_metamethod)
					);
				}
			}
			goto apply_metamethod_error;
		}

		//------------------------------------
		// push stk results...
		if(is_static == true){
			*data->stk_vm_current++=*stk_result_op1;
			*data->stk_vm_current++=*stk_result_op2;
		}else{
			*data->stk_vm_current++=*stk_result_op2;
		}
		//------------------------------------


		if(script_object->isNativeObject()){ // because isNativeObject it can have more than one setter
			if((ptr_function_found = vm_find_native_function(
				vm
				,data->script_type_factory->getScriptType(script_object->idx_script_type)
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

			if(is_static && ((sf->properties & FUNCTION_PROPERTY_STATIC) == 0)){
				error_found=zs_strutils::format("Operator metamethod '%s (aka %s)' is not a static function (i.e add 'static' keyword )",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}

			if((is_static==false) && ((sf->properties & FUNCTION_PROPERTY_STATIC) != 0)){
				error_found=zs_strutils::format("Operator metamethod '%s (aka %s)' is static function and should be a member function (i.e remove 'static' keyword)",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}


			ptr_function_found=(ScriptFunction *)symbol->ref_ptr;

		}


		if((ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){
			// we have to share any object to avoid be removed on function exit

			if(is_static == true){ // the share of the 1st object if static and object
				if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
					if(vm_share_script_object(vm,(ScriptObject *)stk_result_op1->value)==false){
						goto apply_metamethod_error;
					}
				}
			}

			if(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT){
				if(vm_share_script_object(vm,(ScriptObject *)stk_result_op2->value)==false){
					goto apply_metamethod_error;
				}
			}

			vm_execute_function_script(
				vm
				,is_static?NULL:script_object // do not pass script_object as this on static function, it will be destroyed on pop function
				,ptr_function_found
				,stk_args
			);

			n_stk_local_symbols=ptr_function_found->local_variables->count;
		}else{ //
			vm_execute_function_native(
					vm
					,calling_function
					,instruction
					,is_static ? NULL:script_object
					,ptr_function_found
					,stk_args
					,n_stk_args
			);

			n_stk_local_symbols=n_stk_args;
		}

		stk_return=(stk_args+n_stk_local_symbols );
		n_returned_arguments_from_function=data->stk_vm_current-stk_return;

		if(n_returned_arguments_from_function==0){
			error_found="Metamethod function should return a value";
			goto apply_metamethod_error;
		}

		// setup all returned variables from function
		for(int i=0; i < n_returned_arguments_from_function; i++){

			StackElement *stk_ret = --data->stk_vm_current;

			// if a scriptvar --> init shared
			if(stk_ret->properties & STK_PROPERTY_SCRIPT_OBJECT){
				ScriptObjectClass *sv=(ScriptObjectClass *)stk_ret->value;

				// Auto destroy always C when ref == 0
				sv->deleteNativeObjectOnDestroy(true);

				if(sv->shared_pointer == NULL){ // if return this, it holds ptr_shared_pointer
					if(!vm_create_shared_script_object(vm,sv)){
						goto apply_metamethod_error;
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

		if(is_je_case){
			VM_ERROR("Error evaluating case for variable as type '%s': %s"
				,stk_to_typeof_str(data->vm_str_aux[0],data->zs,stk_result_op1)
				,error_found.c_str()
			);
		}else{

			if(stk_result_op2 != NULL){
				VM_ERROR("Metamethod operation '%s' (aka %s). Failed performing operation by types '%s' %s '%s'%s %s"
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,byte_code_metamethod_to_symbol_str(byte_code_metamethod)
					,stk_to_typeof_str(data->vm_str_aux[0],data->zs,stk_result_op1)
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,stk_to_typeof_str(data->vm_str_aux[1],data->zs,stk_result_op2)
					,error_found.empty()?"":":"
					,error_found.c_str()
				);
			}else{
				VM_ERROR("Metamethod operation '%s' (aka %s). Failed performing operation by types %s '%s' %s %s"
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,byte_code_metamethod_to_symbol_str(byte_code_metamethod)
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,stk_to_typeof_str(data->vm_str_aux[0],data->zs,stk_result_op1)
					,error_found.empty()?"":":"
					,error_found.c_str()
				);
			}
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
			VM_ERRORF("internal: Expected stk");
			return;
		}

		if((stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT) == 0){
			//VM_ERROR("internal: Expected object");
			if((data->stk_vm_current->properties & STK_PROPERTY_SCRIPT_OBJECT) == 0){
				VM_ERROR("Variable '%s' as type '%s' it doesn't implements iterator"
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)
					,stk_to_str(data->vm_str_aux[0],data->zs,data->stk_vm_current)
				);
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

			VM_INNER_CALL(so_object,so_function,n_args,"iter",true);

			// ok stk_vm_current holds the iter object
			if((data->stk_vm_current->properties & STK_PROPERTY_SCRIPT_OBJECT) == false){
				VM_ERROR("Expected IteratorObject returned by 'iter' but it was '%s'"
						,stk_to_typeof_str(data->vm_str_aux[0],data->zs,data->stk_vm_current));
				return;
			}



			obj=(ScriptObject *)data->stk_vm_current->value;

			if(!vm_share_script_object(vm,obj)){\
				goto lbl_exit_function;\
			}\



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
			VM_ERROR("Object '%s' not implements 'iter'",obj->getTypeName().c_str());
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

			switch(so_aux->idx_script_type){
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
				if(vm_call_metamethod(
						vm,
						calling_function,
						instruction,
						BYTE_CODE_METAMETHOD_IN,
						stk_result_op2,
						stk_result_op1,
						false
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

