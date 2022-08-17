/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	bool vm_call_metamethod(
		VirtualMachine *vm
		,ScriptFunction *calling_function
		,Instruction *instruction
		,ByteCodeMetamethod byte_code_metamethod
		,StackElement *stk_result_op1
		,StackElement *stk_result_op2
		, bool is_static
		, bool is_je_case

	) {
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		std::string str_stk_result_op1_full_definition="";
		std::string str_stk_result_op2_full_definition="";
		StackElement *stk_vm_current_backup,*stk_args;
		ScriptFunction *ptr_function_found=NULL;
		StackElement ret_obj;
		const char *byte_code_metamethod_operator_str=byte_code_metamethod_to_operator_str(byte_code_metamethod);
		const char *str_symbol_metamethod=byte_code_metamethod_to_symbol_str(byte_code_metamethod);
		std::string error_found="";
		ScriptObject *script_object=NULL;
		std::string str_script_type_object_found="";
		int n_stk_args=is_static?2:1;
		size_t n_stk_local_symbols=0;
		StackElement *stk_return=NULL;
		int n_returned_arguments_from_function=0;

		ret_obj.setUndefined();

		// init stk
		stk_vm_current_backup=data->stk_vm_current;
		stk_args=data->stk_vm_current;

		// op1/op2 should be the object that have the metamethod
		if((stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT)){
			script_object=(ScriptObject *)stk_result_op1->value;
			str_script_type_object_found=script_object->getTypeName();
		}else if ((is_static==true) && (stk_result_op2!=NULL && (stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT))){
			script_object=(ScriptObject *)stk_result_op2->value;
			str_script_type_object_found=script_object->getTypeName();

		}


		if(script_object == NULL){ // cannot perform operation
			if(str_script_type_object_found.empty()){ // not any object found
				// Because script elements can return "null" due undefined properties, do not show any error to not confuse.
				// If is an internal error, fix!
			}else{
				if(instruction->byte_code == BYTE_CODE_JE_CASE){
					error_found=zs_strutils::format("Unable to perform '==' operator for case conditional");
				}else{
					error_found=zs_strutils::format("Type '%s' does not implements metamethod '%s'"
						,str_script_type_object_found.c_str()
						,byte_code_metamethod_to_symbol_str(byte_code_metamethod)
					);
				}
			}
			goto apply_metamethod_error;
		}

		//------------------------------------
		// push stk results...
		if(is_static == true){
			*data->stk_vm_current++=*stk_result_op1;
			*data->stk_vm_current++=*stk_result_op2;
		}else{
			*data->stk_vm_current++=*stk_result_op2;
		}
		//------------------------------------


		if(script_object->isNativeObject()){ // because isNativeObject it can have more than one setter
			if((ptr_function_found = vm_find_native_function(
				vm
				,data->script_type_factory->getScriptType(script_object->idx_script_type)
				,calling_function
				,instruction
				,false
				,str_symbol_metamethod
				,stk_args
				,n_stk_args
			)) == NULL){
				error_found=zs_strutils::format("Operator metamethod '%s (aka %s)' it's not implemented or it cannot find appropriate arguments for calling function",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}

		}else{ // get first item...
			ScriptType *sc=script_object->getScriptType();
			Symbol * symbol = sc->getSymbolMemberFunction(str_symbol_metamethod);

			if(symbol == NULL){
				error_found=zs_strutils::format("Operator metamethod '%s (aka %s)' is not implemented",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}

			ScriptFunction *sf=(ScriptFunction *)symbol->ref_ptr;

			if(is_static && ((sf->properties & FUNCTION_PROPERTY_STATIC) == 0)){
				error_found=zs_strutils::format("Operator metamethod '%s (aka %s)' is not a static function (i.e add 'static' keyword )",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}

			if((is_static==false) && ((sf->properties & FUNCTION_PROPERTY_STATIC) != 0)){
				error_found=zs_strutils::format("Operator metamethod '%s (aka %s)' is static function and should be a member function (i.e remove 'static' keyword)",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}


			ptr_function_found=(ScriptFunction *)symbol->ref_ptr;

		}


		if((ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){
			// we have to share any object to avoid be removed on function exit

			if(is_static == true){ // the share of the 1st object if static and object
				if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
					if(vm_share_script_object(vm,(ScriptObject *)stk_result_op1->value)==false){
						goto apply_metamethod_error;
					}
				}
			}

			if(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT){
				if(vm_share_script_object(vm,(ScriptObject *)stk_result_op2->value)==false){
					goto apply_metamethod_error;
				}
			}

			vm_execute_function_script(
				vm
				,is_static?NULL:script_object // do not pass script_object as this on static function, it will be destroyed on pop function
				,ptr_function_found
				,stk_args
			);

			n_stk_local_symbols=ptr_function_found->local_variables->size();
		}else{ //
			vm_execute_function_native(
					vm
					,calling_function
					,instruction
					,is_static ? NULL:script_object
					,ptr_function_found
					,stk_args
					,n_stk_args
			);

			n_stk_local_symbols=n_stk_args;
		}

		stk_return=(stk_args+n_stk_local_symbols );
		n_returned_arguments_from_function=data->stk_vm_current-stk_return;

		if(n_returned_arguments_from_function==0){
			error_found="Metamethod function should return a value";
			goto apply_metamethod_error;
		}

		// setup all returned variables from function
		for(int i=0; i < n_returned_arguments_from_function; i++){

			StackElement *stk_ret = --data->stk_vm_current;

			// if a scriptvar --> init shared
			if(stk_ret->properties & STK_PROPERTY_SCRIPT_OBJECT){
				ScriptObjectClass *sv=(ScriptObjectClass *)stk_ret->value;

				// Auto destroy always C when ref == 0
				sv->deleteNativeObjectOnDestroy(true);

				if(sv->shared_pointer == NULL){ // if return this, it holds ptr_shared_pointer
					if(!vm_create_shared_script_object(vm,sv)){
						goto apply_metamethod_error;
					}
				}
			}
			// ... and push result if not function constructor...
		}

		ret_obj=stk_return[0];


		// reset stack...
		data->stk_vm_current=stk_vm_current_backup;

		*data->stk_vm_current++ = ret_obj;

		return data->vm_error == false;

	apply_metamethod_error:

		if(is_je_case){
			VM_ERROR("Error evaluating case for variable as type '%s': %s"
				,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
				,error_found.c_str()
			);
		}else{

			if(stk_result_op2 != NULL){
				VM_ERROR("Metamethod operation '%s' (aka %s). Failed performing operation by types '%s' %s '%s'%s %s"
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,byte_code_metamethod_to_symbol_str(byte_code_metamethod)
					,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,stk_to_typeof_str(data->zs,stk_result_op2).c_str()
					,error_found.empty()?"":":"
					,error_found.c_str()
				);
			}else{
				VM_ERROR("Metamethod operation '%s' (aka %s). Failed performing operation by types %s '%s' %s %s"
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,byte_code_metamethod_to_symbol_str(byte_code_metamethod)
					,byte_code_metamethod_to_operator_str(byte_code_metamethod)
					,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
					,error_found.empty()?"":":"
					,error_found.c_str()
				);
			}
		}

		return false;
	}
}
