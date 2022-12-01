/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	bool vm_iterator_init(
			VirtualMachine 		*	_vm
			,ScriptObject 		*	_this_object
			,ScriptFunction 	*	_script_function
			,Instruction 		*	_instruction
			,StackElement 		*	_stk_local_var
	){

		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		StackElement 			stk_aux1;
		Symbol 				*	symbol_iter;
		StackElement 		*	stk_result_op1=NULL;
		StackElement 		*	stk_result_op2=NULL;
		ScriptObject 		*	obj=NULL;
		ScriptType 			*	sc=NULL;
		Instruction			* 	instruction=_instruction;

		VM_POP_STK_TWO;

		// stk_op2 expects to be obj with container

		if((stk_result_op2->properties & STK_PROPERTY_PTR_STK) == 0){
			ZS_VM_ERRORF("internal: Expected stk");
			return false;
		}

		if((stk_result_op1->properties & (STK_PROPERTY_SCRIPT_OBJECT|STK_PROPERTY_CONTAINER_SLOT)) == 0){
			//ZS_VM_ERROR("internal: Expected object");
			if((data->vm_stk_current->properties & (STK_PROPERTY_SCRIPT_OBJECT|STK_PROPERTY_CONTAINER_SLOT)) == 0){
				ZS_VM_ERROR("Variable '%s' as type '%s' it doesn't implements iterator"
					,SFI_GET_SYMBOL_NAME(_script_function,instruction)
					,stk_to_str(data->zs,data->vm_stk_current).c_str()
				);
				return false;
			}
		}

		stk_result_op2 = (StackElement *)(stk_result_op2->value);


		// ok vm_stk_current holds the iter object
		if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
			// get iterator object and references +1
			obj=(ScriptObject *)stk_result_op1->value;
		}else{ // slot container
			// get iterator object and references +1
			obj=((ContainerSlot *)(stk_result_op1->value))->getSrcContainerRef();
		}


		sc=obj->getScriptType();

		symbol_iter=sc->getSymbolMemberFunction("_iter");

		if(symbol_iter != NULL){

			ScriptFunction *so_function=(ScriptFunction *)symbol_iter->ref_ptr;
			ScriptObject *so_object=obj;

			int n_args=0;

			// ObjectScriptObject uses static method in order to call iter (i.e Object::iter(o)
			if((symbol_iter->properties & SYMBOL_PROPERTY_STATIC)!= 0){ //is static
				n_args=1;

				// only stores and not increment (++ ) in order to start the stk arg
				*data->vm_stk_current={(intptr_t)so_object,STK_PROPERTY_SCRIPT_OBJECT};
				so_object=NULL;
			}

			ZS_VM_INNER_CALL(so_object,so_function,n_args,"_iter");

			// ok vm_stk_current holds the iter object
			if(data->vm_stk_current->properties & STK_PROPERTY_SCRIPT_OBJECT){
				// get iterator object and references +1
				obj=(ScriptObject *)data->vm_stk_current->value;
			}else{
				// get iterator object and references +1
				if(data->vm_stk_current->properties & STK_PROPERTY_CONTAINER_SLOT){
					obj=((ContainerSlot *)(data->vm_stk_current->value))->getSrcContainerRef();
				}else{
					ZS_VM_ERROR("Object '%s' does not returns an iterator object",obj->getTypeName());
					return false;
				}
			}


			if(!vm_share_script_object(_vm,obj)){\
				goto lbl_exit_function;\
			}\

			sc=obj->getScriptType();

			// check all functions...
			if(sc->getSymbolMemberFunction("_get")==NULL){
				ZS_VM_ERROR("IteratorObject '%s' does not implement '_get' function",obj->getTypeName());
				return false;
			}

			if(sc->getSymbolMemberFunction("_post_inc")==NULL){
				ZS_VM_ERROR("IteratorObject '%s' does not implement '_post_inc' function",obj->getTypeName());
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
			,ScriptObject 	*_this_object
			,ScriptFunction *_script_function
			,Instruction 	*_instruction
			,StackElement 	*_stk_local_var
	){
		zs_string 			error="";
		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		StackElement 		*	stk_result_op1=NULL;
		StackElement 		*	stk_result_op2=NULL;
		StackElement 			stk_aux1;
		Instruction			*	instruction=_instruction;

		 VM_POP_STK_TWO;

		if(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so_aux=(ScriptObject *)stk_result_op2->value;

			switch(so_aux->idx_script_type){
			case IDX_TYPE_SCRIPT_OBJECT_STRING: // check whether 'char' or 'string' exists
			if(stk_result_op1->properties & STK_PROPERTY_ZS_INT){
				VM_PUSH_STK_BOOLEAN(
					StringScriptObjectWrap_contains(
						data->zs
						,((StringScriptObject *)so_aux)
						,(zs_int)stk_result_op1->value
					)
				);
			}else if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)){
				zs_string str_op1=((StringScriptObject *)stk_result_op1->value)->toString();
				VM_PUSH_STK_BOOLEAN(
					StringScriptObjectWrap_contains(
						data->zs
						,(StringScriptObject *)so_aux
						,&str_op1)
				);
			}else{
				error="operand is not 'zs_int' or 'StringScriptObject' type";
			}
			break;
			case IDX_TYPE_SCRIPT_OBJECT_VECTOR: // check whether value exists...
			//PUSH_STK_BOOLEAN(((VectorScriptObject *)so_aux)->exists(stk_result_op1));
				VM_PUSH_STK_BOOLEAN(
				VectorScriptObjectWrap_contains(
					data->zs,(VectorScriptObject *)so_aux,stk_result_op1
				)
			);
			break;
			case IDX_TYPE_SCRIPT_OBJECT_OBJECT: // check key value exists...
			 if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
				zs_string str_op1=((StringScriptObject *)stk_result_op1->value)->toString();
				VM_PUSH_STK_BOOLEAN(
					ObjectScriptObjectWrap_contains(
						data->zs,(ObjectScriptObject *)so_aux,&str_op1
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
			ZS_VM_ERROR("Cannot perform operation 'in' because %s",error.c_str());
			return false;
		}

		return true;

lbl_exit_function:
		return false;
	}

}
