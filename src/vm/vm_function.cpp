/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{
	ScriptFunction * vm_find_native_function(
			VirtualMachine 		*	_vm
			,ScriptType 		*	_class_obj // if NULL is MainClass
			,ScriptFunction 	*	_calling_function
			,Instruction 		* 	_instruction // call instruction
			,bool 					_is_constructor
			,const std::string & 	_symbol_to_find
			,StackElement 		*	_stk_arg
			,unsigned char 			_n_args
	){

		// by default search over global functions...
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		Instruction	*instruction=_instruction;
		ScriptFunction * ptr_function_found=NULL;
		std::string aux_string;
		int start_param=0;

		Symbol ** stk_elements_builtin_ptr= data->main_function_object->scope_script_function->symbol_functions->data();// vector of symbols
		size_t stk_elements_builtin_len=  data->main_function_object->scope_script_function->symbol_functions->size();// vector of symbols

		if(_class_obj != NULL){
			stk_elements_builtin_ptr=_class_obj->scope_script_type->symbol_functions->data();
			stk_elements_builtin_len=_class_obj->scope_script_type->symbol_functions->size();
		}

		for(int i = (int)(stk_elements_builtin_len-1); i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
			ScriptFunction *irfs = NULL;
			start_param=1;

			VM_EXTRACT_FUNCTION_INFO

			aux_string=irfs->name_script_function;

			bool symbol_equals=aux_string == _symbol_to_find;

			if((symbol_equals && ((int)irfs->params_len == (_n_args+start_param)))){
				// Only check native functions
				if((irfs->properties & FUNCTION_PROPERTY_C_OBJECT_REF)){ /* C! Must match all args...*/
					bool all_check=true; /*  check arguments types ... */
					int arg_idx_script_type=-1;

					for( unsigned k = 0; k < _n_args && all_check;k++){ // ignore first parameter due expects zetscript
						StackElement *current_arg=&_stk_arg[k];
						arg_idx_script_type=irfs->params[k+start_param].idx_script_type;

						if(arg_idx_script_type!=IDX_TYPE_STACK_ELEMENT){
							//unsigned short var_type = GET_STK_PROPERTY_TYPES(current_arg->properties);
							if(current_arg->properties & STK_PROPERTY_PTR_STK){
								current_arg=(StackElement *)current_arg->value;
							}

							switch(current_arg->properties & STK_PROPERTY_TYPES){
								default:
									aux_string="unknow";
									all_check=false;
									break;
								case STK_PROPERTY_ZS_INT:
									all_check=
											arg_idx_script_type==IDX_TYPE_ZS_INT_PTR_C
										  ||arg_idx_script_type==IDX_TYPE_ZS_INT_C
										  ||arg_idx_script_type==IDX_TYPE_ZS_FLOAT_PTR_C;
									break;
								case STK_PROPERTY_ZS_FLOAT:
									all_check=arg_idx_script_type==IDX_TYPE_ZS_FLOAT_PTR_C
											||arg_idx_script_type==IDX_TYPE_ZS_FLOAT_C
											||arg_idx_script_type==IDX_TYPE_ZS_INT_PTR_C
											||arg_idx_script_type==IDX_TYPE_ZS_INT_C;
									break;
								case STK_PROPERTY_BOOL:
									all_check=arg_idx_script_type==IDX_TYPE_BOOL_PTR_C;
									break;
								case STK_PROPERTY_FUNCTION|STK_PROPERTY_MEMBER_FUNCTION:
									all_check=arg_idx_script_type==IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER;
									break;
								case STK_PROPERTY_FUNCTION:
									all_check=arg_idx_script_type==IDX_TYPE_FUNCTION;
									break;
								// decoment to not allow nulls
								/*case STK_PROPERTY_NULL:
									all_check=false;
									break;*/
								case STK_PROPERTY_SCRIPT_OBJECT:

									if(STK_IS_SCRIPT_OBJECT_STRING(current_arg)){
										all_check=arg_idx_script_type==IDX_TYPE_SCRIPT_OBJECT_STRING; // if string object --> direct

										if(all_check==false){ // try native conversions
											all_check =
												(	arg_idx_script_type==IDX_TYPE_STRING_PTR_C && current_arg->value!=0)
											  ||	arg_idx_script_type==IDX_TYPE_CONST_CHAR_PTR_C;
										}
									}else{
										ScriptObject *var_object = NULL;
										var_object=((ScriptObject *)current_arg->value);
										aux_string=var_object->getTypeName();
										all_check=var_object->getScriptType()->extendsFrom(arg_idx_script_type);
									}
									break;
							}
						}
					}

					if(all_check){ /* we found the right function (set it up!) ... */
						ptr_function_found = irfs;
					}
				}

			}
		}

		if(ptr_function_found == NULL){
			std::string class_str=_class_obj==NULL?"":_class_obj->idx_script_type!=IDX_TYPE_CLASS_MAIN?_class_obj->str_script_type:"";
			int n_candidates=0;
			std::string str_candidates="";
			std::string function_name_not_found=
				class_str==""
				?
				_symbol_to_find
				:
				zs_strutils::format("%s::%s",class_str.c_str(),_symbol_to_find.c_str());

			std::string args_str = "";
			/* get arguments... */
			for( unsigned k = 0; k < _n_args;k++){
				StackElement *current_arg=&_stk_arg[k];

				if(k>0){
					args_str.append(",");
				}

				//unsigned short var_type = GET_STK_PROPERTY_TYPES(current_arg->properties);
				if(current_arg->properties & STK_PROPERTY_PTR_STK){
					current_arg=(StackElement *)current_arg->value;
				}


				switch(current_arg->properties & STK_PROPERTY_TYPES){

				default:
					aux_string="unknow";
					break;
				case STK_PROPERTY_ZS_INT:
					aux_string=k_str_zs_int_type;
					break;
				case STK_PROPERTY_ZS_FLOAT:
					aux_string=k_str_zs_float_type;
					break;
				case STK_PROPERTY_BOOL:
					aux_string=k_str_bool_type;
					break;
				case STK_PROPERTY_UNDEFINED:
					aux_string="undefined";
					break;
				case STK_PROPERTY_NULL:
					aux_string="null";
					break;
				case STK_PROPERTY_SCRIPT_OBJECT:
					aux_string = ((ScriptObject *)current_arg->value)->getScriptType()->str_script_type_ptr;
					break;
				}

				args_str.append(zs_rtti::demangle(aux_string.c_str()));

				if(current_arg->properties == STK_PROPERTY_ZS_INT
				||current_arg->properties == STK_PROPERTY_ZS_FLOAT
				||current_arg->properties == STK_PROPERTY_BOOL
				){
					args_str.append(" [*] ");
				}
			}

			for(int i = (int)(stk_elements_builtin_len-1); i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
				start_param=1;
				ScriptFunction *irfs=NULL;

				VM_EXTRACT_FUNCTION_INFO

				if((irfs->name_script_function == _symbol_to_find) && (irfs->properties & FUNCTION_PROPERTY_C_OBJECT_REF)){

					if(n_candidates == 0){
						str_candidates.append("\tPossible candidates are:\n\n");
					}
					str_candidates.append("\t\t-");

					// type if not main
					if(_class_obj!=NULL && _class_obj->idx_script_type!=IDX_TYPE_CLASS_MAIN){
						str_candidates.append(_class_obj->str_script_type);
						str_candidates.append("::");
					}

					// writes script function interface
					// function name
					str_candidates.append(irfs->name_script_function);
					str_candidates.append("(");

					for(int a = 2; a < irfs->params_len; a++){
						if(a>2){
							str_candidates.append(",");
						}

						str_candidates+=zs_rtti::demangle(
							GET_IDX_2_CLASS_C_STR(data->script_type_factory,irfs->params[a].idx_script_type)
						);
					}

					// writes wrap

					str_candidates.append(")");

					str_candidates.append(" -- BIND FUNCTION AS --> ");

					str_candidates.append(zs_rtti::demangle(
							GET_IDX_2_CLASS_C_STR(data->script_type_factory,irfs->idx_script_type_return)
						)
					);

					str_candidates.append(" (*)");
					str_candidates.append("(");

					for(int a = 0; a < irfs->params_len; a++){
						if(a>0){
							str_candidates.append(",");
						}

						str_candidates.append(zs_rtti::demangle(
							GET_IDX_2_CLASS_C_STR(data->script_type_factory,irfs->params[a].idx_script_type)
							)
						);
					}
					str_candidates.append(")\n");

					n_candidates++;
				}
			}

			if(n_candidates == 0){
				VM_ERROR("Cannot call native %s '%s(%s)'. Function not registered\n\n"
					,_is_constructor ? "constructor":class_str==""?"function":"member function"
					,function_name_not_found.c_str()
					,args_str.c_str()
				);

				return NULL;
			}
			else{
				VM_ERROR("Cannot call native %s '%s(%s)'\n\n%s"
					,_is_constructor ? "constructor":class_str==""?"function":"member function"
					,function_name_not_found.c_str()
					,args_str.c_str()
					,str_candidates.c_str()
				);
				return NULL;
			}
		}

		return ptr_function_found;
	}
}
