/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


	bool vm_store(
			VirtualMachine 	*	_vm
			,ScriptFunction *	_calling_function
			,Instruction 	*	_instruction
	){
		VirtualMachineData 			*		data=(VirtualMachineData *)_vm->data;
		StackElement 				*		stk_dst=NULL,
									*		stk_src=NULL;
		zs_vector<StackElement *> *		store_lst_setter_functions=NULL;
		int 								n_element_left_to_store=0;
		StackElement    			*		stk_load_multi_var_src=NULL;
		ContainerSlotStore 			*		container_slot_store=NULL;
		ScriptObject 				*		container_slot_store_object=NULL;
		zs_int 								container_slot_store_id_slot=0;
		void 						*		stk_src_ref_value_copy_aux=NULL;
		StackElement 				*		stk_result_op2=NULL;
		zs_int 						*		stk_src_ref_value=NULL;
		zs_int 						*		stk_dst_ref_value=NULL;
		ScriptObject 				*		so_aux=NULL;
		StackMemberProperty 		*		stk_mp_aux=NULL;
		ScriptFunction 				*		ptr_function_found=NULL;
		uint16_t 							stk_src_properties=0;
		Instruction					*		instruction=_instruction;


		 // n elements left
		n_element_left_to_store=(char)_instruction->value_op1;

		// vm_current - n_element_left_to_store we have src values
		// do +1 is because it has to point to first the stack, due vm_stk_current points to new stk slot
		stk_load_multi_var_src=data->stk_vm_current-(n_element_left_to_store<<1); // it loads n_loads + n_vars to store 2xn_elements to store
		stk_dst=--data->stk_vm_current; // dst first read
		n_element_left_to_store--; // first read

	vm_store_next:
		store_lst_setter_functions=NULL;
		container_slot_store=NULL;
		container_slot_store_object=NULL;
		container_slot_store_id_slot=0;
		stk_src=stk_load_multi_var_src; // store ptr instruction2 op as src_var_value
		//stk_dst=stk_result_op2;
		stk_result_op2=stk_dst;

		// check if by ref
		if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_dst)){
			stk_dst=(StackElement *)(STK_GET_STK_VAR_REF(stk_dst)->value);
		}

		//- check if ptr stk
		if((stk_dst->properties & STK_PROPERTY_PTR_STK)!=0) {
			stk_dst=(StackElement *)stk_dst->value; // value is expect to contents a stack variable
		}else if(stk_dst->properties & STK_PROPERTY_CONTAINER_SLOT_STORE){
			container_slot_store=((ContainerSlotStore *)stk_dst->value);
			container_slot_store_object=container_slot_store->object;
			container_slot_store_id_slot=container_slot_store->id_slot;
			stk_dst=container_slot_store->ptr_stk;
			delete container_slot_store;
			container_slot_store=NULL;
		}else{
			if((stk_dst->properties & STK_PROPERTY_IS_C_VAR_PTR)==0){
				VM_STOP_EXECUTE("Expected l-value on assignment but it was type '%s'"
					,stk_to_typeof_str(data->zs,stk_dst).c_str()
				);
			}
		}

		//-----------------------
		if(stk_dst->properties & STK_PROPERTY_READ_ONLY){
			VM_STOP_EXECUTEF("Assignment to constant variable");
		}

		// store through metamethod
		store_lst_setter_functions=NULL;

		if(STK_IS_SCRIPT_OBJECT_CLASS(stk_dst)){
			if((store_lst_setter_functions=((ScriptObjectClass *)stk_dst->value)->getSetterList(BYTE_CODE_METAMETHOD_SET))!=NULL){

				if(store_lst_setter_functions->size() == 0){
					store_lst_setter_functions=NULL;
				}
			}
		}else if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){
			stk_mp_aux=(StackMemberProperty *)stk_dst->value;\
			if(stk_mp_aux->member_property->metamethod_members.setters.size() > 0){\
				store_lst_setter_functions=&stk_mp_aux->member_property->metamethod_members.setters;\
			}else{ // setter not allowed because it has no setter
				VM_STOP_EXECUTE("'%s' not implements operator '=' (aka '_set')"
					,stk_mp_aux->member_property->property_name.c_str()
				);
			}
		}

		if(store_lst_setter_functions!=NULL){

			StackElement *stk_vm_start=data->stk_vm_current;\
			StackElement *stk_arg=stk_vm_start+1; //start from stk_src
			const char *__STR_SETTER_METAMETHOD__=byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_SET);\
			const char *__STR_AKA_SETTER_METAMETHOD__=byte_code_metamethod_to_operator_str(BYTE_CODE_METAMETHOD_SET);\
			*stk_arg=*stk_src;\
			if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){
				stk_mp_aux=(StackMemberProperty *)(stk_dst->value);
				so_aux=stk_mp_aux->so_object;\
			}else{
				so_aux=(ScriptObjectClass *)stk_dst->value;
			}
			ptr_function_found=(ScriptFunction *)(((Symbol *)(((StackElement *)(store_lst_setter_functions->items[0]))->value))->ref_ptr);\
			if(so_aux->isNativeObject()){ // because object is native, we can have more than one _setter
				if(stk_mp_aux==NULL){
					strcpy(data->vm_str_metamethod_aux,"_set");
				}else{
					ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD(
						data->vm_str_metamethod_aux
						,BYTE_CODE_METAMETHOD_SET
						,stk_mp_aux->member_property->property_name.c_str()
					); // symbol to find"
				}

				if((ptr_function_found=vm_find_native_function( \
						_vm \
						,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
						,_calling_function\
						,instruction\
						,false\
						,data->vm_str_metamethod_aux
						,stk_arg \
						,1))==NULL){ \
					if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){ \
						VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
								,so_aux->getScriptType()->str_script_type.c_str()\
								,stk_mp_aux->member_property->property_name.c_str()\
								,__STR_SETTER_METAMETHOD__\
						);\
					}else{\
						VM_STOP_EXECUTE("Type '%s' does not implement '%s' metamethod" \
								,so_aux->getScriptType()->str_script_type.c_str() \
								,__STR_SETTER_METAMETHOD__\
						);\
					}\
				}\
			}else if(store_lst_setter_functions->size()>1){ // it has overrided metamethods
				Symbol * symbol_setter = so_aux->getScriptType()->getSymbol(__STR_SETTER_METAMETHOD__); \
				if(symbol_setter == NULL){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"\
							,__STR_SETTER_METAMETHOD__\
							,__STR_AKA_SETTER_METAMETHOD__\
					);\
				}\
				if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function",__STR_SETTER_METAMETHOD__,__STR_AKA_SETTER_METAMETHOD__);\
				}\
				ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
			}\
			if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){\
				vm_execute_native_function(\
						_vm\
						,_calling_function\
						,instruction\
						,so_aux\
						,ptr_function_found\
						,stk_arg\
						,1\
				);\
			}else{\
				vm_execute_script_function(\
					_vm\
					,so_aux\
					,ptr_function_found\
					,stk_arg\
				);\
			}\
			data->stk_vm_current=stk_vm_start;

		}else{ // store through script assignment

			if((stk_src->properties & STK_PROPERTY_PTR_STK)!=0) {
				stk_src=(StackElement *)stk_src->value; // value is expect to contents a stack variable
			}


			// TODO: get stk_dst if STK_PROPERTY_SLOT
			StackElement old_stk_dst = *stk_dst; // save dst_var to check after assignment...

			stk_src_ref_value_copy_aux=NULL;//copy aux in case of the var is c and primitive (we have to update value on save)
			stk_src_ref_value=&stk_src->value;
			stk_dst_ref_value=&stk_dst->value;
			if(stk_src->properties & STK_PROPERTY_IS_C_VAR_PTR){ // src is C pointer
				stk_src_ref_value=(zs_int *)((stk_src)->value);
			}
			if(stk_dst->properties & STK_PROPERTY_IS_C_VAR_PTR){ // dst is a C pointer
				// particular case
				if(
						stk_dst->properties != stk_src->properties
					&& (((stk_dst->properties & STK_PROPERTY_ZS_CHAR) && (stk_src->properties & STK_PROPERTY_ZS_INT))==0)){

					 if(stk_dst->properties != stk_src->properties){
						if(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_dst->properties) != GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_src->properties)
						){
							// check particular case
							VM_STOP_EXECUTE(
								"Symbol '%s': different types! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals"
								,SFI_GET_SYMBOL_NAME(_calling_function,instruction)
							);
						}else{ // is object
							VM_STOP_EXECUTEF(
								"Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator (aka set metamethod) in order to perform assign operation"
							);
						}
					}
				}
				stk_dst_ref_value=(zs_int *)((stk_dst)->value);
				stk_src_ref_value_copy_aux=&((stk_dst)->value);
			}
			stk_src_properties=stk_src->properties;

			// init stk_dst
			STK_SET_UNDEFINED(stk_dst);
			if(stk_src_properties == STK_PROPERTY_UNDEFINED){
				stk_dst->properties=STK_PROPERTY_UNDEFINED;
			}else if(stk_src_properties == STK_PROPERTY_NULL){
				stk_dst->properties=STK_PROPERTY_NULL;
			}else if(stk_src_properties & STK_PROPERTY_ZS_INT){
				stk_dst->properties=STK_PROPERTY_ZS_INT;
				old_stk_dst.properties &  STK_PROPERTY_ZS_CHAR?
				*((zs_char *)stk_dst_ref_value)=*((zs_int *)stk_src_ref_value) & 0xff
				:*((zs_int *)stk_dst_ref_value)=*((zs_int *)stk_src_ref_value);
				if(stk_src_ref_value_copy_aux!=NULL)(*(zs_int *)stk_src_ref_value_copy_aux)=*((zs_int *)stk_src_ref_value);
			}else if(stk_src_properties & STK_PROPERTY_ZS_FLOAT){
				stk_dst->properties=STK_PROPERTY_ZS_FLOAT;
				*((zs_float *)stk_dst_ref_value)=*((zs_float *)stk_src_ref_value);
				if(stk_src_ref_value_copy_aux!=NULL)(*(zs_float *)stk_src_ref_value_copy_aux)=*((zs_float *)stk_src_ref_value);
			}else if(stk_src_properties & STK_PROPERTY_BOOL){
				stk_dst->properties=STK_PROPERTY_BOOL;
				*((bool *)stk_dst_ref_value)=*((bool *)stk_src_ref_value);
				if(stk_src_ref_value_copy_aux!=NULL)(*(bool *)stk_src_ref_value_copy_aux)=*((bool *)stk_src_ref_value);
			}else if(stk_src_properties  &  (STK_PROPERTY_FUNCTION | STK_PROPERTY_TYPE | STK_PROPERTY_MEMBER_FUNCTION) ){
				*stk_dst=*stk_src;
			}else if(
				STK_IS_SCRIPT_OBJECT_STRING(stk_src)
							||
				(stk_src_properties & (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR))

			){
				ScriptObjectString *str_object=NULL;

				if(STK_IS_SCRIPT_OBJECT_STRING(stk_dst)){ // dst is string reload
					str_object=(ScriptObjectString *)stk_dst->value;
				}else{ // Generates a zs_string var
					stk_dst->value=(zs_int)(str_object= ZS_NEW_OBJECT_STRING(data->zs));
					stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;
					// create shared ptr
					if(!vm_create_shared_script_object(_vm,str_object)){
						goto lbl_exit_function;
					}
					// share ptr
					if(!vm_share_script_object(_vm,str_object)){
						goto lbl_exit_function;
					}
					//-------------------------------------
				}

				if(stk_src_properties & (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR)){
					str_object->set((const char *)stk_src->value);
				}else{
					str_object->set(stk_to_str(data->zs, stk_src));
				}


			}else if(stk_src_properties & STK_PROPERTY_SCRIPT_OBJECT){// object we pass its reference

				so_aux=(ScriptObject *)stk_src->value;


				// already share and src is type container and dst is slot:
				// becomes a weak pointer to avoid possibly cyclic reference
				if((so_aux->shared_pointer->data.n_shares>0)
					&& (so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_VECTOR)
					&& (container_slot_store_object!=NULL)){

					StackElement *stk_obj=NULL;



					// More tests would be needed see issue #336
					if(container_slot_store_object->getScriptType()->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_VECTOR){
						printf("\nAssing object %p type '%s' to slot '%i'"
								,container_slot_store_object
								,container_slot_store_object->getScriptType()->str_script_type.c_str()
								,(int)container_slot_store_id_slot
						);
						stk_obj=container_slot_store_object->getBuiltinElementAt(container_slot_store_id_slot);
					}else{
						printf("\nAssing object %p type '%s' to slot '%s'"
								,container_slot_store_object
								,container_slot_store_object->getScriptType()->str_script_type.c_str()
								,(const char *)container_slot_store_id_slot
						);
						stk_obj=container_slot_store_object->getProperty((const char *)container_slot_store_id_slot);
					}

					// create weak pointer
					auto weak_pointer=new ScriptObjectWeakPointer(so_aux);


					stk_dst->value=(intptr_t)weak_pointer;
					stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;

					if(!vm_create_shared_script_object(_vm,weak_pointer)){
						goto lbl_exit_function;
					}

					if(!vm_share_script_object(_vm,weak_pointer)){
						goto lbl_exit_function;
					}

						//so_aux->refObject(NULL);//(ScriptObject **)&stk_obj->value);

				}else{ // share
					stk_dst->value=(intptr_t)so_aux;
					stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;

					if(!vm_share_script_object(_vm,so_aux)){
						goto lbl_exit_function;
					}

				}

			}else{
				VM_STOP_EXECUTE("BYTE_CODE_STORE: (internal) cannot determine var type %s"
					,stk_to_typeof_str(data->zs,stk_src).c_str()
				);
			}

			if(stk_src_ref_value_copy_aux!=NULL){
				stk_dst->properties|=STK_PROPERTY_IS_C_VAR_PTR;
			}

			// check old dst value to unref if it was an object ...
			if(
				(old_stk_dst.properties & (STK_PROPERTY_SCRIPT_OBJECT))
			){
				ScriptObject  *old_so=(ScriptObject  *)old_stk_dst.value;


				// unref pointer because new pointer has been attached...
				StackElement *chk_ref=(StackElement *)stk_result_op2->value;


				if(chk_ref->properties & STK_PROPERTY_PTR_STK){
					chk_ref=(StackElement *)chk_ref->value;
				}

				if(!vm_unref_shared_script_object(_vm,old_so,VM_CURRENT_SCOPE_BLOCK)){
					goto lbl_exit_function;
				}
			}
		}


		if(_instruction->byte_code ==BYTE_CODE_STORE_CONST){
			stk_dst->properties |= STK_PROPERTY_READ_ONLY;
		}

		if(n_element_left_to_store-- > 0){
			stk_dst=--data->stk_vm_current;//stk_multi_var_dest++; // left assigment
			stk_src=++stk_load_multi_var_src; // result on the right
			goto vm_store_next;
		}

		return true;

	lbl_exit_function:

		return false;
	}


}
