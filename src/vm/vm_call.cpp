/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	bool vm_inner_call(
		VirtualMachine 	*	_vm
		,ScriptFunction	* 	_script_function
		,Instruction	* 	_instruction
		,ScriptObject 	*	_script_object
		,ScriptFunction *	_script_function_to_call
		,int 				_n_args
	){
		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		StackElement 		*	stk_def_afun_start=data->vm_stk_current;
		int 					n_returned_args_afun=0;
		Instruction			*	instruction=_instruction;


		if((_script_function_to_call)->properties & FUNCTION_PROPERTY_C_OBJECT_REF){
			vm_execute_native_function(
					_vm
					,_script_function
					,instruction
					,_script_object
					,_script_function_to_call
					,stk_def_afun_start
					,_n_args
			);
		}else{
			vm_execute_script_function(
				_vm
				,_script_object
				,_script_function_to_call
				,stk_def_afun_start
			);
		}
		if(data->vm_error == true){

			data->vm_error_callstack_str+=zs_strutils::format(
				"\nat %s (file:%s line:%i)" /* TODO: get full symbol ? */
				, "_iter"
				,SFI_GET_FILE(_script_function,instruction)
				,SFI_GET_LINE(_script_function,instruction)
			);
			return false;
			/*if(goto_on_error){
				goto lbl_exit_function;
			}*/
		}
		n_returned_args_afun=data->vm_stk_current-stk_def_afun_start;
		/* we share pointer (true second arg) to not remove on pop in calling return */
		ZS_CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_def_afun_start,n_returned_args_afun);

		/* reset stack */
		data->vm_stk_current=stk_def_afun_start;

		return true;

	}

	bool vm_call(
		VirtualMachine 	*	_vm
		,ScriptObject 	*	_this_object
		,ScriptFunction *	_script_function
		,Instruction 	*	_instruction
		,StackElement 	*	_stk_local_var
	){
		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		Symbol				*	symbol_aux=NULL;
		StackElement		*	sf_call_stk_function_ref=NULL;
		ScriptFunction 		*	sf_call_script_function = NULL;

		uint8_t		 			sf_call_n_args=0; // number arguments will pass to this function
		StackElement 		*	sf_call_stk_start_arg_call=NULL;
		ScriptObject 		*	sf_call_calling_object = NULL;
		bool			 		sf_call_is_constructor=false;
		size_t 		 			sf_call_n_local_symbols=0;
		bool 			 		sf_call_is_member_function=false;
		StackElement 		*	sf_call_stk_return=NULL;
		int 					sf_call_n_returned_arguments_from_function=0;
		int						sf_call_stk_start_function_object=0;
		int						sf_call_return=0;
		Instruction			*	instruction=_instruction;


		switch(instruction->byte_code){
		default:
			 ZS_VM_STOP_EXECUTE("byte code '%s' not handled",instruction->byte_code);
			break;
			case  BYTE_CODE_CALL: // immediate call this
				 sf_call_calling_object = NULL;
				 sf_call_stk_start_function_object=0;
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=false;
				 sf_call_n_args = INSTRUCTION_GET_PARAMETER_COUNT(instruction); // number arguments will pass to this function
				 sf_call_stk_start_arg_call = (data->vm_stk_current - sf_call_n_args);
				 sf_call_script_function=(ScriptFunction *)(((Symbol *)instruction->value_op2)->ref_ptr);
				 goto execute_function;
			case BYTE_CODE_SUPER_CALL:
				 sf_call_calling_object = _this_object;
				 sf_call_stk_start_function_object=0;
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=true;
				 sf_call_n_args = INSTRUCTION_GET_PARAMETER_COUNT(instruction); // number arguments will pass to this function
				 sf_call_stk_start_arg_call = (data->vm_stk_current - sf_call_n_args);
				 sf_call_script_function=(ScriptFunction *)((Symbol *)instruction->value_op2)->ref_ptr;
				 goto execute_function;
			case  BYTE_CODE_THIS_CALL: // immediate call this
				 sf_call_calling_object = _this_object;
				 sf_call_stk_start_function_object=0;
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=true;
				 sf_call_n_args = INSTRUCTION_GET_PARAMETER_COUNT(instruction); // number arguments will pass to this function
				 sf_call_stk_start_arg_call = (data->vm_stk_current - sf_call_n_args);
				 symbol_aux=NULL;
				 // Since symbol is created on its owner, we have to get symbol from this object. This technique expects
				 // that symbols are ordered
				 if(instruction->value_op2!=ZS_IDX_UNDEFINED){
					 symbol_aux=(Symbol *)_this_object->getScriptType()->getSymbolMemberFunction(((Symbol *)instruction->value_op2)->name);
					 sf_call_script_function=(ScriptFunction *)(symbol_aux->ref_ptr);
					 /*if(symbol_aux==NULL){ // it calls overrided function (top-most)
						 ZS_VM_STOP_EXECUTE("Error call 'this.%s': Cannot find '%s::%s' member function"
								,SFI_GET_SYMBOL_NAME(_script_function,instruction)
								,_this_object->getScriptType()->str_script_type.c_str()
								,SFI_GET_SYMBOL_NAME(_script_function,instruction)
						);
					 }*/

				 }else{ // Member function not exist try to get variable to call if exist
					 sf_call_stk_function_ref=_this_object->get(SFI_GET_SYMBOL_NAME(_script_function,instruction));
					 sf_call_is_member_function=false;
					 if(sf_call_stk_function_ref==NULL){ // it calls overrided function (top-most)
						 ZS_VM_STOP_EXECUTE("Error calling 'this.%s': member variable or function '%s::%s' not exist"
							, SFI_GET_SYMBOL_NAME(_script_function,instruction)
							,_this_object->getScriptType()->str_script_type.c_str()
							, SFI_GET_SYMBOL_NAME(_script_function,instruction)
						);
					 }
					 goto load_function;
				 }

				 //sf_call_script_function=(ScriptFunction *)(symbol_aux->ref_ptr);
				 goto execute_function;
			case BYTE_CODE_INDIRECT_THIS_CALL:
				 sf_call_calling_object = _this_object;
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=false;
				 sf_call_stk_start_function_object=0;
				 sf_call_stk_function_ref=_this_object->get(SFI_GET_SYMBOL_NAME(_script_function,instruction));
				 if(sf_call_stk_function_ref==NULL){ // it calls overrided function (top-most)
					 ZS_VM_STOP_EXECUTE("'variable this.%s' not exist", SFI_GET_SYMBOL_NAME(_script_function,instruction)
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
				 sf_call_stk_function_ref=data->vm_stk_current-(INSTRUCTION_GET_PARAMETER_COUNT(instruction)+1);
				 goto load_function;
			 case  BYTE_CODE_CONSTRUCTOR_CALL:
				 sf_call_is_constructor=false;
				 sf_call_is_member_function=false;
				 sf_call_script_function=NULL;
				 sf_call_stk_function_ref = (data->vm_stk_current-INSTRUCTION_GET_PARAMETER_COUNT(instruction)-1);
				// get object
				sf_call_calling_object=(ScriptObject *)((sf_call_stk_function_ref-1)->value);

				// if we invoke constructor we need to keep object to pass after, else remove object+function
				sf_call_stk_start_function_object=1;
				goto load_function;
		 case  BYTE_CODE_MEMBER_CALL: // calling function after all of args are processed...

			sf_call_script_function=NULL;
			sf_call_stk_function_ref = (data->vm_stk_current-INSTRUCTION_GET_PARAMETER_COUNT(instruction)-1);
			// if we invoke constructor we need to keep object to pass after, else remove object+function
			sf_call_stk_start_function_object=1;//instruction->byte_code==BYTE_CODE_CONSTRUCTOR_CALL?1:0; // object + function

load_function:

			sf_call_is_member_function=false;
			sf_call_is_constructor=instruction->byte_code==BYTE_CODE_CONSTRUCTOR_CALL;

			sf_call_n_args = INSTRUCTION_GET_PARAMETER_COUNT(instruction); // number arguments will pass to this function
			sf_call_stk_start_arg_call = (data->vm_stk_current - sf_call_n_args);

			if(sf_call_stk_function_ref->properties & STK_PROPERTY_MEMBER_FUNCTION){
			  Symbol *symbol=(Symbol *)sf_call_stk_function_ref->value;
			  sf_call_script_function=(ScriptFunction *)symbol->ref_ptr;
			  sf_call_is_member_function=true;
			}else if(STK_IS_MEMBER_FUNCTION_SCRIPT_OBJECT(sf_call_stk_function_ref)){
			  MemberFunctionScriptObject *sofm=(  MemberFunctionScriptObject *)sf_call_stk_function_ref->value;
			  ScriptObject *sofm_object=sofm->getRefObject();
			  if(sofm_object==NULL){
				  ZS_VM_STOP_EXECUTE(
					  "Cannot call function member object '%s' stored in variable '%s' due its own object has been dereferenced"
					  ,sofm->sf_ref->name_script_function.c_str()
					  , SFI_GET_SYMBOL_NAME(_script_function,instruction)
				  );
			  }
			  sf_call_calling_object=sofm_object;
			  sf_call_script_function=sofm->sf_ref;
			  sf_call_is_member_function=true;
			}else{
				sf_call_is_member_function=false;
				if((sf_call_stk_function_ref->properties & (STK_PROPERTY_FUNCTION))==0){
					// error or continue
					if(instruction->byte_code== BYTE_CODE_CONSTRUCTOR_CALL){ // constructor was not found so we do nothing
						// reset stack to last
						if((instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK)==0){
							data->vm_stk_current=sf_call_stk_function_ref;//sf_call_stk_start_arg_call-sf_call_stk_start_function_object;
						}
						goto lbl_exit_function_ok;
					}

					// indirect this call / member call or stk call
					if(instruction->byte_code==BYTE_CODE_INDIRECT_THIS_CALL){

						ZS_VM_STOP_EXECUTE("Cannot call 'this.%s' as type '%s'. 'this.%s' is not function"
								,SFI_GET_SYMBOL_NAME(_script_function,instruction)
								,data->zs->stackElementToStringTypeOf(sf_call_stk_function_ref).c_str()
								,SFI_GET_SYMBOL_NAME(_script_function,instruction)
						);

					}else if(instruction->byte_code==BYTE_CODE_MEMBER_CALL){
						ZS_VM_STOP_EXECUTE("Cannot call '%s'. '%s' is not function or not exist"
								,SFI_GET_SYMBOL_NAME(_script_function,instruction)
								,SFI_GET_SYMBOL_NAME(_script_function,instruction)
						);
					}else if(instruction->byte_code==BYTE_CODE_INDIRECT_LOCAL_CALL){
						ZS_VM_STOP_EXECUTE("Cannot call '%s' as a function. '%s' is type '%s'"
								,SFI_GET_SYMBOL_NAME(_script_function,instruction)
								,SFI_GET_SYMBOL_NAME(_script_function,instruction)
								,data->zs->stackElementToStringTypeOf(sf_call_stk_function_ref).c_str()
						);
					}else{ // STACK CALL
						ZS_VM_STOP_EXECUTE("Error trying to call a function from stack. StackElement value is '%s' as type '%s'"
								,data->zs->stackElementToString(sf_call_stk_function_ref).c_str()
								,data->zs->stackElementToStringTypeOf(sf_call_stk_function_ref).c_str()
								,SFI_GET_SYMBOL_NAME(_script_function,instruction)
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
					ArrayScriptObject *var_args=NULL;
					ScriptObject *so_param=NULL;

					int effective_args=sf_call_n_args < sf_call_script_function->params_len ? sf_call_n_args:sf_call_script_function->params_len;
					ScriptFunctionParam *sf_param=sf_call_script_function->params;

					//-------------------------------------------------------------------------------------
					// PASS ARGS
					for(int i=0;i < sf_call_n_args;i++){
						so_param=NULL; // script object we passing
						uint16_t sfa_properties=sf_param->properties;// ((ScriptFunctionParam *)(*function_param))->properties;
						if((sfa_properties & MSK_SCRIPT_FUNCTION_ARG_PROPERTY_BY_REF)){ // create or pass the var ref object...

							StackElement *check_ref=stk_arg;
							if(stk_arg->properties & STK_PROPERTY_PTR_STK){
								check_ref=(StackElement *)check_ref->value;
							}

							// because arg by ref is always loaded directly we have the object stk...
							if(STK_IS_VAR_REF_SCRIPT_OBJECT(stk_arg)==false) { // create new

								if((stk_arg->properties & STK_PROPERTY_PTR_STK) != STK_PROPERTY_PTR_STK){
									ZS_VM_STOP_EXECUTE("Calling function '%s', parameter '%i': Argument by reference has to be variable"
											,sf_call_script_function->name_script_function.c_str(),i+1);
								}

								VarRefScriptObject *sc=ZS_NEW_OBJECT_VAR_REF(data->zs,*stk_arg);
								if(!vm_create_shared_script_object(_vm,sc)){
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

							if((stk_arg->properties & STK_PROPERTY_CONTAINER_SLOT)){
								stk_arg->value=(zs_int)(((ContainerSlot *)stk_arg->value)->getSrcContainerRef());
								stk_arg->properties=STK_PROPERTY_SCRIPT_OBJECT;
							}

							if(STK_IS_VAR_REF_SCRIPT_OBJECT(stk_arg)==true) { // not passing by ref it gets its value
								*stk_arg=*((VarRefScriptObject *)stk_arg->value)->getStackElementPtr();
							}

							if((stk_arg->properties & STK_PROPERTY_SCRIPT_OBJECT)){
								so_param=(ScriptObject *)stk_arg->value;
								if(so_param->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_STRING && so_param->shared_pointer==NULL){
									StringScriptObject *sc=ZS_NEW_STRING_OBJECT(data->zs);
									if(!vm_create_shared_script_object(_vm,sc)){
										goto lbl_exit_function;
									}
									sc->set(*(((StringScriptObject *)so_param)->str_ptr));
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
								var_args=ZS_NEW_ARRAY_OBJECT(data->zs);
								if(!vm_create_shared_script_object(_vm,var_args)){
									goto lbl_exit_function;
								}

								if(!vm_share_script_object(_vm,var_args)){ // we share pointer +1 to not remove on pop in calling return
									goto lbl_exit_function;
								}

								// push first arg
								var_args->push(stk_arg);
								// replace for vector type...
								stk_arg->value=(zs_int)var_args;
								stk_arg->properties=STK_PROPERTY_SCRIPT_OBJECT;
							}else{ // not push in var arg

								if(so_param != NULL){ // share n+1 to function if not this
									if(!vm_share_script_object(_vm,so_param)){ // By pass object in the arg, it shares pointer +1 to not remove on pop in calling return
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
					// PASS ARGS
					//-------------------------------------------------------------------------------------

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
						*data->vm_stk_current++=param->default_param_value;
						break;
					case STK_PROPERTY_FUNCTION: // we call function that return default value
						ZS_VM_INNER_CALL(
							NULL
							,(ScriptFunction *)(((Symbol *)param->default_param_value.value)->ref_ptr)
							, 0
							,((Symbol *)param->default_param_value.value)->name.c_str()
						)

						// if script object it shares in order to be used as variable in the function to be called
						if(data->vm_stk_current->properties & STK_PROPERTY_SCRIPT_OBJECT){
							if(!vm_share_script_object(_vm,(ScriptObject *)data->vm_stk_current->value)){
								goto lbl_exit_function;
							}
						}
						data->vm_stk_current++;
						break;
					default:
						ZS_VM_STOP_EXECUTE("Internal error: Unexpected default stack element '%s'"
							,data->zs->stackElementToStringTypeOf(&param->default_param_value).c_str()
						);
						break;

					}
					sf_call_n_args++;
				}

				if(sf_call_is_constructor){
					// When the object is being constructed its shares is 0. In the 'constructor' function may pass 'this' throug other functions
					// exposin 'this' candidate to be dereferenced and destroyed. So we share 'this' before the call and unref after call
					if(!vm_share_script_object(_vm,sf_call_calling_object)){
						goto lbl_exit_function;
					}
				}

				vm_execute_script_function(
					_vm
					,sf_call_calling_object
					,sf_call_script_function
					,sf_call_stk_start_arg_call
				);

				if(data->vm_error == false && sf_call_is_constructor==true){
					// When the object is being constructed its shares is 0. In the 'constructor' function may pass 'this' throug other functions
					// exposin 'this' candidate to be dereferenced and destroyed. In the BYTE_CODE_CONSTRUCTOR_CALL was shared +1.
					// In this case deref the shared 'this' is dereferenced
					if(!vm_unref_shared_script_object(
							_vm
							,sf_call_calling_object
							,ZS_VM_CURRENT_SCOPE_BLOCK)
					){
						goto lbl_exit_function;
					}
				}

				sf_call_n_local_symbols=sf_call_script_function->local_variables->size();
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
								_vm
								,sc
								,_script_function
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
					vm_execute_native_function(
						_vm
						,_script_function
						,instruction
						,sf_call_calling_object
						,sf_call_script_function
						,sf_call_stk_start_arg_call
						,sf_call_n_args
					);
				}catch(std::exception & ex){
					data->vm_error = true;
					data->vm_error_description=ex.what();
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
								_script_function->name_script_function=="assert"
							||  _script_function->name_script_function=="error"
							||  _script_function->name_script_function=="outln"
							)
							&&
							(
								sf_call_script_function->name_script_function=="errorNative"
							 || sf_call_script_function->name_script_function=="outlnNative"
							)
						)
						||
						(
							(
								_script_function->name_script_function=="eval"
							&&  sf_call_script_function->name_script_function=="evalNative"
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
						str_class_owner=data->script_type_factory->getScriptType(sf_call_script_function->idx_script_type_owner)->str_script_type.c_str();
					}
					const char * file_src_call=SFI_GET_FILE(_script_function,instruction);
					data->vm_error_callstack_str+=zs_strutils::format(
						"\nat calling function %s%s%s (%sline:%i)" // TODO: get full symbol ?
						,str_class_owner==NULL?"":str_class_owner
						,str_class_owner==NULL?"":"::"
						,sf_call_script_function->name_script_function.c_str()
						,file_src_call?zs_strutils::format("file:%s ",file_src_call).c_str():""
						,SFI_GET_LINE(_script_function,instruction)
					);

				}
				goto lbl_exit_function;
			}

			// calcule returned stack elements
			sf_call_stk_return=(sf_call_stk_start_arg_call+sf_call_n_local_symbols); // +1 points to starting return...
			sf_call_n_returned_arguments_from_function=data->vm_stk_current-sf_call_stk_return;
			sf_call_return=INSTRUCTION_GET_RETURN_COUNT(instruction);

			// setup all returned variables from function
			ZS_CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(sf_call_stk_return,sf_call_n_returned_arguments_from_function)

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

				data->vm_stk_current=sf_call_stk_start_arg_call-sf_call_stk_start_function_object;//(sf_call_stk_start_function_object?0:1);//+n_returned_arguments_from_function; // vm_stk_current points to first stack element

				// no return parameters but the caller expects n_parameters, so
				for(int i=0; i < sf_call_n_returned_arguments_from_function;i++){
					*data->vm_stk_current++=*sf_call_stk_return++;
				}
			}
		}

	lbl_exit_function_ok:
		return true;

	lbl_exit_function:
		return false;
	}
}
