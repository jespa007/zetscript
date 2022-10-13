/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	bool vm_byte_code_new_object_by_value(
			VirtualMachine 	*	_vm
			,ScriptFunction *	_calling_function
			,Instruction 	*	_instruction
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		StackElement *stk_result_op1=NULL;
		ScriptType *sc_aux1=NULL;
		Symbol *symbol_aux=NULL;
		ScriptObject *so_aux=NULL;
		ClassScriptObject *so_class_aux1=NULL;
		Instruction *instruction=_instruction;

		VM_POP_STK_ONE;
		 if(STK_VALUE_IS_TYPE(stk_result_op1)){
			sc_aux1=data->script_type_factory->getScriptType(stk_result_op1->value);
			if(!data->script_type_factory->isScriptTypeInstanceable(stk_result_op1->value)){
				VM_STOP_EXECUTE("'%s' type is not object instanceable",sc_aux1->getTypeName());
			}

			 symbol_aux=NULL;

			 so_aux=NEW_OBJECT_VAR_BY_TYPE_IDX(data->script_type_factory,stk_result_op1->value);

			if(!vm_create_shared_script_object(_vm,so_aux)){
				goto lbl_exit_function;
			}

			data->vm_stk_current->value=(zs_int)so_aux;
			data->vm_stk_current->properties=STK_PROPERTY_SCRIPT_OBJECT;
			data->vm_stk_current++;

			if(so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS){ // custom object by user

				so_class_aux1=(ClassScriptObject *)so_aux;

				so_class_aux1->info_function_new=_calling_function;
				so_class_aux1->instruction_new=_instruction;

				// check for constructor
				symbol_aux=sc_aux1->getSymbolMemberFunction(CONSTRUCTOR_FUNCTION_NAME);

				 if(symbol_aux != NULL){
					 data->vm_stk_current->value=(zs_int)symbol_aux;
					 data->vm_stk_current->properties=STK_PROPERTY_MEMBER_FUNCTION;
					 data->vm_stk_current++;
				 }
			}

			if(symbol_aux == NULL){
				VM_PUSH_STK_UNDEFINED;
			}

		 }else{
			VM_STOP_EXECUTE("var '%s' expected as 'type' but it was '%s'"
					,SFI_GET_SYMBOL_NAME(_calling_function,_instruction)
					, stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1)
			);
		 }
		 return	true;
		 lbl_exit_function:
		 return false;
	}
}
