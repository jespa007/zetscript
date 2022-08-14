/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	struct VM_Foreach{
		StackElement 		   	*key; // iterator element can be zs_string or integer...
		ScriptObjectObject		*ptr; // can be struct or zs_vector...
		unsigned int 		   	idx_current;

	};


	void vm_do_stack_dump(VirtualMachine *_vm){
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


	void vm_init(VirtualMachine *vm,ZetScript *_zs){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;

		// script type factory should be created and initialized
		data->script_function_factory=_zs->getScriptFunctionFactory();
		data->script_type_factory=_zs->getScriptTypeFactory();
		data->scope_factory = _zs->getScopeFactory();
		data->main_function_object = MAIN_FUNCTION(data);
		data->main_class_object = SCRIPT_TYPE_MAIN(data->script_type_factory);
	}

	//============================================================================================================================================
	// POINTER MANANAGER
	bool vm_create_shared_script_object(VirtualMachine *vm,ScriptObject *_obj){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(_obj->shared_pointer == NULL){
			InfoSharedPointerNode *_node = (InfoSharedPointerNode *)ZS_MALLOC(sizeof(InfoSharedPointerNode));
			// init
			_node->previous=NULL;
			_node->next=NULL;
			_node->data.n_shares=0;
			_node->data.ptr_script_object_shared=_obj;
			_node->data.created_scope_block=VM_CURRENT_SCOPE_BLOCK;//data->vm_idx_call; // it saves the zeros nodes where was set

			// insert node into shared nodes ...
			if(!vm_insert_shared_node(vm,&_node->data.created_scope_block->unreferenced_objects,_node)){
				return false;
			}
			_obj->shared_pointer=_node;
			return true;
		}else{
			VM_SET_USER_ERRORF(vm," shared ptr already has a shared pointer data");
		}

		return false;

	}

	bool vm_share_script_object(VirtualMachine *vm,ScriptObject *_obj){
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
			if(!vm_deattach_shared_node(vm,&_node->data.created_scope_block->unreferenced_objects,_node)){
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


	const ScriptFunction * vm_get_current_native_function_call(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->current_call_c_function;
	}

	void vm_set_error(VirtualMachine *vm,const zs_string & str){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		data->vm_error = true;
		strcpy(data->vm_error_str,str.c_str());
		memset(data->vm_error_file,0,sizeof(data->vm_error_file));
		data->vm_error_line=-1;
	}

	bool vm_it_has_error(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->vm_error;
	}

	void vm_set_error_file_line(VirtualMachine *vm, const char *file, int line, const char *in_txt,...){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;

		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		vm_set_error(vm,zs_strutils::format("[%s:%i] %s",file,line,out_txt));

		strcpy(data->vm_error_file,file);
		data->vm_error_line=line;
	}

	zs_string vm_get_error(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->vm_error_str;
	}



	void vm_insert_lifetime_object(VirtualMachine *vm, const char *file, int line, ScriptObject *script_object){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		InfoLifetimeObject *info = (InfoLifetimeObject *)ZS_MALLOC(sizeof(InfoLifetimeObject));

		info->file=file;
		info->line=line;
		info->script_object=script_object;

		if(script_object->shared_pointer != NULL){
			if(script_object->shared_pointer->data.n_shares > 0){
				script_object->shared_pointer->data.n_shares++;
			}
		}

		data->lifetime_object.push_back(info);
	}

	int vm_find_lifetime_object(VirtualMachine *vm,ScriptObject *script_object){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		for(int i=0; i < data->lifetime_object.count; i++){
			InfoLifetimeObject *info=(InfoLifetimeObject *)data->lifetime_object.items[i];
			if(info->script_object==script_object){
				return i;
			}
		}

		return ZS_IDX_UNDEFINED;
	}

	void vm_unref_lifetime_object(VirtualMachine *vm,ScriptObject *script_object){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		int idx = ZS_IDX_UNDEFINED;
		if((idx=vm_find_lifetime_object(vm,script_object))==ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERRORF("Cannot find stack element lifetime");
		}
		InfoLifetimeObject *info=(InfoLifetimeObject *)data->lifetime_object.items[idx];
		data->lifetime_object.erase(idx);

		if(info->script_object->shared_pointer!=NULL){
			vm_unref_shared_script_object(vm,script_object,NULL);
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
		if(((data->stk_vm_current-data->vm_stack)+1)>=VM_STACK_MAX){

			vm_set_error(vm,"Error MAXIMUM stack size reached");
			return;
		}
		*data->stk_vm_current++=stk;
	}

	bool vm_set_stack_element_at(VirtualMachine *vm,unsigned int idx, StackElement stk){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(idx >= VM_STACK_MAX){
			VM_SET_USER_ERRORF(vm,"setStackElement: out of bounds");
			return false;
		}

		data->vm_stack[idx]=stk;
		return true;
	}

	/*int	vm_get_idx_call(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->vm_idx_call;
	}*/

	VM_ScopeBlock  *vm_get_scope_block_main(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return &data->vm_scope_function[0].scope_block[0];
	}

	StackElement *vm_get_current_stack_element(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return data->stk_vm_current;
	}

	StackElement * vm_get_stack_element_at(VirtualMachine *vm, int idx_glb_element){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(idx_glb_element < data->main_function_object->local_variables->count){
			return &data->vm_stack[idx_glb_element];
		}else{
			VM_SET_USER_ERRORF(vm,"getStackElement: out of bounds");
		}
		return NULL;
	}

	/*StackElement * 	vm_get_top_stack_element_from_stack(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		return (data->stk_vm_current);
	}*/

	void vm_reset_error_vars(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;

		data->vm_error_max_stack_reached=false;
		data->vm_error=false;
		memset(data->vm_error_str,0,sizeof(data->vm_error_str));
		memset(data->vm_error_file,0,sizeof(data->vm_error_file));
		data->vm_error_line=-1;
		memset(data->vm_error_callstack_str,0,sizeof(data->vm_error_callstack_str));
	}

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

		// in case user binds script function and calls directy from C code, idx_call will 0 so we have
		// to prevent conflict with first call that is reserved by main_function
		bool first_script_call_from_c=false;
		StackElement stk_return=k_stk_undefined;
		StackElement *stk_start=NULL;


		if(
			calling_function->idx_script_function==IDX_SCRIPT_FUNCTION_MAIN
			&& ((properties & VM_PROPERTY_CALL_FROM_NATIVE) == 0)
		){ // set stack and Init vars for first call...


			if(data->vm_current_scope_function != VM_SCOPE_FUNCTION_MAIN){
				THROW_RUNTIME_ERROR("Internal: vm_idx_call != 0 (%i)",IDX_VM_CURRENT_SCOPE_FUNCTION);
			}

			vm_reset_error_vars(vm);

			stk_start=data->vm_stack;
			n_stk_params=(char)data->main_function_object->local_variables->count;

		}else{ // Not main function -> allow params for other functions
			// push param stack elements...
			stk_start=data->stk_vm_current;
			StackElement *min_stk=&data->vm_stack[data->main_function_object->local_variables->count];
			first_script_call_from_c=false;

			if(data->vm_current_scope_function == VM_SCOPE_FUNCTION_MAIN){

				if((properties & VM_PROPERTY_CALL_FROM_NATIVE)==0){
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
				if(stk_params[i].properties & STK_PROPERTY_SCRIPT_OBJECT){
					ScriptObject *so=(ScriptObject *)stk_params[i].value;

					if(so->shared_pointer==NULL){
						if(vm_create_shared_script_object(vm,so) == false){
							THROW_RUNTIME_ERRORF("Cannot create shared pointer");
						}
					}

					if(vm_share_script_object(vm,so) == false){
						THROW_RUNTIME_ERRORF("Cannot shared pointer");
					}

				}
				*stk_it++=stk_params[i];
			}
		}

		// byte code executing starts here. Later script function can call c++ function, but once in c++ function is not possible by now call script function again.
		vm_execute_function_script(
			vm,
			this_object,
			calling_function,
			stk_start
		);

		// remove empty shared pointers
		if(data->vm_current_scope_function == VM_SCOPE_FUNCTION_MAIN){
			vm_remove_empty_shared_pointers(vm,vm_get_scope_block_main(vm));
		}

		// get number return elements
		if(data->vm_error){
			// set error as false
			// it was error so reset stack and stop execution ...
			vm_do_stack_dump(vm);

			zs_string total_error=data->vm_error_str;
			total_error.append(data->vm_error_callstack_str);

			throw_exception_file_line(data->vm_error_file,data->vm_error_line,total_error.c_str());
		}else{
			int n_returned_arguments_from_function=data->stk_vm_current-(stk_start+calling_function->local_variables->count);

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
		data->stk_vm_current=stk_start;

		// restore idx_call as 0 if first call was a script function called from C
		if(first_script_call_from_c==true){
			data->vm_current_scope_function=VM_SCOPE_FUNCTION_MAIN;
		}

		return stk_return;
	}

	ScriptFunction * vm_find_native_function(
			VirtualMachine *vm
			,ScriptType *class_obj // if NULL is MainClass
			,ScriptFunction *calling_function
			,Instruction * instruction // call instruction
			,bool is_constructor
			,const char * symbol_to_find
			,StackElement *stk_arg
			,unsigned char n_args
	){

		// by default search over global functions...
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		ScriptFunction * ptr_function_found=NULL;
		const char * aux_string;
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

			VM_EXTRACT_FUNCTION_INFO

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
			zs_string function_name_not_found=class_str==""?symbol_to_find:zs_strutils::format("%s::%s",class_str.c_str(),symbol_to_find);
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

				args_str.append(zs_rtti::demangle(aux_string));

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

				VM_EXTRACT_FUNCTION_INFO

				if((irfs->name_script_function == symbol_to_find) && (irfs->properties & FUNCTION_PROPERTY_C_OBJECT_REF)){

					if(n_candidates == 0){
						str_candidates.append("\tPossible candidates are:\n\n");
					}
					str_candidates.append("\t\t-");

					// type if not main
					if(class_obj!=NULL && class_obj->idx_script_type!=IDX_TYPE_CLASS_MAIN){
						str_candidates.append(class_obj->str_script_type);
						str_candidates.append("::");
					}

					// writes script function interface
					// function name
					str_candidates.append(irfs->name_script_function);
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

	bool vm_call_metamethod(
		VirtualMachine *vm
		,ScriptFunction *calling_function
		,Instruction *instruction
		,ByteCodeMetamethod byte_code_metamethod
		,StackElement *stk_result_op1
		,StackElement *stk_result_op2
		, bool is_static
		, bool is_je_case

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
				,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1)
				,error_found.c_str()
			);
		}else{

			if(stk_result_op2 != NULL){
				VM_ERROR("Metamethod operation '%s' (aka %s). Failed performing operation by types '%s' %s '%s'%s %s"
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,byte_code_metamethod_to_symbol_str(byte_code_metamethod)
					,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1)
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,stk_to_typeof_str(VM_STR_AUX_PARAM_1,data->zs,stk_result_op2)
					,error_found.empty()?"":":"
					,error_found.c_str()
				);
			}else{
				VM_ERROR("Metamethod operation '%s' (aka %s). Failed performing operation by types %s '%s' %s %s"
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,byte_code_metamethod_to_symbol_str(byte_code_metamethod)
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1)
					,error_found.empty()?"":":"
					,error_found.c_str()
				);
			}
		}

		return false;
	}

	void vm_iterator_init(VirtualMachine *vm
			 ,ScriptFunction *calling_function
			,Instruction *instruction
			,StackElement *stk_result_op1
			,StackElement *stk_result_op2
	){

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
					,stk_to_str(VM_STR_AUX_PARAM_0,data->zs,data->stk_vm_current)
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

			VM_INNER_CALL(so_object,so_function,n_args,"iter");

			// ok stk_vm_current holds the iter object
			if((data->stk_vm_current->properties & STK_PROPERTY_SCRIPT_OBJECT) == false){
				VM_ERROR("Expected IteratorObject returned by 'iter' but it was '%s'"
						,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,data->stk_vm_current));
				return;
			}



			obj=(ScriptObject *)data->stk_vm_current->value;

			if(!vm_share_script_object(vm,obj)){\
				goto lbl_exit_function;\
			}\



			sc=obj->getScriptType();

			// check all functions...
			if(sc->getSymbolMemberFunction("get")==NULL){
				VM_ERROR("IteratorObject '%s' does not implement 'get' function",obj->getTypeName());
				return;
			}

			if(sc->getSymbolMemberFunction("_post_inc")==NULL){
				VM_ERROR("IteratorObject '%s' does not implement '_post_inc' function",obj->getTypeName());
				return;
			}

			if(sc->getSymbolMemberFunction("end")==NULL){
				VM_ERROR("IteratorObject '%s' does not implement 'end' function",obj->getTypeName());
				return;
			}

			// everything allright store and share pointer
			*stk_result_op2=*data->stk_vm_current;
		}
		else{
			VM_ERROR("Object '%s' not implements 'iter'",obj->getTypeName());
		}

		// get iterator...
lbl_exit_function:

		return;

	}

	bool vm_perform_in_operator(
			VirtualMachine *vm
			 ,ScriptFunction *calling_function
			,Instruction *instruction
			, StackElement *stk_result_op1
			, StackElement *stk_result_op2
	){
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



	bool vm_operation_store(
			VirtualMachine *vm
			,ScriptFunction *calling_function
			,Instruction *instruction
			,StackElement *stk_result_op1
			,StackElement *stk_result_op2
			, ByteCodeMetamethod byte_code_metamethod
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		MemberProperty *member_property=NULL;
		Symbol *symbol_setter=NULL;
		ScriptObject *so_aux=NULL;
		StackElement *stk_var=NULL;
		StackElement stk_aux1;
		StackMemberProperty *stk_mp_aux=NULL;
		MetamethodMembers *ptr_metamethod_members_aux=NULL;
		ScriptFunction *ptr_function_found=NULL;
		MetamethodMemberSetterInfo setter_info;
		const char *__STR_SET_METAMETHOD__=byte_code_metamethod_to_symbol_str(byte_code_metamethod);
		const char *__STR_AKA_SET_METAMETHOD__=byte_code_metamethod_to_operator_str(byte_code_metamethod);

		LOAD_PROPERTIES(byte_code_metamethod); /* saves __STK_VAR_COPY__ --> stk_vm_current points to stk_result_op2 that is the a parameter to pass */\
		setter_info=ptr_metamethod_members_aux->getSetterInfo(byte_code_metamethod);
		if(setter_info.setters->count==0){\
			METAMETHOD_OPERATION_NOT_FOUND(byte_code_metamethod); \
		}\
		ptr_function_found=(ScriptFunction *)((Symbol *)(((StackElement *)setter_info.setters->items[0])->value))->ref_ptr;\
		/* find function if c */ \
		if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){ /* because object is native, we can have more than one _setter */ \
			if((ptr_function_found=vm_find_native_function( \
				vm \
				,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
				,calling_function\
				,instruction\
				,false\
				,member_property==NULL?__STR_SET_METAMETHOD__:(ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(data->vm_str_metamethod_aux,byte_code_metamethod,member_property->property_name))  /* symbol to find */\
				,data->stk_vm_current \
				,1))==NULL\
			){ \
				if(member_property!=NULL){ \
					VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
							,so_aux->getScriptType()->str_script_type\
							,member_property->property_name\
							,__STR_SET_METAMETHOD__\
					);\
				}else{\
					VM_STOP_EXECUTE("Class '%s' does not implement '%s' metamethod" \
							,so_aux->getScriptType()->str_script_type \
							,__STR_SET_METAMETHOD__\
					);\
				}\
			}\
		}else if(setter_info.setters->count>1){\
			symbol_setter = so_aux->getScriptType()->getSymbol(__STR_SET_METAMETHOD__); \
			if(symbol_setter == NULL){\
				VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"\
						,__STR_SET_METAMETHOD__\
						,__STR_AKA_SET_METAMETHOD__\
				);\
			}\
			if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
				VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function",__STR_SET_METAMETHOD__,__STR_AKA_SET_METAMETHOD__);\
			}\
			ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
		}\
		/* call metamethod  */ \
		VM_INNER_CALL(\
			so_aux\
			,ptr_function_found\
			,1 \
			,ptr_function_found->name_script_function.c_str()\
		);\
		/*getter after*/\
		if(ptr_metamethod_members_aux->getter!=NULL){\
			/* call _neg */\
				VM_INNER_CALL(\
					so_aux\
					,(ScriptFunction *)ptr_metamethod_members_aux->getter->ref_ptr\
					,0 \
					,ptr_metamethod_members_aux->getter->name.c_str()\
			);\
		}else{ /* store object */ \
			if(stk_var->properties & STK_PROPERTY_SCRIPT_OBJECT){
				data->stk_vm_current->value=(zs_int)so_aux;\
				data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->stk_vm_current=__STK_VAR_COPY__;
			}
		}
		data->stk_vm_current++;\

		return true;

	lbl_exit_function:

		return false;
	}

	bool vm_store(
			VirtualMachine *vm
			,ScriptFunction *calling_function
			,Instruction *instruction
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		StackElement 				*		stk_dst=NULL,
								    *		stk_src=NULL;
		zs_vector<StackElement *> 	*		store_lst_setter_functions=NULL;
		int 								n_element_left_to_store=0;
		StackElement    			*		stk_load_multi_var_src=NULL;
		ContainerSlotStore 			*		container_slot_store=NULL;
		ScriptObject 				*		container_slot_store_object=NULL;
		zs_int 								container_slot_store_id_slot=0;
		void 						*		stk_src_ref_value_copy_aux=NULL;
		StackElement 				*		stk_result_op2=NULL;
		zs_int 						*		stk_src_ref_value=NULL;
		zs_int 						*		stk_dst_ref_value=NULL;
		ScriptObject 				*		so_aux=NULL;
		StackMemberProperty 		*		stk_mp_aux=NULL;
		ScriptFunction 				*		ptr_function_found=NULL;
		uint16_t 							stk_src_properties=0;


		 // n elements left
		n_element_left_to_store=(char)instruction->value_op1;

		// vm_current - n_element_left_to_store we have src values
		// do +1 is because it has to point to first the stack, due vm_stk_current points to new stk slot
		stk_load_multi_var_src=data->stk_vm_current-(n_element_left_to_store<<1); // it loads n_loads + n_vars to store 2xn_elements to store
		stk_dst=--data->stk_vm_current; // dst first read
		n_element_left_to_store--; // first read

vm_store_next:
		store_lst_setter_functions=NULL;
		container_slot_store=NULL;
		container_slot_store_object=NULL;
		container_slot_store_id_slot=0;
		stk_src=stk_load_multi_var_src; // store ptr instruction2 op as src_var_value
		//stk_dst=stk_result_op2;
		stk_result_op2=stk_dst;

		// check if by ref
		if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_dst)){
			stk_dst=(StackElement *)(STK_GET_STK_VAR_REF(stk_dst)->value);
		}

		//- check if ptr stk
		if((stk_dst->properties & STK_PROPERTY_PTR_STK)!=0) {
			stk_dst=(StackElement *)stk_dst->value; // value is expect to contents a stack variable
		}else if(stk_dst->properties & STK_PROPERTY_CONTAINER_SLOT_STORE){
			container_slot_store=((ContainerSlotStore *)stk_dst->value);
			container_slot_store_object=container_slot_store->object;
			container_slot_store_id_slot=container_slot_store->id_slot;
			stk_dst=container_slot_store->ptr_stk;
			delete container_slot_store;
			container_slot_store=NULL;
		}else{
			if((stk_dst->properties & STK_PROPERTY_IS_C_VAR_PTR)==0){
				VM_STOP_EXECUTE("Expected l-value on assignment but it was type '%s'"
					,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_dst)
				);
			}
		}

		//-----------------------
		if(stk_dst->properties & STK_PROPERTY_READ_ONLY){
			VM_STOP_EXECUTEF("Assignment to constant variable");
		}

		// store through metamethod
		store_lst_setter_functions=NULL;

		if(STK_IS_SCRIPT_OBJECT_CLASS(stk_dst)){
			if((store_lst_setter_functions=((ScriptObjectClass *)stk_dst->value)->getSetterList(BYTE_CODE_METAMETHOD_SET))!=NULL){

				if(store_lst_setter_functions->count == 0){
					store_lst_setter_functions=NULL;
				}
			}
		}else if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){
			stk_mp_aux=(StackMemberProperty *)stk_dst->value;\
			if(stk_mp_aux->member_property->metamethod_members.setters.count > 0){\
				store_lst_setter_functions=&stk_mp_aux->member_property->metamethod_members.setters;\
			}else{ // setter not allowed because it has no setter
				VM_STOP_EXECUTE("'%s' not implements operator '=' (aka '_set')"
					,stk_mp_aux->member_property->property_name
				);
			}
		}

		if(store_lst_setter_functions!=NULL){

			StackElement *stk_vm_start=data->stk_vm_current;\
			StackElement *stk_arg=stk_vm_start+1; //start from stk_src
			const char *__STR_SETTER_METAMETHOD__=byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_SET);\
			const char *__STR_AKA_SETTER_METAMETHOD__=byte_code_metamethod_to_operator_str(BYTE_CODE_METAMETHOD_SET);\
			*stk_arg=*stk_src;\
			if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){
				stk_mp_aux=(StackMemberProperty *)(stk_dst->value);
				so_aux=stk_mp_aux->so_object;\
			}else{
				so_aux=(ScriptObjectClass *)stk_dst->value;
			}
			ptr_function_found=(ScriptFunction *)(((Symbol *)(((StackElement *)(store_lst_setter_functions->items[0]))->value))->ref_ptr);\
			if(so_aux->isNativeObject()){ // because object is native, we can have more than one _setter
				if((ptr_function_found=vm_find_native_function( \
						vm \
						,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
						,calling_function\
						,instruction\
						,false\
						,stk_mp_aux==NULL?"_set":(ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(data->vm_str_metamethod_aux,BYTE_CODE_METAMETHOD_SET,stk_mp_aux->member_property->property_name)) // symbol to find
						,stk_arg \
						,1))==NULL){ \
					if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){ \
						VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
								,so_aux->getScriptType()->str_script_type\
								,stk_mp_aux->member_property->property_name\
								,__STR_SETTER_METAMETHOD__\
						);\
					}else{\
						VM_STOP_EXECUTE("Type '%s' does not implement '%s' metamethod" \
								,so_aux->getScriptType()->str_script_type \
								,__STR_SETTER_METAMETHOD__\
						);\
					}\
				}\
			}else if(store_lst_setter_functions->count>1){ // it has overrided metamethods
				Symbol * symbol_setter = so_aux->getScriptType()->getSymbol(__STR_SETTER_METAMETHOD__); \
				if(symbol_setter == NULL){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"\
							,__STR_SETTER_METAMETHOD__\
							,__STR_AKA_SETTER_METAMETHOD__\
					);\
				}\
				if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function",__STR_SETTER_METAMETHOD__,__STR_AKA_SETTER_METAMETHOD__);\
				}\
				ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
			}\
			if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){\
				vm_execute_function_native(\
						vm\
						,calling_function\
						,instruction\
						,so_aux\
						,ptr_function_found\
						,stk_arg\
						,1\
				);\
			}else{\
				vm_execute_function_script(\
					vm\
					,so_aux\
					,ptr_function_found\
					,stk_arg\
				);\
			}\
			data->stk_vm_current=stk_vm_start;

		}else{ // store through script assignment

			if((stk_src->properties & STK_PROPERTY_PTR_STK)!=0) {
				stk_src=(StackElement *)stk_src->value; // value is expect to contents a stack variable
			}


			// TODO: get stk_dst if STK_PROPERTY_SLOT
			StackElement old_stk_dst = *stk_dst; // save dst_var to check after assignment...

			stk_src_ref_value_copy_aux=NULL;//copy aux in case of the var is c and primitive (we have to update value on save)
			stk_src_ref_value=&stk_src->value;
			stk_dst_ref_value=&stk_dst->value;
			if(stk_src->properties & STK_PROPERTY_IS_C_VAR_PTR){ // src is C pointer
				stk_src_ref_value=(zs_int *)((stk_src)->value);
			}
			if(stk_dst->properties & STK_PROPERTY_IS_C_VAR_PTR){ // dst is a C pointer
				// particular case
				if(
						stk_dst->properties != stk_src->properties
					&& (((stk_dst->properties & STK_PROPERTY_ZS_CHAR) && (stk_src->properties & STK_PROPERTY_ZS_INT))==0)){

					 if(stk_dst->properties != stk_src->properties){
						if(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_dst->properties) != GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_src->properties)
						){
							// check particular case
							VM_STOP_EXECUTE("Symbol '%s': different types! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals",SFI_GET_SYMBOL_NAME(calling_function,instruction));
						}else{ // is object
							VM_STOP_EXECUTEF("Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator (aka set metamethod) in order to perform assign operation");
						}
					}
				}
				stk_dst_ref_value=(zs_int *)((stk_dst)->value);
				stk_src_ref_value_copy_aux=&((stk_dst)->value);
			}
			stk_src_properties=stk_src->properties;

			// init stk_dst
			STK_SET_UNDEFINED(stk_dst);
			if(stk_src_properties == STK_PROPERTY_UNDEFINED){
				stk_dst->properties=STK_PROPERTY_UNDEFINED;
			}else if(stk_src_properties == STK_PROPERTY_NULL){
				stk_dst->properties=STK_PROPERTY_NULL;
			}else if(stk_src_properties & STK_PROPERTY_ZS_INT){
				stk_dst->properties=STK_PROPERTY_ZS_INT;
				old_stk_dst.properties &  STK_PROPERTY_ZS_CHAR?
				*((zs_char *)stk_dst_ref_value)=*((zs_int *)stk_src_ref_value) & 0xff
				:*((zs_int *)stk_dst_ref_value)=*((zs_int *)stk_src_ref_value);
				if(stk_src_ref_value_copy_aux!=NULL)(*(zs_int *)stk_src_ref_value_copy_aux)=*((zs_int *)stk_src_ref_value);
			}else if(stk_src_properties & STK_PROPERTY_ZS_FLOAT){
				stk_dst->properties=STK_PROPERTY_ZS_FLOAT;
				*((zs_float *)stk_dst_ref_value)=*((zs_float *)stk_src_ref_value);
				if(stk_src_ref_value_copy_aux!=NULL)(*(zs_float *)stk_src_ref_value_copy_aux)=*((zs_float *)stk_src_ref_value);
			}else if(stk_src_properties & STK_PROPERTY_BOOL){
				stk_dst->properties=STK_PROPERTY_BOOL;
				*((bool *)stk_dst_ref_value)=*((bool *)stk_src_ref_value);
				if(stk_src_ref_value_copy_aux!=NULL)(*(bool *)stk_src_ref_value_copy_aux)=*((bool *)stk_src_ref_value);
			}else if(stk_src_properties  &  (STK_PROPERTY_FUNCTION | STK_PROPERTY_TYPE | STK_PROPERTY_MEMBER_FUNCTION) ){
				*stk_dst=*stk_src;
			}else if(
				STK_IS_SCRIPT_OBJECT_STRING(stk_src)
							||
				(stk_src_properties & (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR))

			){
				ScriptObjectString *str_object=NULL;

				if(STK_IS_SCRIPT_OBJECT_STRING(stk_dst)){ // dst is string reload
					str_object=(ScriptObjectString *)stk_dst->value;
				}else{ // Generates a zs_string var
					stk_dst->value=(zs_int)(str_object= ZS_NEW_OBJECT_STRING(data->zs));
					stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;
					// create shared ptr
					if(!vm_create_shared_script_object(vm,str_object)){
						goto lbl_exit_function;
					}
					// share ptr
					if(!vm_share_script_object(vm,str_object)){
						goto lbl_exit_function;
					}
					//-------------------------------------
				}

				if(stk_src_properties & (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR)){
					str_object->set((const char *)stk_src->value);
				}else{
					str_object->set(stk_to_str(VM_STR_AUX_PARAM_0,data->zs, stk_src));
				}


			}else if(stk_src_properties & STK_PROPERTY_SCRIPT_OBJECT){// object we pass its reference

				so_aux=(ScriptObject *)stk_src->value;


				// already share and src is type container and dst is slot:
				// becomes a weak pointer to avoid possibly cyclic reference
				if((so_aux->shared_pointer->data.n_shares>0)
					&& (so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_VECTOR)
					&& (container_slot_store_object!=NULL)){

					StackElement *stk_obj=NULL;



					// More tests would be needed see issue #336
					if(container_slot_store_object->getScriptType()->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_VECTOR){
						printf("\nAssing object %p type '%s' to slot '%i'"
								,container_slot_store_object
								,container_slot_store_object->getScriptType()->str_script_type
								,(int)container_slot_store_id_slot
						);
						stk_obj=container_slot_store_object->getBuiltinElementAt(container_slot_store_id_slot);
					}else{
						printf("\nAssing object %p type '%s' to slot '%s'"
								,container_slot_store_object
								,container_slot_store_object->getScriptType()->str_script_type
								,(const char *)container_slot_store_id_slot
						);
						stk_obj=container_slot_store_object->getProperty((const char *)container_slot_store_id_slot);
					}

					// create weak pointer
					auto weak_pointer=new ScriptObjectWeakPointer(so_aux);


					stk_dst->value=(intptr_t)weak_pointer;
					stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;

					if(!vm_create_shared_script_object(vm,weak_pointer)){
						goto lbl_exit_function;
					}

					if(!vm_share_script_object(vm,weak_pointer)){
						goto lbl_exit_function;
					}

						//so_aux->refObject(NULL);//(ScriptObject **)&stk_obj->value);

				}else{ // share
					stk_dst->value=(intptr_t)so_aux;
					stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;

					if(!vm_share_script_object(vm,so_aux)){
						goto lbl_exit_function;
					}

				}

			}else{
				VM_STOP_EXECUTE("BYTE_CODE_STORE: (internal) cannot determine var type %s"
					,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_src)
				);
			}

			if(stk_src_ref_value_copy_aux!=NULL){
				stk_dst->properties|=STK_PROPERTY_IS_C_VAR_PTR;
			}

			// check old dst value to unref if it was an object ...
			if(
				(old_stk_dst.properties & (STK_PROPERTY_SCRIPT_OBJECT))
			){
				ScriptObject  *old_so=(ScriptObject  *)old_stk_dst.value;


				// unref pointer because new pointer has been attached...
				StackElement *chk_ref=(StackElement *)stk_result_op2->value;


				if(chk_ref->properties & STK_PROPERTY_PTR_STK){
					chk_ref=(StackElement *)chk_ref->value;
				}

				if(!vm_unref_shared_script_object(vm,old_so,VM_CURRENT_SCOPE_BLOCK)){
					goto lbl_exit_function;
				}
			}
		}


		if(instruction->byte_code ==BYTE_CODE_STORE_CONST){
			stk_dst->properties |= STK_PROPERTY_READ_ONLY;
		}

		if(n_element_left_to_store-- > 0){
			stk_dst=--data->stk_vm_current;//stk_multi_var_dest++; // left assigment
			stk_src=++stk_load_multi_var_src; // result on the right
			goto vm_store_next;
		}

		return true;

	lbl_exit_function:

		return false;
	}

	bool vm_call(
			VirtualMachine *vm
			,ScriptObject *this_object
			,ScriptFunction *calling_function
			,Instruction *instruction
			,StackElement *_stk_local_var
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		//------------------------------------------------
		// SFCALL
		Symbol			*symbol_aux=NULL;
		StackElement	*sf_call_stk_function_ref=NULL;
		ScriptFunction 	*sf_call_script_function = NULL;

		int		 	 	sf_call_n_args=0; // number arguments will pass to this function
		StackElement 	*sf_call_stk_start_arg_call=NULL;
		ScriptObject 	*sf_call_calling_object = NULL;
		bool			 sf_call_is_constructor=false;
		bool			sf_call_ignore_call=false;
		int 		 	sf_call_n_local_symbols=0;
		bool 			 sf_call_is_member_function=false;
		StackElement 	*sf_call_stk_return=NULL;
		int 			sf_call_n_returned_arguments_from_function=0;
		int				sf_call_stk_start_function_object=0;
		int				sf_call_return=0;

		// SFCALL
		//------------------------------------------------


		switch(instruction->byte_code){
			case  BYTE_CODE_CALL: // immediate call this
				 sf_call_calling_object = NULL;
				 sf_call_stk_start_function_object=0;
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=false;
				 sf_call_n_args = INSTRUCTION_GET_PARAMETER_COUNT(instruction); // number arguments will pass to this function
				 sf_call_stk_start_arg_call = (data->stk_vm_current - sf_call_n_args);
				 sf_call_script_function=(ScriptFunction *)(((Symbol *)instruction->value_op2)->ref_ptr);
				 goto execute_function;
			case BYTE_CODE_SUPER_CALL:
				 sf_call_calling_object = this_object;
				 sf_call_stk_start_function_object=0;
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=true;
				 sf_call_n_args = INSTRUCTION_GET_PARAMETER_COUNT(instruction); // number arguments will pass to this function
				 sf_call_stk_start_arg_call = (data->stk_vm_current - sf_call_n_args);
				 sf_call_script_function=(ScriptFunction *)((Symbol *)instruction->value_op2)->ref_ptr;
				 goto execute_function;
			case  BYTE_CODE_THIS_CALL: // immediate call this
				 sf_call_calling_object = this_object;
				 sf_call_stk_start_function_object=0;
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=true;
				 sf_call_n_args = INSTRUCTION_GET_PARAMETER_COUNT(instruction); // number arguments will pass to this function
				 sf_call_stk_start_arg_call = (data->stk_vm_current - sf_call_n_args);
				 symbol_aux=NULL;
				 // Since symbol is created on its owner, we have to get symbol from this object. This technique expects
				 // that symbols are ordered
				 if(instruction->value_op2!=ZS_IDX_UNDEFINED){
					 symbol_aux=(Symbol *)this_object->getScriptType()->getSymbolMemberFunction(((Symbol *)instruction->value_op2)->name);
				 }
				 if(symbol_aux==NULL){ // it calls overrided function (top-most)
					 VM_STOP_EXECUTE("Error call 'this.%s': Cannot find '%s::%s' member function"
							,SFI_GET_SYMBOL_NAME(calling_function,instruction)
							,this_object->getScriptType()->str_script_type
							,SFI_GET_SYMBOL_NAME(calling_function,instruction)
					);
				 }
				 sf_call_script_function=(ScriptFunction *)(symbol_aux->ref_ptr);
				 goto execute_function;
			case BYTE_CODE_INDIRECT_THIS_CALL:
				 sf_call_calling_object = this_object;
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=false;
				 sf_call_stk_start_function_object=0;
				 sf_call_stk_function_ref=this_object->getProperty(SFI_GET_SYMBOL_NAME(calling_function,instruction));
				 if(sf_call_stk_function_ref==NULL){ // it calls overrided function (top-most)
					 VM_STOP_EXECUTE("'variable this.%s' not exist", SFI_GET_SYMBOL_NAME(calling_function,instruction)
					);
				 }
				 goto load_function;
			case  BYTE_CODE_INDIRECT_LOCAL_CALL: // call from idx var
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=false;
				 sf_call_calling_object = NULL;
				 sf_call_stk_start_function_object=0;
				 sf_call_stk_function_ref=_stk_local_var+instruction->value_op2;
				goto load_function;
			case  BYTE_CODE_INDIRECT_GLOBAL_CALL: // call from idx var
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=false;
				 sf_call_calling_object = NULL;
				 sf_call_stk_start_function_object=0;
				 sf_call_stk_function_ref=data->vm_stack+instruction->value_op2;
				 goto load_function;
			case  BYTE_CODE_STACK_CALL: // stack call
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=false;
				 sf_call_calling_object = NULL;
				 sf_call_stk_start_function_object=0;
				 sf_call_stk_function_ref=data->stk_vm_current-(INSTRUCTION_GET_PARAMETER_COUNT(instruction)+1);
				 goto load_function;
			 case  BYTE_CODE_CONSTRUCTOR_CALL:
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=false;
				 sf_call_script_function=NULL;
				 sf_call_stk_function_ref = (data->stk_vm_current-INSTRUCTION_GET_PARAMETER_COUNT(instruction)-1);
				// get object
				sf_call_calling_object=(ScriptObject *)((sf_call_stk_function_ref-1)->value);

				// if we invoke constructor we need to keep object to pass after, else remove object+function
				sf_call_stk_start_function_object=1;
				goto load_function;
		 case  BYTE_CODE_MEMBER_CALL: // calling function after all of args are processed...

			sf_call_script_function=NULL;
			sf_call_stk_function_ref = (data->stk_vm_current-INSTRUCTION_GET_PARAMETER_COUNT(instruction)-1);
			// if we invoke constructor we need to keep object to pass after, else remove object+function
			sf_call_stk_start_function_object=1;//instruction->byte_code==BYTE_CODE_CONSTRUCTOR_CALL?1:0; // object + function

load_function:

			sf_call_is_member_function=false;
			sf_call_is_constructor=instruction->byte_code==BYTE_CODE_CONSTRUCTOR_CALL;
			sf_call_ignore_call=false;

			sf_call_n_args = INSTRUCTION_GET_PARAMETER_COUNT(instruction); // number arguments will pass to this function
			sf_call_stk_start_arg_call = (data->stk_vm_current - sf_call_n_args);

			if(sf_call_stk_function_ref->properties & STK_PROPERTY_MEMBER_FUNCTION){
			  Symbol *symbol=(Symbol *)sf_call_stk_function_ref->value;
			  sf_call_script_function=(ScriptFunction *)symbol->ref_ptr;
			  sf_call_is_member_function=true;
			}else if(STK_IS_SCRIPT_OBJECT_MEMBER_FUNCTION(sf_call_stk_function_ref)){
			  ScriptObjectMemberFunction *sofm=(  ScriptObjectMemberFunction *)sf_call_stk_function_ref->value;
			  ScriptObject *sofm_object=sofm->ref_object->getRefObject();
			  if(sofm_object==NULL){
				  VM_STOP_EXECUTE(
						  "Cannot call function member object '%s' stored in variable '%s' due its own object has been dereferenced"
						  ,sofm->so_function->name_script_function
						  , SFI_GET_SYMBOL_NAME(calling_function,instruction));
			  }
			  sf_call_calling_object=sofm_object;
			  sf_call_script_function=sofm->so_function;
			  sf_call_is_member_function=true;
			}else{
				sf_call_is_member_function=false;
				if((sf_call_stk_function_ref->properties & (STK_PROPERTY_FUNCTION))==0){
					// error or continue
					if(instruction->byte_code== BYTE_CODE_CONSTRUCTOR_CALL){ // constructor was not found so we do nothing
						// reset stack to last
						if((instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK)==0){
							data->stk_vm_current=sf_call_stk_function_ref;//sf_call_stk_start_arg_call-sf_call_stk_start_function_object;
						}
						goto lbl_exit_function_ok;
					}

					// indirect this call / member call or stk call
					if(instruction->byte_code==BYTE_CODE_INDIRECT_THIS_CALL){

						VM_STOP_EXECUTE("Cannot call 'this.%s' as type '%s'. 'this.%s' is not function"
								,SFI_GET_SYMBOL_NAME(calling_function,instruction)
								,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,sf_call_stk_function_ref)
								,SFI_GET_SYMBOL_NAME(calling_function,instruction)
						);

					}else if(instruction->byte_code==BYTE_CODE_MEMBER_CALL){
						VM_STOP_EXECUTE("Cannot call '%s'. '%s' is not function or not exist"
								,SFI_GET_SYMBOL_NAME(calling_function,instruction)
								,SFI_GET_SYMBOL_NAME(calling_function,instruction)
						);
					}else if(instruction->byte_code==BYTE_CODE_INDIRECT_LOCAL_CALL){
						VM_STOP_EXECUTE("Cannot call '%s' as a function. '%s' is type '%s'"
								,SFI_GET_SYMBOL_NAME(calling_function,instruction)
								,SFI_GET_SYMBOL_NAME(calling_function,instruction)
								,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,sf_call_stk_function_ref)
						);
					}else{ // STACK CALL
						VM_STOP_EXECUTE("Error trying to call a function from stack. StackElement value is '%s' as type '%s'"
								,stk_to_str(VM_STR_AUX_PARAM_0,data->zs,sf_call_stk_function_ref)
								,stk_to_typeof_str(VM_STR_AUX_PARAM_1,data->zs,sf_call_stk_function_ref)
								,SFI_GET_SYMBOL_NAME(calling_function,instruction)
						);
					}
				}
				Symbol *symbol=(Symbol *)sf_call_stk_function_ref->value;
				sf_call_script_function=(ScriptFunction *)symbol->ref_ptr;
			}

execute_function:

			sf_call_n_local_symbols=0;

			// if a c function that it has more than 1 symbol with same number of parameters, so we have to solve and get the right one...
			// call function
			if((sf_call_script_function->properties & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){ // if script function...

				// we pass everything by copy (TODO implement ref)
				if(sf_call_n_args > 0 && sf_call_script_function->params_len > 0){
					StackElement *stk_arg=sf_call_stk_start_arg_call;
					ScriptObjectVector *var_args=NULL;
					ScriptObject *so_param=NULL;

					int effective_args=sf_call_n_args < sf_call_script_function->params_len ? sf_call_n_args:sf_call_script_function->params_len;
					ScriptFunctionParam *sf_param=sf_call_script_function->params;
					for(int i=0;i < sf_call_n_args;i++){
						so_param=NULL; // script object we passing
						uint16_t sfa_properties=sf_param->properties;// ((ScriptFunctionParam *)(*function_param))->properties;
						if((sfa_properties & MSK_SCRIPT_FUNCTION_ARG_PROPERTY_BY_REF)){ // create or pass the var ref object...

							StackElement *check_ref=stk_arg;
							if(stk_arg->properties & STK_PROPERTY_PTR_STK){
								check_ref=(StackElement *)check_ref->value;
							}

							// because arg by ref is always loaded directly we have the object stk...
							if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_arg)==false) { // create new

								if((stk_arg->properties & STK_PROPERTY_PTR_STK) != STK_PROPERTY_PTR_STK){
									VM_STOP_EXECUTE("Calling function '%s', parameter '%i': Argument by reference has to be variable"
											,sf_call_script_function->name_script_function,i+1);
								}

								ScriptObjectVarRef *sc=ZS_NEW_OBJECT_VAR_REF(data->zs,*stk_arg);
								if(!vm_create_shared_script_object(vm,sc)){
									goto lbl_exit_function;
								}
								so_param=sc;
								stk_arg->value=(intptr_t)sc;
								stk_arg->properties=STK_PROPERTY_SCRIPT_OBJECT;
							}else{ // is a var ref already, keep its reference ...
								so_param=(ScriptObject *)stk_arg->value;
							}

						}else{
							if(stk_arg->properties & STK_PROPERTY_PTR_STK){ // get its value
								*stk_arg=*(StackElement *)stk_arg->value;
							}

							if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_arg)==true) { // not passing by ref it gets its value
								*stk_arg=*((ScriptObjectVarRef *)stk_arg->value)->getStackElementPtr();
							}

							if((stk_arg->properties & STK_PROPERTY_SCRIPT_OBJECT)){
								so_param=(ScriptObject *)stk_arg->value;
								if(so_param->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_STRING && so_param->shared_pointer==NULL){
									ScriptObjectString *sc=ZS_NEW_OBJECT_STRING(data->zs);
									if(!vm_create_shared_script_object(vm,sc)){
										goto lbl_exit_function;
									}
									sc->set(*((zs_string *)((ScriptObjectString *)so_param)->value));
									so_param=sc;
									stk_arg->value=(zs_int)sc;
									stk_arg->properties=STK_PROPERTY_SCRIPT_OBJECT;
								}
							} // else if member property it will manage itself on the stack
						}

						if(var_args!=NULL){
							var_args->push(stk_arg); // we do not share pointer here due is already added in a vector
						}else{
							if(sfa_properties & MSK_SCRIPT_FUNCTION_ARG_PROPERTY_VAR_ARGS){ // enter var args
								var_args=ZS_NEW_OBJECT_VECTOR(data->zs);
								if(!vm_create_shared_script_object(vm,var_args)){
									goto lbl_exit_function;
								}

								if(!vm_share_script_object(vm,var_args)){ // we share pointer +1 to not remove on pop in calling return
									goto lbl_exit_function;
								}

								// push first arg
								var_args->push(stk_arg);
								// replace for vector type...
								stk_arg->value=(zs_int)var_args;
								stk_arg->properties=STK_PROPERTY_SCRIPT_OBJECT;
							}else{ // not push in var arg

								if(so_param != NULL){ // share n+1 to function if not this
									if(!vm_share_script_object(vm,so_param)){ // By pass object in the arg, it shares pointer +1 to not remove on pop in calling return
										goto lbl_exit_function;
									}
								}

								sf_param++;
								if(i+1 >= effective_args){
									break;
								}
							}
						}
						stk_arg++;
					}
				}

				// ... we must set the rest of parameters with default value in case user put less params. If params exceds the number of accepted params in function,
				// will be ignored always.
				for(int h = sf_call_n_args; h < sf_call_script_function->params_len; ++h){
					ScriptFunctionParam *param=sf_call_script_function->params+h;

					switch(param->default_param_value.properties){
					case STK_PROPERTY_UNDEFINED:
					case STK_PROPERTY_NULL:
					case STK_PROPERTY_ZS_INT:
					case STK_PROPERTY_BOOL:
					case STK_PROPERTY_ZS_FLOAT:
						*data->stk_vm_current++=param->default_param_value;
						break;
					case STK_PROPERTY_FUNCTION: // we call function that return default value
						VM_INNER_CALL(
							NULL
							,(ScriptFunction *)(((Symbol *)param->default_param_value.value)->ref_ptr)
							, 0
							,((Symbol *)param->default_param_value.value)->name.c_str()
						)

						// if script object it shares in order to be used as variable in the function to be called
						if(data->stk_vm_current->properties & STK_PROPERTY_SCRIPT_OBJECT){
							if(!vm_share_script_object(vm,(ScriptObject *)data->stk_vm_current->value)){
								goto lbl_exit_function;
							}
						}
						data->stk_vm_current++;
						break;
					default:
						VM_STOP_EXECUTE("Internal error: Unexpected default stack element '%s'"
								,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,&param->default_param_value));
						break;

					}
					sf_call_n_args++;
				}

				if(sf_call_is_constructor){
					// When the object is being constructed its shares is 0. In the 'constructor' function may pass 'this' throug other functions
					// exposin 'this' candidate to be dereferenced and destroyed. So we share 'this' before the call and unref after call
					if(!vm_share_script_object(vm,sf_call_calling_object)){
						goto lbl_exit_function;
					}
				}

				vm_execute_function_script(
					vm
					,sf_call_calling_object
					,sf_call_script_function
					,sf_call_stk_start_arg_call
				);

				if(sf_call_is_constructor){
					// When the object is being constructed its shares is 0. In the 'constructor' function may pass 'this' throug other functions
					// exposin 'this' candidate to be dereferenced and destroyed. In the BYTE_CODE_CONSTRUCTOR_CALL was shared +1.
					// In this case deref the shared 'this' is dereferenced
					if(!vm_unref_shared_script_object(vm,sf_call_calling_object,VM_CURRENT_SCOPE_BLOCK)){
						goto lbl_exit_function;
					}
				}

				sf_call_n_local_symbols=sf_call_script_function->local_variables->count;
			}
			else{ // C function
				if(sf_call_script_function->properties & FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME){

					ScriptType *sc=NULL;
					bool ignore_call=false;

					if(
						sf_call_is_member_function
					){
						ignore_call= (sf_call_is_constructor) && sf_call_calling_object->isNativeObject() && sf_call_n_args==0;
						sc=data->script_type_factory->getScriptType(sf_call_calling_object->idx_script_type);
					}else if(sf_call_script_function->idx_script_type_owner != IDX_TYPE_CLASS_MAIN
							&& (sf_call_script_function->properties & FUNCTION_PROPERTY_STATIC)
					){
						sc=data->script_type_factory->getScriptType(sf_call_script_function->idx_script_type_owner);
					}

					if(ignore_call == false)
					{
						ScriptFunction *sf_aux;
						if((sf_aux=vm_find_native_function(
								vm
								,sc
								,calling_function
								,instruction
								,sf_call_is_constructor
								,sf_call_script_function->name_script_function // symbol to find
								,sf_call_stk_start_arg_call
								,sf_call_n_args))==NULL){
							goto lbl_exit_function;
						}
						sf_call_script_function=sf_aux;
					}
				}

				try{
					vm_execute_function_native(
						vm
						,calling_function
						,instruction
						,sf_call_calling_object
						,sf_call_script_function
						,sf_call_stk_start_arg_call
						,sf_call_n_args
					);
				}catch(std::exception & ex){
					data->vm_error = true;
					strcpy(data->vm_error_str,ex.what());
				}

				// restore stk_start_arg_call due in C args are not considered as local symbols (only for scripts)
				sf_call_n_local_symbols=sf_call_n_args;
			}

			if(data->vm_error == true){
				// if System::assert -> not add in callstack trace

				if((
					(
						(
							(
								ZS_STRCMP(calling_function->name_script_function,==,"assert")
							||  ZS_STRCMP(calling_function->name_script_function,==,"error")
							)
							&&
							(
								ZS_STRCMP(sf_call_script_function->name_script_function,==,"errorNative")
							)
						)
						||
						(
							(
								ZS_STRCMP(calling_function->name_script_function,==,"eval")
							&&  ZS_STRCMP(sf_call_script_function->name_script_function,==,"evalNative")
							)
						)
					)==false)

						&&
					(data->vm_error_max_stack_reached==false)
				){
					const char *str_class_owner=NULL;
					if(	(sf_call_script_function->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)!=0
							||
						(sf_call_script_function->properties & FUNCTION_PROPERTY_STATIC)!=0
					){
						str_class_owner=data->script_type_factory->getScriptType(sf_call_script_function->idx_script_type_owner)->str_script_type;
					}
					const char * file_src_call=SFI_GET_FILE(calling_function,instruction);
					sprintf(VM_STR_AUX_PARAM_0
							,"\nat calling function %s%s%s (%sline:%i)" // TODO: get full symbol ?
							,str_class_owner==NULL?"":str_class_owner
							,str_class_owner==NULL?"":"::"
							,sf_call_script_function->name_script_function
							,file_src_call?zs_strutils::format("file:%s ",file_src_call).c_str():""
							,SFI_GET_LINE(calling_function,instruction)
					);
					strcat(data->vm_error_callstack_str,VM_STR_AUX_PARAM_0);

				}
				goto lbl_exit_function;
			}

			// calcule returned stack elements
			sf_call_stk_return=(sf_call_stk_start_arg_call+sf_call_n_local_symbols); // +1 points to starting return...
			sf_call_n_returned_arguments_from_function=data->stk_vm_current-sf_call_stk_return;
			sf_call_return=INSTRUCTION_GET_RETURN_COUNT(instruction);

			// setup all returned variables from function
			CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(sf_call_stk_return,sf_call_n_returned_arguments_from_function,false)

			if((instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK)==0){

				// add as many values expects to the left
				for(int i=sf_call_n_returned_arguments_from_function; i < sf_call_return;i++){
					VM_PUSH_STK_UNDEFINED;
					sf_call_n_returned_arguments_from_function++;
				}

				// return all elements in reverse order in order to get right assignment ...
				// reverse returned items
				for(int i=0; i<(sf_call_n_returned_arguments_from_function>>1); i++){
					StackElement tmp=sf_call_stk_return[sf_call_n_returned_arguments_from_function-i-1];
					sf_call_stk_return[sf_call_n_returned_arguments_from_function-i-1]=sf_call_stk_return[i];
					sf_call_stk_return[i]=tmp;
				}

				// set number of call return
				sf_call_n_returned_arguments_from_function=sf_call_return;

				data->stk_vm_current=sf_call_stk_start_arg_call-sf_call_stk_start_function_object;//(sf_call_stk_start_function_object?0:1);//+n_returned_arguments_from_function; // stk_vm_current points to first stack element

				// no return parameters but the caller expects n_parameters, so
				for(int i=0; i < sf_call_n_returned_arguments_from_function;i++){
					*data->stk_vm_current++=*sf_call_stk_return++;
				}
			}
		}

	lbl_exit_function_ok:
		return true;

	lbl_exit_function:
		return false;
	}

	bool vm_load_field(
		VirtualMachine *vm
		,ScriptObject *this_object
		,ScriptFunction *calling_function
		,Instruction **instruction_it
	){
		Instruction *instruction=(*instruction_it)-1;
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		ScriptObject *so_aux=NULL;
		StackElement *stk_result_op1=NULL;
		const char *str_symbol_aux1=NULL;
		StackElement *stk_var=NULL;
		StackMemberProperty *stk_mp_aux=NULL;
		ScriptType *sc_type=NULL;
		Symbol *sf_member=NULL;
		ScriptObjectMemberFunction *somf=NULL;

		if(
				instruction->byte_code == BYTE_CODE_LOAD_THIS_VARIABLE
				|| instruction->byte_code == BYTE_CODE_PUSH_STK_THIS_VARIABLE
		){
				so_aux=this_object; // take this as default
				goto find_element_object;
		}

load_next_element_object:
		instruction=(*instruction_it)-1;

		if(
			(instruction-1)->byte_code == BYTE_CODE_NEW_OBJECT_BY_TYPE
		){
			stk_result_op1=(data->stk_vm_current-1);
		}
		else{
			VM_POP_STK_ONE; // get var op1 and symbol op2
		}

		if((stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT)!= STK_PROPERTY_SCRIPT_OBJECT)
		{
			if((instruction-1)->byte_code==BYTE_CODE_LOAD_OBJECT_ITEM){
				VM_STOP_EXECUTE(
					"Cannot perform access [ ... %s.%s ], expected '%s' as object but is type '%s' %s"
					,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)
					,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
					,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1)
					,zs_strutils::starts_with(stk_to_typeof_str(VM_STR_AUX_PARAM_1,data->zs,stk_result_op1),"type@")? ". If you are trying to call/access static member of class you need to use static access operator (i.e '::') instead of member access operator (i.e '.')":""
				);
			}else{ // from calling
				VM_STOP_EXECUTE(
					"Cannot perform access '.%s' from variable type '%s'"
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)
					,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1)
				);
			}
		}

		so_aux=((ScriptObject *)stk_result_op1->value);


		if(so_aux == NULL)
		{
			VM_STOP_EXECUTE("var '%s' is not scriptvariable",SFI_GET_SYMBOL_NAME(calling_function,(instruction-1)));
		}

find_element_object:

		str_symbol_aux1=(char *)SFI_GET_SYMBOL_NAME(calling_function,instruction);

		//
		sc_type=so_aux->getScriptType();
		sf_member=sc_type->getSymbolMemberFunction(str_symbol_aux1);
		if(sf_member !=NULL){
			if(
				   ((instruction+1)->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM || ((instruction+1)->byte_code == BYTE_CODE_PUSH_STK_OBJECT_ITEM))
				&& ((instruction->properties & INSTRUCTION_PROPERTY_CALLING_FUNCTION)==0)){
				VM_STOP_EXECUTE(
					"Cannot perform access operation [ ... %s.%s ], because '%s' is a function. It should call function with '()' before '.'"
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)
					,SFI_GET_SYMBOL_NAME(calling_function,instruction+1)
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)
				);
			}

			somf=ZS_NEW_OBJECT_MEMBER_FUNCTION(data->zs,so_aux,(ScriptFunction *)sf_member->ref_ptr);

			 if(!vm_create_shared_script_object(vm,somf)){
				goto lbl_exit_function;
			 }

			data->stk_vm_current->value=(zs_int)somf;
			data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;
			data->stk_vm_current++;

			goto lbl_exit_function_ok;

		}else if((stk_var=so_aux->getProperty(str_symbol_aux1)) == NULL){

			if(instruction->properties & INSTRUCTION_PROPERTY_CALLING_FUNCTION){
				VM_STOP_EXECUTE("Error call function '...%s.%s(...)', where '%s' is type '%s'. Member function '%s::%s' is not defined"
					,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
					,str_symbol_aux1
					,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
					,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,data->stk_vm_current)
					,stk_to_typeof_str(VM_STR_AUX_PARAM_1,data->zs,data->stk_vm_current)
					,str_symbol_aux1
				);
			}

			// something went wrong
			if(data->vm_error == true){
				goto lbl_exit_function;
			}
			//------------------------------------------------------------------
			// pack object+member stk info for store information...
			if(   instruction->byte_code == BYTE_CODE_PUSH_STK_OBJECT_ITEM
			  ||  instruction->byte_code == BYTE_CODE_PUSH_STK_THIS_VARIABLE){

				// if object is C
				//sc_type=so_aux->getScriptType();

				// exceptions
				if(sc_type->idx_script_type<IDX_TYPE_SCRIPT_OBJECT_OBJECT || sc_type->idx_script_type>IDX_TYPE_SCRIPT_OBJECT_OBJECT){
					// Properties from native types or custom internal type through script side cannot be added if not exist, so if not exist throw error.
					if(so_aux->getScriptType()->properties & SCRIPT_TYPE_PROPERTY_C_OBJECT_REF){
						VM_STOP_EXECUTE("Cannot store '...%s.%s', where '%s' is type '%s'. %s property '%s::%s' is not defined"
							,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
							,str_symbol_aux1
							,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
							,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,data->stk_vm_current)
							,sc_type->idx_script_type>IDX_TYPE_SCRIPT_OBJECT_OBJECT?"Native type":"Type"
							,stk_to_typeof_str(VM_STR_AUX_PARAM_1,data->zs,data->stk_vm_current)
							,str_symbol_aux1
						);
					}
				}

				// create new property initialized as undefined
				if((stk_var=so_aux->addProperty((const char *)str_symbol_aux1, data->vm_error_str))==NULL){
					VM_STOP_EXECUTEF(data->vm_error_str);
				}

				if(instruction->properties & INSTRUCTION_PROPERTY_OBJ_ITEM_TO_STORE){
					data->stk_vm_current->value=(zs_int)(new ContainerSlotStore(so_aux,(zs_int)str_symbol_aux1,stk_var));
					data->stk_vm_current->properties=STK_PROPERTY_CONTAINER_SLOT_STORE;
					data->stk_vm_current++;
				}else{
					VM_PUSH_STK_PTR(stk_var);
				}
			}
			else{ // not exists
				data->stk_vm_current->value=0;
				data->stk_vm_current->properties=STK_PROPERTY_UNDEFINED;
				data->stk_vm_current++;
			}
			goto lbl_exit_function_ok;
		}else{

			if((instruction->properties & INSTRUCTION_PROPERTY_CALLING_FUNCTION) && ((stk_var->properties & STK_PROPERTY_FUNCTION)==0)){

				VM_STOP_EXECUTE("Error call function '...%s.%s(...)', where '%s' is type '%s'. Expected '%s::%s' as a function but it is type '%s'"
					,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
					,(const char *)str_symbol_aux1
					,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
					,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,data->stk_vm_current)
					,stk_to_typeof_str(VM_STR_AUX_PARAM_1,data->zs,data->stk_vm_current)
					,(const char *)str_symbol_aux1
					,stk_to_typeof_str(VM_STR_AUX_PARAM_2,data->zs,stk_var)
				);
			}

			// member property. ... if getter, get the value itself and evaluate
			if(
			   (stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY)!=0
			 ){
				stk_mp_aux=(StackMemberProperty *)stk_var->value;

				if(stk_mp_aux->member_property->metamethod_members.getter!=NULL){
					VM_INNER_CALL(
							stk_mp_aux->so_object
							,((ScriptFunction *)stk_mp_aux->member_property->metamethod_members.getter->ref_ptr)
							,0
							,stk_mp_aux->member_property->metamethod_members.getter->name.c_str()
					);

					if(
							// Pass the object if the value is object type >= TYPE_SCRIPT_OBJECT_CLASS ...
							STK_IS_SCRIPT_OBJECT_CLASS(data->stk_vm_current)
						||(
								// ... or return value itself if the next instruction is not for store
								(
									(instruction->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM)
								||	(instruction->byte_code == BYTE_CODE_LOAD_THIS_VARIABLE)
								)
								&& ((instruction->properties & INSTRUCTION_PROPERTY_USE_PUSH_STK)==0)
						  )

					){
						data->stk_vm_current++;
						goto lbl_exit_function_ok;
					}
				}

			}
		}


		if((instruction+1)->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM){ // fast load access without pass through switch instruction
			*data->stk_vm_current++=*stk_var;
			instruction++; // we have to inc current instruction...
			*instruction_it++; //... and instruction iterator
			goto load_next_element_object;
		}

		// load its value for write
		if(instruction->byte_code == BYTE_CODE_PUSH_STK_OBJECT_ITEM || instruction->byte_code == BYTE_CODE_PUSH_STK_THIS_VARIABLE){
			if(instruction->properties & INSTRUCTION_PROPERTY_OBJ_ITEM_TO_STORE){
				data->stk_vm_current->value=(zs_int)(new ContainerSlotStore(so_aux,(zs_int)str_symbol_aux1,stk_var));
				data->stk_vm_current->properties=STK_PROPERTY_CONTAINER_SLOT_STORE;
				data->stk_vm_current++;
			}else{
				VM_PUSH_STK_PTR(stk_var);
			}
		}
		else{ // only read
			*data->stk_vm_current++=*stk_var;
		}

	lbl_exit_function_ok:
		return true;

	lbl_exit_function:
		return false;
	}

	void vm_throw_error_cannot_find_symbol(
			VirtualMachine *vm
			,ScriptFunction *calling_function
			,Instruction *instruction
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;

		const char *__STR_PTR_SYMBOL_TO_FIND__=SFI_GET_SYMBOL_NAME(calling_function,instruction);
		const char *__STR_PTR_END_CLASS__=NULL;

		if((__STR_PTR_END_CLASS__=strstr(__STR_PTR_SYMBOL_TO_FIND__,"::"))!=NULL){ // static access
			char str_script_type[512]={0};

			strncpy(str_script_type,__STR_PTR_SYMBOL_TO_FIND__,__STR_PTR_END_CLASS__-__STR_PTR_SYMBOL_TO_FIND__);


			if(data->zs->getScriptTypeFactory()->getScriptType(str_script_type) == NULL){
				VM_STOP_EXECUTE(
						"type '%s' not exist"
						,str_script_type
				);
			}

			VM_STOP_EXECUTE(
					"static symbol '%s::%s' not exist"
					,str_script_type
					,__STR_PTR_END_CLASS__+2
			);
		}else{
			VM_STOP_EXECUTE(
					"Symbol '%s' not defined"
					,__STR_PTR_SYMBOL_TO_FIND__
			);
		}

	lbl_exit_function:
		return;
	}

	void vm_delete(VirtualMachine *vm){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		if(data->lifetime_object.count>0){
			zs_string created_at="";
			zs_string end="";
			bool some_registers_without_file_line=false;

			zs_string error="\n\nSome lifetime objects returned by virtual machine were not unreferenced:\n\n";
			for(int i=0; i < data->lifetime_object.count;i++ ){
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




}


#include "vm_execute_function_native.cpp"
#include "vm_execute_function_script.cpp"
