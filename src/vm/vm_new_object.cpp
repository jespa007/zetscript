/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	bool vm_new_object_by_value(
			VirtualMachine 	*	_vm
			,Function *	_script_function
			,Instruction 	*	_instruction
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		StackElement *stk_result_op1=NULL;
		Type *sc_aux1=NULL;
		Symbol *symbol_aux=NULL;
		Object *so_aux=NULL;
		ClassObject *so_class_aux1=NULL;
		Instruction *instruction=_instruction;

		VM_POP_STK_ONE;
		 if(STACK_ELEMENT_IS_TYPE(stk_result_op1)){
			sc_aux1=data->type_factory->getType(stk_result_op1->value);
			if(!data->type_factory->isTypeInstanceable(stk_result_op1->value)){
				ZS_VM_STOP_EXECUTE("'%s' type is not instantiable",sc_aux1->getTypeName());
			}

			 symbol_aux=NULL;

			 so_aux=NEW_OBJECT_VAR_BY_TYPE_ID(data->type_factory,stk_result_op1->value);

			vm_create_shared_object(_vm,so_aux);

			data->vm_stk_current->value=(zs_int)so_aux;
			data->vm_stk_current->properties=STACK_ELEMENT_PROPERTY_OBJECT;
			data->vm_stk_current++;

			if(so_aux->type_id >= TYPE_ID_OBJECT_CLASS){ // custom object by user

				so_class_aux1=(ClassObject *)so_aux;

				so_class_aux1->info_function_new=_script_function;
				so_class_aux1->instruction_new=_instruction;

				// check for constructor
				symbol_aux=sc_aux1->getSymbolMemberFunction(ZS_CONSTRUCTOR_FUNCTION_NAME);

				 if(symbol_aux != NULL){
					 data->vm_stk_current->value=(zs_int)symbol_aux;
					 data->vm_stk_current->properties=STACK_ELEMENT_PROPERTY_MEMBER_FUNCTION;
					 data->vm_stk_current++;
				 }
			}

			if(symbol_aux == NULL){
				ZS_VM_PUSH_STK_UNDEFINED;
			}

		 }else{
			ZS_VM_STOP_EXECUTE("var '%s' expected as 'type' but it was '%s'"
				,SFI_GET_SYMBOL_NAME(_script_function,_instruction)
				, data->zs->stackElementToStringTypeOf(ZS_VM_STR_AUX_PARAM_0,stk_result_op1)
			);
		 }
		 return	true;
		 lbl_exit_function:
		 return false;
	}
}
