namespace zetscript{
	bool vm_byte_code_new_string(
			VirtualMachine *vm
			,Instruction *instruction
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		ScriptObject *so_aux=NULL;

		so_aux= ScriptObjectString::newScriptObjectString(data->zs,instruction->getConstantValueOp2ToString(false));
		if(!vm_create_shared_script_object(vm,so_aux)){
			return false;
		}
		(*data->stk_vm_current++)={(zs_int)so_aux,STK_PROPERTY_SCRIPT_OBJECT};
		return true;
	}
}
