/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	bool vm_iterator_init(
			VirtualMachine 		*	vm
			 ,ScriptFunction 	*	calling_function
			,Instruction 		*	instruction
			,StackElement *_stk_local_var
	){

		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		// stk_op1 expects to be stk
		//ScriptFunction *sf_iter=NULL;
		Symbol *symbol_iter;

		StackElement *stk_result_op1=NULL;
		StackElement *stk_result_op2=NULL;

		 VM_POP_STK_TWO;

		// stk_op2 expects to be obj with container

		if((stk_result_op2->properties & STK_PROPERTY_PTR_STK) == 0){
			VM_ERRORF("internal: Expected stk");
			return false;
		}

		if((stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT) == 0){
			//VM_ERROR("internal: Expected object");
			if((data->stk_vm_current->properties & STK_PROPERTY_SCRIPT_OBJECT) == 0){
				VM_ERROR("Variable '%s' as type '%s' it doesn't implements iterator"
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)
					,stk_to_str(data->zs,data->stk_vm_current).c_str()
				);
				return false;
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
						,stk_to_typeof_str(data->zs,data->stk_vm_current).c_str()
				);
				return false;
			}



			obj=(ScriptObject *)data->stk_vm_current->value;

			if(!vm_share_script_object(vm,obj)){\
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

			// everything allright store and share pointer
			*stk_result_op2=*data->stk_vm_current;
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
			VirtualMachine *vm
			 ,ScriptFunction *calling_function
			,Instruction *instruction
			,StackElement *_stk_local_var
	){
		std::string error="";
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		StackElement *stk_result_op1=NULL;
		StackElement *stk_result_op2=NULL;

		 VM_POP_STK_TWO;

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
				std::string str_op1=((ScriptObjectString *)stk_result_op1->value)->toString();
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
				std::string str_op1=((ScriptObjectString *)stk_result_op1->value)->toString();
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
