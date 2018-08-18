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


	inline void CVirtualMachine::REMOVE_0_SHARED_POINTERS(int idxCurrentStack,void *ptr_callc_result){
		tInfoSharedList *list = &zero_shares[idxCurrentStack];
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


	inline bool CVirtualMachine::ASSIGN_STACK_VAR(tStackElement *dst_ins, tStackElement *src_ins,tInfoAsmOp *instruction)
	{
		CScriptVariable *script_var=NULL;
		string *aux_str=NULL;
		void *copy_aux=NULL;//copy aux in case of the var is c and primitive (we have to update stkValue on save)
		void **src_ref=&src_ins->stkValue;
		void **dst_ref=&dst_ins->stkValue;
		if(src_ins->properties & STK_PROPERTY_IS_C_VAR){
			src_ref=(void **)((src_ins)->varRef);
		}
		if(dst_ins->properties & STK_PROPERTY_IS_C_VAR){
			if(!IS_NUMBER_OR_INT(src_ins->properties) && IS_NUMBER_OR_INT(dst_ins->properties)){
				if(GET_INS_PROPERTY_VAR_TYPE(src_ins->properties) != GET_INS_PROPERTY_VAR_TYPE(dst_ins->properties)
				){
					PASTNode ast=vec_ast_node[instruction->idxAstNode];
					writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"different types! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals",ast->symbol_value.c_str());
					return false;
				}else{
					if(
						(GET_INS_PROPERTY_VAR_TYPE(src_ins->properties) == STK_PROPERTY_TYPE_SCRIPTVAR)
					){
							zs_print_info_cr("Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator in order to make the proper operation.");
							return false;
					}
				}
			}
			dst_ref=(void **)((dst_ins)->varRef);
			copy_aux=&((dst_ins)->stkValue);
		}
		unsigned short type_var=src_ins->properties;
		unsigned short runtime_var=0; // there's no reason to heredate runtime_props ?!? GET_INS_PROPERTY_RUNTIME(type_var);
		if(type_var & STK_PROPERTY_TYPE_UNDEFINED){
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_UNDEFINED;
		}else if(type_var & STK_PROPERTY_TYPE_NULL){
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_NULL;
		}else if(type_var & STK_PROPERTY_TYPE_INTEGER){
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_INTEGER;
			*((int *)dst_ref)=*((int *)src_ref);
			if(copy_aux!=NULL)(*(int *)copy_aux)=*((int *)src_ref);
		}else if(type_var & STK_PROPERTY_TYPE_NUMBER){
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_NUMBER;
			*((float *)dst_ref)=*((float *)src_ref);
			if(copy_aux!=NULL)(*(float *)copy_aux)=*((float *)src_ref);
		}else if(type_var & STK_PROPERTY_TYPE_BOOLEAN){
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_BOOLEAN;
			*((bool *)dst_ref)=*((bool *)src_ref);
			if(copy_aux!=NULL)(*(bool *)copy_aux)=*((bool *)src_ref);
		}else if(type_var  &  STK_PROPERTY_TYPE_FUNCTION){
			*dst_ins={(unsigned short)(runtime_var | STK_PROPERTY_TYPE_FUNCTION),
						src_ins->stkValue,
						NULL};
		}else if(type_var & STK_PROPERTY_TYPE_STRING){
			if(dst_ins->properties & STK_PROPERTY_IS_C_VAR){
				*((string *)dst_ins->varRef)=((const char *)src_ins->stkValue);// Assign string
				dst_ins->stkValue=(void *)(((string *)dst_ins->varRef)->c_str());// Because string assignment implies reallocs ptr char it changes, so reassing const char pointer
			}else{
				if(((dst_ins->properties & STK_PROPERTY_TYPE_STRING)==0) || (dst_ins->varRef==NULL)){// Generates a string var
					script_var= NEW_STRING_VAR;
					dst_ins->varRef=script_var;
					aux_str=&(((CStringScriptVariable *)script_var)->m_strValue);
					dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_STRING | STK_PROPERTY_TYPE_SCRIPTVAR;
					script_var->initSharedPtr(true);
				}
				(*aux_str)=((const char *)src_ins->stkValue);
				dst_ins->stkValue=(void *)aux_str->c_str();// Because string assignment implies reallocs ptr char it changes, so reassing const char pointer
			}
		}else if(type_var & STK_PROPERTY_TYPE_SCRIPTVAR){
			script_var=(CScriptVariable *)src_ins->varRef;
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_SCRIPTVAR;
			dst_ins->stkValue=NULL;
			dst_ins->varRef=script_var;
			if((dst_ins->properties & STK_PROPERTY_IS_THIS_VAR) !=  STK_PROPERTY_IS_THIS_VAR){
				sharePointer(script_var->ptr_shared_pointer_node);
			}
		}else{
			writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"(internal) cannot determine var type %i",GET_INS_PROPERTY_VAR_TYPE(src_ins->properties));
			return false;
		}

		if(copy_aux!=NULL)
			dst_ins->properties|=STK_PROPERTY_IS_C_VAR;

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



inline void POP_SCOPE(ptr_callc_result) {
	if(scope_info<(current_scope_info_ptr))
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

		REMOVE_0_SHARED_POINTERS(idxCurrentStack,ptr_callc_result);
		// pop current var
		--current_scope_info_ptr;
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
