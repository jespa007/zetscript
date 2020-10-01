
#define METAMETHOD_2_ARGS 2
#define METAMETHOD_1_ARGS 1

// general
#define PRINT_DUAL_ERROR_OP(c)\
std::string var_type1=stk_result_op1->toString(),\
	   var_type2=stk_result_op2->toString();\
\
THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"cannot perform operator \"%s\" %s  \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
		var_type1.c_str(),\
		c,\
		var_type2.c_str());

#define PRINT_ERROR_OP(c)\
	std::string var_type1=stk_result_op1->toString();\
\
THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"cannot perform preoperator %s\"%s\". Check whether op1 implements the metamethod",\
	c,\
	var_type1.c_str());

#define COPY_FLOAT(d,s)  memcpy((d),(s),sizeof(float))

namespace zetscript{

	inline void VirtualMachine::insertShareNode(InfoSharedList * list, InfoSharedPointerNode *_node){
		if(_node->next != NULL || _node->previous!=NULL) {
			THROW_RUNTIME_ERROR(" Internal error expected node not in list");
		}

		if(list->first == NULL){ /*one  node: trivial ?*/
			_node->previous=_node->next= list->last = list->first =_node;
		}
		else{ /* >1 node add to the end */
			// attach last-previous
			_node->previous=list->last;
			list->last->next=_node;
			list->last=_node;

			// attach next
			_node->next=list->first;
			list->first->previous=_node;
		}
	}

	inline void VirtualMachine::deattachShareNode(InfoSharedList * list, InfoSharedPointerNode *_node){

		if(list == NULL) return;

		if(_node->next == NULL || _node->previous == NULL){
			THROW_RUNTIME_ERROR(" An already deattached node");
		}

		if((_node->previous == _node) && (_node->next == _node)){ // 1 single node...
			list->last=list->first=NULL;
		}
		else{ // dettach and attach next...
			// [1]<->[2]<-> ...[P]<->[C]<->[N]...[M-1]<->[M]
			if(_node == list->first){
				list->first=_node->next;
			}
			_node->previous->next=_node->next;
			_node->next->previous=_node->previous;

		}
		_node->previous = _node->next = NULL;
	}


	inline void VirtualMachine::removeEmptySharedPointers(){
		InfoSharedList *list=&zero_shares[this->idx_current_call];
		InfoSharedPointerNode *next_node=NULL,*current=list->first;


		if(current != NULL){
			bool finish=false;
			do{
				next_node=current->next;
				finish=next_node ==list->first;

				deattachShareNode(list,current);

				delete current->data.shared_ptr;
				current->data.shared_ptr=NULL;
				free(current);

				current=next_node;

			}while(!finish);
		}
	}

	// defer all local vars
	inline void VirtualMachine::popVmScope(bool check_empty_shared_pointers) {

		if(vm_scope<vm_current_scope) // pop 1 scope
		{
			ScriptFunction *scope_info_function=(vm_current_scope-1)->script_function;
			Scope *scope         = (vm_current_scope-1)->scope;
			StackElement         * stk_local_vars=(vm_current_scope-1)->stk_local_vars;
			zs_vector			 * scope_symbols=scope->registered_symbols;
			//ScopeBlockVars   *scope_block_vars=&scope_info_function->scope_block_vars[index];

			for(uint8_t i = 0; i < scope_symbols->count; i++){
				Symbol *scope_symbol=(Symbol *)scope_symbols->items[i];
				StackElement *stk_local_var =&stk_local_vars[scope_symbol->idx_position]; // position where symbol is located on stack

				if(stk_local_var->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
					stk_local_var=(StackElement *)stk_local_var->var_ref;
				}

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
			
			// remove deferred shared pointers except for return value...
			if(check_empty_shared_pointers){
				removeEmptySharedPointers();
			}

			// pop current var
			--vm_current_scope;
		}else{
			THROW_RUNTIME_ERROR("internal error: trying to pop at the bottom");
		}

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

		int idx_offset_function_member_start=0;
		ScriptVar *script_var_object = NULL;
		ScriptFunction * ptr_function_found=NULL;

		ScriptVar *one_param = NULL;
		int n_metam_args=((__METAMETHOD__ == BYTE_CODE_METAMETHOD_NOT\
						|| __METAMETHOD__ == BYTE_CODE_METAMETHOD_NEG\
						|| __METAMETHOD__ == BYTE_CODE_METAMETHOD_SET\
						   )? METAMETHOD_1_ARGS:METAMETHOD_2_ARGS);
		StackElement *mm_test_startArg = vm_stk_current+n_metam_args;
		if(instruction->value_op2 ==0){ /* search for first time , else the function is stored in value_op2 */
			ScriptClass *script_class_aux=NULL;

			const char * symbol_to_find;
			ptr_function_found=NULL;


			if(((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR) == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR))){
				script_var_object = (ScriptVar *)(stk_result_op1->var_ref);
				if(((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) == (MSK_STACK_ELEMENT_PROPERTY_PTR_STK))){
					script_var_object = (ScriptVar *)(((StackElement *)script_var_object))->var_ref;
				}
				if(__METAMETHOD__ == BYTE_CODE_METAMETHOD_SET){
					idx_offset_function_member_start=1;
					one_param = (ScriptVar *)(stk_result_op2->var_ref);
					if(((stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) == (MSK_STACK_ELEMENT_PROPERTY_PTR_STK))){
						one_param = (ScriptVar *)(((StackElement *)one_param))->var_ref;
					}
				}
			}else if(((stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR) == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)) && (n_metam_args==METAMETHOD_2_ARGS)){\
				script_var_object = (ScriptVar *)(stk_result_op2->var_ref);
				if(((stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) == (MSK_STACK_ELEMENT_PROPERTY_PTR_STK))){
					script_var_object = (ScriptVar *)(((StackElement *)script_var_object))->var_ref;
				}
			}else{

				std::string var_type1=stk_result_op1->toString(),
						var_type2="";

				if(n_metam_args==METAMETHOD_1_ARGS){ /* 1 arg*/
					THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"cannot perform operator %s\"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",
							STR(__OVERR_OP),
							var_type1.c_str()
							);
				}else{ /* 2 args*/
					var_type2=stk_result_op2->toString();
					THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"cannot perform operator \"%s\" %s  \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",
							var_type1.c_str(),
							__OVERR_OP__,
							var_type2.c_str());
				}

				return false;
			}

			script_class_aux=GET_SCRIPT_CLASS(this,script_var_object->idx_class);
			zs_vector *stk_elements=script_class_aux->metamethod_operator[__METAMETHOD__];
			StackElement *stk_args=(mm_test_startArg-n_metam_args+idx_offset_function_member_start);
			unsigned n_stk_args=n_metam_args;

			symbol_to_find=ByteCodeMetamethodToStr(__METAMETHOD__);

			if((ptr_function_found = findFunction(
					 calling_object
					,calling_function
					,instruction
					,false
					,(void *)stk_elements->items
					,stk_elements->count
					,symbol_to_find
					,stk_args
					,n_stk_args
					,stk_result_op1
					,stk_result_op2
					,__OVERR_OP__))==NULL)
			{
					return false;
			}
		}else{
			ptr_function_found = (ScriptFunction *)instruction->value_op2;
		}
		/* by default virtual machine gets main object class in order to run functions ... */
		bool error = false;
		StackElement ret_obj=callFunctionScript(
				ptr_function_found
				,script_var_object
				,mm_test_startArg+idx_offset_function_member_start
				//,vm_str_current
				,n_metam_args);

		/* restore ptrCurretOp... */
		vm_stk_current=mm_test_startArg-n_metam_args;
		/* if function is C must register pointer ! */

		if(ret_obj.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){

			((ScriptVar *)(ret_obj.var_ref))->initSharedPtr();

			if(__METAMETHOD__ != BYTE_CODE_METAMETHOD_SET){ /* Auto destroy C when ref == 0 */
				((ScriptVar *)(ret_obj.var_ref))->setDelete_C_ObjectOnDestroy(true);
			}
		}
		if(__METAMETHOD__ != BYTE_CODE_METAMETHOD_SET){ /* Auto destroy C when ref == 0 */
			*vm_stk_current++ = ret_obj;
		}

		return true;
	}

	inline ScriptFunction * VirtualMachine::findFunction(
			ScriptVar *calling_object
			,ScriptFunction *calling_function
			,Instruction * instruction // call instruction
			,bool is_constructor
			,void *stk_elements_ptr // vector of properties
			,int stk_elements_len // vector of properties
			,const std::string & symbol_to_find
			,StackElement *stk_arg
			,unsigned char n_args
			,StackElement *stk_result_op1
			,StackElement *stk_result_op2
			,const char *metamethod_str
			) {

		// by default search over global functions...
		ScriptVar *var_object = NULL;
		ScriptFunction * ptr_function_found=NULL;
		std::string aux_string;
		bool stk_element_are_ptr=stk_elements_ptr!=vm_stack;

		for(int i = stk_elements_len-1; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
			StackElement *stk_element=NULL;

			if(stk_element_are_ptr){
				stk_element=((StackElement **)stk_elements_ptr)[i];//(StackElement *)list_symbols->items[i];
			}else{
				stk_element=&((StackElement *)stk_elements_ptr)[i];
			}

			if((stk_element->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION) == 0){
				continue;
			}


			ScriptFunction *irfs = (ScriptFunction *)stk_element->var_ref;
			aux_string=irfs->symbol.name;

			if(metamethod_str != NULL || (aux_string == symbol_to_find && irfs->params->count == n_args)){
				if((irfs->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF)){ /* C! Must match all args...*/
					bool all_check=true; /*  check arguments types ... */
					int idx_type=-1;
					int arg_idx_type=-1;
					for( unsigned k = 0; k < n_args && all_check;k++){
						StackElement *current_arg=&stk_arg[k];
						arg_idx_type=((FunctionParam *)irfs->params->items[k])->idx_type;

						if(arg_idx_type!=IDX_BUILTIN_TYPE_STACK_ELEMENT){

							if(current_arg->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
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
										  ||arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_PTR_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_INTPTR_T_C;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
									idx_type=IDX_BUILTIN_TYPE_FLOAT_PTR_C;
									all_check=arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_PTR_C
											||arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_C
											||arg_idx_type==IDX_BUILTIN_TYPE_UNSIGNED_INT_C
											||arg_idx_type==IDX_BUILTIN_TYPE_INT_PTR_C
										    ||arg_idx_type==IDX_BUILTIN_TYPE_INT_C
											||arg_idx_type==IDX_BUILTIN_TYPE_INTPTR_T_C;
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
									all_check=false;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
									all_check=false;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR:
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
									var_object=((ScriptVar *)current_arg->var_ref);
									aux_string=var_object->getNativePointerClassName();

									if(arg_idx_type==idx_type){
										all_check=true;
									}
									else{
										ScriptClass *c_class=NULL;
										if(var_type & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
											all_check =
													(	arg_idx_type==IDX_BUILTIN_TYPE_STRING_PTR_C && current_arg->var_ref!=0)
												  ||	arg_idx_type==IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C;
										}else if((c_class=var_object->getNativeClass())!=NULL){ /* check whether the base is ok... */
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
						//call_instruction->value_op2 = (intptr_t)irfs;
						ptr_function_found = irfs;
					}
				}else{ /* type script function  ... */
					//call_instruction->value_op2=(intptr_t)irfs;
					ptr_function_found = irfs;
				}
			}
		}

		if(ptr_function_found == NULL){
			/*if(is_constructor && n_args == 0){ // default constructor not found --> set as not found...
				instruction->properties = MSK_INSTRUCTION_PROPERTY_IGNORE_FUNCTION_CALL;//value_op2 = ZS_FUNCTION_NOT_FOUND_IDX;
			}
			else{*/ // return error elaborate a error message...
				int n_candidates=0;
				std::string str_candidates="";
				std::string args_str = "";
				/* get arguments... */

				for( unsigned k = 0; k < n_args;k++){
					StackElement *current_arg=&stk_arg[k];
					if(current_arg->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
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
						if(current_arg->var_ref==0){ /* is constant char */
							aux_string=	k_str_const_char_type_ptr;
						}
						break;
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
						aux_string="NULL";
						break;
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
						aux_string="undefined";
						break;
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR:
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
						aux_string = ((ScriptVar *)current_arg->var_ref)->getNativePointerClassName();
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

				for(int i = stk_elements_len-1; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
					StackElement *stk_element=NULL;
					if(stk_element_are_ptr){
						stk_element=((StackElement **)stk_elements_ptr)[i];//(StackElement *)list_symbols->items[i];
					}else{
						stk_element=&((StackElement *)stk_elements_ptr)[i];
					}
					if((stk_element->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)== 0){
						continue;
					}

					ScriptFunction *irfs = (ScriptFunction *)stk_element->var_ref;


					if(metamethod_str != NULL || (irfs->symbol.name == symbol_to_find)){

						if(n_candidates == 0){
							str_candidates+="\tPossible candidates are:\n\n";
						}
						str_candidates+="\t\t-"+(calling_object==NULL?""
								:calling_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(calling_object->getClassName()+"::")
								:"")+irfs->symbol.name+"(";

						for(unsigned a = 0; a < irfs->params->count; a++){
							if(a>0){
								str_candidates+=",";
							}

							if(irfs->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF){
								str_candidates+=zs_rtti::demangle(
										GET_IDX_2_CLASS_C_STR(this,((FunctionParam *)irfs->params->items[a])->idx_type
								));
							}else{ /* typic var ... */
								str_candidates+="arg"+zs_strutils::int_to_str(a+1);
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
						
						THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Cannot find %s \"%s%s(%s)\".\n\n",
								is_constructor ? "constructor":"function",
								calling_object==NULL?"":calling_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
										calling_function->getInstructionSymbolName(instruction),
								args_str.c_str()
						);
					}
				}
				else{
					if(metamethod_str!=NULL){
						THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Cannot find metamethod \"%s\" for \"%s%s(%s)\".\n\n%s",
													metamethod_str,
													calling_object==NULL?"":calling_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
													"unknown TODOOOOOO",//((ScriptFunction *)global_symbols->items[0])->symbol.name.c_str(),
													args_str.c_str(),
													str_candidates.c_str());
					}else{
						THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Cannot match %s \"%s%s(%s)\" .\n\n%s",
							is_constructor ? "constructor":"function",
							calling_object==NULL?"":calling_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
									calling_function->getInstructionSymbolName(instruction),
							args_str.c_str(),
							str_candidates.c_str());
					}
				}
			/*}*/
		}

		return ptr_function_found;
	}

	inline StackElement VirtualMachine::performAddString(StackElement *stk_result_op1,StackElement *stk_result_op2){
		// we have to create an new string variable

		std::string *str;
		ScriptVarString *script_var_string = NEW_STRING_VAR;
		StackElement stk_element={(void *)script_var_string->str_value.c_str(),script_var_string, MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR};
		script_var_string->initSharedPtr();

		std::string str1;
		std::string str2;

		std::string * str_dst[]={
			   &str1,
			   &str2
		};

		StackElement * stk_src[]={
			   stk_result_op1,
			   stk_result_op2
		};

		std::string ** str_dst_it=str_dst;
		StackElement ** stk_src_it=stk_src;

		// str1
		for(unsigned i=0; i < 2; i++){
			StackElement *stk_src_item=(*stk_src_it);
			if(stk_src_item->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
				stk_src_item=(StackElement *)stk_src_item->var_ref;
			}

			switch(GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_src_item->properties)){
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
				*(*str_dst_it)="undefined";
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
				*(*str_dst_it)="null";
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
				*(*str_dst_it)=zs_strutils::int_to_str((intptr_t)(stk_src_item)->stk_value);
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
				*(*str_dst_it)=zs_strutils::float_to_str(*((float *)&((stk_src_item)->stk_value)));
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
				*(*str_dst_it)=(stk_src_item)->stk_value == 0?"false":"true";
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
				*(*str_dst_it)=(const char *)(stk_src_item)->stk_value;
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION:
				*(*str_dst_it)="function";
				break;
			default:
				if(stk_src_item->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
					*(*str_dst_it)=((ScriptVar *)(stk_src_item)->var_ref)->toString();
				}
				else{
					*(*str_dst_it)="unknow";
				}

				break;
			}

			str_dst_it++;
			stk_src_it++;

		}

		// save result
		script_var_string->str_value=str1+str2;
		stk_element.stk_value=(void *)script_var_string->str_value.c_str();

		return stk_element;
	}
}
