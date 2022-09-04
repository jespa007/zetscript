/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	bool vm_operation_set_metamethod(
			VirtualMachine 			*	_vm
			,ScriptFunction 		*	_calling_function
			,Instruction 			*	_instruction
			,StackElement 			*	_stk_result_op1
			,StackElement 			*	_stk_result_op2
			, ByteCodeMetamethod 		_byte_code_metamethod
	){
		VirtualMachineData 	*			data=(VirtualMachineData *)_vm->data;
		MemberProperty 		*			member_property=NULL;
		Symbol 				*			symbol_setter=NULL;
		ScriptObject 		*			so_aux=NULL;
		//StackElement 		*			stk_var=NULL;
		StackElement 					stk_aux1;
		StackElement 		*			stk_result_op1=_stk_result_op1;
		StackElement 		*			stk_result_op2=_stk_result_op2;
		StackMemberProperty *			stk_mp_aux=NULL;
		MetamethodMembers 	*			ptr_metamethod_members_aux=NULL;
		ScriptFunction 		*			ptr_function_found=NULL;
		MetamethodMemberSetterInfo 		setter_info;
		const char 			*			str_set_metamethod=byte_code_metamethod_to_symbol_str(_byte_code_metamethod);
		const char 			*			str_aka_set_metamethod=byte_code_metamethod_to_operator_str(_byte_code_metamethod);
		Instruction			*			instruction=_instruction;

		if(_byte_code_metamethod == BYTE_CODE_METAMETHOD_ADD_SET){
			if(	STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)){\
				((zs_string *)(((StringScriptObject *)stk_result_op1->value)->value))->append(\
						(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT)?(((ScriptObject *)stk_result_op2->value)->toString()):stk_to_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op2)\
				);\
				VM_PUSH_STK_SCRIPT_OBJECT(stk_result_op1->value);\
				return true;
			}else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
						&&\
					STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
			){\
				ObjectScriptObject::append(data->zs, (ObjectScriptObject *)stk_result_op1->value,(ObjectScriptObject *)stk_result_op1->value);\
				VM_PUSH_STK_SCRIPT_OBJECT(stk_result_op1->value);\
				return true;
			}
		}


		LOAD_PROPERTIES(_byte_code_metamethod); /* saves stk_aux1 --> vm_stk_current points to stk_result_op2 that is the a parameter to pass */\
		setter_info=ptr_metamethod_members_aux->getSetterInfo(_byte_code_metamethod);
		if(setter_info.setters->size()==0){\
			METAMETHOD_OPERATION_NOT_FOUND(_byte_code_metamethod); \
			goto lbl_exit_function;
		}\
		ptr_function_found=(ScriptFunction *)((Symbol *)(((StackElement *)setter_info.setters->items[0])->value))->ref_ptr;\
		/* find function if c */ \
		if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){ /* because object is native, we can have more than one _setter */ \
			if(member_property==NULL){
				strcpy(
					data->vm_str_metamethod_aux
					,str_set_metamethod
				);
			}else{
				ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD(
					data->vm_str_metamethod_aux
					,_byte_code_metamethod
					,member_property->property_name.c_str()
				);
			}
			if((ptr_function_found=vm_find_native_function( \
				_vm \
				,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
				,_calling_function\
				,_instruction\
				,false\
				,data->vm_str_metamethod_aux
				,data->vm_stk_current \
				,1))==NULL\
			){ \
				if(member_property!=NULL){ \
					VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
							,so_aux->getScriptType()->str_script_type.c_str()\
							,member_property->property_name.c_str()\
							,str_set_metamethod\
					);\
				}else{\
					VM_STOP_EXECUTE("Class '%s' does not implement '%s' metamethod" \
							,so_aux->getScriptType()->str_script_type.c_str() \
							,str_set_metamethod\
					);\
				}\
			}\
		}else if(setter_info.setters->size()>1){\
			symbol_setter = so_aux->getScriptType()->getSymbol(str_set_metamethod); \
			if(symbol_setter == NULL){\
				VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"\
					,str_set_metamethod\
					,str_aka_set_metamethod\
				);\
			}\
			if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
				VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function"
					,str_set_metamethod
					,str_aka_set_metamethod
				);\
			}\
			ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
		}\
		/* call metamethod  */ \
		VM_INNER_CALL(\
			so_aux\
			,ptr_function_found\
			,1 \
			,ptr_function_found->name_script_function.c_str()\
		);\
		/*getter after*/\
		if(ptr_metamethod_members_aux->getter!=NULL){\
			/* call _neg */\
				VM_INNER_CALL(\
					so_aux\
					,(ScriptFunction *)ptr_metamethod_members_aux->getter->ref_ptr\
					,0 \
					,ptr_metamethod_members_aux->getter->name.c_str()\
			);\
		}else{ /* store object */ \
			if(_stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
				data->vm_stk_current->value=(zs_int)so_aux;\
				data->vm_stk_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->vm_stk_current=stk_aux1;
			}
		}
		data->vm_stk_current++;\

		return true;

	lbl_exit_function:

		return false;
	}
}
