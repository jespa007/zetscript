/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	bool vm_call_metamethod(
		VirtualMachine 		*	_vm
		,ScriptFunction 	*	_script_function
		,Instruction 		*	_instruction
		,ByteCodeMetamethod 	_byte_code_metamethod
		,StackElement 		*	_stk_result_op1
		,StackElement 		*	_stk_result_op2
		, bool 					_is_static
		, bool 					_is_je_case

	) {
		VirtualMachineData *	data=(VirtualMachineData *)_vm->data;
		zs_string 				str_stk_result_op1_full_definition="";
		zs_string 				str_stk_result_op2_full_definition="";
		StackElement 		*	stk_vm_current_backup,*stk_args;
		ScriptFunction 		*	ptr_function_found=NULL;
		StackElement 			ret_obj;
		const char 			*	byte_code_metamethod_operator_str=byte_code_metamethod_to_operator_str(_byte_code_metamethod);
		const char 			*	str_symbol_metamethod=byte_code_metamethod_to_symbol_str(_byte_code_metamethod);
		zs_string 				error_found="";
		ScriptObject 		*	script_object=NULL;
		zs_string 				str_script_type_object_found="";
		int 					n_stk_args=_is_static?2:1;
		size_t 					n_stk_local_symbols=0;
		StackElement 		*	stk_return=NULL;
		int 					n_returned_arguments_from_function=0;
		StackElement 		*	stk_result_op1=_stk_result_op1;
		StackElement 		*	stk_result_op2=_stk_result_op2;
		Instruction			*	instruction=_instruction;


		if(_byte_code_metamethod == BYTE_CODE_METAMETHOD_ADD){
			if(		STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)\
						||\
					STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)\
			){\
					StringScriptObject *so_string=StringScriptObject::newStringScriptObjectAddStk(data->zs,stk_result_op1,stk_result_op2);\
					vm_create_shared_script_object(_vm,so_string);\
					VM_PUSH_STK_SCRIPT_OBJECT(so_string);\
					return true;
			}else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
						&&\
					STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
			){\
				script_object=VectorScriptObject::concat(\
							data->zs\
							,(VectorScriptObject *)stk_result_op1->value\
							,(VectorScriptObject *)stk_result_op2->value\
					);\
					vm_create_shared_script_object(_vm,script_object);\
					VM_PUSH_STK_SCRIPT_OBJECT(script_object);\
					return true;
			}else if(STK_IS_SCRIPT_OBJECT_OBJECT(stk_result_op2)\
						&&\
					STK_IS_SCRIPT_OBJECT_OBJECT(stk_result_op2)\
			){\
				script_object=ObjectScriptObject::concat(\
							data->zs\
							,(ObjectScriptObject *)stk_result_op1->value\
							,(ObjectScriptObject *)stk_result_op2->value\
					);\
					vm_create_shared_script_object(_vm,script_object);\
					VM_PUSH_STK_SCRIPT_OBJECT(script_object);\
					return true;
			}
		}

		ret_obj.setUndefined();

		// init stk
		stk_vm_current_backup=data->vm_stk_current;
		stk_args=data->vm_stk_current;

		// op1/op2 should be the object that have the metamethod
		if((stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT)){
			script_object=(ScriptObject *)stk_result_op1->value;
			str_script_type_object_found=script_object->getTypeName();
		}else if ((_is_static==true) && (stk_result_op2!=NULL && (stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT))){
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
						,byte_code_metamethod_to_symbol_str(_byte_code_metamethod)
					);
				}
			}
			goto apply_metamethod_error;
		}

		//------------------------------------
		// push stk results...
		if(_is_static == true){
			*data->vm_stk_current++=*stk_result_op1;
			*data->vm_stk_current++=*stk_result_op2;
		}else{
			*data->vm_stk_current++=*stk_result_op2;
		}
		//------------------------------------


		if(script_object->isNativeObject()){ // because isNativeObject it can have more than one setter
			if((ptr_function_found = vm_find_native_function(
				_vm
				,data->script_type_factory->getScriptType(script_object->idx_script_type)
				,_script_function
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

			if(_is_static && ((sf->properties & FUNCTION_PROPERTY_STATIC) == 0)){
				error_found=zs_strutils::format("Operator metamethod '%s (aka %s)' is not a static function (i.e add 'static' keyword )",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}

			if((_is_static==false) && ((sf->properties & FUNCTION_PROPERTY_STATIC) != 0)){
				error_found=zs_strutils::format("Operator metamethod '%s (aka %s)' is static function and should be a member function (i.e remove 'static' keyword)",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}


			ptr_function_found=(ScriptFunction *)symbol->ref_ptr;

		}


		if((ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){
			// we have to share any object to avoid be removed on function exit
			if(_is_static == true){ // the share of the 1st object if static and object
				if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){
					if(vm_share_script_object(_vm,(ScriptObject *)stk_result_op1->value)==false){
						goto apply_metamethod_error;
					}
				}
			}

			if(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT){
				if(vm_share_script_object(_vm,(ScriptObject *)stk_result_op2->value)==false){
					goto apply_metamethod_error;
				}
			}

			vm_execute_script_function(
				_vm
				,_is_static?NULL:script_object // do not pass script_object as this on static function, it will be destroyed on pop function
				,ptr_function_found
				,stk_args
			);

			n_stk_local_symbols=ptr_function_found->local_variables->size();
		}else{ //
			vm_execute_native_function(
					_vm
					,_script_function
					,instruction
					,_is_static ? NULL:script_object
					,ptr_function_found
					,stk_args
					,n_stk_args
			);

			n_stk_local_symbols=n_stk_args;
		}

		stk_return=(stk_args+n_stk_local_symbols );
		n_returned_arguments_from_function=data->vm_stk_current-stk_return;

		if(n_returned_arguments_from_function==0){
			error_found="Metamethod function should return a value";
			goto apply_metamethod_error;
		}

		// setup all returned variables from function
		for(int i=0; i < n_returned_arguments_from_function; i++){

			StackElement *stk_ret = --data->vm_stk_current;

			// if a scriptvar --> init shared
			if(stk_ret->properties & STK_PROPERTY_SCRIPT_OBJECT){
				ClassScriptObject *sv=(ClassScriptObject *)stk_ret->value;

				// Auto destroy always C when ref == 0
				sv->deleteNativeObjectOnDestroy(true);

				if(sv->shared_pointer == NULL){ // if return this, it holds ptr_shared_pointer
					if(!vm_create_shared_script_object(_vm,sv)){
						goto apply_metamethod_error;
					}
				}
			}
			// ... and push result if not function constructor...
		}

		ret_obj=stk_return[0];


		// reset stack...
		data->vm_stk_current=stk_vm_current_backup;

		*data->vm_stk_current++ = ret_obj;

		return data->vm_error == false;

	apply_metamethod_error:

		if(_is_je_case){
			ZS_VM_ERROR("Error evaluating case for variable as type '%s': %s"
				,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
				,error_found.c_str()
			);
		}else{
			zs_string tip=". The operation is incompatible or its metamethod is not defined properly";
			if(((stk_result_op2!=NULL) && STK_VALUE_IS_MEMBER_PROPERTY(stk_result_op2)) || STK_VALUE_IS_MEMBER_PROPERTY(stk_result_op1)){
				tip=". Check whether any of the member property involved in the operation has defined the getter (i.e _get) properly";
			}

			if(stk_result_op2 != NULL){
				ZS_VM_ERROR("Operator '%s' (aka %s) cannot be performed as operation with types '(%s) %s (%s)'%s%s%s"
					,byte_code_metamethod_to_operator_str(_byte_code_metamethod)
					,byte_code_metamethod_to_symbol_str(_byte_code_metamethod)
					,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
					,byte_code_metamethod_to_operator_str(_byte_code_metamethod)
					,stk_to_typeof_str(data->zs,stk_result_op2).c_str()
					,error_found.empty()?"":":"
					,error_found.c_str()
					,tip.c_str()
				);
			}else{
				ZS_VM_ERROR("Operator '%s' (aka %s) cannot be performed as operation with types '%s (%s)'%s%s%s"
					,byte_code_metamethod_to_operator_str(_byte_code_metamethod)
					,byte_code_metamethod_to_symbol_str(_byte_code_metamethod)
					,byte_code_metamethod_to_operator_str(_byte_code_metamethod)
					,stk_to_typeof_str(data->zs,stk_result_op1).c_str()
					,error_found.empty()?"":":"
					,error_found.c_str()
					,tip.c_str()
				);
			}
		}

		return false;
	}

	bool vm_call_metamethod_set(
			VirtualMachine 			*	_vm
			,ScriptFunction 		*	_script_function
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
				(((StringScriptObject *)stk_result_op1->value)->str_ptr)->append(\
						(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT)?(((ScriptObject *)stk_result_op2->value)->toString()):stk_to_str(ZS_VM_STR_AUX_PARAM_0,data->zs,stk_result_op2)\
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

		// particular errors
		if((stk_result_op2->properties==0) || (stk_result_op2->properties & STK_PROPERTY_NULL)){
			ZS_VM_MAIN_ERROR(\
					VM_MAIN_ERROR_LOAD_PROPERTIES_ERROR\
					,stk_result_op2\
					,_byte_code_metamethod\
			);\
		}


		LOAD_PROPERTIES(_byte_code_metamethod); /* saves stk_aux1 --> vm_stk_current points to stk_result_op2 that is the a parameter to pass */\
		setter_info=ptr_metamethod_members_aux->getSetterInfo(_byte_code_metamethod);
		if(setter_info.setters->size()==0){\
			METAMETHOD_OPERATION_NOT_FOUND(_byte_code_metamethod); \
			goto lbl_exit_function;
		}\
		ptr_function_found=(ScriptFunction *)((Symbol *)(((StackElement *)setter_info.setters->items[0])->value))->ref_ptr;\
		/* find function if c */ \
		if((ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF)==0){

			// call_metamethod_set is always non-static, so it shares stk_result_op2 if object to
			// avoid be removed on function exit stk_result_op1 is always the _this object
			if(stk_result_op2->properties & STK_PROPERTY_SCRIPT_OBJECT){
				if(vm_share_script_object(_vm,(ScriptObject *)stk_result_op2->value)==false){
					ZS_VM_STOP_EXECUTEF("Internal error: cannot share script object");
				}
			}

			if(setter_info.setters->size()>1){
				symbol_setter = so_aux->getScriptType()->getSymbol(str_set_metamethod);
				if(symbol_setter == NULL){
					ZS_VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"
						,str_set_metamethod
						,str_aka_set_metamethod
					);
				}
				if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){
					ZS_VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function"
						,str_set_metamethod
						,str_aka_set_metamethod
					);\
				}
				ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;
			}
		}else{ /* because object is native, we can have more than one _setter */ \
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
				,_script_function\
				,_instruction\
				,false\
				,data->vm_str_metamethod_aux
				,data->vm_stk_current \
				,1))==NULL\
			){ \
				if(member_property!=NULL){ \
					ZS_VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
							,so_aux->getScriptType()->str_script_type.c_str()\
							,member_property->property_name.c_str()\
							,str_set_metamethod\
					);\
				}else{\
					ZS_VM_STOP_EXECUTE("Class '%s' does not implement '%s' metamethod" \
							,so_aux->getScriptType()->str_script_type.c_str() \
							,str_set_metamethod\
					);\
				}\
			}\
		}
		/* call metamethod  */ \
		ZS_VM_INNER_CALL(\
			so_aux\
			,ptr_function_found\
			,1 \
			,ptr_function_found->name_script_function.c_str()\
		);\
		/*getter after*/\
		if(ptr_metamethod_members_aux->getter!=NULL){\
			/* call _neg */\
				ZS_VM_INNER_CALL(\
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

	bool vm_call_metamethod_operation_post(
		VirtualMachine 			*	_vm
		,ScriptFunction 		*	_script_function
		,Instruction 			*	_instruction
		,StackElement 			*	_stk_result_op1
		,ByteCodeMetamethod 		_byte_code_metamethod
		,ByteCodeMetamethod			_pre_operation=BYTE_CODE_METAMETHOD_INVALID

	){
		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		ScriptObject 		*	so_aux=NULL;
		StackElement		*	stk_result_op1=_stk_result_op1;
		MetamethodMembers 	*	ptr_metamethod_members_aux=NULL;
		Symbol 				*	symbol_metamethod_post=NULL;
		Symbol 				*	symbol_metamethod_pre_operation=NULL;
		MemberProperty		*	member_property=NULL;
		StackElement			stk_aux1;
		StackMemberProperty	*	stk_mp_aux=NULL;
		Instruction			*	instruction=_instruction;

		LOAD_PROPERTIES(_byte_code_metamethod);\

		switch(_byte_code_metamethod){
		case BYTE_CODE_METAMETHOD_POST_INC:
			symbol_metamethod_post=ptr_metamethod_members_aux->post_inc;
			break;
		case BYTE_CODE_METAMETHOD_POST_DEC:
			symbol_metamethod_post=ptr_metamethod_members_aux->post_dec;
			break;
		default:
			break;

		}

		if(symbol_metamethod_post==NULL){\
			METAMETHOD_OPERATION_NOT_FOUND(_byte_code_metamethod); \
		}\

		switch(_pre_operation){
		case BYTE_CODE_METAMETHOD_NEG:
			if(ptr_metamethod_members_aux->neg==NULL){\
				METAMETHOD_OPERATION_NOT_FOUND(BYTE_CODE_METAMETHOD_NEG); \
			}\
			symbol_metamethod_pre_operation=ptr_metamethod_members_aux->neg;
			break;
		case BYTE_CODE_METAMETHOD_BWC:
			if(ptr_metamethod_members_aux->bwc==NULL){\
				METAMETHOD_OPERATION_NOT_FOUND(BYTE_CODE_METAMETHOD_BWC); \
			}\
			symbol_metamethod_pre_operation=ptr_metamethod_members_aux->bwc;
			break;
		default:
			if(ptr_metamethod_members_aux->getter!=NULL){\
				symbol_metamethod_pre_operation=ptr_metamethod_members_aux->getter;
			}
			break;
		}

		if(symbol_metamethod_pre_operation != NULL){
			/* call _neg */\
			ZS_VM_INNER_CALL(\
					so_aux\
					,(ScriptFunction *)symbol_metamethod_pre_operation->ref_ptr\
					, 0 \
					,symbol_metamethod_pre_operation->name\
			);\
		}else{ /* store object */ \
			if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){\
				data->vm_stk_current->value=(zs_int)so_aux;\
				data->vm_stk_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->vm_stk_current=__ZS_STK_VAR_COPY__;\
			}\
		}\
		data->vm_stk_current++;\

		/* call post operation metamethod */\
		ZS_VM_INNER_CALL(\
				so_aux\
				,(ScriptFunction *)symbol_metamethod_post->ref_ptr\
				, 0 \
				,symbol_metamethod_post->name\
		);\
		return true;

lbl_exit_function:
		return false;

	}

	bool vm_call_metamethod_operation_pre(
		VirtualMachine 			*	_vm
		,ScriptFunction 		*	_script_function
		,Instruction 			*	_instruction
		,StackElement 			*	_stk_result_op1
		,ByteCodeMetamethod 		_byte_code_metamethod
	){

		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		ScriptObject 		*	so_aux=NULL;
		StackElement		*	stk_result_op1=_stk_result_op1;
		MetamethodMembers 	*	ptr_metamethod_members_aux=NULL;
		Symbol 				*	symbol_metamethod_pre=NULL;
		MemberProperty		*	member_property=NULL;
		StackElement			stk_aux1;
		StackMemberProperty	*	stk_mp_aux=NULL;
		Instruction			*	instruction=_instruction;

		LOAD_PROPERTIES(_byte_code_metamethod);\

		switch(_byte_code_metamethod){
		case BYTE_CODE_METAMETHOD_PRE_INC:
			symbol_metamethod_pre=ptr_metamethod_members_aux->pre_inc;
			break;
		case BYTE_CODE_METAMETHOD_PRE_DEC:
			symbol_metamethod_pre=ptr_metamethod_members_aux->pre_dec;
			break;
		default:
			break;
		}

		/* call pre operation metamethod */\
		if(symbol_metamethod_pre==NULL){\
			METAMETHOD_OPERATION_NOT_FOUND(_byte_code_metamethod); \
		}\
		ZS_VM_INNER_CALL(\
				so_aux\
				,(ScriptFunction *)symbol_metamethod_pre->ref_ptr\
				,0 \
				,symbol_metamethod_pre->name\
		);\
		/*getter after*/\
		if(ptr_metamethod_members_aux->getter!=NULL){\
			/* call _neg */\
			ZS_VM_INNER_CALL(\
					so_aux\
					,(ScriptFunction *)ptr_metamethod_members_aux->getter->ref_ptr\
					,0 \
					,ptr_metamethod_members_aux->getter->name\
			);\
		}else{ /* store object */ \
			if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){\
				data->vm_stk_current->value=(zs_int)so_aux;\
				data->vm_stk_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->vm_stk_current=__ZS_STK_VAR_COPY__;\
			}\
		}\
		data->vm_stk_current++;\

		return true;

lbl_exit_function:

		return false;
	}
}
