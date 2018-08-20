#include "CScriptFunctionObject.h"
#include "CScriptClass.h"
namespace zetscript{

		void  writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);


		//CVirtualMachine * CVirtualMachine::m_virtualMachine = NULL;
		//vector<CVirtualMachine::CVirtualMachine> CVirtualMachine::ALE;
		// static: only defined in this module...
		#define IS_NUMBER_OR_INT(properties)\
			(properties & (STK_PROPERTY_TYPE_NUMBER|STK_PROPERTY_TYPE_INTEGER))



		// general
		#define PRINT_DUAL_ERROR_OP(c)\
		string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp1),\
			   var_type2=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp2);\
		\
		writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"cannot perform operator \"%s\" %s  \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
				var_type1.c_str(),\
				c,\
				var_type2.c_str());


		#define PRINT_ERROR_OP(c)\
			string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp1);\
		\
		writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"cannot perform preoperator %s\"%s\". Check whether op1 implements the metamethod",\
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
			CScriptFunctionObject *ptr_info_function=(current_scope_info_ptr-1)->ptr_info_function;
			int index         = (current_scope_info_ptr-1)->index;
			tStackElement         *ptr_local_var=(current_scope_info_ptr-1)->ptr_local_var;
			for(int i = 0; i < ptr_info_function->object_info.info_var_scope[index].n_var_index; i++){
				int idx_local_var = ptr_info_function->object_info.info_var_scope[index].var_index[i];
				tStackElement *ptr_ale =&ptr_local_var[idx_local_var];
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
					STK_PROPERTY_TYPE_UNDEFINED,
					0,
					0
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


/*
inline void SHARE_LIST_INSERT(list,_node){
	if(list.first == NULL){
		_node->previous=_node->next= list.last = list.first =_node;
	}
	else{
		list.last->next=_node;
		_node->previous=list.last;
		list.last=_node;
		list.first->previous=list.last;
		list.last->next=list.first;
	}
}

inline void SHARE_LIST_DEATTACH(list,_node)
{
		if(_node->next == _node){//one  node: trivial ?
			list.first = list.last = _node->next = _node->previous=NULL;
		}else{// >1 node
			PInfoSharedPointerNode previous=_node->previous;
			PInfoSharedPointerNode next=_node->next;
			if(_node==list.first){
				list.first = next;
			}
			else if(_node==list.last){
				list.last = previous;
			}
			previous->next = next;
			next->previous = previous;
		}
}





*/

	inline CScriptFunctionObject * CVirtualMachine::FIND_FUNCTION(
			vector<tSymbolInfo> *m_functionSymbol
			,vector<int> *vec_global_functions
			,tInfoAsmOp * iao
			,bool is_constructor
			,const string & symbol_to_find
			,CScriptVariable *calling_object
			,tInfoAsmOp *instruction
			,tStackElement *ptrResultInstructionOp1
			,tStackElement *ptrResultInstructionOp2
			,tStackElement *startArg
			,unsigned char n_args
			,const char *metamethod_str
			) {

		// by default search over global functions...

		int size_fun_vec=vec_global_functions->size()-1;
		CScriptVariable *var_object = NULL;
		CScriptFunctionObject * ptr_function_found=NULL;
		string aux_string;

		// if m_functionSymbol not NULL search over m_functionSymbols....
		if(m_functionSymbol != NULL){
			size_fun_vec=m_functionSymbol->size()-1;
		}

		for(int i = size_fun_vec; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
			CScriptFunctionObject *irfs = NULL;

			if(m_functionSymbol != NULL){
				irfs = (CScriptFunctionObject *)m_functionSymbol->at(i).object.stkValue;
				aux_string=CCompiler::getSymbolNameFromSymbolRef(m_functionSymbol->at(i).symbol_value);

			}else{
				irfs=vec_script_function_object_node[vec_global_functions->at(i)];
				aux_string=CCompiler::getSymbolNameFromSymbolRef(irfs->object_info.symbol_info.symbol_ref);
			}

			bool match_signature = metamethod_str != NULL;
			if(!match_signature){
				match_signature = (aux_string == symbol_to_find && irfs->m_arg.size() == n_args);
			}

			if(match_signature){
				if((irfs->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF)){ /* C! Must match args...*/
							bool all_check=true; /*  check arguments types ... */
							int idx_type=-1;
							int arg_idx_type=-1;
							for( unsigned k = 0; k < n_args && all_check;k++){
								tStackElement *currentArg=&startArg[k];
								arg_idx_type=irfs->m_arg[k].idx_type;

								if(arg_idx_type==IDX_STACK_ELEMENT){
									/* do nothing because is already trivial ! */
								}
								else{
								if(currentArg->properties & STK_PROPERTY_IS_STACKVAR){
									currentArg = (tStackElement *)currentArg->varRef;
								}

								unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(currentArg->properties);

								switch(var_type){
									default:
										aux_string="unknow";
										all_check=false;
										break;
									case STK_PROPERTY_TYPE_INTEGER:
										idx_type=IDX_CLASS_INT_PTR_C;
										all_check=
												arg_idx_type==IDX_CLASS_INT_PTR_C
											  ||arg_idx_type==IDX_CLASS_INT_C
											  ||arg_idx_type==IDX_CLASS_UNSIGNED_INT_C
											  ||arg_idx_type==IDX_CLASS_INTPTR_T_C;
										break;
									case STK_PROPERTY_TYPE_NUMBER:
										idx_type=IDX_CLASS_FLOAT_PTR_C;
										all_check=arg_idx_type==IDX_CLASS_FLOAT_PTR_C
												||arg_idx_type==IDX_CLASS_FLOAT_C;
										break;
									case STK_PROPERTY_TYPE_BOOLEAN:
										idx_type=IDX_CLASS_BOOL_PTR_C;
										all_check=
												arg_idx_type==IDX_CLASS_BOOL_PTR_C
											  ||arg_idx_type==IDX_CLASS_BOOL_C;

										break;
									case STK_PROPERTY_TYPE_STRING:
										idx_type=IDX_CLASS_STRING_PTR_C;

										all_check =
												(	arg_idx_type==IDX_CLASS_STRING_PTR_C && currentArg->varRef!=0)
											  ||	arg_idx_type==IDX_CLASS_CONST_CHAR_PTR_C;
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
														(	arg_idx_type==IDX_CLASS_STRING_PTR_C && currentArg->varRef!=0)
													  ||	arg_idx_type==IDX_CLASS_CONST_CHAR_PTR_C;
											}else if((c_class=var_object->get_C_Class())!=NULL){ /* check whether the base is ok... */
												all_check=CScriptClass::isIdxClassInstanceOf(c_class->idxClass,arg_idx_type);
											}else{ /* fail ... */
												all_check=false;
											}
										}

										break;
									}
								}
							}
							if(all_check){ /* we found the right function (set it up!) ... */
								iao->index_op2 = i;
								ptr_function_found = irfs;
							}
				}else{ /* type script function  ... */
					iao->index_op2=i;
					ptr_function_found = irfs;
				}
			}
		}

		if(ptr_function_found == NULL){
			if(is_constructor && n_args == 0){ /* default constructor not found --> set as not found... */
				iao->index_op2 = ZS_FUNCTION_NOT_FOUND_IDX;
			}
			else{ // return error elaborate a error message...
				int n_candidates=0;
				string str_candidates="";
				string args_str = "";
				/* get arguments... */

				for( unsigned k = 0; k < n_args;k++){
					tStackElement *currentArg=&startArg[k];
					if(currentArg->properties & STK_PROPERTY_IS_STACKVAR){
						currentArg = (tStackElement *)currentArg->varRef;
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
						aux_string=(*CScriptClass::INT_TYPE_STR);
						break;
					case STK_PROPERTY_TYPE_NUMBER:
						aux_string=*CScriptClass::FLOAT_TYPE_STR;
						break;
					case STK_PROPERTY_TYPE_BOOLEAN:
						aux_string=*CScriptClass::BOOL_TYPE_STR;
						break;
					case STK_PROPERTY_TYPE_STRING:
						aux_string=*CScriptClass::STRING_PTR_TYPE_STR;
						if(currentArg->varRef==NULL){ /* is constant char */
							aux_string=	*CScriptClass::CONST_CHAR_PTR_TYPE_STR;
						}
						break;
					case STK_PROPERTY_TYPE_NULL:
					case STK_PROPERTY_TYPE_UNDEFINED:
					case STK_PROPERTY_TYPE_SCRIPTVAR:
					case STK_PROPERTY_TYPE_SCRIPTVAR|STK_PROPERTY_TYPE_STRING:
						aux_string = ((CScriptVariable *)currentArg->varRef)->getPointer_C_ClassName();
						break;
					}
					args_str+=demangle(aux_string);

					if(var_type == STK_PROPERTY_TYPE_INTEGER
					||var_type == STK_PROPERTY_TYPE_NUMBER
					||var_type == STK_PROPERTY_TYPE_BOOLEAN
					){
						args_str+=" [*] ";
					}
				}

				for(int i = size_fun_vec; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
					CScriptFunctionObject *irfs = NULL;
					if(m_functionSymbol!=NULL){
						irfs = (CScriptFunctionObject *)m_functionSymbol->at(i).object.stkValue;
					}else{
						irfs=vec_script_function_object_node[vec_global_functions->at(i)];
					}

					bool match_signature = metamethod_str != NULL;

					if(!match_signature){
						match_signature = irfs->object_info.symbol_info.symbol_ref == vec_ast_node[iao->idxAstNode]->symbol_value;
					}

					if(match_signature){

						if(n_candidates == 0){
							str_candidates+="ttPossible candidates are:nn";
						}
						str_candidates+="\t\t-"+(calling_object==NULL?"":calling_object->idxScriptClass!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::"):"")+GET_SYMBOL_NAME(irfs->object_info.symbol_info.symbol_ref)+"(";

						for(unsigned a = 0; a < irfs->m_arg.size(); a++){
							if(a>0){
								str_candidates+=",";
							}

							if(irfs->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF){
								str_candidates+=demangle(GET_IDX_2_CLASS_C_STR(irfs->m_arg[a].idx_type));
							}else{ /* typic var ... */
								str_candidates+="arg"+CZetScriptUtils::intToString(a+1);
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
						writeErrorMsg(GET_AST_FILENAME_LINE(iao->idxAstNode),"Cannot find %s \"%s%s(%s)\".\n\n",
										is_constructor ? "constructor":"function",
										calling_object==NULL?"":calling_object->idxScriptClass!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
										AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode),
										args_str.c_str()
						);
					}
				}
				else{
					if(metamethod_str!=NULL){
						writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Cannot find metamethod \"%s\" for \"%s%s(%s)\".\n\n%s",
													metamethod_str,
													calling_object==NULL?"":calling_object->idxScriptClass!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
													GET_SYMBOL_NAME(vec_script_function_object_node[vec_global_functions->at(0)]->object_info.symbol_info.symbol_ref).c_str(),
													args_str.c_str(),
													str_candidates.c_str());
					}else{
						writeErrorMsg(GET_AST_FILENAME_LINE(iao->idxAstNode),"Cannot match %s \"%s%s(%s)\" .\n\n%s",
							is_constructor ? "constructor":"function",
							calling_object==NULL?"":calling_object->idxScriptClass!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
							AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode),
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
