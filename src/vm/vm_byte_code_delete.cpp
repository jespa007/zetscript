namespace zetscript{
	bool vm_byte_code_delete(
		VirtualMachine *vm
		,ScriptFunction *calling_function
		,Instruction *instruction
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		StackElement *stk_result_op1=NULL;
		ScriptObject *so_aux=NULL;
		ScriptObjectClass *so_class_aux1=NULL;

		VM_POP_STK_ONE;
		//script_var
		if(stk_result_op1->properties & STK_PROPERTY_PTR_STK){
			stk_result_op1=(StackElement *)stk_result_op1->value;
		}

		if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
			so_class_aux1=NULL;

			so_aux = (ScriptObject *)(stk_result_op1)->value;

			if(!vm_unref_shared_script_object(vm,so_aux,NULL)){
				return false;
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

		return true;
	}
}
