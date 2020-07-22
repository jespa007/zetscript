
#define METAMETHOD_2_ARGS 2
#define METAMETHOD_1_ARGS 1

// general
#define PRINT_DUAL_ERROR_OP(c)\
std::string var_type1=stk_result_op1->toString(),\
	   var_type2=stk_result_op2->toString();\
\
writeError(SFI_GET_FILE_LINE(calling_function,instruction),"cannot perform operator \"%s\" %s  \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
		var_type1.c_str(),\
		c,\
		var_type2.c_str());

#define PRINT_ERROR_OP(c)\
	std::string var_type1=stk_result_op1->toString();\
\
writeError(SFI_GET_FILE_LINE(calling_function,instruction),"cannot perform preoperator %s\"%s\". Check whether op1 implements the metamethod",\
	c,\
	var_type1.c_str());

#define COPY_FLOAT(d,s)  memcpy((d),(s),sizeof(float))

namespace zetscript{

	void  writeError(const char *filename, int line, const  char  *string_text, ...);

	inline void VirtualMachine::removeEmptySharedPointers(int idx_stack,void *ptr_callc_result){
		InfoSharedList *list = &zero_shares[idx_stack];
		PInfoSharedPointerNode first_node,current;
		first_node=current=list->first;
		if(current != NULL){
			bool finish=false;
			do{
				PInfoSharedPointerNode next_node=current->next;
				finish=next_node ==first_node;
				bool delete_node=true;
				if(ptr_callc_result!=NULL){
					if(ptr_callc_result==current->data.shared_ptr){
						delete_node=false;
					}
				}
				if(delete_node){
					delete current->data.shared_ptr;
					current->data.shared_ptr=NULL;
				}
				free(current);
				current=next_node;
			}while(!finish);
		}
		list->first=list->last=NULL;
	}

	inline bool VirtualMachine::popVmScope(int idx_stack,void * ptr_callc_result, unsigned char properties) {
		bool search=true;

		while((vm_scope<vm_scope_current) && search)
		{
			ScriptFunction *scope_info_function=(vm_scope_current-1)->script_function;
			int index         = (vm_scope_current-1)->index;
			StackElement         *stk_local_vars=(vm_scope_current-1)->stk_local_vars;
			ScopeBlockVars   *scope_block_vars=&scope_info_function->scope_block_vars[index];

			if(scope_block_vars!=NULL){

				for(uint8_t i = 0; i < scope_block_vars->n_local_vars; i++){
					int idx_local_var = scope_block_vars->idx_local_var[i];
					StackElement *stk_local_var =&stk_local_vars[idx_local_var];
					ScriptVar *var = NULL;
					switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_local_var->properties)){
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR:
						var =((ScriptVar *)(stk_local_var->var_ref));
						if(var){
							if(var->ptr_shared_pointer_node != NULL){
								var->unrefSharedPtr();
							}
						}
					}
					*stk_local_var={
						0,
						0,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED
					};
				}
			}
			removeEmptySharedPointers(idx_stack,ptr_callc_result);

			search=false;
			if((properties & ScopeProperty::SCOPE_PROPERTY_BREAK)!=0){
				search=((vm_scope_current-1)->properties & ScopeProperty::SCOPE_PROPERTY_BREAK) != ScopeProperty::SCOPE_PROPERTY_BREAK;
			}

			if((properties & ScopeProperty::SCOPE_PROPERTY_CONTINUE)!=0){
				search=((vm_scope_current-1)->properties & ScopeProperty::SCOPE_PROPERTY_CONTINUE) != ScopeProperty::SCOPE_PROPERTY_CONTINUE;
			}
			// pop current var
			--vm_scope_current;
		}
		return true;
	}

	inline bool VirtualMachine::applyMetamethod(
										ScriptVar *calling_object
										,ScriptFunction *calling_function
										,Instruction *instruction
										,const char *__OVERR_OP__
										,ByteCodeMetamethod __METAMETHOD__
										,StackElement *stk_result_op1
										,StackElement *stk_result_op2

										) {

		int idxOffsetFunctionMemberStart=0;
		ScriptVar *script_var_object = NULL;
		ScriptFunction * ptr_function_found=NULL;

		ScriptVar *one_param = NULL;
		int n_metam_args=((__METAMETHOD__ == BYTE_CODE_METAMETHOD_NOT\
						|| __METAMETHOD__ == BYTE_CODE_METAMETHOD_NEG\
						|| __METAMETHOD__ == BYTE_CODE_METAMETHOD_SET\
						   )? METAMETHOD_1_ARGS:METAMETHOD_2_ARGS);
		StackElement *mm_test_startArg = stk_current+n_metam_args;
		if(instruction->value_op2 ==0){ /* search for first time , else the function is stored in value_op2 */
			ScriptClass *script_class_aux=NULL;

			const char * symbol_to_find;
			ptr_function_found=NULL;


			if(((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR) == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR))){
				script_var_object = (ScriptVar *)(stk_result_op1->var_ref);
				if(((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) == (MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT))){
					script_var_object = (ScriptVar *)(((StackElement *)script_var_object))->var_ref;
				}
				if(__METAMETHOD__ == BYTE_CODE_METAMETHOD_SET){
					idxOffsetFunctionMemberStart=1;
					one_param = (ScriptVar *)(stk_result_op2->var_ref);
					if(((stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) == (MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT))){
						one_param = (ScriptVar *)(((StackElement *)one_param))->var_ref;
					}
				}
			}else if(((stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR) == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)) && (n_metam_args==METAMETHOD_2_ARGS)){\
				script_var_object = (ScriptVar *)(stk_result_op2->var_ref);
				if(((stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) == (MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT))){
					script_var_object = (ScriptVar *)(((StackElement *)script_var_object))->var_ref;
				}
			}else{

				std::string var_type1=stk_result_op1->toString(),
						var_type2="";

				if(n_metam_args==METAMETHOD_1_ARGS){ /* 1 arg*/
					writeError(SFI_GET_FILE_LINE(calling_function,instruction),"cannot perform operator %s\"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
							STR(__OVERR_OP),
							var_type1.c_str()
							);\
				}else{ /* 2 args*/
					var_type2=stk_result_op2->toString();
					writeError(SFI_GET_FILE_LINE(calling_function,instruction),"cannot perform operator \"%s\" %s  \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
							var_type1.c_str(),
							__OVERR_OP__,
							var_type2.c_str());
				}

				return false;
			}

			script_class_aux=GET_SCRIPT_CLASS(this,script_var_object->idx_class);
			std::vector<ScriptFunction *> *global_functions=&script_class_aux->metamethod_operator[__METAMETHOD__];
			StackElement *startArgs=(mm_test_startArg-n_metam_args+idxOffsetFunctionMemberStart);
			unsigned n_args=n_metam_args;

			symbol_to_find=ByteCodeMetamethodToStr(__METAMETHOD__);

			if((ptr_function_found = findFunction(
					 calling_object
					,calling_function
					,instruction
					,instruction
					,NULL
					,global_functions

					,false
					,symbol_to_find

					,stk_result_op1
					,stk_result_op2
					,startArgs
					,n_args\
					,__OVERR_OP__))==NULL)
			{
					return false;
			}
		}else{
			ptr_function_found = (ScriptFunction *)instruction->value_op2;
		}
		/* by default virtual machine gets main object class in order to run functions ... */
		bool error = false;
		StackElement ret_obj=callFunctionInternal(ptr_function_found,script_var_object,error,mm_test_startArg+idxOffsetFunctionMemberStart,vm_str_current,n_metam_args);
		if(error){
			return false;
		}

		/* restore ptrCurretOp... */
		stk_current=mm_test_startArg-n_metam_args;
		/* if function is C must register pointer ! */

		if(ret_obj.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){

			if(!((ScriptVar *)(ret_obj.var_ref))->initSharedPtr()){
				return false;
			}
			if(__METAMETHOD__ != BYTE_CODE_METAMETHOD_SET){ /* Auto destroy C when ref == 0 */
				((ScriptVar *)(ret_obj.var_ref))->setDelete_C_ObjectOnDestroy(true);
			}
		}
		if(__METAMETHOD__ != BYTE_CODE_METAMETHOD_SET){ /* Auto destroy C when ref == 0 */
			*stk_current++ = ret_obj;
		}

		return true;
	}

	inline ScriptFunction * VirtualMachine::findFunction(
			ScriptVar *calling_object
			,ScriptFunction *calling_function
			,Instruction *instruction
			,Instruction * call_ale_instruction

			,std::vector<FunctionSymbol> *function_symbol
			,std::vector<ScriptFunction *> *global_functions

			,bool is_constructor
			,const std::string & symbol_to_find

			,StackElement *stk_result_op1
			,StackElement *stk_result_op2
			,StackElement *start_arg
			,unsigned char n_args
			,const char *metamethod_str
			) {

		// by default search over global functions...
		int size_fun_vec=global_functions->size()-1;
		ScriptVar *var_object = NULL;
		ScriptFunction * ptr_function_found=NULL;
		std::string aux_string;

		// if function_symbol not NULL search over m_functionSymbols....
		if(function_symbol != NULL){
			size_fun_vec=function_symbol->size()-1;
		}

		for(int i = size_fun_vec; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
			ScriptFunction *irfs = NULL;

			if(function_symbol != NULL){
				irfs = (ScriptFunction *)function_symbol->at(i).object.stk_value;
				aux_string=function_symbol->at(i).key_value;

			}else{
				irfs=global_functions->at(i);
				aux_string=irfs->symbol_info.symbol->name;//CEval::getSymbolNameFromSymbolRef(irfs->symbol_info.symbol_ref);
			}

			bool match_signature = metamethod_str != NULL;
			if(!match_signature){
				match_signature = (aux_string == symbol_to_find && irfs->arg_info.size() == n_args);
			}

			if(match_signature){
				if((irfs->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF)){ /* C! Must match args...*/
					bool all_check=true; /*  check arguments types ... */
					int idx_type=-1;
					int arg_idx_type=-1;
					for( unsigned k = 0; k < n_args && all_check;k++){
						StackElement *current_arg=&start_arg[k];
						arg_idx_type=irfs->arg_info[k].idx_type;

						if(arg_idx_type==IDX_BUILTIN_TYPE_STACK_ELEMENT){

							if(current_arg->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT){
								current_arg = (StackElement *)current_arg->var_ref;
							}

							unsigned short var_type = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(current_arg->properties);

							switch(var_type){
								default:
									aux_string="unknow";
									all_check=false;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
									idx_type=IDX_BUILTIN_TYPE_INT_PTR_C;
									all_check=
											arg_idx_type==IDX_BUILTIN_TYPE_INT_PTR_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_INT_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_UNSIGNED_INT_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_INTPTR_T_C;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
									idx_type=IDX_BUILTIN_TYPE_FLOAT_PTR_C;
									all_check=arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_PTR_C
											||arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_C;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
									idx_type=IDX_BUILTIN_TYPE_BOOL_PTR_C;
									all_check=
											arg_idx_type==IDX_BUILTIN_TYPE_BOOL_PTR_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_BOOL_C;

									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
									idx_type=IDX_BUILTIN_TYPE_STRING_PTR_C;

									all_check =
											(	arg_idx_type==IDX_BUILTIN_TYPE_STRING_PTR_C && current_arg->var_ref!=0)
										  ||	arg_idx_type==IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR:
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
									var_object=((ScriptVar *)current_arg->var_ref);
									aux_string=var_object->getPointer_C_ClassName();

									if(arg_idx_type==idx_type){
										all_check=true;
									}
									else{
										ScriptClass *c_class=NULL;
										if(var_type & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
											all_check =
													(	arg_idx_type==IDX_BUILTIN_TYPE_STRING_PTR_C && current_arg->var_ref!=0)
												  ||	arg_idx_type==IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C;
										}else if((c_class=var_object->Get_C_Class())!=NULL){ /* check whether the base is ok... */
											all_check=IS_IDX_BUILTIN_TYPE_CLASS_INSTANCEOF(zs,c_class->idx_class,arg_idx_type);
										}else{ /* fail ... */
											all_check=false;
										}
									}

									break;
							}
						}
					}

					if(all_check){ /* we found the right function (set it up!) ... */
						call_ale_instruction->value_op2 = (intptr_t)irfs;
						ptr_function_found = irfs;
					}
				}else{ /* type script function  ... */
					call_ale_instruction->value_op2=(intptr_t)irfs;
					ptr_function_found = irfs;
				}
			}
		}

		if(ptr_function_found == NULL){
			if(is_constructor && n_args == 0){ /* default constructor not found --> set as not found... */
				call_ale_instruction->properties = MSK_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL;//value_op2 = ZS_FUNCTION_NOT_FOUND_IDX;
			}
			else{ // return error elaborate a error message...
				int n_candidates=0;
				std::string str_candidates="";
				std::string args_str = "";
				/* get arguments... */

				for( unsigned k = 0; k < n_args;k++){
					StackElement *current_arg=&start_arg[k];
					if(current_arg->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT){
						current_arg = (StackElement *)current_arg->var_ref;
					}

					if(k>0){
						args_str+=",";
					}
					unsigned short var_type = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(current_arg->properties);

					switch(var_type){

					default:
						aux_string="unknow";
						break;
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
						aux_string=k_str_int_type;
						break;
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
						aux_string=k_str_float_type;
						break;
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
						aux_string=k_str_bool_type;
						break;
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
						aux_string=k_str_string_type_ptr;
						if(current_arg->var_ref==NULL){ /* is constant char */
							aux_string=	k_str_const_char_type_ptr;
						}
						break;
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR:
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
						aux_string = ((ScriptVar *)current_arg->var_ref)->getPointer_C_ClassName();
						break;
					}
					args_str+=zs_rtti::demangle(aux_string);

					if(var_type == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER
					||var_type == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT
					||var_type == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN
					){
						args_str+=" [*] ";
					}
				}

				for(int i = size_fun_vec; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
					ScriptFunction *irfs = NULL;
					if(function_symbol!=NULL){
						irfs = (ScriptFunction *)function_symbol->at(i).object.stk_value;
					}else{
						irfs=global_functions->at(i);
					}

					bool match_signature = metamethod_str != NULL;

					if(!match_signature){
						match_signature = irfs->symbol_info.symbol->name == irfs->getInstructionSymbolName(call_ale_instruction);
					}

					if(match_signature){

						if(n_candidates == 0){
							str_candidates+="ttPossible candidates are:nn";
						}
						str_candidates+="\t\t-"+(calling_object==NULL?"":calling_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(calling_object->getClassName()+"::"):"")+irfs->symbol_info.symbol->name+"(";

						for(unsigned a = 0; a < irfs->arg_info.size(); a++){
							if(a>0){
								str_candidates+=",";
							}

							if(irfs->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF){
								str_candidates+=zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,irfs->arg_info[a].idx_type));
							}else{ /* typic var ... */
								str_candidates+="arg"+zs_strutils::intToString(a+1);
							}
						}
						str_candidates+=");\n";
						n_candidates++;
					}
				}

				if(n_candidates == 0){
					if(metamethod_str != NULL){
						if(n_args==2){
							PRINT_DUAL_ERROR_OP(metamethod_str);

						}else{
							PRINT_ERROR_OP(metamethod_str);

						}
					}else{
						writeError(SFI_GET_FILE_LINE(calling_function,call_ale_instruction),"Cannot find %s \"%s%s(%s)\".\n\n",
										is_constructor ? "constructor":"function",
										calling_object==NULL?"":calling_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
												calling_function->getInstructionSymbolName(call_ale_instruction),
										args_str.c_str()
						);
					}
				}
				else{
					if(metamethod_str!=NULL){
						writeError(SFI_GET_FILE_LINE(calling_function,call_ale_instruction),"Cannot find metamethod \"%s\" for \"%s%s(%s)\".\n\n%s",
													metamethod_str,
													calling_object==NULL?"":calling_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
													global_functions->at(0)->symbol_info.symbol->name.c_str(),
													args_str.c_str(),
													str_candidates.c_str());
					}else{
						writeError(SFI_GET_FILE_LINE(calling_function,call_ale_instruction),"Cannot match %s \"%s%s(%s)\" .\n\n%s",
							is_constructor ? "constructor":"function",
							calling_object==NULL?"":calling_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
									calling_function->getInstructionSymbolName(call_ale_instruction),
							args_str.c_str(),
							str_candidates.c_str());
					}
				}
				return NULL;
			}
		}

		return ptr_function_found;
	}
}
