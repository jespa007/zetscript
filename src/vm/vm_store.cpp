/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


	bool vm_store(
			VirtualMachine 	*	_vm
			,ScriptFunction *	_script_function
			,Instruction 	*	_instruction
	){
		VirtualMachineData 			*		data=(VirtualMachineData *)_vm->data;
		StackElement 				*		stk_dst=NULL,
									*		stk_src=NULL;
		zs_vector<StackElement *> 	*		store_lst_setter_functions=NULL;
		int 								n_element_left_to_store=0;
		StackElement    			*		stk_load_multi_var_src=NULL;
		ContainerSlot				*		dst_container_slot=NULL;
		ScriptObject 				*		so_aux=NULL;
		StackElementMemberProperty 		*		stk_mp_aux=NULL;
		ScriptFunction 				*		ptr_function_found=NULL;
		uint16_t 							stk_src_properties=0;
		Instruction					*		instruction=_instruction;
		bool								unref_old_stk_script_object=true;


		 // n elements left
		n_element_left_to_store=(char)_instruction->value_op1;

		// vm_current - n_element_left_to_store we have src values
		// do +1 is because it has to point to first the stack, due vm_stk_current points to new stk slot
		stk_load_multi_var_src=data->vm_stk_current-(n_element_left_to_store<<1); // it loads n_loads + n_vars to store 2xn_elements to store
		stk_dst=--data->vm_stk_current; // dst first read
		n_element_left_to_store--; // first read

	vm_store_next:
		store_lst_setter_functions=NULL;
		dst_container_slot=NULL;
		unref_old_stk_script_object=true;

		stk_src=stk_load_multi_var_src; // store ptr instruction2 op as src_var_value


		// check if by ref
		if(STK_IS_VAR_REF_SCRIPT_OBJECT(stk_dst)){
			stk_dst=(StackElement *)(STK_GET_STK_VAR_REF(stk_dst)->value);
		}

		//- check for packed dst_stk
		if(stk_dst->properties & STK_PROPERTY_PTR_STK) {
			stk_dst=(StackElement *)stk_dst->value; // value is expect to contents a stack variable
		}else if(stk_dst->properties & STK_PROPERTY_CONTAINER_SLOT){
			dst_container_slot=((ContainerSlot *)stk_dst->value);
			stk_dst=dst_container_slot->getPtrStackElement();
		 }else if((stk_dst->properties & STK_PROPERTY_ZS_CHAR_PTR)==0){
			ZS_VM_STOP_EXECUTE(
				"Expected l-value on assignment but it was type '%s'"
				,data->zs->stackElementToStringTypeOf(stk_dst).c_str()
			);
		}

		//-----------------------
		if(stk_dst->properties & STK_PROPERTY_READ_ONLY){
			ZS_VM_STOP_EXECUTEF("Assignment to constant variable");
		}

		// store through metamethod
		store_lst_setter_functions=NULL;

		if(STK_IS_CLASS_SCRIPT_OBJECT(stk_dst)){
			if((store_lst_setter_functions=((ClassScriptObject *)stk_dst->value)->getSetterList(METAMETHOD_BYTE_CODE_SET))!=NULL){

				if(store_lst_setter_functions->size() == 0){
					store_lst_setter_functions=NULL;
				}
			}
		}else if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){
			stk_mp_aux=(StackElementMemberProperty *)stk_dst->value;\
			if(stk_mp_aux->member_property!=NULL && stk_mp_aux->member_property->metamethod_members.setters.size() > 0){\
				store_lst_setter_functions=&stk_mp_aux->member_property->metamethod_members.setters;\
			}else{ // setter not allowed because it has no setter
				ZS_VM_STOP_EXECUTE("'%s::%s' not implements operator '=' (aka '_set')"
					,stk_mp_aux->member_property->script_type->getTypeName()
					,stk_mp_aux->member_property->property_name.c_str()
				);
			}
		}

		if(store_lst_setter_functions!=NULL){

			StackElement *stk_vm_start=data->vm_stk_current;\
			StackElement *stk_arg=stk_vm_start+1; //start from stk_src
			const char *__STR_SETTER_METAMETHOD__=metamethod_byte_code_to_symbol_str(METAMETHOD_BYTE_CODE_SET);\
			const char *__STR_AKA_SETTER_METAMETHOD__=metamethod_byte_code_to_operator_str(METAMETHOD_BYTE_CODE_SET);\
			*stk_arg=*stk_src;\
			if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){
				stk_mp_aux=(StackElementMemberProperty *)(stk_dst->value);
				so_aux=stk_mp_aux->so_object;\
			}else{
				so_aux=(ClassScriptObject *)stk_dst->value;
			}
			ptr_function_found=(ScriptFunction *)(((Symbol *)(((StackElement *)(store_lst_setter_functions->items[0]))->value))->ref_ptr);\
			if(so_aux->isNativeObject()){ // because object is native, we can have more than one _setter
				if(stk_mp_aux==NULL){
					strcpy(data->vm_str_metamethod_aux,"_set");
				}else{
					ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_BYTE_CODE(
						data->vm_str_metamethod_aux
						,METAMETHOD_BYTE_CODE_SET
						,stk_mp_aux->member_property->property_name.c_str()
					); // symbol to find"
				}

				if((ptr_function_found=vm_find_native_function( \
						_vm \
						,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
						,_script_function\
						,instruction\
						,false\
						,data->vm_str_metamethod_aux
						,stk_arg \
						,1))==NULL){ \
					if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){ \
						ZS_VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
								,so_aux->getScriptType()->str_script_type.c_str()\
								,stk_mp_aux->member_property->property_name.c_str()\
								,__STR_SETTER_METAMETHOD__\
						);\
					}else{\
						ZS_VM_STOP_EXECUTE("Type '%s' does not implement '%s' metamethod" \
								,so_aux->getScriptType()->str_script_type.c_str() \
								,__STR_SETTER_METAMETHOD__\
						);\
					}\
				}\
			}else if(store_lst_setter_functions->size()>1){ // it has overrided metamethods
				Symbol * symbol_setter = so_aux->getScriptType()->getSymbol(__STR_SETTER_METAMETHOD__); \
				if(symbol_setter == NULL){\
					ZS_VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"\
							,__STR_SETTER_METAMETHOD__\
							,__STR_AKA_SETTER_METAMETHOD__\
					);\
				}\
				if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
					ZS_VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function",__STR_SETTER_METAMETHOD__,__STR_AKA_SETTER_METAMETHOD__);\
				}\
				ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
			}\
			if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){\
				vm_execute_native_function(\
						_vm\
						,_script_function\
						,instruction\
						,so_aux\
						,ptr_function_found\
						,stk_arg\
						,1\
				);\
			}else{\

				// if argument is a script object should share, because on returning
				// it does pop and it could deallocate twice
				if(stk_arg->properties & STK_PROPERTY_SCRIPT_OBJECT){
					vm_share_script_object(_vm,(ScriptObject *)stk_arg->value);
				}

				vm_execute_script_function(\
					_vm\
					,so_aux\
					,ptr_function_found\
					,stk_arg\
				);\
			}\
			data->vm_stk_current=stk_vm_start;

		}else{ // store through script assignment

			if((stk_src->properties & STK_PROPERTY_PTR_STK)!=0) {
				stk_src=(StackElement *)stk_src->value; // value is expect to contents a stack variable
			}

			StackElement old_stk_dst = *stk_dst; // save dst_var to check after assignment...
			stk_src_properties=stk_src->properties;

			// init stk_dst

			if(stk_src_properties == STK_PROPERTY_UNDEFINED){
				stk_dst->value=0;
				stk_dst->properties=STK_PROPERTY_UNDEFINED;
			}else if(stk_src_properties == STK_PROPERTY_NULL){
				stk_dst->value=0;
				stk_dst->properties=STK_PROPERTY_NULL;
			}else if(stk_src_properties & STK_PROPERTY_ZS_INT){
				if((stk_dst->properties & STK_PROPERTY_ZS_CHAR_PTR)==0){
					stk_dst->value=stk_src->value;
					stk_dst->properties=STK_PROPERTY_ZS_INT;
				}else{
					*((zs_char *)stk_dst->value)=stk_src->value;
				}
			}else if(stk_src_properties & STK_PROPERTY_ZS_FLOAT){
				stk_dst->properties=STK_PROPERTY_ZS_FLOAT;
				stk_dst->value=0; // reset value
				ZS_WRITE_INTPTR_FLOAT(&stk_dst->value,ZS_READ_INTPTR_FLOAT(stk_src->value));
			}else if(stk_src_properties & STK_PROPERTY_BOOL){
				stk_dst->properties=STK_PROPERTY_BOOL;
				stk_dst->value=stk_src->value;
			}else if(stk_src_properties  &  (STK_PROPERTY_FUNCTION | STK_PROPERTY_TYPE | STK_PROPERTY_MEMBER_FUNCTION) ){
				*stk_dst=*stk_src;
			}else if(
				STK_IS_STRING_SCRIPT_OBJECT(stk_src)
							||
				(stk_src_properties & (STK_PROPERTY_ZS_CHAR_PTR))

			){
				StringScriptObject *str_object=NULL;

				if(STK_IS_STRING_SCRIPT_OBJECT(stk_dst)){ // dst is string reload
					str_object=(StringScriptObject *)stk_dst->value;
					unref_old_stk_script_object=false;
				}else{ // Generates a zs_string var
					stk_dst->value=(zs_int)(str_object= ZS_NEW_STRING_OBJECT(data->zs));
					stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;
					// create shared ptr
					vm_create_shared_script_object(_vm,str_object);
					// share ptr
					vm_share_script_object(_vm,str_object);
					//-------------------------------------
				}

				if(stk_src_properties & (STK_PROPERTY_ZS_CHAR_PTR)){
					str_object->set((const char *)stk_src->value);
				}else{
					str_object->set(((StringScriptObject *)(stk_src->value))->get());
				}


			}else if(stk_src_properties & (STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_CONTAINER_SLOT)){// object we pass its reference

				if(stk_src_properties & STK_PROPERTY_CONTAINER_SLOT){
					so_aux=((ContainerSlot *)stk_src->value)->getSrcContainerRef();
				}
				else{
					so_aux=(ScriptObject *)stk_src->value;
				}

				// if dst and src don't references same object
				if(
						(
								dst_container_slot!=NULL
									?
								dst_container_slot->getSrcContainerRef()==so_aux
									:
								old_stk_dst.value==(zs_int)so_aux
						)==false)
				{

					// src is type container and dst is slot:
					if( ZS_VM_CHECK_CONTAINER_FOR_SLOT(so_aux)
						   	   	   	   &&
						   (dst_container_slot!=NULL)
					){

						vm_assign_container_slot(_vm,dst_container_slot, (ContainerScriptObject *)so_aux);

					}else{ // assign object

						// unref current
						stk_dst->value=(intptr_t)so_aux;
						stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;
					}

					vm_share_script_object(_vm,so_aux);
				}else{

					// because is same references, it has to be not dereferenced
					unref_old_stk_script_object=false;
#ifdef __ZS_LOG_CONTAINER_SLOT__
					printf("[%s:%i] Warning assigning same object %p type %s\n"
							,SFI_GET_FILE(_script_function,instruction)\
							,SFI_GET_LINE(_script_function,instruction)\
							,(void *)so_aux
							,so_aux->getScriptType()->str_script_type.c_str()
					);
#endif
				}

			}else{
				ZS_VM_STOP_EXECUTE("BYTE_CODE_STORE: (internal) cannot determine var type %s"
					,data->zs->stackElementToStringTypeOf(stk_src).c_str()
				);
			}

			// if old dst value was a script object, it has to be dereferenced because it was written by other type ...
			if(
				(old_stk_dst.properties & (STK_PROPERTY_SCRIPT_OBJECT))
								&&
					unref_old_stk_script_object==true
			){
				ScriptObject  *old_so=(ScriptObject  *)old_stk_dst.value;

				vm_unref_shared_script_object(_vm,old_so,ZS_VM_CURRENT_SCOPE_BLOCK);
			}else if(
					(old_stk_dst.properties & STK_PROPERTY_CONTAINER_SLOT)!=0
										&&
					(stk_dst->properties & STK_PROPERTY_CONTAINER_SLOT)==0
				){

				ContainerSlot::deleteContainerSlot((ContainerSlot *)old_stk_dst.value);
#ifdef __ZS_LOG_CONTAINER_SLOT__
				printf("Unassigns container slot by '%s'\n",stk_to_typeof_str(data->zs,stk_src).c_str());
#endif
			}
		}

		// check whether dst_container_slot is not referenced
		if(
				(dst_container_slot != NULL && (dst_container_slot->isReferenced()==false)) //(((so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS) && so_aux->isNativeObject()) == false )
		){
			ContainerSlot::deleteContainerSlot(dst_container_slot);
			//dst_container_slot=NULL;
		}

		if(_instruction->byte_code ==BYTE_CODE_STORE_CONST){
			stk_dst->properties |= STK_PROPERTY_READ_ONLY;
		}

		if(n_element_left_to_store-- > 0){
			stk_dst=--data->vm_stk_current;//stk_multi_var_dest++; // left assigment
			stk_src=++stk_load_multi_var_src; // result on the right
			goto vm_store_next;
		}

		return true;

	lbl_exit_function:

		return false;
	}


}
