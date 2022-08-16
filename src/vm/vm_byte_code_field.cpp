/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


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
					,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
					,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
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
							,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
							,sc_type->idx_script_type>IDX_TYPE_SCRIPT_OBJECT_OBJECT?"Native type":"Type"
							,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
							,str_symbol_aux1
						);
					}
				}

				// create new property initialized as undefined
				if((stk_var=so_aux->addProperty((const char *)str_symbol_aux1, data->vm_error_str))==NULL){
					VM_STOP_EXECUTEF(data->vm_error_str.c_str());
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
					,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
					,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
					,(const char *)str_symbol_aux1
					,stk_to_typeof_str(data->zs,stk_var).c_str()
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

	StackElement *vm_load_this_element(
		VirtualMachine			* vm
		,ScriptObject			* this_object
		,ScriptFunction 		* calling_function
		,Instruction			*instruction
		,short _offset
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		StackMemberProperty *stk_mp_aux=NULL;
		StackElement *_stk_result=NULL;\
		if(_offset != (uint8_t)ZS_IDX_UNDEFINED){ \
			_stk_result = this_object->getBuiltinElementAt(_offset); \
		} \
		if(_stk_result != NULL && (_stk_result->properties & STK_PROPERTY_MEMBER_PROPERTY)){ \
			stk_mp_aux=(StackMemberProperty *)_stk_result->value; \
			if(stk_mp_aux->member_property->metamethod_members.getter != NULL){ \
				VM_INNER_CALL( \
						stk_mp_aux->so_object \
						,(ScriptFunction *)stk_mp_aux->member_property->metamethod_members.getter->ref_ptr \
						,0 \
						,stk_mp_aux->member_property->metamethod_members.getter->name.c_str() \
				); \
				/* getter requires stack to save value and avoid destroy previuos value*/ \
				_stk_result=data->stk_vm_current; \
			}else{ \
				VM_STOP_EXECUTE( \
						"Property '%s' does not implements _get metamethod",SFI_GET_SYMBOL_NAME(calling_function,instruction) \
				); \
			}\
		} \

		return _stk_result;
	lbl_exit_function:
		return NULL;
	}

}
