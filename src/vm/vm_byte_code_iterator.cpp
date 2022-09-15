/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	bool vm_iterator_init(
			VirtualMachine 		*	_vm
			,ScriptObject 		*	_this_object
			,ScriptFunction 	*	_calling_function
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
			VM_ERRORF("internal: Expected stk");
			return false;
		}

		if((stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT) == 0){
			//VM_ERROR("internal: Expected object");
			if((data->vm_stk_current->properties & STK_PROPERTY_SCRIPT_OBJECT) == 0){
				VM_ERROR("Variable '%s' as type '%s' it doesn't implements iterator"
					,SFI_GET_SYMBOL_NAME(_calling_function,instruction)
					,stk_to_str(data->zs,data->vm_stk_current).c_str()
				);
				return false;
			}
		}

		stk_result_op2 = (StackElement *)(stk_result_op2->value);
		obj=(ScriptObject *)stk_result_op1->value;
		sc=obj->getScriptType();

		symbol_iter=sc->getSymbolMemberFunction("iter");

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

			VM_INNER_CALL(so_object,so_function,n_args,"iter");

			// ok vm_stk_current holds the iter object
			if((data->vm_stk_current->properties & STK_PROPERTY_SCRIPT_OBJECT) == false){
				VM_ERROR("Expected IteratorObject returned by 'iter' but it was '%s'"
						,stk_to_typeof_str(data->zs,data->vm_stk_current).c_str()
				);
				return false;
			}


			// get iterator object and references +1
			obj=(ScriptObject *)data->vm_stk_current->value;

			if(!vm_share_script_object(_vm,obj)){\
				goto lbl_exit_function;\
			}\

			sc=obj->getScriptType();

			// check all functions...
			if(sc->getSymbolMemberFunction("get")==NULL){
				VM_ERROR("IteratorObject '%s' does not implement 'get' function",obj->getTypeName());
				return false;
			}

			if(sc->getSymbolMemberFunction("_post_inc")==NULL){
				VM_ERROR("IteratorObject '%s' does not implement '_post_inc' function",obj->getTypeName());
				return false;
			}

			if(sc->getSymbolMemberFunction("end")==NULL){
				VM_ERROR("IteratorObject '%s' does not implement 'end' function",obj->getTypeName());
				return false;
			}

			// everything allright -> store iterator obj
			*stk_result_op2=*data->vm_stk_current;
		}
		else{
			VM_ERROR("Object '%s' not implements 'iter'",obj->getTypeName());
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
			,ScriptFunction *_calling_function
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
						_calling_function,
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
			VM_ERROR("Cannot perform operation 'in' because %s",error.c_str());
			return false;
		}

		return true;

lbl_exit_function:
		return false;
	}

}
