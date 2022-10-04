/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	bool vm_load_field(
		VirtualMachine 	*	_vm
		,ScriptObject 	*	_this_object
		,ScriptFunction *	_calling_function
		,Instruction 	**	_instruction_it
	){
		VirtualMachineData 			*	data=(VirtualMachineData *)_vm->data;
		Instruction 				*	instruction=(*_instruction_it)-1;
		ScriptObject 				*	so_aux=NULL;
		StackElement 				*	stk_result_op1=NULL;
		const char 					*	str_symbol_aux1=NULL;
		StackElement 				*	stk_var=NULL;
		StackMemberProperty 		*	stk_mp_aux=NULL;
		ScriptType					*	sc_type=NULL;
		Symbol 						*	sf_member=NULL;
		MemberFunctionScriptObject	*	somf=NULL;

		if(
				instruction->byte_code == BYTE_CODE_LOAD_THIS_VARIABLE
				|| instruction->byte_code == BYTE_CODE_PUSH_STK_THIS_VARIABLE
		){
				so_aux=_this_object; // take this as default
				goto find_element_object;
		}

	load_next_element_object:

		instruction=(*_instruction_it)-1;

		if(
			(instruction-1)->byte_code == BYTE_CODE_NEW_OBJECT_BY_TYPE
		){
			stk_result_op1=(data->vm_stk_current-1);
		}
		else{
			VM_POP_STK_ONE; // get var op1 and symbol op2
		}

		if((stk_result_op1->properties & (STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_CONTAINER_SLOT)) == 0)
		{
			if((instruction-1)->byte_code==BYTE_CODE_LOAD_OBJECT_ITEM){
				VM_STOP_EXECUTE(
					"Cannot perform access [ ... %s.%s ], expected '%s' as object but is type '%s' %s"
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction-1)
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction)
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction-1)
					,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
					,zs_strutils::starts_with(stk_to_typeof_str(data->zs,stk_result_op1),"type@")? ". If you are trying to call/access static member of class you need to use static access operator (i.e '::') instead of member access operator (i.e '.')":""
				);
			}else{ // from calling
				VM_STOP_EXECUTE(
					"Cannot perform access '.%s' from variable type '%s'"
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction)
					,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
				);
			}
		}

		if(stk_result_op1->properties & STK_PROPERTY_CONTAINER_SLOT){
			so_aux=((ContainerSlot *)stk_result_op1->value)->getSrcContainerRef();
		}
		else{
			so_aux=((ScriptObject *)stk_result_op1->value);
		}

		if(so_aux == NULL)
		{
			VM_STOP_EXECUTE("var '%s' is not scriptvariable",SFI_GET_SYMBOL_NAME(_calling_function,(instruction-1)));
		}



	find_element_object:

		str_symbol_aux1=(char *)SFI_GET_SYMBOL_NAME(_calling_function,instruction);

		//
		sc_type=so_aux->getScriptType();
		sf_member=sc_type->getSymbolMemberFunction(str_symbol_aux1);
		if(sf_member !=NULL){
			if(
				   ((instruction+1)->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM || ((instruction+1)->byte_code == BYTE_CODE_PUSH_STK_OBJECT_ITEM))
				&& ((instruction->properties & INSTRUCTION_PROPERTY_CALLING_FUNCTION)==0)){
				VM_STOP_EXECUTE(
					"Cannot perform access operation [ ... %s.%s ], because '%s' is a function. It should call function with '()' before '.'"
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction)
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction+1)
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction)
				);
			}

			somf=ZS_NEW_OBJECT_MEMBER_FUNCTION(data->zs,so_aux,(ScriptFunction *)sf_member->ref_ptr);

			 if(!vm_create_shared_script_object(_vm,somf)){
				goto lbl_exit_function;
			 }

			data->vm_stk_current->value=(zs_int)somf;
			data->vm_stk_current->properties=STK_PROPERTY_SCRIPT_OBJECT;
			data->vm_stk_current++;

			goto lbl_exit_function_ok;


		}else if((stk_var=so_aux->getProperty(str_symbol_aux1)) == NULL){
			// property is not defined

			if(instruction->properties & INSTRUCTION_PROPERTY_CALLING_FUNCTION){
				VM_STOP_EXECUTE("Error call function '...%s.%s(...)', where '%s' is type '%s'. Member function '%s::%s' is not defined"
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction-1)
					,str_symbol_aux1
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction-1)
					,stk_to_typeof_str(data->zs,data->vm_stk_current).c_str()
					,stk_to_typeof_str(data->zs,data->vm_stk_current).c_str()
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
							,SFI_GET_SYMBOL_NAME(_calling_function,instruction-1)
							,str_symbol_aux1
							,SFI_GET_SYMBOL_NAME(_calling_function,instruction-1)
							,stk_to_typeof_str(data->zs,data->vm_stk_current).c_str()
							,sc_type->idx_script_type>IDX_TYPE_SCRIPT_OBJECT_OBJECT?"Native type":"Type"
							,stk_to_typeof_str(data->zs,data->vm_stk_current).c_str()
							,str_symbol_aux1
						);
					}
				}

				// create new property initialized as undefined
				if((stk_var=so_aux->addProperty((const char *)str_symbol_aux1, data->vm_error_description))==NULL){
					VM_STOP_EXECUTEF(data->vm_error_description.c_str());
				}

				// if it has to push container slot and slot itself is not container slot push new one
				if((instruction->properties & INSTRUCTION_PROPERTY_CONTAINER_SLOT_ASSIGMENT)
						&&
				((stk_var->properties & STK_PROPERTY_CONTAINER_SLOT)!=STK_PROPERTY_CONTAINER_SLOT)
						&&
					VM_CHECK_CONTAINER_FOR_SLOT(so_aux)
				){
					VM_PUSH_NEW_CONTAINER_SLOT(
						(ContainerScriptObject *)so_aux
						,(zs_int)str_symbol_aux1
						,stk_var
					);
				}else{
					VM_PUSH_STK_PTR(stk_var);
				}

			}
			else{ // not exists
				data->vm_stk_current->value=0;
				data->vm_stk_current->properties=STK_PROPERTY_UNDEFINED;
				data->vm_stk_current++;
			}
			goto lbl_exit_function_ok;
		}else{

			if((instruction->properties & INSTRUCTION_PROPERTY_CALLING_FUNCTION) && ((stk_var->properties & STK_PROPERTY_FUNCTION)==0)){

				VM_STOP_EXECUTE("Error call function '...%s.%s(...)', where '%s' is type '%s'. Expected '%s::%s' as a function but it is type '%s'"
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction-1)
					,(const char *)str_symbol_aux1
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction-1)
					,stk_to_typeof_str(data->zs,data->vm_stk_current).c_str()
					,stk_to_typeof_str(data->zs,data->vm_stk_current).c_str()
					,(const char *)str_symbol_aux1
					,stk_to_typeof_str(data->zs,stk_var).c_str()
				);
			}

			// Is member property. ... if it has getter function defined, get the value itself and evaluate
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
							STK_IS_SCRIPT_OBJECT_CLASS(data->vm_stk_current)
						||(
								// ... or return value itself if the next instruction is not for store
								(
									(instruction->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM)
								||	(instruction->byte_code == BYTE_CODE_LOAD_THIS_VARIABLE)
								)
								&& ((instruction->properties & INSTRUCTION_PROPERTY_USE_PUSH_STK)==0)
						  )

					){
						data->vm_stk_current++;
						goto lbl_exit_function_ok;
					}
				}

			}
		}


		if((instruction+1)->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM){ // fast load access without pass through switch instruction
			*data->vm_stk_current++=*stk_var;
			instruction++; // we have to inc current instruction...
			(*_instruction_it)++; //... and instruction iterator
			goto load_next_element_object;
		}

		// load its value for write
		if(instruction->byte_code == BYTE_CODE_PUSH_STK_OBJECT_ITEM || instruction->byte_code == BYTE_CODE_PUSH_STK_THIS_VARIABLE){
			if((instruction->properties & INSTRUCTION_PROPERTY_CONTAINER_SLOT_ASSIGMENT)
					&&
			((stk_var->properties & STK_PROPERTY_CONTAINER_SLOT)!=STK_PROPERTY_CONTAINER_SLOT)
					&&
				VM_CHECK_CONTAINER_FOR_SLOT(so_aux)
			){
				VM_PUSH_NEW_CONTAINER_SLOT(
					(ContainerScriptObject *)so_aux
					,(zs_int)str_symbol_aux1
					,stk_var
				);
			}else{
				VM_PUSH_STK_PTR(stk_var);
			}
		}
		else{ // only read
			*data->vm_stk_current++=*stk_var;
		}

	lbl_exit_function_ok:
		return true;

	lbl_exit_function:
		return false;
	}

	StackElement *vm_load_this_element(
		VirtualMachine	 		*	_vm
		,ScriptObject			* 	_this_object
		,ScriptFunction 		* 	_calling_function
		,Instruction			*	_instruction
		,short 						_offset
	){
		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		StackMemberProperty *	stk_mp_aux=NULL;
		StackElement 		*	stk_result=NULL;
		Instruction			*	instruction=_instruction;

		if(_offset != (uint8_t)ZS_IDX_UNDEFINED){
			stk_result = _this_object->getBuiltinElementAt(_offset);
		}
		if(stk_result != NULL && (stk_result->properties & STK_PROPERTY_MEMBER_PROPERTY)){
			stk_mp_aux=(StackMemberProperty *)stk_result->value;
			if(stk_mp_aux->member_property->metamethod_members.getter != NULL){
				// saves current stack element due the return will overwrite current stackpositin
				StackElement *stk_back=data->vm_stk_current;
				data->vm_stk_current++;
				VM_INNER_CALL(
						stk_mp_aux->so_object
						,(ScriptFunction *)stk_mp_aux->member_property->metamethod_members.getter->ref_ptr
						,0 \
						,stk_mp_aux->member_property->metamethod_members.getter->name.c_str()
				);
				/* restore */
				stk_result=data->vm_stk_current;
				data->vm_stk_current=stk_back;
			}else{
				VM_STOP_EXECUTE(
						"Property '%s' does not implements _get metamethod",SFI_GET_SYMBOL_NAME(_calling_function,instruction)
				);
			}
		}

		return stk_result;
	lbl_exit_function:
		return NULL;
	}

	bool vm_push_container_item(
		VirtualMachine 			*	_vm
		,ScriptObject 			*	_this_object
		,ScriptFunction 		*	_calling_function
		,Instruction 			*	_instruction
		,StackElement 			*	_stk_local_var
		,bool						_dst_container_is_object
	){
		VirtualMachineData 		*	data=(VirtualMachineData *)_vm->data;
		StackElement 			*	stk_result_op1=NULL;
		StackElement 			*	stk_result_op2=NULL;
		ContainerScriptObject 	*	dst_container=NULL;
		ScriptObject 			*	so_aux=NULL;
		StackElement 				stk_src=k_stk_undefined,
								*	stk_dst=NULL,
								*	stk_var=NULL;

		StackElement				stk_aux1;
		void					*	stk_src_ref_value=NULL,
								*	stk_dst_ref_value=NULL;
		uint16_t					stk_src_properties=0;
		Instruction				*	instruction=_instruction;
		zs_int						id_slot=ZS_IDX_UNDEFINED;

		if(_dst_container_is_object==true){

			VM_POP_STK_TWO; // first must be a string that describes property name and the other the variable itself ...

			stk_var=(data->vm_stk_current-1);
			if(STK_IS_SCRIPT_OBJECT_OBJECT(stk_var) == 0){
				VM_STOP_EXECUTE("Expected object but is type '%s'",stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_var));
			}

			dst_container = (ContainerScriptObject *)stk_var->value;

			if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1) == 0){
				VM_STOP_EXECUTE("Internal: Expected stk_result_op1 as string but is type '%s'"
						,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1)
				);
			}

			if((stk_var =dst_container->addProperty(
					((StringScriptObject *)stk_result_op1->value)->toString()
					,data->vm_error_description)
			)==NULL){
				VM_STOP_EXECUTEF(data->vm_error_description.c_str());
			}

			id_slot=(zs_int)(((StringScriptObject *)stk_result_op1->value)->getConstChar());
			stk_dst=stk_var;
			stk_src=*stk_result_op2;

		}else{ // is vector

			VM_POP_STK_ONE; // only pops the value, the last is the vector variable itself

			stk_var=(data->vm_stk_current-1);
			if(STK_IS_SCRIPT_OBJECT_VECTOR(stk_var) == 0){
				VM_STOP_EXECUTE("Expected vector but is type '%s'",stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_var));
			}

			dst_container = (ContainerScriptObject *)stk_var->value;
			id_slot=((VectorScriptObject *)dst_container)->length();
			stk_dst=((VectorScriptObject *)dst_container)->pushNewUserSlot();
			stk_src=*stk_result_op1;
		}

		if(stk_src.properties & STK_PROPERTY_PTR_STK){
			stk_src=*((StackElement *)stk_src.value);
		}
		//------
		if(STK_IS_SCRIPT_OBJECT_VAR_REF(&stk_src)){ \
			stk_src=(*(StackElement *)((STK_GET_STK_VAR_REF(&stk_src)->value))); \
		} \

		if(STK_VALUE_IS_CONTAINER_SLOT(&stk_src)){
			stk_src.properties=STK_PROPERTY_SCRIPT_OBJECT; \
			stk_src.value=(zs_int)((ContainerSlot *)(stk_src.value))->getSrcContainerRef();
		}

		stk_src_ref_value=&stk_src.value; \
		stk_dst_ref_value=&stk_dst->value; \
		if(stk_src.properties & STK_PROPERTY_IS_C_VAR_PTR){ /* src is C pointer */ \
			stk_src_ref_value=(zs_int *)((stk_src).value); \
		}\
		stk_src_properties=stk_src.properties;\
		if(stk_src_properties == STK_PROPERTY_UNDEFINED){\
			stk_dst->properties=STK_PROPERTY_UNDEFINED;\
		}else if(stk_src_properties == STK_PROPERTY_NULL){\
			stk_dst->properties=STK_PROPERTY_NULL;\
		}else if(stk_src_properties & STK_PROPERTY_ZS_INT){\
			stk_dst->properties=STK_PROPERTY_ZS_INT;\
			*((zs_int *)stk_dst_ref_value)=*((zs_int *)stk_src_ref_value);\
		}else if(stk_src_properties & STK_PROPERTY_ZS_FLOAT){\
			stk_dst->properties=STK_PROPERTY_ZS_FLOAT;\
			*((zs_float *)stk_dst_ref_value)=*((zs_float *)stk_src_ref_value);\
		}else if(stk_src_properties & STK_PROPERTY_BOOL){\
			stk_dst->properties=STK_PROPERTY_BOOL;\
			*((bool *)stk_dst_ref_value)=*((bool *)stk_src_ref_value);\
		}else if(stk_src_properties  &  (STK_PROPERTY_FUNCTION | STK_PROPERTY_TYPE | STK_PROPERTY_MEMBER_FUNCTION) ){\
			*stk_dst=stk_src;\
		}else if(stk_src_properties & STK_PROPERTY_SCRIPT_OBJECT){\

			if(STK_IS_SCRIPT_OBJECT_STRING(&stk_src)){\
				stk_dst->value=(zs_int)(so_aux= new StringScriptObject(data->zs));\
				stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;\
				if(!vm_create_shared_script_object(_vm,so_aux)){\
					goto lbl_exit_function;\
				}\
				if(!vm_share_script_object(_vm,so_aux)){\
					goto lbl_exit_function;\
				}\
				((StringScriptObject *)(so_aux))->set(((StringScriptObject *)stk_src.value)->get());
			}else{ \
				ContainerScriptObject *src_container=(ContainerScriptObject *)stk_src.value;

				if(VM_CHECK_CONTAINER_FOR_SLOT(src_container)){
					ContainerSlot *container_slot=ContainerSlot::newContainerSlot(
						dst_container
						,id_slot
						,stk_dst
					);

					vm_assign_container_slot(_vm,container_slot, src_container);

				}else{
					// is not container
					stk_dst->value=(intptr_t)src_container;\
					stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;\
				}

				// share always because it's not possible have cyclic reference of each push item object/vector
				if(!vm_share_script_object(_vm,src_container)){\
					goto lbl_exit_function;\
				}\
			}\
		}else{\
			VM_STOP_EXECUTE("VM_SET_CONTAINER_ELEMENT:(internal) cannot determine var type %s"\
				,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,&stk_src)\
			);\
		}\
		//----

		return true;

lbl_exit_function:
		return false;

	}

	bool vm_load_vector_item(
			VirtualMachine 	*_vm
			,ScriptObject 	*_this_object
			,ScriptFunction *_calling_function
			,Instruction 	*_instruction
			,StackElement 	*_stk_local_var

	){
		VirtualMachineData 			*	data=(VirtualMachineData *)_vm->data;
		StackElement 				*	stk_result_op1=NULL;
		StackElement 				*	stk_result_op2=NULL;
		ScriptObject 				*	so_aux=NULL;
		int								index_aux1=0;
		StackElement				*	stk_var=NULL;
		const char 					*	str_symbol_aux1=NULL;
		StackElement					stk_aux1;
		Instruction					*	instruction=_instruction;

		VM_POP_STK_TWO;
		so_aux=NULL;

		if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op1)){
			stk_result_op1 = ((VarRefScriptObject *)stk_result_op1->value)->getStackElementPtr();
		}

		stk_var=NULL;
		// determine object ...
		if(stk_result_op1->properties & (STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_CONTAINER_SLOT)){

			if(stk_result_op1->properties & STK_PROPERTY_CONTAINER_SLOT){
					so_aux=((ContainerSlot *)stk_result_op1->value)->getSrcContainerRef();
			}else{
				so_aux=(ScriptObject *)stk_result_op1->value;
			}


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

					if(index_aux1 >= (int)((VectorScriptObject *)so_aux)->length() || index_aux1 < 0){
						VM_STOP_EXECUTEF("Error accessing vector, index out of bounds");
					}

					if((stk_var =((VectorScriptObject *)so_aux)->getUserElementAt(index_aux1))==NULL){
						goto lbl_exit_function;
					} \
				}
				else{
					// is object
					if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)==0){ \
						VM_STOP_EXECUTEF("Expected string for object access");
					}
					// Save STK_PROPERTY_SLOT if not BYTE_CODE_LOAD_VECTOR_ITEM
					stk_var = ((ObjectScriptObject *)so_aux)->getProperty(
							((StringScriptObject *)(stk_result_op2->value))->get()
					);
					if(stk_var == NULL){
						if(instruction->byte_code == BYTE_CODE_PUSH_STK_VECTOR_ITEM){
							if((stk_var =((ObjectScriptObject *)so_aux)->addProperty(
									((StringScriptObject *)(stk_result_op2->value))->get(),
									data->vm_error_description
								)
							)==NULL){
								VM_STOP_EXECUTEF(data->vm_error_description.c_str());
							}
						}else{
							VM_STOP_EXECUTE("property '%s' not exist in object",((StringScriptObject *)(stk_result_op2->value))->getConstChar());
						}
					}
				}

				if(instruction->byte_code == BYTE_CODE_LOAD_VECTOR_ITEM){
					*data->vm_stk_current++=*stk_var;
				}else{
					// dest to write
					if((instruction->properties & INSTRUCTION_PROPERTY_CONTAINER_SLOT_ASSIGMENT)
								&&
							VM_CHECK_CONTAINER_FOR_SLOT(so_aux)
					){

						VM_PUSH_NEW_CONTAINER_SLOT(
							(ContainerScriptObject *)so_aux
							,(zs_int)str_symbol_aux1
							,stk_var
						);

					}else{
						VM_PUSH_STK_PTR(stk_var);
					}

				}
				goto lbl_exit_ok;
			}else if(so_aux->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_STRING){
				if(STK_VALUE_IS_ZS_INT(stk_result_op2)==false){ \
					VM_STOP_EXECUTEF("Expected integer index for String access");
				}

				zs_char *ptr_char=(zs_char *)&((StringScriptObject *)so_aux)->str_ptr->c_str()[stk_result_op2->value];
				if(instruction->byte_code == BYTE_CODE_LOAD_VECTOR_ITEM){
					data->vm_stk_current->value=((zs_int)(*ptr_char));
					data->vm_stk_current->properties=STK_PROPERTY_ZS_INT;
				}else{ // push stk
					data->vm_stk_current->value=(zs_int)ptr_char;
					data->vm_stk_current->properties=STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR;
				}
				data->vm_stk_current++;
				goto lbl_exit_ok;
			}else{
				VM_STOP_EXECUTEF("Expected String,Vector or Object for access '[]' operation"); \
			}
		}else{
			VM_STOP_EXECUTE("Expected object for access '[]' operation but it was type '%s'",stk_to_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1)); \
		}

	lbl_exit_ok:

		return true;

	lbl_exit_function:

		return false;
	}
}
