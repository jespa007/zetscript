/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	bool vm_call_metamethod(
		VirtualMachine 		*		_vm
		,Function 	*		_script_function
		,Instruction 		*		_instruction
		,Metamethod 	_metamethod
		,StackElement 		*		_stk_result_op1
		,StackElement 		*		_stk_result_op2
		, bool 						_is_static
		, bool 						_is_je_case

	) {
		VirtualMachineData *	data=(VirtualMachineData *)_vm->data;
		String 				str_stk_result_op1_full_definition="";
		String 				str_stk_result_op2_full_definition="";
		StackElement 		*	stk_vm_current_backup,*stk_args;
		Function 		*	ptr_function_found=NULL;
		StackElement 			ret_obj;
		const char 			*	operator_str=MetamethodHelper::getMetamethodOperatorName(_metamethod);
		const char 			*	str_symbol_metamethod=MetamethodHelper::getMetamethodSymbolName(_metamethod);
		String 				error_found="";
		Object 		*	object=NULL;
		String 				str_type_object_found="";
		int 					n_stk_args=MetamethodHelper::getMetamethodNumberArguments(_metamethod);//_is_static?2:1;
		size_t 					n_stk_local_symbols=0;
		StackElement 		*	stk_return=NULL;
		int 					n_returned_arguments_from_function=0;
		StackElement 		*	stk_result_op1=_stk_result_op1;
		StackElement 		*	stk_result_op2=_stk_result_op2;
		Instruction			*	instruction=_instruction;


		if(_metamethod == METAMETHOD_ADD){
			if(		STACK_ELEMENT_IS_STRING_OBJECT(stk_result_op1)\
						||\
					STACK_ELEMENT_IS_STRING_OBJECT(stk_result_op2)\
			){\
					StringObject *so_string=StringObject::newStringObjectAddStk(data->zs,stk_result_op1,stk_result_op2);\
					vm_create_shared_object(_vm,so_string);\
					ZS_VM_PUSH_STK_OBJECT(so_string);\
					return true;
			}else if(STACK_ELEMENT_IS_ARRAY_OBJECT(stk_result_op1)\
						&&\
					STACK_ELEMENT_IS_ARRAY_OBJECT(stk_result_op2)\
			){\
				object=ArrayObject::concat(\
							data->zs\
							,(ArrayObject *)stk_result_op1->value\
							,(ArrayObject *)stk_result_op2->value\
					);\
					vm_create_shared_object(_vm,object);\
					ZS_VM_PUSH_STK_OBJECT(object);\
					return true;
			}else if(STACK_ELEMENT_IS_DICTIONARY_OBJECT(stk_result_op1)\
						&&\
					STACK_ELEMENT_IS_DICTIONARY_OBJECT(stk_result_op2)\
			){\
				object=DictionaryObject::concat(\
							data->zs\
							,(DictionaryObject *)stk_result_op1->value\
							,(DictionaryObject *)stk_result_op2->value\
					);\
					vm_create_shared_object(_vm,object);\
					ZS_VM_PUSH_STK_OBJECT(object);\
					return true;
			}
		}

		ret_obj.setUndefined();

		// init stk
		stk_vm_current_backup=data->vm_stk_current;
		stk_args=data->vm_stk_current;

		// op1/op2 should be the object that have the metamethod

		if((stk_result_op1->properties & STACK_ELEMENT_PROPERTY_OBJECT)){
			object=(Object *)stk_result_op1->value;
			str_type_object_found=object->getTypeName();
		}else if ((_is_static==true) && (stk_result_op2!=NULL && (stk_result_op2->properties & STACK_ELEMENT_PROPERTY_OBJECT))){
			object=(Object *)stk_result_op2->value;
			str_type_object_found=object->getTypeName();
		}


		if(object == NULL){ // cannot perform operation
			if(str_type_object_found.empty()){ // not any object found
				// Because script elements can return "null" due undefined properties, do not show any error to not confuse.
				// If is an internal error, fix!
			}else{
				if(instruction->byte_code == BYTE_CODE_JE_CASE){
					error_found=String::format("Unable to perform '==' operator for case conditional");
				}else{
					error_found=String::format("Type '%s' does not implements metamethod '%s'"
						,str_type_object_found.toConstChar()
						,MetamethodHelper::getMetamethodSymbolName(_metamethod)
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
			if(n_stk_args>0){
				*data->vm_stk_current++=*stk_result_op2;
			}
		}
		//------------------------------------


		if(object->isNativeObject()){ // because isNativeObject it can have more than one setter
			if((ptr_function_found = vm_find_native_function(
				_vm
				,data->type_factory->getType(object->type_id)
				,_script_function
				,instruction
				,false
				,str_symbol_metamethod
				,stk_args
				,n_stk_args
			)) == NULL){
				error_found=String::format("Operator metamethod '%s (aka %s)' it's not implemented or it cannot find appropriate arguments for calling function",str_symbol_metamethod,operator_str);
				goto apply_metamethod_error;
			}

		}else{ // get first item...
			Type *sc=object->getType();
			Symbol * symbol = sc->getSymbolMemberFunction(str_symbol_metamethod);

			if(symbol == NULL){
				error_found=String::format("Operator metamethod '%s (aka %s)' is not implemented",str_symbol_metamethod,operator_str);
				goto apply_metamethod_error;
			}

			Function *sf=(Function *)symbol->ref_ptr;

			if(_is_static && ((sf->properties & FUNCTION_PROPERTY_STATIC) == 0)){
				error_found=String::format("Operator metamethod '%s (aka %s)' is not a static function (i.e add 'static' keyword )",str_symbol_metamethod,operator_str);
				goto apply_metamethod_error;
			}

			if((_is_static==false) && ((sf->properties & FUNCTION_PROPERTY_STATIC) != 0)){
				error_found=String::format("Operator metamethod '%s (aka %s)' is static function and should be a member function (i.e remove 'static' keyword)",str_symbol_metamethod,operator_str);
				goto apply_metamethod_error;
			}


			ptr_function_found=(Function *)symbol->ref_ptr;

		}


		if((ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){
			// we have to share any object to avoid be removed on function exit
			if(_is_static == true){ // the share of the 1st object if static and object
				if(stk_result_op1->properties & STACK_ELEMENT_PROPERTY_OBJECT){
					vm_share_object(_vm,(Object *)stk_result_op1->value);
				}
			}

			if(stk_result_op2!=NULL){
				if(stk_result_op2->properties & STACK_ELEMENT_PROPERTY_OBJECT){
					vm_share_object(_vm,(Object *)stk_result_op2->value);
				}
			}

			vm_execute_script_function(
				_vm
				,_is_static?NULL:object // do not pass object as this on static function, it will be destroyed on pop function
				,ptr_function_found
				,stk_args
			);

			n_stk_local_symbols=ptr_function_found->local_variables->size();
		}else{ //
			vm_execute_native_function(
					_vm
					,_script_function
					,instruction
					,_is_static ? NULL:object
					,ptr_function_found
					,stk_args
					,n_stk_args
			);

			n_stk_local_symbols=n_stk_args;
		}

		stk_return=(stk_args+n_stk_local_symbols );
		n_returned_arguments_from_function=data->vm_stk_current-stk_return;

		if(n_returned_arguments_from_function==0){
			error_found="MetamethodHelper function should return a value";
			goto apply_metamethod_error;
		}

		ZS_CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_return,n_returned_arguments_from_function);

		ret_obj=stk_return[0];

		// reset stack...
		data->vm_stk_current=stk_vm_current_backup;

		*data->vm_stk_current++ = ret_obj;

		return data->vm_error == false;

	apply_metamethod_error:

		if(_is_je_case){
			ZS_VM_ERROR("Error evaluating case for variable as type '%s': %s"
				,data->zs->stackElementToStringTypeOf(stk_result_op1).toConstChar()
				,error_found.toConstChar()
			);
		}else{
			String tip=". The operation is incompatible or its metamethod is not defined properly";
			if(((stk_result_op2!=NULL) && STACK_ELEMENT_IS_MEMBER_PROPERTY(stk_result_op2)) || STACK_ELEMENT_IS_MEMBER_PROPERTY(stk_result_op1)){
				tip=". Check whether any of the member property involved in the operation has defined the getter (i.e _get) properly";
			}

			if(stk_result_op2 != NULL){
				ZS_VM_ERROR("Operator '%s' (aka %s) cannot be performed as operation with types '(%s) %s (%s)'%s%s%s"
					,MetamethodHelper::getMetamethodOperatorName(_metamethod)
					,MetamethodHelper::getMetamethodSymbolName(_metamethod)
					,data->zs->stackElementToStringTypeOf(stk_result_op1).toConstChar()
					,MetamethodHelper::getMetamethodOperatorName(_metamethod)
					,data->zs->stackElementToStringTypeOf(stk_result_op2).toConstChar()
					,error_found.empty()?"":":"
					,error_found.toConstChar()
					,tip.toConstChar()
				);
			}else{
				ZS_VM_ERROR("Operator '%s' (aka %s) cannot be performed as operation with types '%s (%s)'%s%s%s"
					,MetamethodHelper::getMetamethodOperatorName(_metamethod)
					,MetamethodHelper::getMetamethodSymbolName(_metamethod)
					,MetamethodHelper::getMetamethodOperatorName(_metamethod)
					,data->zs->stackElementToStringTypeOf(stk_result_op1).toConstChar()
					,error_found.empty()?"":":"
					,error_found.toConstChar()
					,tip.toConstChar()
				);
			}
		}

		return false;
	}

	bool vm_call_metamethod_set(
			VirtualMachine 			*		_vm
			,Function 		*		_script_function
			,Instruction 			*		_instruction
			,StackElement 			*		_stk_result_op1
			,StackElement 			*		_stk_result_op2
			, Metamethod 		_metamethod
	){
		VirtualMachineData 			*			data=(VirtualMachineData *)_vm->data;
		MemberProperty 				*			member_property=NULL;
		Symbol 						*			symbol_setter=NULL;
		Object 				*			so_aux=NULL;
		//StackElement 				*			stk_var=NULL;
		StackElement 							stk_aux1;
		StackElement 				*			stk_result_op1=_stk_result_op1;
		StackElement 				*			stk_result_op2=_stk_result_op2;
		StackElementMemberProperty 	*			stk_mp_aux=NULL;
		MetamethodMembers 			*			ptr_metamethod_members_aux=NULL;
		Function 				*			ptr_function_found=NULL;
		MetamethodMemberSetterInfo 				setter_info;
		const char 					*			str_set_metamethod=MetamethodHelper::getMetamethodSymbolName(_metamethod);
		const char 					*			str_aka_set_metamethod=MetamethodHelper::getMetamethodOperatorName(_metamethod);
		Instruction					*			instruction=_instruction;

		if(_metamethod == METAMETHOD_ADD_ASSIGN){
			if(	STACK_ELEMENT_IS_STRING_OBJECT(stk_result_op1)){\
				(((StringObject *)stk_result_op1->value)->str_ptr)->append(\
						(stk_result_op2->properties & STACK_ELEMENT_PROPERTY_OBJECT)?(((Object *)stk_result_op2->value)->toString()):data->zs->stackElementToString(ZS_VM_STR_AUX_PARAM_0,ZS_VM_STR_AUX_MAX_LENGTH,stk_result_op2)\
				);\
				ZS_VM_PUSH_STK_OBJECT(stk_result_op1->value);\
				return true;
			}else if(STACK_ELEMENT_IS_ARRAY_OBJECT(stk_result_op1)\
						&&\
					STACK_ELEMENT_IS_ARRAY_OBJECT(stk_result_op2)\
			){\
				DictionaryObject::append(data->zs, (DictionaryObject *)stk_result_op1->value,(DictionaryObject *)stk_result_op1->value);\
				ZS_VM_PUSH_STK_OBJECT(stk_result_op1->value);\
				return true;
			}
		}

		// particular errors
		if((stk_result_op2->properties==0) || (stk_result_op2->properties & STACK_ELEMENT_PROPERTY_NULL)){
			ZS_VM_MAIN_ERROR(\
					ZS_VM_MAIN_ERROR_ZS_LOAD_PROPERTIES_ERROR\
					,stk_result_op2\
					,_metamethod\
			);\
		}

		ZS_LOAD_PROPERTIES(_metamethod); /* saves stk_aux1 --> vm_stk_current points to stk_result_op2 that is the a parameter to pass */\
		setter_info=ptr_metamethod_members_aux->getSetterInfo(_metamethod);
		if(setter_info.setters->size()==0){\
			ZS_METAMETHOD_OPERATION_NOT_FOUND(_metamethod); \
			goto lbl_exit_function;
		}\
		ptr_function_found=(Function *)((Symbol *)(((StackElement *)setter_info.setters->get(0))->value))->ref_ptr;\
		/* find function if c */ \
		if((ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF)==0){

			// call_metamethod_set is always non-static, so it shares stk_result_op2 if object to
			// avoid be removed on function exit stk_result_op1 is always the _this object
			if(stk_result_op2->properties & STACK_ELEMENT_PROPERTY_OBJECT){
				vm_share_object(_vm,(Object *)stk_result_op2->value);
			}

			if(setter_info.setters->size()>1){
				symbol_setter = so_aux->getType()->getSymbol(str_set_metamethod);
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
				ptr_function_found=(Function *)symbol_setter->ref_ptr;
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
					,member_property->property_name.toConstChar()
					,_metamethod
				);
			}
			if((ptr_function_found=vm_find_native_function( \
				_vm \
				,data->type_factory->getType(so_aux->type_id)\
				,_script_function\
				,_instruction\
				,false\
				,data->vm_str_metamethod_aux
				,data->vm_stk_current \
				,1))==NULL\
			){ \
				if(member_property!=NULL){ \
					ZS_VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s' or doesn't match its parameter argument types. \nDetail : %s\n"\
							,so_aux->getType()->name.toConstChar()\
							,member_property->property_name.toConstChar()\
							,str_set_metamethod\
							,data->vm_error_description.toConstChar()\
					);\
				}else{\
					ZS_VM_STOP_EXECUTE("Type '%s' does not implement '%s' metamethod  or doesn't match its parameter argument types. \nDetail : %s" \
							,so_aux->getType()->name.toConstChar() \
							,str_set_metamethod\
							,data->vm_error_description.toConstChar()\
					);\
				}\
			}\
		}
		/* call metamethod  */ \
		ZS_VM_INNER_CALL(\
			so_aux\
			,ptr_function_found\
			,1 \
		);\
		/*getter after*/\
		if(ptr_metamethod_members_aux->getter!=NULL){\
			/* call _neg */\
				ZS_VM_INNER_CALL(\
					so_aux\
					,(Function *)ptr_metamethod_members_aux->getter->ref_ptr\
					,0 \
			);\
		}else{ /* store object */ \
			if(_stk_result_op1->properties & STACK_ELEMENT_PROPERTY_OBJECT){
				data->vm_stk_current->value=(zs_int)so_aux;\
				data->vm_stk_current->properties=STACK_ELEMENT_PROPERTY_OBJECT;\
			}else{\
				*data->vm_stk_current=stk_aux1;
			}
		}
		data->vm_stk_current++;\

		return true;

	lbl_exit_function:

		return false;
	}

	bool vm_call_metamethod_operation_pre_post(
		VirtualMachine 				*		_vm
		,Function 			*		_script_function
		,Instruction 				*		_instruction
		,StackElement 				*		_stk_result_op1
		,Metamethod 			_metamethod
	){

		Object 				*	so_aux=NULL;
		StackElement				*	stk_result_op1=_stk_result_op1;
		MetamethodMembers 			*	ptr_metamethod_members_aux=NULL;
		Symbol 						*	symbol_metamethod_pre=NULL;
		MemberProperty				*	member_property=NULL;
		StackElement					stk_aux1;
		StackElementMemberProperty	*	stk_mp_aux=NULL;
		Instruction					*	instruction=_instruction;

		ZS_LOAD_PROPERTIES(_metamethod);\

		switch(_metamethod){
		case METAMETHOD_PRE_INC:
			symbol_metamethod_pre=ptr_metamethod_members_aux->preinc;
			break;
		case METAMETHOD_PRE_DEC:
			symbol_metamethod_pre=ptr_metamethod_members_aux->predec;
			break;
		case METAMETHOD_POST_INC:
			symbol_metamethod_pre=ptr_metamethod_members_aux->postinc;
			break;
		case METAMETHOD_POST_DEC:
			symbol_metamethod_pre=ptr_metamethod_members_aux->postdec;
			break;
		default:
			break;
		}

		if(symbol_metamethod_pre == NULL){
			ZS_METAMETHOD_OPERATION_NOT_FOUND(_metamethod);
			goto lbl_exit_function;
		}

		/* call pre operation metamethod */\
		if(vm_inner_call(
				_vm
				,_script_function
				,instruction
				,so_aux
				,(Function *)symbol_metamethod_pre->ref_ptr
				,0
				,true
		)==false){\
			goto lbl_exit_function;\
		}\

		return true;

lbl_exit_function:

		return false;
	}
}
