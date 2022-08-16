/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define __STR_PTR_SYMBOL_TO_FIND__ 	str_symbol_aux1
#define __STR_PTR_END_CLASS__ 		str_symbol_aux2

namespace zetscript{


	void vm_execute_function_script(
			VirtualMachine			* vm,
			ScriptObject			* this_object,
			ScriptFunction 			* calling_function,
			StackElement 		  	* _stk_local_var
	    ){
		// This is the main script function body, and due there's a lot of code, is important to reuse as many variables as possible
		// and not abuse of temporal variables returning from classes to keep HEAP memory region low to avoid StackOverflow in recursion
		//
		// generic vars for management
		VirtualMachineData *data = (VirtualMachineData*)vm->data;
		Instruction 			*	instruction=calling_function->instructions; // starting instruction
		ScriptObject 			*	so_aux=NULL;
		StackElement 			*	stk_result_op1=NULL;
		StackElement 			*	stk_result_op2=NULL;
		StackElement 				stk_aux1,stk_aux2;
		StackElement 			*	stk_var=NULL;
		Symbol 		 			*	symbol_aux=NULL;
		ScriptType	  			*	sc_aux1=NULL;
		const char 				*	str_symbol_aux1=NULL;
		MetamethodMembers 		*	ptr_metamethod_members_aux=NULL;
		MemberProperty 			*	member_property=NULL;
		ScriptObjectClass		*	so_class_aux1=NULL;

		uint32_t 					msk_properties=0;
		StackElement 			*	stk_dst=NULL;
		StackElement 			*	stk_src=NULL;
		zs_int 					*	stk_src_ref_value=NULL;
		zs_int 					*	stk_dst_ref_value=NULL;
		uint16_t 					stk_src_properties=0;
		StackMemberProperty 	*	stk_mp_aux=NULL;
		void					*	ptr_ptr_void_ref=NULL;
		int 						index_aux1=0;

		Instruction 			*	instruction_it=calling_function->instructions;
		StackElement 			*	stk_start=_stk_local_var+calling_function->local_variables->count;   // <-- here starts stk for aux vars for operations ..

		if (IDX_VM_CURRENT_SCOPE_FUNCTION >= VM_FUNCTION_CALL_MAX) {
			VM_ERROR_AND_RETF("Reached max stack");
			return;
		}

		data->stk_vm_current = stk_start;

		if(((stk_start-data->vm_stack)+calling_function->min_code_stack_needed)>=VM_STACK_MAX){
			data->vm_error_max_stack_reached=true;
			VM_STOP_EXECUTEF("Error MAXIMUM stack size reached");
		}

#ifdef __DEBUG__
		ZS_LOG_DEBUG("Executing function %s ...",calling_function->symbol.name);
#endif

		// Init local vars ...
		data->vm_current_scope_function->current_scope_block=data->vm_current_scope_function->first_scope_block;
		data->vm_current_scope_function->stk_local_vars=_stk_local_var;
		data->vm_current_scope_function++;


		// init local variables symbols (except arguments) as undefined
		if((calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN)){
			VM_PUSH_SCOPE(calling_function->scope_script_function);
			for(index_aux1=calling_function->params_len; index_aux1 <calling_function->local_variables->count; index_aux1++){
				STK_SET_UNDEFINED(_stk_local_var+ index_aux1);
			}
		}

		//-----------------------------------------------------------------------------------------------------------------------
		//
		// MAIN LOOP
		//
		for(;;){ // foreach byte code instruction ...

			instruction = instruction_it++;

			switch(instruction->byte_code){
			case BYTE_CODE_PUSH_STK_GLOBAL: // load variable ...
				VM_PUSH_STK_PTR(data->vm_stack + instruction->value_op2);
				continue;
			case BYTE_CODE_PUSH_STK_LOCAL: // load variable ...
				VM_PUSH_STK_PTR(_stk_local_var+instruction->value_op2);
				continue;
			// load
			case BYTE_CODE_LOAD_GLOBAL: // load variable ...
				*data->stk_vm_current++=*(data->vm_stack+instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_LOCAL: // load variable ...
				*data->stk_vm_current++=*(_stk_local_var+instruction->value_op2);
				continue;
			case BYTE_CODE_EQU:  // ==
				VM_POP_STK_TWO;
				VM_OPERATION_COMPARE(==, BYTE_CODE_METAMETHOD_EQU,false);
				continue;
			case BYTE_CODE_NOT_EQU:  // !=
				VM_POP_STK_TWO;
				VM_OPERATION_COMPARE(!=, BYTE_CODE_METAMETHOD_NOT_EQU,false);
				continue;
			case BYTE_CODE_LT:  // <
				VM_POP_STK_TWO;
				VM_OPERATION_COMPARE(<, BYTE_CODE_METAMETHOD_LT,false);
				continue;
			case BYTE_CODE_LTE:  // <=
				VM_POP_STK_TWO;
				VM_OPERATION_COMPARE(<=, BYTE_CODE_METAMETHOD_LTE,false);
				continue;
			case BYTE_CODE_GT:  // >
				VM_POP_STK_TWO;
				VM_OPERATION_COMPARE(>,BYTE_CODE_METAMETHOD_GT,false);
				continue;
			case BYTE_CODE_GTE:  // >=
				VM_POP_STK_TWO;
				VM_OPERATION_COMPARE(>=,BYTE_CODE_METAMETHOD_GTE,false);
				continue;
			case BYTE_CODE_LOGIC_AND:  // &&
				VM_POP_STK_TWO;
				VM_OPERATION_LOGIC(&&);
				continue;
			case BYTE_CODE_LOGIC_OR:  // ||
				VM_POP_STK_TWO;
				VM_OPERATION_LOGIC(||);
				continue;
			case BYTE_CODE_TYPEOF:
				VM_POP_STK_ONE;
				*data->stk_vm_current++ = stk_result_op1->typeOf();
				continue;
			case BYTE_CODE_ADD: // +
				VM_POP_STK_TWO;
				VM_OPERATION_ADD;
				continue;
			case BYTE_CODE_SUB: // -
				VM_POP_STK_TWO;
				VM_OPERATION_ARITHMETIC(-, BYTE_CODE_METAMETHOD_SUB);
				continue;
			case BYTE_CODE_MUL: // *
				VM_POP_STK_TWO;
				VM_OPERATION_ARITHMETIC(*, BYTE_CODE_METAMETHOD_MUL);
				continue;
			case BYTE_CODE_DIV: // /
				VM_POP_STK_TWO;
				VM_OPERATION_DIV;
				continue;
			case BYTE_CODE_MOD: // /
				VM_POP_STK_TWO;
				VM_OPERATION_MOD(BYTE_CODE_METAMETHOD_MOD);
				continue;
			case BYTE_CODE_BITWISE_AND: // &
				VM_POP_STK_TWO;
				VM_OPERATION_BINARY(&, BYTE_CODE_METAMETHOD_AND);
				continue;
			case BYTE_CODE_BITWISE_OR: // *
				VM_POP_STK_TWO;
				VM_OPERATION_BINARY(| , BYTE_CODE_METAMETHOD_OR);
				continue;
			case BYTE_CODE_BITWISE_XOR: // ^
				VM_POP_STK_TWO;
				VM_OPERATION_BINARY(^, BYTE_CODE_METAMETHOD_XOR);
				continue;
			case BYTE_CODE_SHR: // >>
				VM_POP_STK_TWO;
				VM_OPERATION_BINARY(>> , BYTE_CODE_METAMETHOD_SHR);
				continue;
			case BYTE_CODE_SHL: // <<
				VM_POP_STK_TWO;
				VM_OPERATION_BINARY(<< , BYTE_CODE_METAMETHOD_SHL);
				continue;
			case BYTE_CODE_NOT: // !
				VM_POP_STK_ONE;
				if(stk_result_op1->properties & STK_PROPERTY_BOOL){ // operation will result as integer.
					VM_PUSH_STK_BOOLEAN((!((bool)(stk_result_op1->value))));
				}else if(stk_result_op1->properties & STK_PROPERTY_ZS_INT){
					VM_PUSH_STK_BOOLEAN((!((zs_int)(stk_result_op1->value))));
				}else if(stk_result_op1->properties & STK_PROPERTY_ZS_FLOAT){
					VM_PUSH_STK_BOOLEAN(!((*((zs_float *)(&stk_result_op1->value)))==0));
				}else{
					if(vm_call_metamethod(
						vm
						,calling_function
						,instruction
						,BYTE_CODE_METAMETHOD_NOT
						,stk_result_op1
						,NULL
					)==false){
						goto lbl_exit_function;
					}
				}
				continue;
			case BYTE_CODE_NEG: // -
				VM_POP_STK_ONE;
				if(stk_result_op1->properties & STK_PROPERTY_ZS_INT){ // operation will result as integer.
					VM_PUSH_STK_ZS_INT((-((zs_int)(stk_result_op1->value))));
				}else if(stk_result_op1->properties & STK_PROPERTY_ZS_FLOAT){
					VM_PUSH_STK_ZS_FLOAT(-*((zs_float *)&stk_result_op1->value));
				}else{ // try metamethod ...
					if(!vm_call_metamethod(
							vm
							,calling_function
							,instruction
							,BYTE_CODE_METAMETHOD_NEG
							,stk_result_op1
							,NULL
					)){
						goto lbl_exit_function;
					}
				}
				continue;
			 case BYTE_CODE_INSTANCEOF: // check instance of ...
				 VM_POP_STK_ONE;
				switch(instruction->value_op2){
				case ZS_IDX_UNDEFINED:
					VM_STOP_EXECUTE("type '%s' does not exist ",SFI_GET_SYMBOL_NAME(calling_function,instruction));
					break;
				case IDX_TYPE_ZS_INT_C:
					VM_PUSH_STK_BOOLEAN((stk_result_op1->properties & STK_PROPERTY_ZS_INT)!=0);
					break;
				case IDX_TYPE_ZS_FLOAT_C:
					VM_PUSH_STK_BOOLEAN((stk_result_op1->properties & STK_PROPERTY_ZS_FLOAT)!=0);
					break;
				case IDX_TYPE_BOOL_C:
					VM_PUSH_STK_BOOLEAN((stk_result_op1->properties & STK_PROPERTY_BOOL)!=0);
					break;
				case IDX_TYPE_FUNCTION:
					VM_PUSH_STK_BOOLEAN((stk_result_op1->properties & STK_PROPERTY_FUNCTION)!=0);
					break;
				default:
					if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
						VM_PUSH_STK_BOOLEAN(data->script_type_factory->scriptTypeInheritsFrom(			//
								((ScriptObjectObject *)(stk_result_op1->value))->idx_script_type // A
								, instruction->value_op2		// B
						));
					}else{
						VM_PUSH_STK_BOOLEAN(false);
					}
					break;
				}
				continue;
			 case BYTE_CODE_JMP:
				instruction_it=instruction+instruction->value_op2;
				continue;
			 case BYTE_CODE_JNT: // goto if not true ... goes end to conditional.
				VM_POP_STK_ONE;
				if((stk_result_op1->properties & STK_PROPERTY_BOOL)==0){
					VM_STOP_EXECUTE("Expected boolean expression but it was '%s'",stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1));
				}
				if(stk_result_op1->value == 0){
					instruction_it=instruction+instruction->value_op2;
				}
				continue;
			 case BYTE_CODE_JT: // goto if true ... goes end to conditional.
				VM_POP_STK_ONE;
				if((stk_result_op1->properties & STK_PROPERTY_BOOL)==0){
					VM_STOP_EXECUTE("Expected boolean expression but it was '%s'",stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1));
				}
				if(stk_result_op1->value != 0){
					instruction_it=instruction+instruction->value_op2;
				}
				continue;
			 case BYTE_CODE_JE_CASE:  // especial j for switch
				VM_POP_STK_ONE_LOAD2; // reads switch value and case value
				VM_OPERATION_COMPARE(==, BYTE_CODE_METAMETHOD_EQU,true);
				VM_POP_STK_ONE; // retrieve result...
				if(stk_result_op1->value != 0){ // if true goto
					// reset stack
					data->stk_vm_current=stk_start;

					// goto to state
					instruction_it=instruction+instruction->value_op2;
				}
				continue;
			case BYTE_CODE_JMP_CASE:
  			    data->stk_vm_current=stk_start;
				instruction_it=instruction+instruction->value_op2;
				continue;
			case BYTE_CODE_LOAD_UNDEFINED:
				VM_PUSH_STK_UNDEFINED;
				continue;
			case BYTE_CODE_LOAD_NULL:
				VM_PUSH_STK_NULL;
				continue;
			case BYTE_CODE_LOAD_FUNCTION: // expect constant and function has the same behaviour...
				VM_PUSH_STK_SCRIPT_FUNCTION(instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_ZS_INT:
				data->stk_vm_current->value=instruction->value_op2;
				data->stk_vm_current->properties=STK_PROPERTY_ZS_INT;
				data->stk_vm_current++;
				continue;
			case BYTE_CODE_LOAD_ZS_FLOAT:
				data->stk_vm_current->value=instruction->value_op2;
				data->stk_vm_current->properties=STK_PROPERTY_ZS_FLOAT;
				data->stk_vm_current++;
				continue;
			case BYTE_CODE_LOAD_BOOL:
				data->stk_vm_current->value=instruction->value_op2;
				data->stk_vm_current->properties=STK_PROPERTY_BOOL;
				data->stk_vm_current++;
				continue;
			case BYTE_CODE_LOAD_STACK_ELEMENT:
			case BYTE_CODE_LOAD_STRING:
				*data->stk_vm_current++=*((StackElement *)instruction->value_op2);
				continue;
			 case BYTE_CODE_PUSH_SCOPE:
				VM_PUSH_SCOPE(instruction->value_op2);
				continue;
			 case BYTE_CODE_POP_SCOPE:
				 vm_pop_scope(vm);
				 continue;
			 case BYTE_CODE_POST_INC:
				 VM_OPERATION_POST(++,BYTE_CODE_METAMETHOD_POST_INC,ptr_metamethod_members_aux->post_inc);
				 continue;
			 case BYTE_CODE_NEG_POST_INC:
				 VM_OPERATION_NEG_POST(++,BYTE_CODE_METAMETHOD_POST_INC,ptr_metamethod_members_aux->post_inc);
				 continue;
			 case BYTE_CODE_POST_DEC:
				 VM_OPERATION_POST(--,BYTE_CODE_METAMETHOD_POST_DEC,ptr_metamethod_members_aux->post_dec);
				 continue;
			 case BYTE_CODE_NEG_POST_DEC:
				 VM_OPERATION_POST(--,BYTE_CODE_METAMETHOD_POST_DEC,ptr_metamethod_members_aux->post_dec);
				 continue;
			 case BYTE_CODE_PRE_INC:
				 VM_OPERATION_PRE(++,BYTE_CODE_METAMETHOD_PRE_INC,ptr_metamethod_members_aux->pre_inc);
				 continue;
			 case BYTE_CODE_PRE_DEC:
				 VM_OPERATION_PRE(--,BYTE_CODE_METAMETHOD_PRE_DEC,ptr_metamethod_members_aux->pre_dec);
				 continue;
			case BYTE_CODE_ADD_STORE:
				LOAD_OPS_SET_OPERATION;
				VM_OPERATION_ADD_SET();
				continue;
			case BYTE_CODE_SUB_STORE:
				LOAD_OPS_SET_OPERATION;
				VM_OPERATION_ARITHMETIC_SET(-=,BYTE_CODE_METAMETHOD_SUB_SET);
				continue;
			case BYTE_CODE_MUL_STORE:
				LOAD_OPS_SET_OPERATION;
				VM_OPERATION_ARITHMETIC_SET(*=,BYTE_CODE_METAMETHOD_MUL_SET);
				continue;
			case BYTE_CODE_DIV_STORE:
				LOAD_OPS_SET_OPERATION;
				VM_OPERATION_DIV_SET();
				continue;
			case BYTE_CODE_MOD_STORE:
				LOAD_OPS_SET_OPERATION;
				VM_OPERATION_MOD_SET();
				continue;
			case BYTE_CODE_BITWISE_AND_STORE:
				VM_OPERATION_BINARY_SET(&=,BYTE_CODE_METAMETHOD_ADD_SET);
				continue;
			case BYTE_CODE_BITWISE_OR_STORE:
				LOAD_OPS_SET_OPERATION;
				VM_OPERATION_BINARY_SET(|=,BYTE_CODE_METAMETHOD_OR_SET);
				continue;
			case BYTE_CODE_BITWISE_XOR_STORE:
				LOAD_OPS_SET_OPERATION;
				VM_OPERATION_BINARY_SET(^=,BYTE_CODE_METAMETHOD_XOR_SET);
				continue;
			case BYTE_CODE_SHL_STORE:
				LOAD_OPS_SET_OPERATION;
				VM_OPERATION_BINARY_SET(<<=,BYTE_CODE_METAMETHOD_SHL_SET);
				continue;
			case BYTE_CODE_SHR_STORE:
				LOAD_OPS_SET_OPERATION;
				VM_OPERATION_BINARY_SET(>>=,BYTE_CODE_METAMETHOD_SHR_SET);
				continue;
			case BYTE_CODE_STORE_CONST:
			case BYTE_CODE_STORE:
				if(!vm_store(
					vm
					,calling_function
					,instruction
				)){
					goto lbl_exit_function;\
				}
				if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){
					data->stk_vm_current=stk_start;
				}
				continue;

			//----- immediate call
			case  BYTE_CODE_CALL: // immediate call this
			case  BYTE_CODE_SUPER_CALL:
			case  BYTE_CODE_THIS_CALL: // immediate call this
			case  BYTE_CODE_INDIRECT_THIS_CALL:
			case  BYTE_CODE_INDIRECT_LOCAL_CALL: // call from idx var
			case  BYTE_CODE_INDIRECT_GLOBAL_CALL: // call from idx var
			case  BYTE_CODE_STACK_CALL: // stack call
			case  BYTE_CODE_CONSTRUCTOR_CALL:
			case  BYTE_CODE_MEMBER_CALL: // calling function after all of args are processed...
				if(!vm_call(
						vm
						,this_object
						,calling_function
						,instruction
						,_stk_local_var
				)){
					goto lbl_exit_function;\
				}

				if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){
					data->stk_vm_current=stk_start;
				}
				continue;
			 case  BYTE_CODE_RET:
				for(stk_var=data->stk_vm_current-1;stk_var>=stk_start;stk_var--){ // can return something. value is +1 from stack
					// if scriptvariable and in the zeros list, deattach
					if(stk_var->properties & STK_PROPERTY_SCRIPT_OBJECT){
						so_aux=(ScriptObject *)stk_var->value;

						//special case for constant string object (they don't are shared elements)
						if(so_aux->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_STRING && (so_aux->shared_pointer==NULL)){
							// if is not shared is constant...
							so_aux=ScriptObjectString::newScriptObjectString(data->zs,so_aux->toString());
							stk_var->properties=STK_PROPERTY_SCRIPT_OBJECT;
							stk_var->value=(zs_int)so_aux;
						}else{
							if(so_aux->shared_pointer->data.n_shares<=1){ // was created here... remove share data

								// deatch from list of created scope block to avoid double dealloc on pop scope
								if(so_aux->shared_pointer->data.n_shares == 0){
									if(vm_deattach_shared_node(
											vm
											,&so_aux->shared_pointer->data.created_scope_block->unreferenced_objects
											,so_aux->shared_pointer
									)==false){
										goto lbl_exit_function;
									}
								}
								free(so_aux->shared_pointer);
								so_aux->shared_pointer=NULL;
							}
							//-------------------------------------------------------------------
						}
					}
				}
				goto lbl_return_function;
			 case  BYTE_CODE_NEW_OBJECT_BY_TYPE:

				 	so_aux=NEW_OBJECT_VAR_BY_TYPE_IDX(data->script_type_factory,instruction->value_op1);

					if(!vm_create_shared_script_object(vm,so_aux)){
						goto lbl_exit_function;
					}

					if(so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS){
						so_class_aux1=(ScriptObjectClass *)so_aux;
						so_class_aux1->info_function_new=calling_function;
						so_class_aux1->instruction_new=instruction;
					}
					(*data->stk_vm_current++)={(zs_int)so_aux,STK_PROPERTY_SCRIPT_OBJECT};
					continue;
			 case  BYTE_CODE_NEW_OBJECT_BY_VALUE:
				 	 VM_POP_STK_ONE;
				 	 if(STK_VALUE_IS_TYPE(stk_result_op1)){
				 		sc_aux1=data->script_type_factory->getScriptType(stk_result_op1->value);
						if(!data->script_type_factory->isScriptTypeInstanceable(stk_result_op1->value)){
							VM_STOP_EXECUTE("'%s' type is not object instanceable",sc_aux1->getTypeName());
						}

				 		 symbol_aux=NULL;

				 		 so_aux=NEW_OBJECT_VAR_BY_TYPE_IDX(data->script_type_factory,stk_result_op1->value);

						if(!vm_create_shared_script_object(vm,so_aux)){
							goto lbl_exit_function;
						}

						data->stk_vm_current->value=(zs_int)so_aux;
						data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;
						data->stk_vm_current++;

						if(so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS){ // custom object by user

							so_class_aux1=(ScriptObjectClass *)so_aux;

							so_class_aux1->info_function_new=calling_function;
							so_class_aux1->instruction_new=instruction;

							// check for constructor
							symbol_aux=sc_aux1->getSymbolMemberFunction(CONSTRUCTOR_FUNCTION_NAME);

							 if(symbol_aux != NULL){
								 data->stk_vm_current->value=(zs_int)symbol_aux;
								 data->stk_vm_current->properties=STK_PROPERTY_MEMBER_FUNCTION;
								 data->stk_vm_current++;
							 }
						}

						if(symbol_aux == NULL){
							VM_PUSH_STK_UNDEFINED;
						}

				 	 }else{
				 		VM_STOP_EXECUTE("var '%s' expected as 'type' but it was '%s'"
				 				,SFI_GET_SYMBOL_NAME(calling_function,instruction)
								, stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1)
						);
				 	 }
				 	 continue;
			 case BYTE_CODE_NEW_VECTOR: // Create new vector...
					so_aux=ZS_NEW_OBJECT_VECTOR(data->zs);
					if(!vm_create_shared_script_object(vm,so_aux)){
						goto lbl_exit_function;
					}
					data->stk_vm_current->value=(zs_int)so_aux;
					data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;
					data->stk_vm_current++;
					continue;
			 case  BYTE_CODE_NEW_OBJECT: // Create new object...
				 	so_aux=ZS_NEW_OBJECT_OBJECT(data->zs);
					if(!vm_create_shared_script_object(vm,so_aux)){
						goto lbl_exit_function;
					}
					(*data->stk_vm_current++)={(zs_int)so_aux,STK_PROPERTY_SCRIPT_OBJECT};
					continue;

			 case  BYTE_CODE_NEW_STRING: // Create new string...
				 so_aux= ScriptObjectString::newScriptObjectString(data->zs,instruction->getConstantValueOp2ToString(false));
					if(!vm_create_shared_script_object(vm,so_aux)){
						goto lbl_exit_function;
					}

					(*data->stk_vm_current++)={(zs_int)so_aux,STK_PROPERTY_SCRIPT_OBJECT};
					continue;
			 case  BYTE_CODE_DELETE:
					VM_POP_STK_ONE;
					//script_var
					if(stk_result_op1->properties & STK_PROPERTY_PTR_STK){
						stk_result_op1=(StackElement *)stk_result_op1->value;
					}

					if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
						so_class_aux1=NULL;

						so_aux = (ScriptObject *)(stk_result_op1)->value;

						if(!vm_unref_shared_script_object(vm,so_aux,NULL)){
							goto lbl_exit_function;
						}

						if(so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS)
						{ // max ...
							so_class_aux1=(ScriptObjectClass *)so_aux;

							if(so_class_aux1->isCreatedByContructor()){
								so_class_aux1->deleteNativeObjectOnDestroy(true);
							}
						}
						STK_SET_UNDEFINED(stk_result_op1);
					}
					continue;
			 case BYTE_CODE_RESET_STACK:
				 data->stk_vm_current=stk_start;
				 continue;
			case BYTE_CODE_IT_INIT:
				VM_POP_STK_TWO;
				vm_iterator_init(vm
						,calling_function
						,instruction
						,stk_result_op1
						,stk_result_op2);
				if(data->vm_error) {
					goto lbl_exit_function;
				}
				// always it does a reset
				data->stk_vm_current=stk_start;
				continue;
			 case BYTE_CODE_IN:
				 VM_POP_STK_TWO;
				 if(vm_perform_in_operator(
						 vm
						 ,calling_function
						 ,instruction
						 ,stk_result_op1
						 ,stk_result_op2)==false){
					goto lbl_exit_function;
				 }
				 continue;
				case BYTE_CODE_LOAD_TYPE:
					VM_PUSH_STK_TYPE(instruction->value_op2);
					continue;
				case BYTE_CODE_PUSH_VECTOR_ITEM:
					VM_POP_STK_ONE; // only pops the value, the last is the vector variable itself

					if((data->stk_vm_current-1)->properties & STK_PROPERTY_SCRIPT_OBJECT){
						so_aux = (ScriptObject *)(data->stk_vm_current-1)->value;
						if(so_aux->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_VECTOR){ // push value ...
							// op1 is now the src value ...
							stk_src=stk_result_op1;
							if(stk_src->properties & STK_PROPERTY_PTR_STK){
								stk_src=(StackElement *)stk_result_op1->value;
							}

							stk_dst=((ScriptObjectVector *)so_aux)->pushNewUserSlot();
						}
					}

					if(so_aux==NULL){
						VM_STOP_EXECUTEF("Expected vector object");
					}
					VM_SET_CONTAINER_ELEMENT;
					continue;
				case BYTE_CODE_PUSH_OBJECT_ITEM:
					VM_POP_STK_TWO; // first must be a string that describes variable name and the other the variable itself ...

					stk_var=(data->stk_vm_current-1);
					if(STK_IS_SCRIPT_OBJECT_OBJECT(stk_var) == 0){
						VM_STOP_EXECUTE("Expected object but is type '%s'",stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_var));
					}

					so_aux = (ScriptObject *)stk_var->value;

					if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1) == 0){
						VM_STOP_EXECUTE("Internal: Expected stk_result_op1 as string but is type '%s'"
								,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1)
						);
					}

					//const char *str = (const char *)stk_result_op1->value;
					stk_src=stk_result_op2;
					if((stk_var =so_aux->addProperty(stk_to_str(VM_STR_AUX_PARAM_0,data->zs, stk_result_op1),data->vm_error_str))==NULL){
						VM_STOP_EXECUTEF(data->vm_error_str.c_str());
					}

					stk_dst=stk_var;
					VM_SET_CONTAINER_ELEMENT;
					continue;
				case BYTE_CODE_PUSH_STK_THIS: // load variable ...
					VM_PUSH_STK_PTR(this_object->getThisProperty());
					continue;
				case BYTE_CODE_PUSH_STK_MEMBER_VAR: // direct load
					VM_PUSH_STK_PTR(this_object->getBuiltinElementAt(instruction->value_op2));
					continue;
				// access vector (i.e vec[1]) or access object (i.e obj["aa"])
				case BYTE_CODE_PUSH_STK_VECTOR_ITEM:
				case BYTE_CODE_LOAD_VECTOR_ITEM:
					VM_POP_STK_TWO;
					so_aux=NULL;
					if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op1)){
						stk_result_op1 = ((ScriptObjectVarRef *)stk_result_op1->value)->getStackElementPtr();
					}
					stk_var=NULL;
					// determine object ...
					if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
						so_aux=(ScriptObject *)stk_result_op1->value;
						if(		   so_aux->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_VECTOR
								|| so_aux->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_OBJECT
								|| so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS
						){

							if(so_aux->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_VECTOR){
								index_aux1=0;

								if(STK_VALUE_IS_ZS_INT(stk_result_op2)){ \
									index_aux1=stk_result_op2->value;
								}else if(STK_VALUE_IS_ZS_FLOAT(stk_result_op2)){ \
									index_aux1=*((zs_float*)&stk_result_op2->value);
								}else{
									VM_STOP_EXECUTEF("Expected index value for vector access");
								}

								if(index_aux1 >= (int)((ScriptObjectVector *)so_aux)->length() || index_aux1 < 0){
									VM_STOP_EXECUTEF("Error accessing vector, index out of bounds");
								}

								if((stk_var =((ScriptObjectVector *)so_aux)->getUserElementAt(index_aux1))==NULL){
									goto lbl_exit_function;
								} \
							}
							else{
								if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)==0){ \
									VM_STOP_EXECUTEF("Expected string for object access");
								}
								// Save STK_PROPERTY_SLOT if not BYTE_CODE_LOAD_VECTOR_ITEM
								stk_var = ((ScriptObjectObject *)so_aux)->getProperty(
										stk_to_str(VM_STR_AUX_PARAM_0,data->zs, stk_result_op2)
								);
								if(stk_var == NULL){
									if(instruction->byte_code == BYTE_CODE_PUSH_STK_VECTOR_ITEM){
										if((stk_var =((ScriptObjectObject *)so_aux)->addProperty(
												stk_to_str(VM_STR_AUX_PARAM_0,data->zs, stk_result_op2), data->vm_error_str)
										)==NULL){
											VM_STOP_EXECUTEF(data->vm_error_str.c_str());
										}
									}
								}
							}
							if(instruction->byte_code == BYTE_CODE_LOAD_VECTOR_ITEM){
								*data->stk_vm_current++=*stk_var;
							}else{
								if(instruction->properties & INSTRUCTION_PROPERTY_OBJ_ITEM_TO_STORE){
									data->stk_vm_current->value=(zs_int)(new ContainerSlotStore(so_aux,(zs_int)str_symbol_aux1,stk_var));
									data->stk_vm_current->properties=STK_PROPERTY_CONTAINER_SLOT_STORE;
									data->stk_vm_current++;
								}else{
									VM_PUSH_STK_PTR(stk_var);
								}
							}
							continue;
						}else if(so_aux->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_STRING){
							if(STK_VALUE_IS_ZS_INT(stk_result_op2)==false){ \
								VM_STOP_EXECUTEF("Expected integer index for String access");
							}

							zs_char *ptr_char=(zs_char *)&((zs_string *)((ScriptObjectString *)so_aux)->value)->c_str()[stk_result_op2->value];
							if(instruction->byte_code == BYTE_CODE_LOAD_VECTOR_ITEM){
								data->stk_vm_current->value=((zs_int)(*ptr_char));
								data->stk_vm_current->properties=STK_PROPERTY_ZS_INT;
							}else{ // push stk
								data->stk_vm_current->value=(zs_int)ptr_char;
								data->stk_vm_current->properties=STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR;
							}
							data->stk_vm_current++;
							continue;
						}else{
							VM_STOP_EXECUTEF("Expected String,Vector or Object for access '[]' operation"); \
						}
					}else{
						VM_STOP_EXECUTE("Expected object for access '[]' operation but it was type '%s'",stk_to_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1)); \
					}
					continue;
				case BYTE_CODE_LOAD_REF:
					*data->stk_vm_current++=*STK_GET_STK_VAR_REF(_stk_local_var+instruction->value_op2);
					continue;
				case BYTE_CODE_LOAD_THIS: // load variable ...
					*data->stk_vm_current++=*this_object->getThisProperty();
					continue;
				case BYTE_CODE_LOAD_THIS_FUNCTION:// direct load
					symbol_aux=(Symbol *)this_object->getScriptType()->getSymbolMemberFunction(((Symbol *)instruction->value_op2)->name);
					if(symbol_aux==NULL){ // it calls overrided function (top-most)
						 VM_STOP_EXECUTE("Error load 'this.%s': Cannot find '%s::%s' member function"
								,((Symbol *)instruction->value_op2)->name
								,this_object->getScriptType()->str_script_type
								,((Symbol *)instruction->value_op2)->name
						);
					 }

					so_aux=ZS_NEW_OBJECT_MEMBER_FUNCTION(data->zs,this_object,(ScriptFunction *)(symbol_aux->ref_ptr));

					 if(!vm_create_shared_script_object(vm,so_aux)){
							goto lbl_exit_function;
					 }
					 data->stk_vm_current->value=(zs_int)so_aux;
					 data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;
					 data->stk_vm_current++;
					continue;
				case BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT:
					so_aux=(ScriptObjectClass *)((data->stk_vm_current-1)->value);
					if(instruction->value_op2 == ZS_IDX_UNDEFINED){
						VM_PUSH_STK_UNDEFINED;
					}else{
						data->stk_vm_current->value=(zs_int) so_aux->getScriptType()->scope_script_type->symbol_functions->items[instruction->value_op2];
						data->stk_vm_current->properties=STK_PROPERTY_MEMBER_FUNCTION;
						data->stk_vm_current++;
					}
					continue;
				case BYTE_CODE_LOAD_THIS_VARIABLE:
				case BYTE_CODE_PUSH_STK_THIS_VARIABLE:
				case BYTE_CODE_PUSH_STK_OBJECT_ITEM:
				case BYTE_CODE_LOAD_OBJECT_ITEM:
					if(!vm_load_field(
							vm
							,this_object
							,calling_function
							,&instruction_it
					)){
						goto lbl_exit_function;
					}
					continue;
			case BYTE_CODE_FIND_VARIABLE:
			case BYTE_CODE_UNRESOLVED_CALL:
				vm_throw_error_cannot_find_symbol(
					vm
					, calling_function
					, instruction
				);
				goto lbl_exit_function;
			case BYTE_CODE_END_FUNCTION:
			default:
				goto lbl_exit_function;
			}

		 }

	lbl_exit_function:
		// default return null
		 STK_SET_UNDEFINED(data->stk_vm_current);
		// reset stack and set last stk return null;
		data->stk_vm_current=_stk_local_var;

	lbl_return_function:


		//=========================
		// POP STACK


		while (
			(VM_CURRENT_SCOPE_FUNCTION->current_scope_block > VM_CURRENT_SCOPE_FUNCTION->first_scope_block)
		){
			vm_pop_scope(vm); // do not check removeEmptySharedPointers to have better performance
		}

		--data->vm_current_scope_function;


		// POP STACK
		//=========================
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



	bool vm_string_compare(
		VirtualMachine *vm
		, StackElement *_stk1
		, StackElement *_stk2
		, ByteCodeMetamethod _byte_code_metamethod
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		zs_string str1=stk_to_str(data->zs,_stk1);
		zs_string str2=stk_to_str(data->zs, _stk2);

		switch(_byte_code_metamethod){
		case BYTE_CODE_METAMETHOD_EQU:
			return ZS_STRCMP(str1.c_str(),==,str1.c_str());
		case BYTE_CODE_METAMETHOD_LT:
			return ZS_STRCMP(str1.c_str(),<,str1.c_str());
		case BYTE_CODE_METAMETHOD_GT:
			return ZS_STRCMP(str1.c_str(),>,str1.c_str());
		case BYTE_CODE_METAMETHOD_LTE:
			return ZS_STRCMP(str1.c_str(),<=,str1.c_str());
		case BYTE_CODE_METAMETHOD_GTE:
			return ZS_STRCMP(str1.c_str(),>=,str1.c_str());
		}

		return false;
	}	
}

#include "vm_byte_code_call_metamethod.cpp"
#include "vm_byte_code_call.cpp"
#include "vm_byte_code_field.cpp"
#include "vm_byte_code_iterator.cpp"
#include "vm_byte_code_store.cpp"
#include "vm_function.cpp"
