/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	bool vm_iterator_init(
			VirtualMachine 		*	_vm
			,Object 		*	_this_object
			,Function 	*	_script_function
			,Instruction 		*	_instruction
			,StackElement 		*	_stk_local_var
	){

		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		StackElement 			stk_aux1;
		Symbol 				*	symbol_iter;
		StackElement 		*	stk_result_op1=NULL;
		StackElement 		*	stk_result_op2=NULL;
		Object 		*	obj=NULL;
		Type 			*	sc=NULL;
		Instruction			* 	instruction=_instruction;

		VM_POP_STK_TWO;

		// stk_op2 expects to be obj with container

		if((stk_result_op2->properties & STACK_ELEMENT_PROPERTY_PTR_STK) == 0){
			ZS_VM_ERRORF("internal: Expected stk");
			return false;
		}

		if((stk_result_op1->properties & (STACK_ELEMENT_PROPERTY_OBJECT|STACK_ELEMENT_PROPERTY_CONTAINER_SLOT)) == 0){
			//ZS_VM_ERROR("internal: Expected object");
			if((data->vm_stk_current->properties & (STACK_ELEMENT_PROPERTY_OBJECT|STACK_ELEMENT_PROPERTY_CONTAINER_SLOT)) == 0){
				ZS_VM_ERROR("Variable '%s' as type '%s' it doesn't implements iterator"
					,SFI_GET_SYMBOL_NAME(_script_function,instruction)
					,data->zs->stackElementToString(data->vm_stk_current).toConstChar()
				);
				return false;
			}
		}

		stk_result_op2 = (StackElement *)(stk_result_op2->value);


		// ok vm_stk_current holds the iter object
		if(stk_result_op1->properties & STACK_ELEMENT_PROPERTY_OBJECT){
			// get iterator object and references +1
			obj=(Object *)stk_result_op1->value;
		}else{ // slot container
			// get iterator object and references +1
			obj=((ContainerSlot *)(stk_result_op1->value))->getSrcContainerRef();
		}


		sc=obj->getType();

		symbol_iter=sc->getSymbolMemberFunction("_iter");

		if(symbol_iter != NULL){

			Function *so_function=(Function *)symbol_iter->ref_ptr;
			Object *so_object=obj;

			int n_args=0;

			// ObjectObject uses static method in order to call iter (i.e Object::iter(o)
			if((symbol_iter->properties & SYMBOL_PROPERTY_STATIC)!= 0){ //is static
				n_args=1;

				// only stores and not increment (++ ) in order to start the stk arg
				*data->vm_stk_current={(intptr_t)so_object,STACK_ELEMENT_PROPERTY_OBJECT};
				so_object=NULL;
			}

			ZS_VM_INNER_CALL(so_object,so_function,n_args);

			// ok vm_stk_current holds the iter object
			if(data->vm_stk_current->properties & STACK_ELEMENT_PROPERTY_OBJECT){
				// get iterator object and references +1
				obj=(Object *)data->vm_stk_current->value;
			}else{
				// get iterator object and references +1
				if(data->vm_stk_current->properties & STACK_ELEMENT_PROPERTY_CONTAINER_SLOT){
					obj=((ContainerSlot *)(data->vm_stk_current->value))->getSrcContainerRef();
				}else{
					ZS_VM_ERROR("Object '%s' does not returns an iterator object",obj->getTypeName());
					return false;
				}
			}


			vm_share_object(_vm,obj);

			sc=obj->getType();

			// check all functions...
			if(sc->getSymbolMemberFunction("_get")==NULL){
				ZS_VM_ERROR("IteratorObject '%s' does not implement '_get' function",obj->getTypeName());
				return false;
			}

			if(sc->getSymbolMemberFunction("_next")==NULL){
				ZS_VM_ERROR("IteratorObject '%s' does not implement '_next' function",obj->getTypeName());
				return false;
			}

			if(sc->getSymbolMemberFunction("_end")==NULL){
				ZS_VM_ERROR("IteratorObject '%s' does not implement '_end' function",obj->getTypeName());
				return false;
			}

			// everything allright -> store iterator obj
			*stk_result_op2=*data->vm_stk_current;
		}
		else{
			ZS_VM_ERROR("Object '%s' not implements 'iter'",obj->getTypeName());
			return false;
		}

		return true;

		// get iterator...
	lbl_exit_function:

		return false;

	}

	bool vm_perform_in_operator(
			VirtualMachine 	*_vm
			,Object 	*_this_object
			,Function *_script_function
			,Instruction 	*_instruction
			,StackElement 	*_stk_local_var
	){
		String 			error="";
		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		StackElement 		*	stk_result_op1=NULL;
		StackElement 		*	stk_result_op2=NULL;
		StackElement 			stk_result_op1_aux;
		StackElement 			stk_result_op2_aux;

		StackElement 			stk_aux1;
		Instruction			*	instruction=_instruction;

		 VM_POP_STK_TWO;

		 stk_result_op1_aux=data->zs->unwrapStackElement(*stk_result_op1);
		 stk_result_op2_aux=data->zs->unwrapStackElement(*stk_result_op2);


		if(stk_result_op2_aux.properties & STACK_ELEMENT_PROPERTY_OBJECT){
			Object *so_aux=(Object *)stk_result_op2_aux.value;

			switch(so_aux->type_id){
			case TYPE_ID_OBJECT_STRING: // check whether 'char' or 'string' exists
			if(stk_result_op1->properties & STACK_ELEMENT_PROPERTY_INT){
				ZS_VM_PUSH_STK_BOOLEAN(
					StringObjectZs_contains(
						data->zs
						,((StringObject *)so_aux)
						,(zs_int)stk_result_op1->value
					)
				);
			}else if(STACK_ELEMENT_IS_STRING_OBJECT(&stk_result_op1_aux)){
				String str_op1=((StringObject *)stk_result_op1_aux.value)->toString();
				ZS_VM_PUSH_STK_BOOLEAN(
					StringObjectZs_contains(
						data->zs
						,(StringObject *)so_aux
						,&str_op1)
				);
			}else{
				error="operand is not 'zs_int' or 'StringObject' type";
			}
			break;
			case TYPE_ID_OBJECT_ARRAY: // check whether value exists...
			//PUSH_STK_BOOLEAN(((ArrayObject *)so_aux)->exists(stk_result_op1));
				ZS_VM_PUSH_STK_BOOLEAN(
				ArrayObjectZs_contains(
					data->zs,(ArrayObject *)so_aux,&stk_result_op1_aux
				)
			);
			break;
			case TYPE_ID_OBJECT_OBJECT: // check key value exists...
			 if(stk_result_op1->properties & STACK_ELEMENT_PROPERTY_OBJECT){
				String str_op1=((StringObject *)stk_result_op1_aux.value)->toString();
				ZS_VM_PUSH_STK_BOOLEAN(
					ObjectObjectZs_contains(
						data->zs,(ObjectObject *)so_aux,&str_op1
					)
				);

			 }else{
			 }
			break;
			default:
				if(vm_call_metamethod(
						_vm,
						_script_function,
						_instruction,
						METAMETHOD_IN,
						&stk_result_op2_aux,
						&stk_result_op1_aux,
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
			ZS_VM_ERROR("Cannot perform operation 'in' because %s",error.toConstChar());
			return false;
		}

		return true;

lbl_exit_function:
		return false;
	}

}
