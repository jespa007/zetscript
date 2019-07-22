namespace zetscript{

		void  write_error(const char *filename, int line, const  char  *string_text, ...);

		#define METAMETHOD_2_ARGS 2
		#define METAMETHOD_1_ARGS 1
		//CVirtualMachine * CVirtualMachine::m_virtualMachine = NULL;
		//std::vector<CVirtualMachine::CVirtualMachine> CVirtualMachine::ALE;
		// static: only defined in this module...
		#define IS_NUMBER_OR_INT(properties)\
			(properties & (STK_PROPERTY_TYPE_NUMBER|STK_PROPERTY_TYPE_INTEGER))



		// general
		#define PRINT_DUAL_ERROR_OP(c)\
		std::string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(stkResultOp1),\
			   var_type2=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(stkResultOp2);\
		\
		write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot perform operator \"%s\" %s  \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
				var_type1.c_str(),\
				c,\
				var_type2.c_str());


		#define PRINT_ERROR_OP(c)\
			std::string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(stkResultOp1);\
		\
		write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot perform preoperator %s\"%s\". Check whether op1 implements the metamethod",\
			c,\
			var_type1.c_str());


	inline void CVirtualMachine::REMOVE_0_SHARED_POINTERS(int idx_stack,void *ptr_callc_result){
		tInfoSharedList *list = &zero_shares[idx_stack];
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


	inline bool CVirtualMachine::POP_SCOPE_CALL(int idx_stack,void * ptr_callc_result, unsigned char properties) {
		bool search=true;


		while((scope_info<current_scope_info_ptr) && search)
		{
			CScriptFunction *ptr_info_function=(current_scope_info_ptr-1)->ptr_info_function;
			int index         = (current_scope_info_ptr-1)->index;
			StackElement         *ptr_local_var=(current_scope_info_ptr-1)->ptr_local_var;
			for(int i = 0; i < ptr_info_function->lut_scope_symbol[index].n_var_index; i++){
				int idx_local_var = ptr_info_function->lut_scope_symbol[index].var_index[i];
				StackElement *ptr_ale =&ptr_local_var[idx_local_var];
				CScriptVariable *var = NULL;
				switch(GET_INS_PROPERTY_VAR_TYPE(ptr_ale->properties)){
				case STK_PROPERTY_TYPE_STRING:
				case STK_PROPERTY_TYPE_SCRIPTVAR:
					var =((CScriptVariable *)(ptr_ale->varRef));
					if(var){
						if(var->ptr_shared_pointer_node != NULL){
							var->unrefSharedPtr();
						}
					}
				}
				*ptr_ale={
					0,
					0,
					STK_PROPERTY_TYPE_UNDEFINED
				};
			}

			REMOVE_0_SHARED_POINTERS(idx_stack,ptr_callc_result);

			search=false;
			if((properties & SCOPE_PROPERTY::BREAK)!=0){
				search=((current_scope_info_ptr-1)->properties & SCOPE_PROPERTY::BREAK) != SCOPE_PROPERTY::BREAK;
			}

			if((properties & SCOPE_PROPERTY::CONTINUE)!=0){
				search=((current_scope_info_ptr-1)->properties & SCOPE_PROPERTY::CONTINUE) != SCOPE_PROPERTY::CONTINUE;
			}


			// pop current var
			--current_scope_info_ptr;


		}

		return true;
	}


	inline bool CVirtualMachine::APPLY_METAMETHOD(
										CScriptVariable *calling_object
										,CScriptFunction *info_function
										,OpCodeInstruction *instruction
										,const char *__OVERR_OP__
										,METAMETHOD_OPERATOR __METAMETHOD__
										,StackElement *stkResultOp1
										,StackElement *stkResultOp2

										) {

		int idxOffsetFunctionMemberStart=0;
		CScriptVariable *script_var_object = NULL;
		CScriptFunction * ptr_function_found=NULL;

		CScriptVariable *one_param = NULL;
		int n_metam_args=((__METAMETHOD__ == NOT_METAMETHOD\
						|| __METAMETHOD__ == NEG_METAMETHOD\
						|| __METAMETHOD__ == SET_METAMETHOD\
						   )? METAMETHOD_1_ARGS:METAMETHOD_2_ARGS);
		StackElement *mm_test_startArg = stkCurrentData+n_metam_args;
		if(instruction->index_op2 ==0){ /* search for first time , else the function is stored in index_op2 */
			CScriptClass *script_class_aux=NULL;

			const char * symbol_to_find;
			ptr_function_found=NULL;


			if(((stkResultOp1->properties & STK_PROPERTY_TYPE_SCRIPTVAR) == (STK_PROPERTY_TYPE_SCRIPTVAR))){
				script_var_object = (CScriptVariable *)(stkResultOp1->varRef);
				if(((stkResultOp1->properties & STK_PROPERTY_IS_STACKVAR) == (STK_PROPERTY_IS_STACKVAR))){
					script_var_object = (CScriptVariable *)(((StackElement *)script_var_object))->varRef;
				}
				if(__METAMETHOD__ == SET_METAMETHOD){
					idxOffsetFunctionMemberStart=1;
					one_param = (CScriptVariable *)(stkResultOp2->varRef);
					if(((stkResultOp2->properties & STK_PROPERTY_IS_STACKVAR) == (STK_PROPERTY_IS_STACKVAR))){
						one_param = (CScriptVariable *)(((StackElement *)one_param))->varRef;
					}
				}
			}else if(((stkResultOp2->properties & STK_PROPERTY_TYPE_SCRIPTVAR) == (STK_PROPERTY_TYPE_SCRIPTVAR)) && (n_metam_args==METAMETHOD_2_ARGS)){\
				script_var_object = (CScriptVariable *)(stkResultOp2->varRef);
				if(((stkResultOp2->properties & STK_PROPERTY_IS_STACKVAR) == (STK_PROPERTY_IS_STACKVAR))){
					script_var_object = (CScriptVariable *)(((StackElement *)script_var_object))->varRef;
				}
			}else{

				std::string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(stkResultOp1),
						var_type2="";

				if(n_metam_args==METAMETHOD_1_ARGS){ /* 1 arg*/
					write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot perform operator %s\"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
							STR(__OVERR_OP),
							var_type1.c_str()
							);\
				}else{ /* 2 args*/
					var_type2=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(stkResultOp2);
					write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot perform operator \"%s\" %s  \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
							var_type1.c_str(),
							__OVERR_OP__,
							var_type2.c_str());
				}

				return false;
			}

			script_class_aux=GET_SCRIPT_CLASS(zs,script_var_object->idx_class);
			std::vector<CScriptFunction *> *vec_global_functions=&script_class_aux->metamethod_operator[__METAMETHOD__];
			StackElement *startArgs=(mm_test_startArg-n_metam_args+idxOffsetFunctionMemberStart);
			unsigned n_args=n_metam_args;

			symbol_to_find=GET_METAMETHOD(zs,__METAMETHOD__);

			/*#define FIND_FUNCTION(iao, is_constructor, symbol_to_find,size_fun_vec,vec_global_functions,startArgs, n_args,scope_type)*/ \
			if((ptr_function_found = FIND_FUNCTION(
					 calling_object
					,info_function
					,instruction
					,instruction
					,NULL
					,vec_global_functions

					,false
					,symbol_to_find

					,stkResultOp1
					,stkResultOp2
					,startArgs
					,n_args\
					,__OVERR_OP__))==NULL)
			{
					return false;
			}
		}else{
			ptr_function_found = (CScriptFunction *)instruction->index_op2;
		}
		/* by default virtual machine gets main object class in order to run functions ... */
		bool error = false;
		StackElement ret_obj=execute_internal(ptr_function_found,script_var_object,error,mm_test_startArg+idxOffsetFunctionMemberStart,ptrCurrentStr,n_metam_args);
		if(error){
			return false;
		}

		/* restore ptrCurretOp... */
		stkCurrentData=mm_test_startArg-n_metam_args;
		/* if function is C must register pointer ! */

		if(ret_obj.properties & STK_PROPERTY_TYPE_SCRIPTVAR){

			if(!((CScriptVariable *)(ret_obj.varRef))->initSharedPtr()){
				return false;
			}
			if(__METAMETHOD__ != SET_METAMETHOD){ /* Auto destroy C when ref == 0 */
				((CScriptVariable *)(ret_obj.varRef))->setDelete_C_ObjectOnDestroy(true);
			}
		}
		if(__METAMETHOD__ != SET_METAMETHOD){ /* Auto destroy C when ref == 0 */
			*stkCurrentData++ = ret_obj;
		}

		return true;
	}

	inline CScriptFunction * CVirtualMachine::FIND_FUNCTION(
			CScriptVariable *calling_object
			,CScriptFunction *info_function
			,OpCodeInstruction *instruction
			,OpCodeInstruction * callAleInstruction

			,std::vector<FunctionSymbol> *m_functionSymbol
			,std::vector<CScriptFunction *> *vec_global_functions

			,bool is_constructor
			,const std::string & symbol_to_find

			,StackElement *stkResultOp1
			,StackElement *stkResultOp2
			,StackElement *startArg
			,unsigned char n_args
			,const char *metamethod_str
			) {

		// by default search over global functions...
		int size_fun_vec=vec_global_functions->size()-1;
		CScriptVariable *var_object = NULL;
		CScriptFunction * ptr_function_found=NULL;
		std::string aux_string;

		// if m_functionSymbol not NULL search over m_functionSymbols....
		if(m_functionSymbol != NULL){
			size_fun_vec=m_functionSymbol->size()-1;
		}

		for(int i = size_fun_vec; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
			CScriptFunction *irfs = NULL;

			if(m_functionSymbol != NULL){
				irfs = (CScriptFunction *)m_functionSymbol->at(i).object.stkValue;
				aux_string=m_functionSymbol->at(i).key_value;

			}else{
				irfs=vec_global_functions->at(i);
				aux_string=irfs->symbol_info.symbol->name;//CEval::getSymbolNameFromSymbolRef(irfs->symbol_info.symbol_ref);
			}

			bool match_signature = metamethod_str != NULL;
			if(!match_signature){
				match_signature = (aux_string == symbol_to_find && irfs->m_arg.size() == n_args);
			}

			if(match_signature){
				if((irfs->symbol_info.properties & PROPERTY_C_OBJECT_REF)){ /* C! Must match args...*/
							bool all_check=true; /*  check arguments types ... */
							int idx_type=-1;
							int arg_idx_type=-1;
							for( unsigned k = 0; k < n_args && all_check;k++){
								StackElement *currentArg=&startArg[k];
								arg_idx_type=irfs->m_arg[k].idx_type;

								if(arg_idx_type==IDX_STACK_ELEMENT){
									/* do nothing because is already trivial ! */
								}
								else{
								if(currentArg->properties & STK_PROPERTY_IS_STACKVAR){
									currentArg = (StackElement *)currentArg->varRef;
								}

								unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(currentArg->properties);

								switch(var_type){
									default:
										aux_string="unknow";
										all_check=false;
										break;
									case STK_PROPERTY_TYPE_INTEGER:
										idx_type=IDX_TYPE_INT_PTR_C;
										all_check=
												arg_idx_type==IDX_TYPE_INT_PTR_C
											  ||arg_idx_type==IDX_TYPE_INT_C
											  ||arg_idx_type==IDX_TYPE_UNSIGNED_INT_C
											  ||arg_idx_type==IDX_TYPE_INTPTR_T_C;
										break;
									case STK_PROPERTY_TYPE_NUMBER:
										idx_type=IDX_TYPE_FLOAT_PTR_C;
										all_check=arg_idx_type==IDX_TYPE_FLOAT_PTR_C
												||arg_idx_type==IDX_TYPE_FLOAT_C;
										break;
									case STK_PROPERTY_TYPE_BOOLEAN:
										idx_type=IDX_TYPE_BOOL_PTR_C;
										all_check=
												arg_idx_type==IDX_TYPE_BOOL_PTR_C
											  ||arg_idx_type==IDX_TYPE_BOOL_C;

										break;
									case STK_PROPERTY_TYPE_STRING:
										idx_type=IDX_TYPE_STRING_PTR_C;

										all_check =
												(	arg_idx_type==IDX_TYPE_STRING_PTR_C && currentArg->varRef!=0)
											  ||	arg_idx_type==IDX_TYPE_CONST_CHAR_PTR_C;
										break;
									case STK_PROPERTY_TYPE_NULL:
									case STK_PROPERTY_TYPE_UNDEFINED:
										break;
									case STK_PROPERTY_TYPE_SCRIPTVAR:
									case STK_PROPERTY_TYPE_SCRIPTVAR|STK_PROPERTY_TYPE_STRING:
										var_object=((CScriptVariable *)currentArg->varRef);
										aux_string=var_object->getPointer_C_ClassName();

										if(arg_idx_type==idx_type){
											all_check=true;
										}
										else{
											CScriptClass *c_class=NULL;
											if(var_type & STK_PROPERTY_TYPE_STRING){
												all_check =
														(	arg_idx_type==IDX_TYPE_STRING_PTR_C && currentArg->varRef!=0)
													  ||	arg_idx_type==IDX_TYPE_CONST_CHAR_PTR_C;
											}else if((c_class=var_object->get_C_Class())!=NULL){ /* check whether the base is ok... */
												all_check=IS_IDX_CLASS_INSTANCEOF(zs,c_class->idx_class,arg_idx_type);
											}else{ /* fail ... */
												all_check=false;
											}
										}

										break;
									}
								}
							}
							if(all_check){ /* we found the right function (set it up!) ... */
								callAleInstruction->index_op2 = (intptr_t)irfs;
								ptr_function_found = irfs;
							}
				}else{ /* type script function  ... */
					callAleInstruction->index_op2=(intptr_t)irfs;
					ptr_function_found = irfs;
				}
			}
		}

		if(ptr_function_found == NULL){
			if(is_constructor && n_args == 0){ /* default constructor not found --> set as not found... */
				callAleInstruction->properties = INS_PROPERTY_NO_FUNCTION_CALL;//index_op2 = ZS_FUNCTION_NOT_FOUND_IDX;
			}
			else{ // return error elaborate a error message...
				int n_candidates=0;
				std::string str_candidates="";
				std::string args_str = "";
				/* get arguments... */

				for( unsigned k = 0; k < n_args;k++){
					StackElement *currentArg=&startArg[k];
					if(currentArg->properties & STK_PROPERTY_IS_STACKVAR){
						currentArg = (StackElement *)currentArg->varRef;
					}

					if(k>0){
						args_str+=",";
					}

					unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(currentArg->properties);

					switch(var_type){

					default:
						aux_string="unknow";
						break;
					case STK_PROPERTY_TYPE_INTEGER:
						aux_string=zs->INT_TYPE_STR;
						break;
					case STK_PROPERTY_TYPE_NUMBER:
						aux_string=zs->FLOAT_TYPE_STR;
						break;
					case STK_PROPERTY_TYPE_BOOLEAN:
						aux_string=zs->BOOL_TYPE_STR;
						break;
					case STK_PROPERTY_TYPE_STRING:
						aux_string=zs->STRING_PTR_TYPE_STR;
						if(currentArg->varRef==NULL){ /* is constant char */
							aux_string=	zs->CONST_CHAR_PTR_TYPE_STR;
						}
						break;
					case STK_PROPERTY_TYPE_NULL:
					case STK_PROPERTY_TYPE_UNDEFINED:
					case STK_PROPERTY_TYPE_SCRIPTVAR:
					case STK_PROPERTY_TYPE_SCRIPTVAR|STK_PROPERTY_TYPE_STRING:
						aux_string = ((CScriptVariable *)currentArg->varRef)->getPointer_C_ClassName();
						break;
					}
					args_str+=rtti::demangle(aux_string);

					if(var_type == STK_PROPERTY_TYPE_INTEGER
					||var_type == STK_PROPERTY_TYPE_NUMBER
					||var_type == STK_PROPERTY_TYPE_BOOLEAN
					){
						args_str+=" [*] ";
					}
				}

				for(int i = size_fun_vec; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
					CScriptFunction *irfs = NULL;
					if(m_functionSymbol!=NULL){
						irfs = (CScriptFunction *)m_functionSymbol->at(i).object.stkValue;
					}else{
						irfs=vec_global_functions->at(i);
					}

					bool match_signature = metamethod_str != NULL;

					if(!match_signature){
						match_signature = irfs->symbol_info.symbol->name == irfs->INSTRUCTION_getSymbolName(callAleInstruction);
					}

					if(match_signature){

						if(n_candidates == 0){
							str_candidates+="ttPossible candidates are:nn";
						}
						str_candidates+="\t\t-"+(calling_object==NULL?"":calling_object->idx_class!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::"):"")+irfs->symbol_info.symbol->name+"(";

						for(unsigned a = 0; a < irfs->m_arg.size(); a++){
							if(a>0){
								str_candidates+=",";
							}

							if(irfs->symbol_info.properties & PROPERTY_C_OBJECT_REF){
								str_candidates+=rtti::demangle(zs->getScriptClass(irfs->m_arg[a].idx_type));
							}else{ /* typic var ... */
								str_candidates+="arg"+string::int_2_string(a+1);
							}
						}
						str_candidates+=");n";
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
						write_error(INSTRUCTION_GET_FILE_LINE(info_function,callAleInstruction),"Cannot find %s \"%s%s(%s)\".\n\n",
										is_constructor ? "constructor":"function",
										calling_object==NULL?"":calling_object->idx_class!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
										info_function->INSTRUCTION_getSymbolName(callAleInstruction),
										args_str.c_str()
						);
					}
				}
				else{
					if(metamethod_str!=NULL){
						write_error(INSTRUCTION_GET_FILE_LINE(info_function,callAleInstruction),"Cannot find metamethod \"%s\" for \"%s%s(%s)\".\n\n%s",
													metamethod_str,
													calling_object==NULL?"":calling_object->idx_class!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
													vec_global_functions->at(0)->symbol_info.symbol->name.c_str(),
													args_str.c_str(),
													str_candidates.c_str());
					}else{
						write_error(INSTRUCTION_GET_FILE_LINE(info_function,callAleInstruction),"Cannot match %s \"%s%s(%s)\" .\n\n%s",
							is_constructor ? "constructor":"function",
							calling_object==NULL?"":calling_object->idx_class!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
							info_function->INSTRUCTION_getSymbolName(callAleInstruction),
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
