/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */



namespace zetscript{

	StackElement *vm_load_this_element(
		VirtualMachine			* vm
		,ScriptObject			* this_object
		,ScriptFunction 		* calling_function
		,Instruction			*instruction
		,short idx
	);

	void vm_execute_function_script(
			VirtualMachine			* vm,
			ScriptObject			* this_object,
			ScriptFunction 			* calling_function,
			StackElement 		  	* _stk_local_var
	    ){

		VirtualMachineData *data = (VirtualMachineData*)vm->data;
		Instruction * instructions = calling_function->instructions;//calling_instruction;
		Instruction *instruction=instructions; // starting instruction

		if(IDX_VM_CURRENT_SCOPE_FUNCTION >= VM_FUNCTION_CALL_MAX){
			VM_ERROR_AND_RETF("Reached max stack");
		}

		ScriptObject *so_aux=NULL;
		StackElement *stk_result_op1=NULL;
		StackElement *stk_result_op2=NULL;
		StackElement stk_aux;
		StackElement *stk_var=NULL;
		Symbol 		 *symbol_aux;
		StackElement stk_var_copy;
		const char *str_symbol=NULL;
		//------------------------------------------------
		// STORE
		zs_vector 		*		store_lst_setter_functions=NULL;
		int 					n_element_left_to_store=0;
		MetamethodMembers *		ptr_metamethod_members_aux=NULL;
		void			*		ptr_ptr_void_ref=NULL;
		StackElement    *		stk_load_multi_var_src=NULL;
		MemberProperty	* 		member_property=NULL;
		ContainerSlotStore *	container_slot_store=NULL;
		ScriptObject 	*		container_slot_store_object=NULL;
		zs_int 					container_slot_store_id_slot=0;
		//------------------------------------------------
		// SFCALL
		StackElement	*sf_call_stk_function_ref=NULL;
		ScriptFunction 	*sf_call_script_function = NULL;

		int			 	 sf_call_n_args=0; // number arguments will pass to this function
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
		uint32_t msk_properties=0;
		StackElement *stk_dst=NULL;
		StackElement *stk_src=NULL;
		zs_int *stk_src_ref_value=NULL;
		zs_int *stk_dst_ref_value=NULL;
		uint16_t stk_src_properties=0;
		void *stk_src_ref_value_copy_aux=NULL;

		Instruction *instruction_it=instructions;
		StackElement *stk_start=_stk_local_var+calling_function->local_variables->count;   // <-- here starts stk for aux vars for operations ..

		data->stk_vm_current = stk_start;
		//data->vm_idx_call++;

		if(((stk_start-data->vm_stack)+calling_function->min_code_stack_needed)>=VM_STACK_MAX){
			data->vm_error_max_stack_reached=true;
			VM_STOP_EXECUTEF("Error MAXIMUM stack size reached");
		}

#ifdef __DEBUG__
		ZS_LOG_DEBUG("Executing function %s ...",calling_function->symbol.name.c_str());
#endif

		// Init local vars ...
		//data->vm_current_scope_function->current_scope_block = NULL;
		data->vm_current_scope_function->current_scope_block=data->vm_current_scope_function->first_scope_block;
		data->vm_current_scope_function->stk_local_vars=_stk_local_var;

		// preserve global scope vars
		if((calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN)){

			if(calling_function->scope_script_function->symbol_variables->count>0){
				VM_PUSH_SCOPE(calling_function->scope_script_function);
			}

			// reset local variables symbols (not arg symbols)
			for(int i=calling_function->params_len;i<calling_function->local_variables->count;i++){
				STK_SET_UNDEFINED(_stk_local_var+i);
			}
		}
		data->vm_current_scope_function++;

		//-----------------------------------------------------------------------------------------------------------------------
		//
		// MAIN LOOP
		//
		 for(;;){ // foreach byte code instruction ...

			instruction = instruction_it++;

			switch(instruction->byte_code){
			default:
				VM_STOP_EXECUTE("byte code '%s' not implemented",byte_code_to_str(instruction->byte_code));
			case BYTE_CODE_END_FUNCTION:
				goto lbl_exit_function;
			case BYTE_CODE_PUSH_STK_GLOBAL: // load variable ...
				VM_PUSH_STK_PTR(data->vm_stack + instruction->value_op2);
				continue;
			case BYTE_CODE_PUSH_STK_LOCAL: // load variable ...
				VM_PUSH_STK_PTR(_stk_local_var+instruction->value_op2);
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
					ScriptObject *obj=(ScriptObject *)stk_result_op1->value;
					if(		   obj->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_VECTOR
							|| obj->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_OBJECT
							|| obj->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS
					){

						if(obj->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_VECTOR){
							ScriptObjectVector *so_vector=(ScriptObjectVector *)obj;
							zs_int index=0;

							if(STK_VALUE_IS_ZS_INT(stk_result_op2)){ \
								index=stk_result_op2->value;
							}else if(STK_VALUE_IS_ZS_FLOAT(stk_result_op2)){ \
								index=*((zs_float*)&stk_result_op2->value);
							}else{
								VM_STOP_EXECUTEF("Expected index value for vector access");
							}

							if(index >= (int)so_vector->length()){
								VM_STOP_EXECUTEF("Error accessing vector, index out of bounds");
							}

							// TODO: save STK_PROPERTY_SLOT if not BYTE_CODE_LOAD_VECTOR_ITEM
							if((stk_var =so_vector->getUserElementAt(index))==NULL){
								goto lbl_exit_function;
							} \
						}
						else{
							ScriptObjectObject *so_object = (ScriptObjectObject *)obj;
							if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)==0){ \
								VM_STOP_EXECUTEF("Expected string for object access");
							}
							// TODO: save STK_PROPERTY_SLOT if not BYTE_CODE_LOAD_VECTOR_ITEM
							stk_var = so_object->getProperty(stk_to_str(data->zs, stk_result_op2));
							if(stk_var == NULL){
								if(instruction->byte_code == BYTE_CODE_PUSH_STK_VECTOR_ITEM){
									if((stk_var =so_object->addProperty(stk_to_str(data->zs, stk_result_op2), data->vm_error_str))==NULL){
										VM_STOP_EXECUTEF(data->vm_error_str.c_str());
									}
								}
							}
						}
						if(instruction->byte_code == BYTE_CODE_LOAD_VECTOR_ITEM){
							*data->stk_vm_current++=*stk_var;
						}else{
							if(instruction->properties & INSTRUCTION_PROPERTY_OBJ_ITEM_TO_STORE){
								data->stk_vm_current->value=(zs_int)(new ContainerSlotStore(so_aux,(zs_int)str_symbol,stk_var));
								data->stk_vm_current->properties=STK_PROPERTY_CONTAINER_SLOT_STORE;
								data->stk_vm_current++;
							}else{
								VM_PUSH_STK_PTR(stk_var);
							}
						}
						continue;
					}else if(obj->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_STRING){
						ScriptObjectString *so_string=(ScriptObjectString *)stk_result_op1->value;

						if(STK_VALUE_IS_ZS_INT(stk_result_op2)==false){ \
							VM_STOP_EXECUTEF("Expected integer index for String access");
						}


						zs_char *ptr_char=(zs_char *)&((zs_string *)so_string->value)->c_str()[stk_result_op2->value];
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
						VM_STOP_EXECUTEF("Expected String,Vector or Object for access '[]' opertaion"); \
					}
				}else{
					VM_STOP_EXECUTE("Expected object for access '[]' operation but it was type '%s'",stk_to_str(data->zs,stk_result_op1).c_str()); \

				}
				continue;
			// load
			case BYTE_CODE_LOAD_GLOBAL: // load variable ...
				*data->stk_vm_current++=*(data->vm_stack+instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_LOCAL: // load variable ...
				*data->stk_vm_current++=*(_stk_local_var+instruction->value_op2);
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
							,((Symbol *)instruction->value_op2)->name.c_str()
							,this_object->getScriptType()->str_script_type.c_str()
							,((Symbol *)instruction->value_op2)->name.c_str()
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
					data->stk_vm_current->value= so_aux->getScriptType()->scope_script_type->symbol_functions->items[instruction->value_op2];
					data->stk_vm_current->properties=STK_PROPERTY_MEMBER_FUNCTION;
					data->stk_vm_current++;
				}
				continue;
			case BYTE_CODE_PUSH_STK_THIS_VARIABLE:
			case BYTE_CODE_LOAD_THIS_VARIABLE:
				stk_var=NULL;
				so_aux=this_object; // take this as default
				stk_var=vm_load_this_element(vm,this_object,calling_function,instruction,instruction->value_op2);
				goto find_element_object;

			case BYTE_CODE_PUSH_STK_OBJECT_ITEM:
			case BYTE_CODE_LOAD_OBJECT_ITEM:
load_next_element_object:
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
							,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
							,zs_strutils::starts_with(stk_to_typeof_str(data->zs,stk_result_op1),"type@")? ". If you are trying to call/access static member of class you need to use static access operator (i.e '::') instead of member access operator (i.e '.')":""
						);
					}else{ // from calling
						VM_STOP_EXECUTE(
							"Cannot perform access '.%s' from variable type '%s'"
							,SFI_GET_SYMBOL_NAME(calling_function,instruction)
							,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
						);
					}
				}

				so_aux=((ScriptObject *)stk_result_op1->value);


				if(so_aux == NULL)
				{
					VM_STOP_EXECUTE("var '%s' is not scriptvariable",SFI_GET_SYMBOL_NAME(so_aux,(instruction-1)));
				}

find_element_object:

				//if(stk_var==NULL)
				{ // load element from object or dynamic member element from this
					str_symbol=(char *)SFI_GET_SYMBOL_NAME(calling_function,instruction);

					//
					ScriptType *sc=so_aux->getScriptType();
					Symbol *sf_member=sc->getSymbolMemberFunction(str_symbol);
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

						ScriptObjectMemberFunction *somf=ZS_NEW_OBJECT_MEMBER_FUNCTION(data->zs,so_aux,(ScriptFunction *)sf_member->ref_ptr);

						 if(!vm_create_shared_script_object(vm,somf)){
							goto lbl_exit_function;
						 }

						data->stk_vm_current->value=(zs_int)somf;
						data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;
						data->stk_vm_current++;

						continue;

					}else if((stk_var=so_aux->getProperty(str_symbol)) == NULL){

						if(instruction->properties & INSTRUCTION_PROPERTY_CALLING_FUNCTION){
							VM_STOP_EXECUTE("Error call function '...%s.%s(...)', where '%s' is type '%s'. Member function '%s::%s' is not defined"
								,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
								,str_symbol
								,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
								,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
								,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
								,str_symbol
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
							ScriptType *sc_type=so_aux->getScriptType();

							// exceptions
							if(sc_type->idx_script_type<IDX_TYPE_SCRIPT_OBJECT_OBJECT || sc_type->idx_script_type>IDX_TYPE_SCRIPT_OBJECT_OBJECT){
								// Properties from native types or custom internal type through script side cannot be added if not exist, so if not exist throw error.
								if(so_aux->getScriptType()->properties & SCRIPT_TYPE_PROPERTY_C_OBJECT_REF){
									VM_STOP_EXECUTE("Cannot store '...%s.%s', where '%s' is type '%s'. %s property '%s::%s' is not defined"
										,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
										,str_symbol
										,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
										,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
										,sc_type->idx_script_type>IDX_TYPE_SCRIPT_OBJECT_OBJECT?"Native type":"Type"
										,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
										,str_symbol
									);
								}
							}

							// create new property initialized as undefined
							if((stk_var=so_aux->addProperty((const char *)str_symbol, data->vm_error_str))==NULL){
								VM_STOP_EXECUTEF(data->vm_error_str.c_str());
							}

							if(instruction->properties & INSTRUCTION_PROPERTY_OBJ_ITEM_TO_STORE){
								data->stk_vm_current->value=(zs_int)(new ContainerSlotStore(so_aux,(zs_int)str_symbol,stk_var));
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
						continue;
					}else{

						if((instruction->properties & INSTRUCTION_PROPERTY_CALLING_FUNCTION) && ((stk_var->properties & STK_PROPERTY_FUNCTION)==0)){

							VM_STOP_EXECUTE("Error call function '...%s.%s(...)', where '%s' is type '%s'. Expected '%s::%s' as a function but it is type '%s'"
								,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
								,(const char *)str_symbol
								,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
								,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
								,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
								,(const char *)str_symbol
								,stk_to_typeof_str(data->zs,stk_var).c_str()
							);
						}

						// member property. ... if getter, get the value itself and evaluate
						if(
						   (stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY)!=0
						 ){
							StackMemberProperty *stk_mp=(StackMemberProperty *)stk_var->value;

							if(stk_mp->member_property->metamethod_members.getter!=NULL){
								VM_INNER_CALL_ONLY_RETURN(
										stk_mp->so_object
										,((ScriptFunction *)stk_mp->member_property->metamethod_members.getter->ref_ptr)
										,stk_mp->member_property->metamethod_members.getter->name.c_str()
										,true
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
									continue;
								}
							}

						}
					}
				}

				if((instruction+1)->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM){ // fast load access without pass through switch instruction
					*data->stk_vm_current++=*stk_var;
					instruction++; // we have to inc current instruction...
					instruction_it++; //... and instruction iterator
					goto load_next_element_object;
				}

				// load its value for write
				if(instruction->byte_code == BYTE_CODE_PUSH_STK_OBJECT_ITEM || instruction->byte_code == BYTE_CODE_PUSH_STK_THIS_VARIABLE){
					if(instruction->properties & INSTRUCTION_PROPERTY_OBJ_ITEM_TO_STORE){
						data->stk_vm_current->value=(zs_int)(new ContainerSlotStore(so_aux,(zs_int)str_symbol,stk_var));
						data->stk_vm_current->properties=STK_PROPERTY_CONTAINER_SLOT_STORE;
						data->stk_vm_current++;
					}else{
						VM_PUSH_STK_PTR(stk_var);
					}
				}
				else{ // only read
					*data->stk_vm_current++=*stk_var;
				}
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
					VM_STOP_EXECUTE("Expected object but is type '%s'",stk_to_typeof_str(data->zs,stk_var).c_str());
				}

				so_aux = (ScriptObject *)stk_var->value;

				if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1) == 0){
					VM_STOP_EXECUTE("Internal: Expected stk_result_op1 as string but is type '%s'"
							,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
					);
				}

				//const char *str = (const char *)stk_result_op1->value;
				stk_src=stk_result_op2;
				if((stk_var =so_aux->addProperty(stk_to_str(data->zs, stk_result_op1),data->vm_error_str))==NULL){
					VM_STOP_EXECUTEF(data->vm_error_str.c_str());
				}

				stk_dst=stk_var;
				VM_SET_CONTAINER_ELEMENT;
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
							,stk_to_typeof_str(data->zs,stk_dst).c_str()
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
					StackMemberProperty *	stk_mp=(StackMemberProperty *)stk_dst->value;\
					if(stk_mp->member_property->metamethod_members.setters.count > 0){\
						store_lst_setter_functions=&stk_mp->member_property->metamethod_members.setters;\
					}else{ // setter not allowed because it has no setter
						VM_STOP_EXECUTE("'%s' not implements operator '=' (aka '_set')"
							,stk_mp->member_property->property_name.c_str()
						);
					}
				}

				if(store_lst_setter_functions!=NULL){

					StackElement *stk_vm_start=data->stk_vm_current;\
					StackElement *stk_arg=stk_vm_start+1; /*start from stk_src */\
					const char *__STR_SETTER_METAMETHOD__=byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_SET);\
					const char *__STR_AKA_SETTER_METAMETHOD__=byte_code_metamethod_to_operator_str(BYTE_CODE_METAMETHOD_SET);\
					*stk_arg=*stk_src;\
					StackMemberProperty *	stk_mp=NULL;\
					if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){
						stk_mp=(StackMemberProperty *)(stk_dst->value);
						so_aux=stk_mp->so_object;\
					}else{
						so_aux=(ScriptObjectClass *)stk_dst->value;
					}
					ScriptFunction *ptr_function_found=(ScriptFunction *)(((Symbol *)(((StackElement *)(store_lst_setter_functions->items[0]))->value))->ref_ptr);\
					if(so_aux->isNativeObject()){ /* because object is native, we can have more than one _setter */ \
						if((ptr_function_found=vm_find_native_function( \
								vm \
								,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
								,calling_function\
								,instruction\
								,false\
								,stk_mp==NULL?"_set":(ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(BYTE_CODE_METAMETHOD_SET,stk_mp->member_property->property_name)).c_str() /* symbol to find */\
								,stk_arg \
								,1))==NULL){ \
							if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){ \
								VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
										,so_aux->getScriptType()->str_script_type.c_str()\
										,stk_mp->member_property->property_name.c_str()\
										,__STR_SETTER_METAMETHOD__\
								);\
							}else{\
								VM_STOP_EXECUTE("Type '%s' does not implement '%s' metamethod" \
										,so_aux->getScriptType()->str_script_type.c_str() \
										,__STR_SETTER_METAMETHOD__\
								);\
							}\
						}\
					}else if(store_lst_setter_functions->count>1){ /* it has overrided metamethods */\
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
								,so_aux\
								,ptr_function_found\
								,calling_function\
								,instruction\
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

					if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_src)){
						stk_src=(StackElement *)((STK_GET_STK_VAR_REF(stk_src)->value));
					}

					// TODO: get stk_dst if STK_PROPERTY_SLOT
					StackElement old_stk_dst = *stk_dst; // save dst_var to check after assignment...

					stk_src_ref_value_copy_aux=NULL;/*copy aux in case of the var is c and primitive (we have to update value on save) */
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
							str_object->set(stk_to_str(data->zs, stk_src));
						}


					}else if(stk_src_properties & STK_PROPERTY_SCRIPT_OBJECT){// object we pass its reference

						so_aux=(ScriptObject *)stk_src->value;

						stk_dst->value=(intptr_t)so_aux;
						stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;


						if(container_slot_store_object!=NULL){

							// TODO: Save to map
							ScriptObject *src_object=so_aux;

							// Possibly cyclic reference
							if((so_aux->isNativeObject()==false) && (so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_VECTOR)){
								if(container_slot_store_object->getScriptType()->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_VECTOR){
									printf("\nAssing object %p type '%s' to slot '%i'"
											,container_slot_store_object
											,container_slot_store_object->getScriptType()->str_script_type.c_str()
											,(int)container_slot_store_id_slot
									);
								}else{
									printf("\nAssing object %p type '%s' to slot '%s'"
											,container_slot_store_object
											,container_slot_store_object->getScriptType()->str_script_type.c_str()
											,(const char *)container_slot_store_id_slot
									);
								}
							}
						}

						if(!vm_share_script_object(vm,so_aux)){
							goto lbl_exit_function;
						}

					}else{
						VM_STOP_EXECUTE("BYTE_CODE_STORE: (internal) cannot determine var type %s"
							,stk_to_typeof_str(data->zs,stk_src).c_str()
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
						if(STK_IS_SCRIPT_OBJECT_VAR_REF(chk_ref)){
							ScriptObjectVarRef *so_var_ref=(ScriptObjectVarRef *)chk_ref->value;
							//data->vm_idx_call=so_var_ref->getIdxCall(); // put the vm_idx_call where it was created
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
				else if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){
					data->stk_vm_current=stk_start;
				}/*else{
					*data->stk_vm_current++=*stk_dst;
				}*/
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
			case BYTE_CODE_TYPEOF:
				VM_POP_STK_ONE;
				*data->stk_vm_current++=stk_result_op1->typeOf();
				continue;
			case BYTE_CODE_ADD: // +
				VM_POP_STK_TWO;
				VM_OPERATION_ADD(BYTE_CODE_METAMETHOD_ADD);
				continue;
			case BYTE_CODE_SUB: // -
				VM_POP_STK_TWO;
				VM_OPERATION_ARITHMETIC(-,BYTE_CODE_METAMETHOD_SUB);
				continue;
			case BYTE_CODE_MUL: // *
				VM_POP_STK_TWO;
				VM_OPERATION_ARITHMETIC(*,BYTE_CODE_METAMETHOD_MUL);
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
				VM_OPERATION_BINARY(|, BYTE_CODE_METAMETHOD_OR);
				continue;
			 case BYTE_CODE_BITWISE_XOR: // ^
				VM_POP_STK_TWO;
				VM_OPERATION_BINARY(^, BYTE_CODE_METAMETHOD_XOR);
				continue;
			 case BYTE_CODE_SHR: // >>
				VM_POP_STK_TWO;
				VM_OPERATION_BINARY(>>,BYTE_CODE_METAMETHOD_SHR);
				continue;
			 case BYTE_CODE_SHL: // <<
				VM_POP_STK_TWO;
				VM_OPERATION_BINARY(<<, BYTE_CODE_METAMETHOD_SHL);
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
						bool b = data->script_type_factory->scriptTypeInheritsFrom(			//
								((ScriptObjectObject *)(stk_result_op1->value))->idx_script_type // A
								, instruction->value_op2		// B
						);
						VM_PUSH_STK_BOOLEAN(b);
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
					VM_STOP_EXECUTE("Expected boolean expression but it was '%s'",stk_to_typeof_str(data->zs,stk_result_op1).c_str());
				}
				if(stk_result_op1->value == 0){
					instruction_it=instruction+instruction->value_op2;
				}
				continue;
			 case BYTE_CODE_JT: // goto if true ... goes end to conditional.
				VM_POP_STK_ONE;
				if((stk_result_op1->properties & STK_PROPERTY_BOOL)==0){
					VM_STOP_EXECUTE("Expected boolean expression but it was '%s'",stk_to_typeof_str(data->zs,stk_result_op1).c_str());
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
			//----- immediate call
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
							,this_object->getScriptType()->str_script_type.c_str()
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
				  if(sofm->so_object==NULL){
					  VM_STOP_EXECUTE(
							  "Cannot call function member object '%s' stored in variable '%s' due its own object has been dereferenced"
							  ,sofm->so_function->name_script_function.c_str()
							  , SFI_GET_SYMBOL_NAME(calling_function,instruction));
				  }
				  sf_call_calling_object=sofm->so_object;
				  sf_call_script_function=sofm->so_function;
				  sf_call_is_member_function=true;
				}else{
					sf_call_is_member_function=false;
					if((sf_call_stk_function_ref->properties & (STK_PROPERTY_FUNCTION))==0){
						// error or continue
						if(instruction->byte_code== BYTE_CODE_CONSTRUCTOR_CALL){ // constructor was not found so we do nothing
							// reset stack to last
							if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){
								data->stk_vm_current=stk_start;
							}else{
								data->stk_vm_current=sf_call_stk_function_ref;//sf_call_stk_start_arg_call-sf_call_stk_start_function_object;
							}
							continue;
						}

						// indirect this call / member call or stk call
						if(instruction->byte_code==BYTE_CODE_INDIRECT_THIS_CALL){

							VM_STOP_EXECUTE("Cannot call 'this.%s' as type '%s'. 'this.%s' is not function"
									,SFI_GET_SYMBOL_NAME(calling_function,instruction)
									,stk_to_typeof_str(data->zs,sf_call_stk_function_ref).c_str()
									,SFI_GET_SYMBOL_NAME(calling_function,instruction)
							);

						}else if(instruction->byte_code==BYTE_CODE_MEMBER_CALL){
							VM_STOP_EXECUTE("Cannot call '%s'. '%s' is not function or not exist"
									,SFI_GET_SYMBOL_NAME(calling_function,instruction)
									,SFI_GET_SYMBOL_NAME(calling_function,instruction)
							);
						}else{ // STACK CALL
							VM_STOP_EXECUTE("Error trying to call a function from stack. StackElement value is '%s' as type '%s'"
									,stk_to_str(data->zs,sf_call_stk_function_ref).c_str()
									,stk_to_typeof_str(data->zs,sf_call_stk_function_ref).c_str()
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
						for(int i=0;i < sf_call_n_args;++i){
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
												,sf_call_script_function->name_script_function.c_str(),i+1);
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
										sc->set(stk_to_str(data->zs,stk_arg));
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
									if(i+1 >= (int)effective_args){
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
							VM_INNER_CALL_ONLY_RETURN(
								NULL
								,(ScriptFunction *)(((Symbol *)param->default_param_value.value)->ref_ptr)
								,((Symbol *)param->default_param_value.value)->name.c_str()
								,true
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
									,stk_to_typeof_str(data->zs,&param->default_param_value).c_str());
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
							,sf_call_calling_object
							,sf_call_script_function
							,calling_function
							,instruction
							,sf_call_stk_start_arg_call
							,sf_call_n_args

						);
					}catch(std::exception & ex){
						data->vm_error = true;
						data->vm_error_str=ex.what();
					}

					// restore stk_start_arg_call due in C args are not considered as local symbols (only for scripts)
					sf_call_n_local_symbols=sf_call_n_args;
				}

				if(data->vm_error == true){
					// if System::assert -> not add in callstack trace

					if((((calling_function->name_script_function=="assert" || calling_function->name_script_function=="error") && (sf_call_script_function->name_script_function=="errorNative"))
							|| ((calling_function->name_script_function=="eval") && (sf_call_script_function->name_script_function=="evalNative"))
					)==false
							&&
					data->vm_error_max_stack_reached==false
					){
						const char *str_class_owner=NULL;
						if(	(sf_call_script_function->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)!=0
								||
							(sf_call_script_function->properties & FUNCTION_PROPERTY_STATIC)!=0
						){
							str_class_owner=data->script_type_factory->getScriptType(sf_call_script_function->idx_script_type_owner)->str_script_type.c_str();
						}
						const char * file_src_call=SFI_GET_FILE(calling_function,instruction);
						data->vm_error_callstack_str+=zs_strutils::format(
							"\nat calling function %s%s%s (%sline:%i)" // TODO: get full symbol ?
							,str_class_owner==NULL?"":str_class_owner
							,str_class_owner==NULL?"":"::"
							,sf_call_script_function->name_script_function.c_str()
							,file_src_call?zs_strutils::format("file:%s ",file_src_call).c_str():""
							,SFI_GET_LINE(calling_function,instruction)
						);

					}
					goto lbl_exit_function;
				}

				// calcule returned stack elements
				sf_call_stk_return=(sf_call_stk_start_arg_call+sf_call_n_local_symbols); // +1 points to starting return...
				sf_call_n_returned_arguments_from_function=data->stk_vm_current-sf_call_stk_return;
				sf_call_return=INSTRUCTION_GET_RETURN_COUNT(instruction);

				// setup all returned variables from function
				CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(sf_call_stk_return,sf_call_n_returned_arguments_from_function,false)

				if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){
					data->stk_vm_current=stk_start;
					continue;
				}

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
				continue;
			 case  BYTE_CODE_RET:
				for(StackElement *stk_it=data->stk_vm_current-1;stk_it>=stk_start;stk_it--){ // can return something. value is +1 from stack
					// if scriptvariable and in the zeros list, deattach
					if(stk_it->properties & STK_PROPERTY_SCRIPT_OBJECT){
						//if(!IS_STK_THIS(stk_it)){

							ScriptObject *script_var=(ScriptObject *)stk_it->value;

							//special case for constant string object (they don't are shared elements)
							if(script_var->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_STRING && (script_var->shared_pointer==NULL)){
								// if is not shared is constant...
								ScriptObjectString *sc=ZS_NEW_OBJECT_STRING(data->zs);
								sc->set(script_var->toString());
								stk_it->properties=STK_PROPERTY_SCRIPT_OBJECT;
								stk_it->value=(zs_int)sc;
							}else{
								if(script_var->shared_pointer->data.n_shares<=1){ // was created here... remove

									// deatch from list of created scope block to avoid double dealloc on pop scope
									if(script_var->shared_pointer->data.n_shares == 0){
										if(vm_deattach_shared_node(vm,&script_var->shared_pointer->data.created_scope_block->unreferenced_objects,script_var->shared_pointer)==false){
											goto lbl_exit_function;
										}
									}
									free(script_var->shared_pointer);
									script_var->shared_pointer=NULL;
								}
								//int idx_call=IDX_VM_CURRENT_SCOPE_FUNCTION;
								/*if(script_var->shared_pointer->data.created_idx_call==data->vm_idx_call){ // the variable was created in this function ctx
									// only removes all refs from local
									if(script_var->shared_pointer->data.n_shares == 0){ // removes from zero shares to not to be removed automatically on pop scope
										if(vm_deattach_shared_node(vm,&data->zero_shares[script_var->shared_pointer->data.created_idx_call],script_var->shared_pointer)==false){
											goto lbl_exit_function;
										}
									}
									else{
										// other wise remove from shared vars
										if(vm_deattach_shared_node(vm,&data->shared_vars,script_var->shared_pointer)==false){
												goto lbl_exit_function;
										}
									}
									// free shared pointer and set as NULL and is returned as new object
									free(script_var->shared_pointer);
									script_var->shared_pointer=NULL;
								}*/

								//-------------------------------------------------------------------
							}
						//}
					}
				}
				goto lbl_return_function;
			 case  BYTE_CODE_NEW_OBJECT_BY_TYPE:

				 	so_aux=NEW_OBJECT_VAR_BY_TYPE_IDX(data->script_type_factory,instruction->value_op1);

					if(!vm_create_shared_script_object(vm,so_aux)){
						goto lbl_exit_function;
					}

					if(so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS){
						ScriptObjectClass *so_class_aux=(ScriptObjectClass *)so_aux;
						so_class_aux->info_function_new=calling_function;
						so_class_aux->instruction_new=instruction;
					}
					(*data->stk_vm_current++)={(zs_int)so_aux,STK_PROPERTY_SCRIPT_OBJECT};
					continue;
			 case  BYTE_CODE_NEW_OBJECT_BY_VALUE:
				 	 VM_POP_STK_ONE;
				 	 if(STK_VALUE_IS_TYPE(stk_result_op1)){
				 		ScriptType *sc=data->script_type_factory->getScriptType(stk_result_op1->value);
						if(!data->script_type_factory->isScriptTypeInstanceable(stk_result_op1->value)){
							VM_STOP_EXECUTE("'%s' type is not object instanceable",sc->getTypeName());
						}

				 		 Symbol *constructor_function=NULL;

				 		 so_aux=NEW_OBJECT_VAR_BY_TYPE_IDX(data->script_type_factory,stk_result_op1->value);

						if(!vm_create_shared_script_object(vm,so_aux)){
							goto lbl_exit_function;
						}

						data->stk_vm_current->value=(zs_int)so_aux;
						data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;
						data->stk_vm_current++;

						if(so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS){ // custom object by user

							ScriptObjectClass *so_class_aux=(ScriptObjectClass *)so_aux;

							so_class_aux->info_function_new=calling_function;
							so_class_aux->instruction_new=instruction;

							// check for constructor
							 constructor_function=sc->getSymbolMemberFunction(CONSTRUCTOR_FUNCTION_NAME);

							 if(constructor_function != NULL){
								 data->stk_vm_current->value=(zs_int)constructor_function;
								 data->stk_vm_current->properties=STK_PROPERTY_MEMBER_FUNCTION;
								 data->stk_vm_current++;
							 }
						}

						if(constructor_function == NULL){
							VM_PUSH_STK_UNDEFINED;
						}

				 	 }else{
				 		VM_STOP_EXECUTE("var '%s' expected as 'type' but it was '%s'",SFI_GET_SYMBOL_NAME(calling_function,instruction), stk_to_typeof_str(data->zs,stk_result_op1).c_str());
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
						ScriptObjectClass *script_object_class=NULL;
						StackElement *se=stk_result_op1;

						so_aux = (ScriptObject *)(se)->value;

						if(!vm_unref_shared_script_object(vm,so_aux,NULL)){
							goto lbl_exit_function;
						}

						if(so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS)
						{ // max ...
							script_object_class=(ScriptObjectClass *)so_aux;

							if(script_object_class->isCreatedByContructor()){
								script_object_class->deleteNativeObjectOnDestroy(true);
							}
						}
						STK_SET_UNDEFINED(se);
					}
					continue;
			 case BYTE_CODE_PUSH_SCOPE:
				VM_PUSH_SCOPE(instruction->value_op2);
				continue;
			 case BYTE_CODE_POP_SCOPE:
				 VM_POP_SCOPE;
				 continue;
			 case BYTE_CODE_RESET_STACK:
				 data->stk_vm_current=stk_start;
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

			case BYTE_CODE_FIND_VARIABLE:
			case BYTE_CODE_UNRESOLVED_CALL:
				{
					const char *ptr_str_symbol_to_find=SFI_GET_SYMBOL_NAME(calling_function,instruction);
					const char *str_end_class=NULL;

					if((str_end_class=strstr(ptr_str_symbol_to_find,"::"))!=NULL){ // static access
						char str_script_type[512]={0};

						strncpy(str_script_type,ptr_str_symbol_to_find,str_end_class-ptr_str_symbol_to_find);


						if(data->zs->getScriptTypeFactory()->getScriptType(str_script_type) == NULL){
							VM_STOP_EXECUTE(
									"type '%s' not exist"
									,str_script_type
							);
						}

						VM_STOP_EXECUTE(
								"static symbol '%s::%s' not exist"
								,str_script_type
								,str_end_class+2
						);
					}else{
						VM_STOP_EXECUTE(
								"Symbol '%s' not defined"
								,ptr_str_symbol_to_find
						);
					}
				}
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
		//if(data->vm_current_scope_function>VM_SCOPE_FUNCTION_MAIN){


			// preserve global objects
			//if(data->vm_current_scope_function > VM_SCOPE_FUNCTION_FIRST){

			while(
				(data->vm_current_scope_function->current_scope_block > data->vm_current_scope_function->first_scope_block)
				){
				VM_POP_SCOPE; // do not check removeEmptySharedPointers to have better performance
			}

			/*if((data->zero_shares+data->vm_idx_call)->first!=NULL){
				vm_remove_empty_shared_pointers(vm,data->vm_idx_call);
			}*/

			;
			//}

			--data->vm_current_scope_function;

			//--data->vm_idx_call;
		//}

		// POP STACK
		//=========================
	}

	StackElement *vm_load_this_element(
			VirtualMachine			* vm
			,ScriptObject			* this_object
			,ScriptFunction 		* calling_function
			,Instruction			*instruction
			,short	idx
	){
		VirtualMachineData *data = (VirtualMachineData*)vm->data;
		StackElement *stk_var=NULL;

		if(idx != ZS_IDX_UNDEFINED){
			stk_var = this_object->getBuiltinElementAt(idx);
		}

		if(stk_var != NULL && (stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY)){
			StackMemberProperty *stk_mp=(StackMemberProperty *)stk_var->value;
			if(stk_mp->member_property->metamethod_members.getter != NULL){

				VM_INNER_CALL_ONLY_RETURN(
						stk_mp->so_object
						,(ScriptFunction *)stk_mp->member_property->metamethod_members.getter->ref_ptr
						,stk_mp->member_property->metamethod_members.getter->name.c_str()
						,true
				);

				stk_var=data->stk_vm_current;
			}
		}

lbl_exit_function:

		return stk_var;
	}
}
