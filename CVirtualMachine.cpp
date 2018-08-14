/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

#define RETURN_ERROR\
	error=true;\
	return callc_result;


#define METAMETHOD_2_ARGS 2
#define METAMETHOD_1_ARGS 1

namespace zetscript{

	void  writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);


//CVirtualMachine * CVirtualMachine::m_virtualMachine = NULL;
//vector<CVirtualMachine::CVirtualMachine> CVirtualMachine::ALE;
// static: only defined in this module...


	// general
	#define PRINT_DUAL_ERROR_OP(c)\
	string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp1),\
		   var_type2=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp2);\
	\
	writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"cannot perform operator \"%s\" %s  \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
			var_type1.c_str(),\
			c,\
			var_type2.c_str());\
			RETURN_ERROR;

	#define PRINT_ERROR_OP(c)\
		string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp1);\
	\
	writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"cannot perform preoperator %s\"%s\". Check whether op1 implements the metamethod",\
		c,\
		var_type1.c_str());\
		RETURN_ERROR;

	#define COPY_NUMBER(d,s)  memcpy((d),(s),sizeof(float))

	#define LOAD_INT_OP(ptr_result_instruction) \
			((int)((intptr_t)(ptr_result_instruction->stkValue)))




	#define LOAD_BOOL_OP(ptr_result_instruction) \
			(((bool)(ptr_result_instruction->stkValue)))


	#define LOAD_STRING_OP(ptr_result_instruction) \
			(((const char *)(ptr_result_instruction->stkValue)))


	#define IS_NUMBER_OR_INT(properties)\
		(properties & (STK_PROPERTY_TYPE_NUMBER|STK_PROPERTY_TYPE_INTEGER))

	// Check types
	#define IS_NUMBER(properties) \
	(properties & STK_PROPERTY_TYPE_NUMBER)


	#define IS_INT(properties) \
	(properties & STK_PROPERTY_TYPE_INTEGER)


	#define IS_STRING(properties) \
	(properties & STK_PROPERTY_TYPE_STRING)

	#define IS_CHAR_PTR(properties) \
	(properties & STK_PROPERTY_TYPE_CHAR_PTR)


	#define IS_BOOLEAN(properties) \
	(properties & STK_PROPERTY_TYPE_BOOLEAN)

	#define IS_UNDEFINED(properties) \
	(properties & STK_PROPERTY_TYPE_UNDEFINED)

	#define IS_FUNCTION(properties) \
	(properties & STK_PROPERTY_TYPE_FUNCTION)

	#define IS_VAR(properties) \
	(properties & STK_PROPERTY_TYPE_SCRIPTVAR)

	#define IS_VECTOR(ptr_result_instruction) \
	(( ptr_result_instruction->properties & STK_PROPERTY_TYPE_SCRIPTVAR) &&\
	 (((CScriptVariable *)(ptr_result_instruction->stkValue))->idxScriptClass==IDX_CLASS_VECTOR))

	#define IS_GENERIC_NUMBER(properties) \
	((properties & STK_PROPERTY_TYPE_INTEGER) ||\
	(properties & STK_PROPERTY_TYPE_NUMBER))


	#define OP1_AND_OP2_ARE_NUMBERS \
	(IS_GENERIC_NUMBER(ptrResultInstructionOp1->properties) && IS_GENERIC_NUMBER(ptrResultInstructionOp2->properties))

	#define OP1_IS_STRING_AND_OP2_IS_NUMBER \
	(ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_STRING) && \
	IS_GENERIC_NUMBER(ptrResultInstructionOp2->type_var)

	#define OP1_IS_NUMBER_AND_OP2_IS_STRING \
	(ptrResultInstructionOp2->properties & STK_PROPERTY_TYPE_STRING) && \
	IS_GENERIC_NUMBER(ptrResultInstructionOp1->type_var)

	#define OP1_IS_STRING_AND_OP2_IS_BOOLEAN \
	(ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_STRING) && \
	(ptrResultInstructionOp2->properties & STK_PROPERTY_TYPE_BOOLEAN)


	#define OP1_AND_OP2_ARE_BOOLEANS \
	(ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_BOOLEAN) && \
	(ptrResultInstructionOp2->properties & STK_PROPERTY_TYPE_BOOLEAN)

	#define OP1_AND_OP2_ARE_STRINGS \
	(ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_STRING) && \
	(ptrResultInstructionOp2->properties & STK_PROPERTY_TYPE_STRING)

	#define OP1_AND_OP2_ARE_UNDEFINED \
			(ptrResultInstructionOp1->stkValue == VM_UNDEFINED) && \
			(ptrResultInstructionOp2->stkValue == VM_UNDEFINED)

	#define OP1_AND_OP2_ARE_NULL \
			(ptrResultInstructionOp1->stkValue == VM_NULL) && \
			(ptrResultInstructionOp2->stkValue == VM_NULL)



	#define OP1_OR_OP2_IS_UNDEFINED \
			(ptrResultInstructionOp1->stkValue == VM_UNDEFINED) || \
			(ptrResultInstructionOp2->stkValue == VM_UNDEFINED)

	#define OP1_OR_OP2_ARE_NULL \
			(ptrResultInstructionOp1->stkValue == VM_NULL) || \
			(ptrResultInstructionOp2->stkValue == VM_NULL)


	#define PUSH_UNDEFINED \
	*ptrCurrentOp++={STK_PROPERTY_TYPE_UNDEFINED,0,NULL};

	#define PUSH_NULL \
	*ptrCurrentOp++={STK_PROPERTY_TYPE_NULL,0,NULL};

	#define PUSH_BOOLEAN(init_value) \
	*ptrCurrentOp++={STK_PROPERTY_TYPE_BOOLEAN,(void *)((intptr_t)(init_value)),NULL};

	#define PUSH_INTEGER(init_value) \
	*ptrCurrentOp++={STK_PROPERTY_TYPE_INTEGER,(void *)((intptr_t)(init_value)),NULL};

	#define PUSH_NUMBER(init_value) \
	{\
		float aux=(float)(init_value);\
		COPY_NUMBER(&ptrCurrentOp->stkValue,&aux);\
		ptrCurrentOp->properties=STK_PROPERTY_TYPE_NUMBER;\
		ptrCurrentOp++;\
	}

	#define PUSH_STRING(init_value)\
		if(ptrCurrentStr==ptrLastStr){writeErrorMsg(GET_AST_FILENAME_LINE(ZS_UNDEFINED_IDX),"Error stkString out-stack");RETURN_ERROR;}\
		*ptrCurrentStr++=init_value;\
		*ptrCurrentOp++={STK_PROPERTY_TYPE_STRING,(void *)((ptrCurrentStr-1)->c_str()),NULL};\



	#define PUSH_FUNCTION(function_prop,fun_obj,class_obj) \
	*ptrCurrentOp++={(unsigned short)(STK_PROPERTY_TYPE_FUNCTION|function_prop),(void *)(fun_obj),class_obj};

	#define PUSH_SCRIPTVAR(var_ref) \
	*ptrCurrentOp++={INS_PROPERTY_SCRIPTVAR,NULL,var_ref};


	// Push stack var value (as varValue and put ptr as ref second value)...
	#define PUSH_STACK_VAR(stack_ref_var) \
		*ptrCurrentOp++={(unsigned short)(((stack_ref_var)->properties)|STK_PROPERTY_IS_STACKVAR),(stack_ref_var)->stkValue,stack_ref_var};


	#define PROCESS_MOD_OPERATION \
	{ \
		unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
		if(properties == STK_PROPERTY_TYPE_INTEGER){\
				PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) % LOAD_INT_OP(ptrResultInstructionOp2));\
		}\
		else if(properties == (STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_TYPE_NUMBER)){\
				if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){\
					COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
					PUSH_NUMBER(fmod(LOAD_INT_OP(ptrResultInstructionOp1) , f_aux_value2));\
				}else{\
					COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
					PUSH_NUMBER(fmod(f_aux_value1 , LOAD_INT_OP(ptrResultInstructionOp2)));\
				}\
		}\
		else if(properties == STK_PROPERTY_TYPE_NUMBER){\
				COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
				COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
				PUSH_NUMBER(fmod(f_aux_value1 , f_aux_value2));\
		}\
		else{\
			APPLY_METAMETHOD(%,MOD_METAMETHOD);\
		}\
	}

	#define PROCESS_ARITHMETIC_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
		if(properties == STK_PROPERTY_TYPE_INTEGER){\
				PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
		}\
		else if(properties == (STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_TYPE_NUMBER)){\
				if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){\
					COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
					PUSH_NUMBER(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ f_aux_value2);\
				}else{\
					COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
					PUSH_NUMBER(f_aux_value1 __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
				}\
		}\
		else if(properties == STK_PROPERTY_TYPE_NUMBER){\
				COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
				COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
				PUSH_NUMBER(f_aux_value1 __OVERR_OP__ f_aux_value2);\
		}\
		else{\
			APPLY_METAMETHOD(__OVERR_OP__, __METAMETHOD__);\
		}\
	}

	#define PROCESS_COMPARE_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_VAR_TYPE(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
		if(properties & STK_PROPERTY_TYPE_INTEGER){\
				PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
		}\
		else if(properties == STK_PROPERTY_TYPE_BOOLEAN){\
			PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_BOOL_OP(ptrResultInstructionOp2));\
		}\
		else if(properties == (STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_TYPE_NUMBER)){\
				if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){\
					COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ f_aux_value2);\
				}else{\
					COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
					PUSH_BOOLEAN(f_aux_value1 __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
				}\
		}\
		else if(properties == STK_PROPERTY_TYPE_NUMBER){\
				COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
				COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
				PUSH_BOOLEAN(f_aux_value1 __OVERR_OP__ f_aux_value2);\
		}\
		else if((ptrResultInstructionOp1->properties&ptrResultInstructionOp2->properties) == STK_PROPERTY_TYPE_STRING){\
			PUSH_BOOLEAN(STRCMP(LOAD_STRING_OP(ptrResultInstructionOp1), __OVERR_OP__ ,LOAD_STRING_OP(ptrResultInstructionOp2)));\
		}else{\
			APPLY_METAMETHOD(__OVERR_OP__, __METAMETHOD__);\
		}\
	}

	#define PROCESS_LOGIC_OPERATION(__OVERR_OP__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_VAR_TYPE(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
		if(properties == STK_PROPERTY_TYPE_BOOLEAN){\
			PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_BOOL_OP(ptrResultInstructionOp2));\
		}else{\
			PRINT_DUAL_ERROR_OP(STR(__OVERR_OP__));\
		}\
	}

	#define PROCESS_BINARY_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_VAR_TYPE(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
		if(properties == STK_PROPERTY_TYPE_INTEGER){\
			PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
		}else{\
			APPLY_METAMETHOD(__OVERR_OP__, __METAMETHOD__);\
		}\
	}

	#define PERFORM_PRE_POST_OPERATOR(ldrOp, __OPERATOR__) \
	{\
		void **ref=(void **)(&((ldrOp)->stkValue));\
		if(ldrOp->properties & STK_PROPERTY_IS_C_VAR){\
			ref=(void **)((ldrOp)->varRef);\
		}\
		switch(GET_INS_PROPERTY_VAR_TYPE((ldrOp)->properties)){\
		case STK_PROPERTY_TYPE_INTEGER:\
				(*((int *)(ref)))__OPERATOR__;\
				break;\
		case STK_PROPERTY_TYPE_NUMBER:\
				(*((float *)(ref)))__OPERATOR__;\
				break;\
		default:\
			writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode)," Cannot perform pre/post operator (%s)",STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ldrOp));\
			RETURN_ERROR;\
			break;\
		}\
	}


	#define ASSIGN_STACK_VAR(dst_ins, src_ins) \
	{\
		CScriptVariable *script_var=NULL;\
		string *aux_str=NULL;\
		void *copy_aux=NULL;/*copy aux in case of the var is c and primitive (we have to update stkValue on save) */\
		void **src_ref=&src_ins->stkValue;\
		void **dst_ref=&dst_ins->stkValue;\
		if(src_ins->properties & STK_PROPERTY_IS_C_VAR){\
			src_ref=(void **)((src_ins)->varRef);\
		}\
		if(dst_ins->properties & STK_PROPERTY_IS_C_VAR){\
			if(!IS_NUMBER_OR_INT(src_ins->properties) && IS_NUMBER_OR_INT(dst_ins->properties)){\
				if(GET_INS_PROPERTY_VAR_TYPE(src_ins->properties) != GET_INS_PROPERTY_VAR_TYPE(dst_ins->properties)\
				){\
					PASTNode ast=vec_ast_node[instruction->idxAstNode];\
					writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"different types! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals",ast->symbol_value.c_str());\
					RETURN_ERROR;\
				}else{\
					if(\
						(GET_INS_PROPERTY_VAR_TYPE(src_ins->properties) == STK_PROPERTY_TYPE_SCRIPTVAR)\
					){\
							zs_print_info_cr("Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator in order to make the proper operation.");\
							RETURN_ERROR;\
					}\
				}\
			}\
			dst_ref=(void **)((dst_ins)->varRef);\
			copy_aux=&((dst_ins)->stkValue);\
		}\
		unsigned short type_var=src_ins->properties;\
		unsigned short runtime_var=0; /* there's no reason to heredate runtime_props ?!? GET_INS_PROPERTY_RUNTIME(type_var);*/\
		if(type_var & STK_PROPERTY_TYPE_UNDEFINED){\
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_UNDEFINED;\
		}else if(type_var & STK_PROPERTY_TYPE_NULL){\
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_NULL;\
		}else if(type_var & STK_PROPERTY_TYPE_INTEGER){\
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_INTEGER;\
			*((int *)dst_ref)=*((int *)src_ref);\
			if(copy_aux!=NULL)(*(int *)copy_aux)=*((int *)src_ref);\
		}else if(type_var & STK_PROPERTY_TYPE_NUMBER){\
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_NUMBER;\
			*((float *)dst_ref)=*((float *)src_ref);\
			if(copy_aux!=NULL)(*(float *)copy_aux)=*((float *)src_ref);\
		}else if(type_var & STK_PROPERTY_TYPE_BOOLEAN){\
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_BOOLEAN;\
			*((bool *)dst_ref)=*((bool *)src_ref);\
			if(copy_aux!=NULL)(*(bool *)copy_aux)=*((bool *)src_ref);\
		}else if(type_var  &  STK_PROPERTY_TYPE_FUNCTION){\
			*dst_ins={(unsigned short)(runtime_var | STK_PROPERTY_TYPE_FUNCTION),\
						src_ins->stkValue,\
						NULL};\
		}else if(type_var & STK_PROPERTY_TYPE_STRING){\
			if(dst_ins->properties & STK_PROPERTY_IS_C_VAR){\
				*((string *)dst_ins->varRef)=((const char *)src_ins->stkValue);/* Assign string */\
				dst_ins->stkValue=(void *)(((string *)dst_ins->varRef)->c_str());/* Because string assignment implies reallocs ptr char it changes, so reassing const char pointer */\
			}else{\
				if(((dst_ins->properties & STK_PROPERTY_TYPE_STRING)==0) || (dst_ins->varRef==NULL)){/* Generates a string var */  \
					script_var= NEW_STRING_VAR;\
					dst_ins->varRef=script_var;\
					aux_str=&(((CStringScriptVariable *)script_var)->m_strValue);\
					dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_STRING | STK_PROPERTY_TYPE_SCRIPTVAR;\
					script_var->initSharedPtr(true);\
				}\
				(*aux_str)=((const char *)src_ins->stkValue);\
				dst_ins->stkValue=(void *)aux_str->c_str();/* Because string assignment implies reallocs ptr char it changes, so reassing const char pointer */ \
			}\
		}else if(type_var & STK_PROPERTY_TYPE_SCRIPTVAR){\
			script_var=(CScriptVariable *)src_ins->varRef;\
			dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_SCRIPTVAR;\
			dst_ins->stkValue=NULL;\
			dst_ins->varRef=script_var;\
			if((dst_ins->properties & STK_PROPERTY_IS_THIS_VAR) !=  STK_PROPERTY_IS_THIS_VAR){\
				sharePointer(script_var->ptr_shared_pointer_node);\
			}\
		}else{\
			writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"(internal) cannot determine var type %i",GET_INS_PROPERTY_VAR_TYPE(src_ins->properties));\
			RETURN_ERROR;\
		}\
		if(copy_aux!=NULL)dst_ins->properties|=STK_PROPERTY_IS_C_VAR;\
	}

	#define SHARE_LIST_INSERT(list,_node){\
			if(list.first == NULL){\
				_node->previous=_node->next= list.last = list.first =_node;\
			}\
			else{\
				list.last->next=_node;\
				_node->previous=list.last;\
				list.last=_node;\
				list.first->previous=list.last;\
				list.last->next=list.first;\
			}\
		}

	#define SHARE_LIST_DEATTACH(list,_node) \
	{\
			if(_node->next == _node){/*one  node: trivial ?*/\
				list.first = list.last = _node->next = _node->previous=NULL;\
			}else{/* >1 node */\
				PInfoSharedPointerNode previous=_node->previous;\
				PInfoSharedPointerNode next=_node->next;\
				if(_node==list.first){\
					list.first = next;\
				}\
				else if(_node==list.last){\
					list.last = previous;\
				}\
				previous->next = next;\
				next->previous = previous;\
			}\
	}

	#define PUSH_SCOPE(_index,_ptr_info_function, _ptr_local_var) {\
		if(current_scope_info_ptr >=  MAX_SCOPE_INFO){writeErrorMsg(NULL,0,"reached max scope"); RETURN_ERROR;}\
		*current_scope_info_ptr++={(short)(_index),_ptr_info_function,_ptr_local_var};\
	}



	#define POP_SCOPE(ptr_callc_result) \
	if(scope_info<(current_scope_info_ptr))\
	{\
		CScriptFunctionObject *ptr_info_function=(current_scope_info_ptr-1)->ptr_info_function;\
		int index         = (current_scope_info_ptr-1)->index;\
		tStackElement         *ptr_local_var=(current_scope_info_ptr-1)->ptr_local_var;\
		for(int i = 0; i < ptr_info_function->object_info.info_var_scope[index].n_var_index; i++){\
			int idx_local_var = ptr_info_function->object_info.info_var_scope[index].var_index[i];\
			tStackElement *ptr_ale =&ptr_local_var[idx_local_var];\
			CScriptVariable *var = NULL;\
			switch(GET_INS_PROPERTY_VAR_TYPE(ptr_ale->properties)){\
			case STK_PROPERTY_TYPE_STRING:\
			case STK_PROPERTY_TYPE_SCRIPTVAR:\
				var =((CScriptVariable *)(ptr_ale->varRef));\
				if(var){\
					if(var->ptr_shared_pointer_node != NULL){\
						var->unrefSharedPtr();\
					}\
				}\
			}\
			*ptr_ale={\
				STK_PROPERTY_TYPE_UNDEFINED,\
				0,\
				0\
			};\
		}\
	\
		REMOVE_0_SHARED_POINTERS(idxCurrentStack,ptr_callc_result);\
		/* pop current var */ \
		--current_scope_info_ptr;\
	}


	#define CALL_GC SHARED_LIST_DESTROY(zero_shares[idxCurrentStack])

#define FIND_FUNCTION(m_functionSymbol, iao, is_constructor, symbol_to_find,size_fun_vec,vec_global_functions,startArg, n_args, metamethod_str) \
 for(int i = size_fun_vec; i>=0 && aux_function_info==NULL; i--){ /* search all function that match symbol ... */ \
	CScriptFunctionObject *irfs = NULL;\
	if(m_functionSymbol != NULL){\
		irfs = (CScriptFunctionObject *)m_functionSymbol->at(i).object.stkValue;\
		aux_string=CCompiler::getSymbolNameFromSymbolRef(m_functionSymbol->at(i).symbol_value);\
	}else{\
		irfs=vec_script_function_object_node[vec_global_functions->at(i)];\
		aux_string=CCompiler::getSymbolNameFromSymbolRef(irfs->object_info.symbol_info.symbol_ref);\
	}\
\
	bool match_signature = metamethod_str != NULL;\
	if(!match_signature){\
		match_signature = (aux_string == symbol_to_find && irfs->m_arg.size() == n_args);\
	}\
	if(match_signature){\
		if((irfs->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF)){ /* C! Must match args...*/ \
					bool all_check=true; /*  check arguments types ... */ \
					int idx_type=-1;\
					int arg_idx_type=-1;\
					for( unsigned k = 0; k < n_args && all_check;k++){\
						tStackElement *currentArg=&startArg[k];\
						arg_idx_type=irfs->m_arg[k].idx_type;\
\
						if(arg_idx_type==IDX_STACK_ELEMENT){\
							/* do nothing because is already trivial ! */ \
						} \
						else{ \
						if(currentArg->properties & STK_PROPERTY_IS_STACKVAR){ \
							currentArg = (tStackElement *)currentArg->varRef; \
						} \
\
						unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(currentArg->properties); \
\
						switch(var_type){\
							default:\
								aux_string="unknow";\
								all_check=false;\
								break;\
							case STK_PROPERTY_TYPE_INTEGER:\
								idx_type=IDX_CLASS_INT_PTR_C;\
								all_check=\
										arg_idx_type==IDX_CLASS_INT_PTR_C\
									  ||arg_idx_type==IDX_CLASS_INT_C\
									  ||arg_idx_type==IDX_CLASS_UNSIGNED_INT_C\
									  ||arg_idx_type==IDX_CLASS_INTPTR_T_C;\
								break;\
							case STK_PROPERTY_TYPE_NUMBER:\
								idx_type=IDX_CLASS_FLOAT_PTR_C;\
								all_check=arg_idx_type==IDX_CLASS_FLOAT_PTR_C\
										||arg_idx_type==IDX_CLASS_FLOAT_C;\
								break;\
							case STK_PROPERTY_TYPE_BOOLEAN:\
								idx_type=IDX_CLASS_BOOL_PTR_C;\
								all_check=\
										arg_idx_type==IDX_CLASS_BOOL_PTR_C\
									  ||arg_idx_type==IDX_CLASS_BOOL_C;\
\
								break;\
							case STK_PROPERTY_TYPE_STRING:\
								idx_type=IDX_CLASS_STRING_PTR_C;\
\
								all_check =\
										(	arg_idx_type==IDX_CLASS_STRING_PTR_C && currentArg->varRef!=0)\
									  ||	arg_idx_type==IDX_CLASS_CONST_CHAR_PTR_C;\
								break;\
							case STK_PROPERTY_TYPE_NULL:\
							case STK_PROPERTY_TYPE_UNDEFINED:\
								break;\
							case STK_PROPERTY_TYPE_SCRIPTVAR:\
							case STK_PROPERTY_TYPE_SCRIPTVAR|STK_PROPERTY_TYPE_STRING:\
								var_object=((CScriptVariable *)currentArg->varRef);\
								aux_string=var_object->getPointer_C_ClassName();\
\
								if(arg_idx_type==idx_type){\
									all_check=true;\
								}\
								else{\
									CScriptClass *c_class=NULL;\
									if(var_type & STK_PROPERTY_TYPE_STRING){ \
										all_check =\
												(	arg_idx_type==IDX_CLASS_STRING_PTR_C && currentArg->varRef!=0)\
											  ||	arg_idx_type==IDX_CLASS_CONST_CHAR_PTR_C;\
									}else if((c_class=var_object->get_C_Class())!=NULL){ /* check whether the base is ok... */ \
										all_check=CScriptClass::isIdxClassInstanceOf(c_class->idxClass,arg_idx_type); \
									}else{ /* fail ... */ \
										all_check=false; \
									}\
								}\
\
								break;\
							}\
						}\
					}\
					if(all_check){ /* we found the right function (set it up!) ... */ \
						iao->index_op2 = i;\
						aux_function_info = irfs;\
					}\
		}else{ /* type script function  ... */ \
			iao->index_op2=i; \
			aux_function_info = irfs; \
		} \
	} \
} \
\
if(aux_function_info == NULL){\
	if(is_constructor && n_args == 0){ /* default constructor not found --> set as not found... */ \
		iao->index_op2 = ZS_FUNCTION_NOT_FOUND_IDX; \
	} \
	else{ \
		int n_candidates=0; \
		string str_candidates=""; \
		string args_str = "";\
		/* get arguments... */ \
		\
		for( unsigned k = 0; k < n_args;k++){ \
			tStackElement *currentArg=&startArg[k]; \
			if(currentArg->properties & STK_PROPERTY_IS_STACKVAR){ \
				currentArg = (tStackElement *)currentArg->varRef; \
			} \
			if(k>0){\
				args_str+=",";\
			}\
			unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(currentArg->properties);\
			switch(var_type){\
			default:\
				aux_string="unknow";\
				break;\
			case STK_PROPERTY_TYPE_INTEGER:\
				aux_string=(*CScriptClass::INT_TYPE_STR);\
				break;\
			case STK_PROPERTY_TYPE_NUMBER:\
				aux_string=*CScriptClass::FLOAT_TYPE_STR;\
				break;\
			case STK_PROPERTY_TYPE_BOOLEAN:\
				aux_string=*CScriptClass::BOOL_TYPE_STR;\
				break;\
			case STK_PROPERTY_TYPE_STRING:\
				aux_string=*CScriptClass::STRING_PTR_TYPE_STR;\
				if(currentArg->varRef==NULL){ /* is constant char */ \
					aux_string=	*CScriptClass::CONST_CHAR_PTR_TYPE_STR;\
				}\
				break;\
			case STK_PROPERTY_TYPE_NULL:\
			case STK_PROPERTY_TYPE_UNDEFINED:\
			case STK_PROPERTY_TYPE_SCRIPTVAR:\
			case STK_PROPERTY_TYPE_SCRIPTVAR|STK_PROPERTY_TYPE_STRING:\
				aux_string = ((CScriptVariable *)currentArg->varRef)->getPointer_C_ClassName();\
				break;\
			}\
			args_str+=demangle(aux_string);\
			\
			if(var_type == STK_PROPERTY_TYPE_INTEGER \
			||var_type == STK_PROPERTY_TYPE_NUMBER \
			||var_type == STK_PROPERTY_TYPE_BOOLEAN \
			){\
				args_str+=" [*] ";\
			}\
		}\
		\
		for(int i = size_fun_vec; i>=0 && aux_function_info==NULL; i--){ /* search all function that match symbol ... */ \
			CScriptFunctionObject *irfs = NULL; \
			if(m_functionSymbol!=NULL){ \
				irfs = (CScriptFunctionObject *)m_functionSymbol->at(i).object.stkValue; \
			}else{ \
				irfs=vec_script_function_object_node[vec_global_functions->at(i)]; \
			} \
			bool match_signature = metamethod_str != NULL;\
			if(!match_signature){\
				match_signature = irfs->object_info.symbol_info.symbol_ref == vec_ast_node[iao->idxAstNode]->symbol_value;\
			}\
				if( \
					 (match_signature) \
					){ \
						if(n_candidates == 0){ \
							str_candidates+="\t\tPossible candidates are:\n\n"; \
						} \
						str_candidates+="\t\t-"+(calling_object==NULL?"":calling_object->idxScriptClass!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::"):"")+GET_SYMBOL_NAME(irfs->object_info.symbol_info.symbol_ref)+"("; \
\
						for(unsigned a = 0; a < irfs->m_arg.size(); a++){\
							if(a>0){\
								str_candidates+=",";\
							}\
\
							if(irfs->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF){\
								str_candidates+=demangle(GET_IDX_2_CLASS_C_STR(irfs->m_arg[a].idx_type));\
							}else{ /* typic var ... */ \
								str_candidates+="arg"+CZetScriptUtils::intToString(a+1); \
							} \
						} \
						str_candidates+=");\n";\
						n_candidates++;\
					}\
		}\
		if(n_candidates == 0){\
			if(metamethod_str != NULL){\
				if(n_args==2){\
					PRINT_DUAL_ERROR_OP(metamethod_str);\
				}else{\
					PRINT_ERROR_OP(metamethod_str);\
				}\
			}else{\
			writeErrorMsg(GET_AST_FILENAME_LINE(iao->idxAstNode),"Cannot find %s \"%s%s(%s)\".\n\n",\
					is_constructor ? "constructor":"function",\
					calling_object==NULL?"":calling_object->idxScriptClass!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",\
					AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode),\
					 args_str.c_str()\
					);\
			}\
		}\
		else{\
			if(metamethod_str!=NULL){\
				writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Cannot find metamethod \"%s\" for \"%s%s(%s)\".\n\n%s", \
												metamethod_str, \
												calling_object==NULL?"":calling_object->idxScriptClass!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",\
												GET_SYMBOL_NAME(vec_script_function_object_node[vec_global_functions->at(0)]->object_info.symbol_info.symbol_ref).c_str(),\
												args_str.c_str(),\
												str_candidates.c_str()); \
			}else{\
				writeErrorMsg(GET_AST_FILENAME_LINE(iao->idxAstNode),"Cannot match %s \"%s%s(%s)\" .\n\n%s",\
						is_constructor ? "constructor":"function",\
						calling_object==NULL?"":calling_object->idxScriptClass!=IDX_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",\
						AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode),\
						args_str.c_str(),\
						str_candidates.c_str());\
			}\
		}\
		RETURN_ERROR;\
	}\
}


	#define APPLY_METAMETHOD(__OVERR_OP__, __METAMETHOD__) \
		int idxOffsetFunctionMemberStart=0;\
		CScriptVariable *script_var_object = NULL;\
		CScriptVariable *one_param = NULL;\
		int n_metam_args=((__METAMETHOD__ == NOT_METAMETHOD\
				        || __METAMETHOD__ == NEG_METAMETHOD\
						|| __METAMETHOD__ == SET_METAMETHOD\
   	   	   	   	   	   )? METAMETHOD_1_ARGS:METAMETHOD_2_ARGS);\
		tStackElement *mm_test_startArg = ptrCurrentOp+n_metam_args; \
		if(instruction->index_op2 ==ZS_UNDEFINED_IDX){ /* search for first time , else the function is stored in index_op2 */\
			script_class_aux=NULL; \
			vec_global_functions=NULL; \
			aux_function_info = NULL; \
			\
			\
			\
			if(((ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_SCRIPTVAR) == (STK_PROPERTY_TYPE_SCRIPTVAR))){\
				script_var_object = (CScriptVariable *)(ptrResultInstructionOp1->varRef);\
				if(((ptrResultInstructionOp1->properties & STK_PROPERTY_IS_STACKVAR) == (STK_PROPERTY_IS_STACKVAR))){\
					script_var_object = (CScriptVariable *)(((tStackElement *)script_var_object))->varRef;\
				}\
				if(__METAMETHOD__ == SET_METAMETHOD){\
					idxOffsetFunctionMemberStart=1;\
					one_param = (CScriptVariable *)(ptrResultInstructionOp2->varRef);\
					if(((ptrResultInstructionOp2->properties & STK_PROPERTY_IS_STACKVAR) == (STK_PROPERTY_IS_STACKVAR))){\
						one_param = (CScriptVariable *)(((tStackElement *)one_param))->varRef;\
					}\
				}\
			}else if(((ptrResultInstructionOp2->properties & STK_PROPERTY_TYPE_SCRIPTVAR) == (STK_PROPERTY_TYPE_SCRIPTVAR)) && (n_metam_args==METAMETHOD_2_ARGS)){\
				script_var_object = (CScriptVariable *)(ptrResultInstructionOp2->varRef);\
				if(((ptrResultInstructionOp2->properties & STK_PROPERTY_IS_STACKVAR) == (STK_PROPERTY_IS_STACKVAR))){\
					script_var_object = (CScriptVariable *)(((tStackElement *)script_var_object))->varRef;\
				}\
			}else{\
				\
				string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp1),\
						var_type2="";\
				\
				if(n_metam_args==METAMETHOD_1_ARGS){ /* 1 arg*/\
					writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"cannot perform operator %s\"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
							STR(__OVERR_OP),\
							var_type1.c_str()\
							);\
				}else{ /* 2 args*/ \
					var_type2=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp2);\
					writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"cannot perform operator \"%s\" %s  \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
							var_type1.c_str(),\
							STR(__OVERR_OP__),\
							var_type2.c_str());\
				}\
				\
				RETURN_ERROR;\
			}\
\
			script_class_aux=REGISTERED_CLASS_NODE(script_var_object->idxScriptClass);\
			vector<tSymbolInfo> *m_functionSymbol=NULL;\
			vec_global_functions=&script_class_aux->metamethod_operator[__METAMETHOD__];\
			int size_fun_vec = vec_global_functions->size()-1;\
			bool is_constructor = false;\
			tStackElement *startArgs=(mm_test_startArg-n_metam_args+idxOffsetFunctionMemberStart);\
			unsigned n_args=n_metam_args;\
			symbol_to_find=CScriptClass::getMetamethod(__METAMETHOD__);\
			\
			/*#define FIND_FUNCTION(iao, is_constructor, symbol_to_find,size_fun_vec,vec_global_functions,startArgs, n_args,scope_type)*/ \
			FIND_FUNCTION(\
					m_functionSymbol\
					,instruction\
					,is_constructor\
					,symbol_to_find\
					,size_fun_vec\
					,vec_global_functions\
					,startArgs\
					,n_args\
					,STR(__OVERR_OP__));\
		}else{ \
			aux_function_info = (CScriptFunctionObject *)instruction->index_op2; \
		} \
		/* by default virtual machine gets main object class in order to run functions ... */ \
		ret_obj=execute_internal(aux_function_info,script_var_object,error,mm_test_startArg+idxOffsetFunctionMemberStart,ptrCurrentStr,n_metam_args);\
		if(error){\
			RETURN_ERROR; \
		} \
		\
		/* restore ptrCurretOp... */ \
		ptrCurrentOp=mm_test_startArg-n_metam_args; \
		/* if function is C must register pointer ! */ \
		 \
		if(ret_obj.properties & STK_PROPERTY_TYPE_SCRIPTVAR){ \
		\
			if(!((CScriptVariable *)(ret_obj.varRef))->initSharedPtr()){ \
				RETURN_ERROR; \
			} \
			if(__METAMETHOD__ != SET_METAMETHOD){ /* Auto destroy C when ref == 0 */\
				((CScriptVariable *)(ret_obj.varRef))->setDelete_C_ObjectOnDestroy(true);\
			}\
		} \
		if(__METAMETHOD__ != SET_METAMETHOD){ /* Auto destroy C when ref == 0 */\
			*ptrCurrentOp++ = ret_obj; \
		}



	const char * CVirtualMachine::STR_GET_TYPE_VAR_INDEX_INSTRUCTION(tStackElement *ptr_info_ale){
		const char * result="undefined";
		if(IS_INT(ptr_info_ale->properties))
			result= "int";
		else if(IS_NUMBER(ptr_info_ale->properties))
			result= "float";
		else if(IS_BOOLEAN(ptr_info_ale->properties))
			result= "bool";
		else if(IS_STRING(ptr_info_ale->properties))
			result= "string";
		else if(IS_FUNCTION(ptr_info_ale->properties))
			result= "function";

		else if(IS_VAR(ptr_info_ale->properties)){
			tStackElement *var=ptr_info_ale;
			if(ptr_info_ale->properties & STK_PROPERTY_IS_STACKVAR){
				var=(tStackElement *)var->varRef;
			}
			result=((CScriptVariable *)var->varRef)->getClassName().c_str();
		}

		return result;
	}

	void CVirtualMachine::stackDumped(){
		// derefer all variables in all scopes (except main )...
		while(scope_info<(current_scope_info_ptr)){
			POP_SCOPE(NULL);
		}

		idxCurrentStack=0;
	}

	CVirtualMachine::CVirtualMachine(){

		//-----------------------------------------------------------
		// set memory manager

		memset(zero_shares,0,sizeof(zero_shares));
		memset(shared_var,0,sizeof(shared_var));
		//memset(stack,0,sizeof(stack));

		tStackElement *aux=stack;

		for(int i=0; i < VM_LOCAL_VAR_MAX_STACK;i++){
			*aux++={STK_PROPERTY_TYPE_UNDEFINED,0,NULL};
		}

		idxCurrentStack=0;
		//------------------

		f_return_value=0;
		s_return_value="unknow";

		cancel_execution=false;

		ptrCurrentOp=NULL;
		current_scope_info_ptr = scope_info;

		f_aux_value1=0;
		f_aux_value2=0;

		custom_error = "unknow_error";

		idx_laststatment=0;

		ptrLastStr=&stkString[VM_MAX_AUX_STRINGS-1]; // aux values for string ...
		ptrCurrentStr=NULL;

		MAX_SCOPE_INFO = &scope_info[VM_MAX_SCOPES-1];

		current_ast_node_call_c_function=-1;

		main_function_object = NULL;

		vec_script_function_object_node = NULL;
		vec_ast_node = NULL;

		size_vec_script_function_object_node = 0;
		size_vec_ast_node = 0;


	}

	void CVirtualMachine::iniStackVar(unsigned int pos,const tStackElement & stk){

		if(pos < VM_LOCAL_VAR_MAX_STACK){
			stack[pos]=stk;
		}else{
			writeErrorMsg(GET_AST_FILENAME_LINE(ZS_UNDEFINED_IDX),"Attempt to assign stack element over limit (%i)",pos);
		}

	}

	void CVirtualMachine::clearGlobals(){
		CScriptFunctionObject  *main_function = GET_SCRIPT_FUNCTION_OBJECT(0);

		// zero shares have a late loading so it can be null at first time...
		if(zero_shares == NULL){
			return;
		}

		if(main_function == NULL){ // not created.
			return;
		}


		for(unsigned i = 0; i < main_function->object_info.local_symbols.m_registeredVariable.size(); i++){
			tStackElement *ptr_ale =&stack[i];
			CScriptVariable *var = NULL;
			//switch(GET_INS_PROPERTY_VAR_TYPE(ptr_ale->properties)){
			//case STK_PROPERTY_TYPE_STRING:
			if(ptr_ale->properties &STK_PROPERTY_TYPE_SCRIPTVAR){
				var =((CScriptVariable *)(ptr_ale->varRef));
				if(var){
					if(var->ptr_shared_pointer_node != NULL){
						var->unrefSharedPtr();
					}
				}
			}
			//}
		}

		REMOVE_0_SHARED_POINTERS(0,NULL);


		// clear all stack variables
		tStackElement *aux=stack;
		for(int i=0; i < VM_LOCAL_VAR_MAX_STACK;i++){
			*aux++={STK_PROPERTY_TYPE_UNDEFINED,0,NULL};
		}
		//int idxCurrentStack
	}



	#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

	#define print_vm_cr zs_print_info_cr
	#else
	#define print_vm_cr(s,...)
	#endif


	//============================================================================================================================================
	// POINTER MANANAGER
	string stk_C_TypeStr(const tStackElement & stk_v){
		if(stk_v.properties & STK_PROPERTY_TYPE_INTEGER){
			return demangle(typeid(int).name());
		}else if(stk_v.properties & STK_PROPERTY_TYPE_NUMBER){
			return demangle(typeid(float).name());
		}else if(stk_v.properties & STK_PROPERTY_TYPE_BOOLEAN){
			return demangle(typeid(bool).name());
		}else if(stk_v.properties & STK_PROPERTY_TYPE_STRING){
			return demangle(typeid(string).name());
		}else if(stk_v.properties & STK_PROPERTY_TYPE_SCRIPTVAR){


			CScriptClass *c = CScriptClass::getScriptClassByIdx(((CScriptVariable *)(stk_v.varRef))->idxScriptClass);

			if(c!=NULL){
				return demangle(c->classPtrType);
			}
		}
		return "unknow";
	}

	PInfoSharedPointerNode CVirtualMachine::newSharedPointer(CScriptVariable *_var_ptr){
		//int index = CVirtualMachine::getFreeCell();
		PInfoSharedPointerNode _node = (PInfoSharedPointerNode)malloc(sizeof(tInfoSharedPointerNode));
		_node->data.n_shares=0;
		_node->data.shared_ptr=_var_ptr;
		_node->currentStack = idxCurrentStack;

		// insert node into shared nodes ...
		SHARE_LIST_INSERT(zero_shares[idxCurrentStack],_node);
		//zero_shares[idxCurrentStack].InsertNode(_node);
		return _node;
	}


	bool CVirtualMachine::sharePointer(PInfoSharedPointerNode _node){

		unsigned char *n_shares = &_node->data.n_shares;

		bool move_to_shared_list=*n_shares==0;

		if(*n_shares >= MAX_SHARES_VARIABLE){
			THROW_RUNTIME_ERROR("MAX SHARED VARIABLES (Max. %i)",MAX_SHARES_VARIABLE);
			return false;

		}

		(*n_shares)++;

		if(move_to_shared_list){

			// Mov to shared pointer...
			SHARE_LIST_DEATTACH(zero_shares[_node->currentStack],_node);
			// update current stack due different levels from functions!
			_node->currentStack=idxCurrentStack;
			SHARE_LIST_INSERT(shared_var[idxCurrentStack],_node);
		}

		return true;

	}

	void CVirtualMachine::unrefSharedScriptVar(PInfoSharedPointerNode _node, bool remove_if_0){

		unsigned char *n_shares = &_node->data.n_shares;
		if(*n_shares > 0){
			if(--(*n_shares)==0){ // mov back to 0s shares (candidate to be deleted on GC check)

				SHARE_LIST_DEATTACH(shared_var[_node->currentStack],_node);

				if(remove_if_0){ // remove node and data instead...
					free(_node);
					delete _node->data.shared_ptr;
				}
				else{ // insert into zero array.. if not referenced anymore will be removed by REMOVE_0_SHARED
					// update current stack due different levels from functions!
					_node->currentStack=idxCurrentStack;
					SHARE_LIST_INSERT(zero_shares[idxCurrentStack],_node);
				}
			}
		}
	}


	#define POP_TWO \
	ptrResultInstructionOp2=--ptrCurrentOp;\
	ptrResultInstructionOp1=--ptrCurrentOp;

	#define READ_TWO_POP_ONE \
	ptrResultInstructionOp2=--ptrCurrentOp;\
	ptrResultInstructionOp1=(ptrCurrentOp-1);


	#define POP_ONE \
	ptrResultInstructionOp1=--ptrCurrentOp;

	#define CHK_JMP \
	if(instruction->index_op2 != ZS_UNDEFINED_IDX){\
	\
		if(current_statment!=&m_listStatements[instruction->index_op2]){\
			ptr_statment_iteration=&m_listStatements[instruction->index_op2];\
			goto lbl_exit_statment;\
		}\
	\
		if((char)(instruction->index_op1) != ZS_UNDEFINED_IDX){\
			instruction_it=&(*current_statment)[instruction->index_op1];\
		}\
	\
	}

	int CVirtualMachine::getCurrentAstNodeCall_C_Function(){
		return current_ast_node_call_c_function;
	}

	//============================================================================================================================================
	tStackElement  CVirtualMachine::call_C_function(
			intptr_t fun_ptr,
			const CScriptFunctionObject *irfs,
			bool & error,
			tStackElement *ptrArg,
			unsigned char n_args,
			int idxAstNode){

		tStackElement callc_result={STK_PROPERTY_TYPE_UNDEFINED,0,0};

		intptr_t converted_param[MAX_N_ARGS];
		intptr_t result=0;
		tStackElement *currentArg;
		current_ast_node_call_c_function = idxAstNode;
		//float aux_float=0;

		if(n_args>MAX_N_ARGS){
			writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"Max run-time args! (Max:%i Provided:%i)",MAX_N_ARGS,n_args);
			RETURN_ERROR;
		}


		if((irfs->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) {
			writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"Function is not registered as C");
			RETURN_ERROR;
		}

		if(fun_ptr==0){
			writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"Null function");
			//return &callc_result;//CScriptVariable::UndefinedSymbol;
			RETURN_ERROR;
		}

		if((char)irfs->m_arg.size() != n_args){
			writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"C argument VS scrip argument doestn't match sizes");
			RETURN_ERROR;
		}

		if(irfs->m_arg.size() > MAX_N_ARGS){
			writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"Reached max param for C function (Current: %i Max Allowed: %i)",irfs->m_arg.size(),MAX_N_ARGS);
			RETURN_ERROR;
			//return &callc_result;//CScriptVariable::UndefinedSymbol;
		}

		// convert parameters script to c...
		for(unsigned char  i = 0; i < n_args;i++){

			currentArg=&ptrArg[i];

			if(!stk2var(currentArg,irfs->m_arg[i].idx_type,(intptr_t *)&converted_param[i],error_str)){
				writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"Function %s, param %i: %s. The function C %s that was found for first time it has different argument types now.",
																irfs->object_info.symbol_info.symbol_ref.c_str(),
																i,
																error_str.c_str(),
																irfs->object_info.symbol_info.symbol_ref.c_str()
																);
				RETURN_ERROR;
			}
		}

		zs_print_debug_cr("pre_call %i",n_args);

		if(irfs->idx_return_type == IDX_CLASS_VOID_C){ // getInstance()->getIdxClassVoid()){

			switch(n_args){
			case 0:
				(*((std::function<void ()> *)fun_ptr))();
				break;
			case 1:
				(*((std::function<void (intptr_t)> *)fun_ptr))(converted_param[0]);
				break;
			case 2:
				(*((std::function<void (intptr_t,intptr_t)> *)fun_ptr))(
						converted_param[0],
						converted_param[1]
										);
				break;
			case 3:
				(*((std::function<void (intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
						converted_param[0],
						converted_param[1],
						converted_param[2]
										);
				break;
			case 4:
				(*((std::function<void (intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
						converted_param[0],
						converted_param[1],
						converted_param[2],
						converted_param[3]
										);
				break;
			case 5:
				(*((std::function<void (intptr_t,intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
						converted_param[0],
						converted_param[1],
						converted_param[2],
						converted_param[3],
						converted_param[4]
					);
				break;
			case 6:
				(*((std::function<void (intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
						converted_param[0],
						converted_param[1],
						converted_param[2],
						converted_param[3],
						converted_param[4],
						converted_param[5]
										);
				break;
			}

		}else{

			if(irfs->idx_return_type==IDX_CLASS_BOOL_C){  // we must do a bool cast in order to get float return.
				switch(n_args){
				case 0:
					result=(*((std::function<bool ()> *)fun_ptr))();
					break;
				case 1:
					result=(*((std::function<bool (intptr_t)> *)fun_ptr))(converted_param[0]);
					break;
				case 2:

					result=(*((std::function<bool (intptr_t,intptr_t)> *)fun_ptr))(
							converted_param[0],
							converted_param[1]
											);
					break;
				case 3:
					result=(*((std::function<bool (intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
							converted_param[0],
							converted_param[1],
							converted_param[2]
											);
					break;
				case 4:
					result=(*((std::function<bool (intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
							converted_param[0],
							converted_param[1],
							converted_param[2],
							converted_param[3]
											);
					break;
				case 5:
					result=(*((std::function<bool (intptr_t,intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
							converted_param[0],
							converted_param[1],
							converted_param[2],
							converted_param[3],
							converted_param[4]
						);
					break;
				case 6:
					result=(*((std::function<bool (intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
							converted_param[0],
							converted_param[1],
							converted_param[2],
							converted_param[3],
							converted_param[4],
							converted_param[5]
											);
					break;

				}
			}else if(irfs->idx_return_type==IDX_CLASS_FLOAT_C){ // we must do a float cast in order to get float return.
					float aux_flt;
					switch(n_args){
					case 0:
						aux_flt =(*((std::function<float ()> *)fun_ptr))();
						break;
					case 1:
						aux_flt =(*((std::function<float (intptr_t)> *)fun_ptr))(converted_param[0]);
						break;
					case 2:

						aux_flt =(*((std::function<float (intptr_t,intptr_t)> *)fun_ptr))(
								converted_param[0],
								converted_param[1]
												);
						break;
					case 3:
						aux_flt =(*((std::function<float (intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
								converted_param[0],
								converted_param[1],
								converted_param[2]
												);
						break;
					case 4:
						aux_flt =(*((std::function<float (intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
								converted_param[0],
								converted_param[1],
								converted_param[2],
								converted_param[3]
												);
						break;
					case 5:
						aux_flt =(*((std::function<float (intptr_t,intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
								converted_param[0],
								converted_param[1],
								converted_param[2],
								converted_param[3],
								converted_param[4]
							);
						break;
					case 6:
						aux_flt =(*((std::function<float (intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
								converted_param[0],
								converted_param[1],
								converted_param[2],
								converted_param[3],
								converted_param[4],
								converted_param[5]
												);
						break;

					}

					memcpy(&result, &aux_flt, sizeof(float));
			}else{ // generic int

				switch(n_args){
				case 0:
					result=(*((std::function<intptr_t ()> *)fun_ptr))();
					break;
				case 1:
					result=(*((std::function<intptr_t (intptr_t)> *)fun_ptr))(converted_param[0]);
					break;
				case 2:

					result=(*((std::function<intptr_t (intptr_t,intptr_t)> *)fun_ptr))(
							converted_param[0],
							converted_param[1]
											);
					break;
				case 3:
					result=(*((std::function<intptr_t (intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
							converted_param[0],
							converted_param[1],
							converted_param[2]
											);
					break;
				case 4:
					result=(*((std::function<intptr_t (intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
							converted_param[0],
							converted_param[1],
							converted_param[2],
							converted_param[3]
											);
					break;
				case 5:
					result=(*((std::function<intptr_t (intptr_t,intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
							converted_param[0],
							converted_param[1],
							converted_param[2],
							converted_param[3],
							converted_param[4]
						);
					break;
				case 6:
					result=(*((std::function<intptr_t (intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t)> *)fun_ptr))(
							converted_param[0],
							converted_param[1],
							converted_param[2],
							converted_param[3],
							converted_param[4],
							converted_param[5]
											);
					break;

				}


			}

			callc_result=var2stk(result,irfs->idx_return_type);
		}





		return callc_result;
	}

	tStackElement * CVirtualMachine::getStackElement(unsigned int idx_glb_element){
		CScriptFunctionObject  *main_function = GET_SCRIPT_FUNCTION_OBJECT(0);

		if(idx_glb_element < main_function->object_info.local_symbols.m_registeredVariable.size()){
			return &stack[idx_glb_element];
		}

		return NULL;
	}

	void CVirtualMachine::setError(const char *str){

		writeErrorMsg("custom_user",-1,str);

		custom_error = str;
		cancel_execution = true;
	}

	void CVirtualMachine::cancelExecution(){
		custom_error=NULL;
		cancel_execution = true;
	}

	void CVirtualMachine::destroyCache(){
		main_function_object=NULL;
		if(vec_script_function_object_node!= NULL){
			free(vec_script_function_object_node);
			vec_script_function_object_node=NULL;
			size_vec_script_function_object_node=0;
		}

		if(vec_ast_node!= NULL){
			free(vec_ast_node);
			vec_ast_node=NULL;
			size_vec_ast_node=0;
		}

	}

	void CVirtualMachine::buildCache(){

		destroyCache();

		main_function_object = GET_SCRIPT_FUNCTION_OBJECT(0);
		vector<CScriptFunctionObject *> *vec_script_function_object_node_aux=CScriptFunctionObject::getVectorScriptFunctionObjectNode();
		size_vec_script_function_object_node=vec_script_function_object_node_aux->size();
		vec_script_function_object_node=(CScriptFunctionObject **)malloc(sizeof(CScriptFunctionObject *)*size_vec_script_function_object_node);
		for(unsigned i=0; i < size_vec_script_function_object_node; i++){
			vec_script_function_object_node[i]=vec_script_function_object_node_aux->at(i);
		}


		vector<CASTNode * > * vec_ast_node_aux = CASTNode::getVectorASTNode();
		size_vec_ast_node=vec_ast_node_aux->size();
		vec_ast_node=(CASTNode **)malloc(sizeof(CASTNode *)*size_vec_ast_node);
		for(unsigned i=0; i < size_vec_ast_node; i++){
			vec_ast_node[i]=vec_ast_node_aux->at(i);
		}
	}


	tStackElement  CVirtualMachine::execute(
			 CScriptFunctionObject *info_function,
			 CScriptVariable *this_object,
			 bool & error,
			const vector<tStackElement> & arg
			){


		tStackElement callc_result={STK_PROPERTY_TYPE_UNDEFINED,0,0};

		if(info_function==NULL){
			RETURN_ERROR;
		}


		if(idxCurrentStack==0){ // set stack for first call

			if(main_function_object->object_info.statment_op == NULL){ // no statments
				RETURN_ERROR;
			}
			cancel_execution=false;


			ptrCurrentOp=stack;
			//*ptrCurrentOp={STK_PROPERTY_TYPE_UNDEFINED,0,0}; // ini first op

			if(info_function->object_info.idxScriptFunctionObject != 0){ // calls script function from C : preserve stack space for global vars
				ptrCurrentOp=&stack[main_function_object->object_info.local_symbols.m_registeredVariable.size()];
			}
		}

		int n_arg=0;
		if(arg.size()>0){ // pass parameters...

			for(unsigned i = 0; i < arg.size(); i++){
				*ptrCurrentOp++=arg[i];
			}
			//advance idxBaseStk...
			//idxBaseStk+=arg->size();
			n_arg=arg.size();
		}

		// Script function starts here.... later script function can call c++ function, but once in c++ function is not possible by now call script function again.


		tStackElement info=execute_internal(
				info_function,
				this_object,
				error,
				ptrCurrentOp,
				NULL,
				n_arg);


		if(error){ // it was error so reset stack and stop execution ? ...
			stackDumped();
		}


		return info;
	}


	tStackElement CVirtualMachine::execute_internal(
			CScriptFunctionObject * info_function,
			CScriptVariable       * this_object,
			bool & error,
			tStackElement 		  * _ptrStartOp,
			string 		  		  * _ptrStartStr,
			unsigned char n_args,
			int idxAstNode){

		error=false;
		tStackElement callc_result={STK_PROPERTY_TYPE_UNDEFINED,0,0};

		string *ptrStartStr;
		tStackElement *ptrStartOp;

		if(info_function == NULL){
			RETURN_ERROR;
		}

		vector<tInfoVariableSymbol> * local_var=&info_function->object_info.local_symbols.m_registeredVariable;

		ptrStartOp =_ptrStartOp;
		ptrStartStr =_ptrStartStr;

		if(ptrStartStr == NULL){
			ptrStartStr=stkString;
		}

		tStackElement *ptrLocalVar=NULL;
		tStackElement *ptrArg=NULL;
		tVM_ScopeInfo * ptrStartScopeInfo=NULL;

		zs_print_debug_cr("Executing function %s ...",info_function->object_info.symbol_info.symbol_ref.c_str());
		int idxBaseStk=(ptrStartOp-stack);//>>sizeof(tStackElement *);

		if(idxBaseStk<n_args){
			writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"Internal error (idxBaseStk<n_args) (%i<%i)",idxBaseStk,n_args);
			RETURN_ERROR;
		}

		if(n_args>0){
			ptrArg=(ptrStartOp-n_args);
		}

		ptrLocalVar=ptrStartOp;

		if(idxCurrentStack < MAX_FUNCTION_CALL){

			/* tVM_ScopeInfo		scope_info[VM_LOCAL_VAR_MAX_STACK];
			string 				stkString[VM_LOCAL_VAR_MAX_STACK]; // aux values for string ...
			string              *ptrLastStr;
			string              *ptrCurrentStr;

			 tStackElement     stack[VM_LOCAL_VAR_MAX_STACK];*/

			//printf("call:%i left:%lu\n",idxCurrentStack,MAX_SCOPE_INFO-current_scope_info_ptr);// scope_info[VM_LOCAL_VAR_MAX_STACK-1]);
			idxCurrentStack++;
		}
		else{
			writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"Reached max stack");
			RETURN_ERROR;
		}

		if((info_function->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // C-Call

			intptr_t  fun_ptr = info_function->object_info.symbol_info.ref_ptr;

			if((info_function->object_info.symbol_info.properties &  SYMBOL_INFO_PROPERTIES::PROPERTY_STATIC_REF) != SYMBOL_INFO_PROPERTIES::PROPERTY_STATIC_REF){ // if not static then is function depends of object ...

				if(this_object!= NULL && this_object != CZetScript::getInstance()->getMainObject()){
					fun_ptr = this_object->getFunctionSymbolByIndex(info_function->object_info.symbol_info.idxSymbol)->proxy_ptr;
				}
			}

			tStackElement se = call_C_function(fun_ptr,info_function,error,ptrArg,n_args, idxAstNode);

			if(idxCurrentStack > 0){
				idxCurrentStack--;
			}
			else{
				writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"Reached min stack");
				RETURN_ERROR;
			}


			return se;
		}

		PASTNode ast = vec_ast_node[info_function->object_info.symbol_info.idxAstNode];

		short scope_index =  ast->idxScope;


		if(ast->keyword_info == KEYWORD_TYPE::FUNCTION_KEYWORD){
			scope_index=vec_ast_node[ast->children[1]]->idxScope;
		}

		if(info_function->object_info.idxScriptFunctionObject != 0){

			PUSH_SCOPE(scope_index,info_function,ptrLocalVar);
			ptrStartScopeInfo = current_scope_info_ptr;
		}

		//CScriptVariable *ret=VM_UNDEFINED;
		callc_result ={STK_PROPERTY_TYPE_UNDEFINED, NULL};
		PtrStatment m_listStatements = info_function->object_info.statment_op;

		//bool end_by_ret=false;

		//=========================================
		// PUSH STACK
		// reserve vars and assign argv vars ...
		//pushStack(info_function, n_args);
		unsigned size_local_var=local_var->size();
		unsigned n_total_vars=n_args+size_local_var;

		if((idxBaseStk+n_total_vars) >=  VM_LOCAL_VAR_MAX_STACK){
			writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"Error MAXIMUM stack size reached");
			RETURN_ERROR;
		}

		// init local vars ...
		if(info_function->object_info.idxScriptFunctionObject != 0){ // is not main function, so we have to initialize vars.


			if(idxCurrentStack > 1){ // not global vars, then initialize variables as undefined...
				tStackElement *ptr_aux = ptrLocalVar;
				for(unsigned i = 0; i < size_local_var; i++){

					// if C then pass its properties...
					*ptr_aux++={
							STK_PROPERTY_TYPE_UNDEFINED, // starts undefined.
							0,							 // no value assigned.
							0 						     // no varref related.
					};
				}
			}
		}

		ptrStartOp=&ptrLocalVar[size_local_var];

		// PUSH STACK
		//=========================================
		//bool	aux_boolean=false;
		float aux_float=0.0;


		CScriptFunctionObject * aux_function_info=NULL;
		tStackElement ret_obj;

		CScriptVariable *svar=NULL;
		CScriptVariable *calling_object=NULL;
		CScriptClass *script_class_aux;

		vector<int> *vec_global_functions=NULL;

		tStackElement *ptrResultInstructionOp1=NULL;//&stkResultInstruction[index_op1+startIdxStkResultInstruction];
		tStackElement *ptrResultInstructionOp2=NULL;//&stkResultInstruction[index_op2+startIdxStkResultInstruction];
		tStackElement *ldrVar;
		unsigned short pre_post_properties=0;
		unsigned short instruction_properties=0;
		tSymbolInfo *si;
		CScriptVariable *var_object = NULL;

		unsigned short scope_type=0;//GET_INS_PROPERTY_SCOPE_TYPE(instruction->instruction_properties);

		PtrStatment current_statment = NULL,
					ptr_statment_iteration=m_listStatements;



		tStackElement *dst_ins=NULL;
		tStackElement *src_ins=NULL;
		bool ok=false;


		tInfoAsmOp *instruction_it;

		for(;;){ // foreach stament ...


			current_statment = ptr_statment_iteration++;
			instruction_it = *current_statment;

			if(!instruction_it){
				goto lbl_exit_function;
			}

			// reset stack to start...
			ptrCurrentStr=ptrStartStr;
			ptrCurrentOp=ptrStartOp;

			//-----------------------------------------------------------------------------------------------------------------------
			//
			// START OPERATOR MANAGEMENT
			//

			 for(;;){ // foreach asm instruction ...

				tInfoAsmOp * instruction = instruction_it++;

				const unsigned char operator_type=instruction->operator_type;
				const unsigned char index_op1=instruction->index_op1;

				switch(operator_type){
				//default:
				//	writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
				//	RETURN_ERROR;
				case END_STATMENT:
					goto lbl_exit_statment;
				case LOAD:
				case VGET:

					if(index_op1==LOAD_TYPE::LOAD_TYPE_VARIABLE || operator_type==VGET){
						if(operator_type==VGET){

							bool ok=false;

							POP_TWO;

							if( (ptrResultInstructionOp1->properties & (STK_PROPERTY_TYPE_SCRIPTVAR | STK_PROPERTY_IS_STACKVAR)) == (STK_PROPERTY_TYPE_SCRIPTVAR | STK_PROPERTY_IS_STACKVAR)){
								var_object = (CScriptVariable *)(((tStackElement *)ptrResultInstructionOp1->varRef)->varRef);
							}

							if(var_object != NULL){
								if(var_object->idxScriptClass == IDX_CLASS_VECTOR){

									if(IS_INT(ptrResultInstructionOp2->properties)){
										// determine object ...
										CVectorScriptVariable * vec = (CVectorScriptVariable *)var_object;
										int v_index = LOAD_INT_OP(ptrResultInstructionOp2);

										// check indexes ...
										if(v_index < 0){
											writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Negative index vector (%i)",v_index);
											RETURN_ERROR;
										}

										if(v_index >= (int)(vec->m_objVector.size())){
											writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Index vector out of bounds (%i)",v_index);
											RETURN_ERROR;
										}

										ldrVar = &vec->m_objVector[v_index];;
										ok = true;
									}else{
										writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Expected vector-index as integer");
										RETURN_ERROR;
									}
								}
							}

							if(!ok){
								writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Variable \"%s\" is not type vector",
									AST_SYMBOL_VALUE_CONST_CHAR((*current_statment)[instruction->index_op1].idxAstNode)
								);
								RETURN_ERROR;
							}

						}else{ // load variable ...

							instruction_properties=instruction->instruction_properties;
							scope_type=GET_INS_PROPERTY_SCOPE_TYPE(instruction_properties);

							switch(scope_type){
							default: // global...
								ldrVar = &stack[instruction->index_op2];
								break;
							case INS_PROPERTY_ACCESS_SCOPE:
							case INS_PROPERTY_THIS_SCOPE:
								if(instruction->idxAstNode != -1){
									ast = vec_ast_node[instruction->idxAstNode];

								}

								if(scope_type == INS_PROPERTY_ACCESS_SCOPE){
									POP_ONE; // get var op1 and symbol op2

									if((ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_SCRIPTVAR)!= STK_PROPERTY_TYPE_SCRIPTVAR)
									{

										PASTNode ast=AST_NODE(instruction->idxAstNode);
										PASTNode parent = AST_NODE(ast->idxAstParent);
										ast=AST_NODE(parent->children[0]);  // get left node ... is the object ancestor.
										writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"var \"%s\" is not scriptvariable",ast->symbol_value.c_str());
										RETURN_ERROR;
									}


									CScriptVariable  * base_var = (CScriptVariable  *)ptrResultInstructionOp1->varRef;
									if(ptrResultInstructionOp1->properties & STK_PROPERTY_IS_STACKVAR) {
										tStackElement *stk_ins=((tStackElement *)ptrResultInstructionOp1->varRef);

										if(stk_ins->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
											base_var=((CScriptVariable *)stk_ins->varRef);
										}
									}

									if(base_var == NULL)
									{
										PASTNode ast=AST_NODE(instruction->idxAstNode);
										PASTNode parent = AST_NODE(ast->idxAstParent);
										ast=AST_NODE(parent->children[0]); // get left node ... is the object ancestor.
										writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"var \"%s\" is not scriptvariable",ast->symbol_value.c_str());

										RETURN_ERROR;
									}

									if((si = base_var->getVariableSymbol(ast->symbol_value,true))==NULL){

										tInfoAsmOp *previous= (instruction-1);
										string parent_symbol="unknow";

										if(previous->idxAstNode != -1){
											PASTNode aa= AST_NODE(previous->idxAstNode);
											parent_symbol=aa->symbol_value;
										}

										writeErrorMsg(GET_AST_FILENAME_LINE(ast->idxAstNode),"Variable \"%s\" as type \"%s\" has not symbol \"%s\"",parent_symbol.c_str(),base_var->getClassName().c_str(), ast->symbol_value.c_str());
										RETURN_ERROR;
									}
								}
								else{ // this scope ...
									if((si = this_object->getVariableSymbolByIndex(instruction->index_op2))==NULL){
										writeErrorMsg(GET_AST_FILENAME_LINE(ast->idxAstNode),"cannot find symbol \"this.%s\"",ast->symbol_value.c_str());
										RETURN_ERROR;
									}
								}
								ldrVar=&si->object;
								break;
							case INS_PROPERTY_LOCAL_SCOPE:
								ldrVar = &ptrLocalVar[instruction->index_op2];
								break;
							}

							if(instruction->instruction_properties&INS_CHECK_IS_FUNCTION){
								if((ldrVar->properties & STK_PROPERTY_TYPE_FUNCTION) != STK_PROPERTY_TYPE_FUNCTION){
									writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"\"%s\" expected function variable but is \"%s\"",AST_NODE(instruction->idxAstNode)->symbol_value.c_str(), STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ldrVar));
									RETURN_ERROR;
								}
							}
						}

						pre_post_properties = GET_INS_PROPERTY_PRE_POST_OP(instruction_properties);


						// all preoperators makes load var as constant ...
						switch(pre_post_properties){
						case INS_PROPERTY_PRE_INC:
								PERFORM_PRE_POST_OPERATOR(ldrVar,++);
								(*ptrCurrentOp++)=*ldrVar;
								continue;
						case INS_PROPERTY_PRE_DEC:
								PERFORM_PRE_POST_OPERATOR(ldrVar,--);
								(*ptrCurrentOp++)=*ldrVar;
								continue;
						case INS_PROPERTY_POST_DEC:
								(*ptrCurrentOp++)=*ldrVar;
								PERFORM_PRE_POST_OPERATOR(ldrVar,--);
								continue;
						case INS_PROPERTY_POST_INC:
								(*ptrCurrentOp++)=*ldrVar;
								PERFORM_PRE_POST_OPERATOR(ldrVar,++);
								continue;
						case INS_PROPERTY_PRE_NEG:
								switch(GET_INS_PROPERTY_VAR_TYPE(ldrVar->properties)){
								case STK_PROPERTY_TYPE_INTEGER:
									if(ldrVar->properties& STK_PROPERTY_IS_C_VAR){
										*ptrCurrentOp++={STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_IS_STACKVAR|STK_PROPERTY_IS_C_VAR,(void *)(-(*((intptr_t *)ldrVar->varRef))),ldrVar};
									}else{
										*ptrCurrentOp++={STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_IS_STACKVAR,(void *)(-(((intptr_t)ldrVar->stkValue))),ldrVar};
									}
									break;
								case STK_PROPERTY_TYPE_BOOLEAN:
									if(ldrVar->properties& STK_PROPERTY_IS_C_VAR){
										*ptrCurrentOp++={STK_PROPERTY_TYPE_BOOLEAN|STK_PROPERTY_IS_STACKVAR|STK_PROPERTY_IS_C_VAR,(void *)(!(*((bool *)ldrVar->varRef))),ldrVar};
									}else{
										*ptrCurrentOp++={STK_PROPERTY_TYPE_BOOLEAN|STK_PROPERTY_IS_STACKVAR,(void *)(!(((bool)ldrVar->stkValue))),ldrVar};
									}
									break;

								case STK_PROPERTY_TYPE_NUMBER:
									if(ldrVar->properties& STK_PROPERTY_IS_C_VAR){
										COPY_NUMBER(&aux_float,ldrVar->varRef);
									}else{
										COPY_NUMBER(&aux_float,&ldrVar->stkValue);
									}
									aux_float=-aux_float;
									COPY_NUMBER(&ptrCurrentOp->stkValue,&aux_float);
									*ptrCurrentOp={STK_PROPERTY_TYPE_NUMBER|STK_PROPERTY_IS_STACKVAR,ptrCurrentOp->stkValue,ldrVar};

									if(ldrVar->properties& STK_PROPERTY_IS_C_VAR){
										ptrCurrentOp->properties|=STK_PROPERTY_IS_C_VAR;
									}

									ptrCurrentOp++;
									break;
								default:
									writeErrorMsg(GET_AST_FILENAME_LINE(ast->idxAstNode),"internal error:cannot perform pre operator - because is not number");
									RETURN_ERROR;
								}
								continue;
						default:

							// update var if needed ...
							PUSH_STACK_VAR(ldrVar);
							break;
						}

						// ok in case is C we must udpate stkValue becaus it can be updated from C++. (only primitives)
						if(ldrVar->properties & STK_PROPERTY_IS_C_VAR){
							switch(GET_INS_PROPERTY_VAR_TYPE(ldrVar->properties)){
							case STK_PROPERTY_TYPE_INTEGER:
								//(ptrCurrentOp-1)->stkValue=(void *)((*((int *)ldrVar->varRef)));
								memcpy(&((ptrCurrentOp-1)->stkValue),ldrVar->varRef,sizeof(int));
								break;
							case STK_PROPERTY_TYPE_NUMBER:
								COPY_NUMBER(&((ptrCurrentOp-1)->stkValue),ldrVar->varRef);
								break;
							case STK_PROPERTY_TYPE_BOOLEAN:
								(ptrCurrentOp-1)->stkValue=(void *)((*((bool *)ldrVar->varRef)));
								break;
							}
						}



						continue;

					}else if(index_op1==LOAD_TYPE::LOAD_TYPE_NULL){
						PUSH_NULL;
						continue;
					}else if(index_op1==LOAD_TYPE::LOAD_TYPE_UNDEFINED){
						PUSH_UNDEFINED;
						continue;
					}else if(index_op1==LOAD_TYPE::LOAD_TYPE_CONSTANT){
						(*ptrCurrentOp)=*(((CCompiler::tInfoConstantValue *)instruction->index_op2));

						pre_post_properties = GET_INS_PROPERTY_PRE_POST_OP(instruction->instruction_properties);

						// all preoperators makes load var as constant ...
						switch(pre_post_properties){
						case INS_PROPERTY_PRE_NEG:
								switch(GET_INS_PROPERTY_VAR_TYPE(ptrCurrentOp->properties)){
								case STK_PROPERTY_TYPE_INTEGER:
									ptrCurrentOp->stkValue=(void *)(-((intptr_t)ptrCurrentOp->stkValue));
									break;
								case STK_PROPERTY_TYPE_BOOLEAN:
									ptrCurrentOp->stkValue=(void *)(!((bool)ptrCurrentOp->stkValue));
									break;
								case STK_PROPERTY_TYPE_NUMBER:
									COPY_NUMBER(&aux_float,&ptrCurrentOp->stkValue);
									aux_float=-aux_float;
									COPY_NUMBER(&ptrCurrentOp->stkValue,&aux_float);
									break;
								default:
									writeErrorMsg(GET_AST_FILENAME_LINE(ast->idxAstNode),"internal error:cannot perform pre operator - constant because is not numeric");
									RETURN_ERROR;
								}
								break;
						}
						ptrCurrentOp++;
						continue;
					}else if(index_op1== LOAD_TYPE::LOAD_TYPE_FUNCTION){

						unsigned short extra_flags=(instruction->instruction_properties&INS_PROPERTY_CONSTRUCT_CALL)?STK_PROPERTY_CONSTRUCTOR_FUNCTION:0;
						extra_flags|=(instruction->index_op2==ZS_FUNCTION_NOT_FOUND_IDX) ?STK_PROPERTY_UNRESOLVED_FUNCTION:0;
						void *function_obj=NULL;
						vector<int> *vec_functions;
						CScriptVariable * class_obj=NULL;
						int index_op2 = (int)instruction->index_op2;
						instruction_properties=instruction->instruction_properties;
						scope_type=GET_INS_PROPERTY_SCOPE_TYPE(instruction_properties);

						if(scope_type==INS_PROPERTY_LOCAL_SCOPE){ // local gets functions from info_function ...
							vec_functions=&info_function->object_info.local_symbols.vec_idx_registeredFunction;
						}else if(scope_type == INS_PROPERTY_ACCESS_SCOPE){
							tStackElement *var=NULL;
							if(instruction_properties & INS_PROPERTY_CONSTRUCT_CALL){
								var=(ptrCurrentOp-1);
							}else{
								POP_ONE;
								var = ptrResultInstructionOp1;
							}

							tStackElement *stk_ins=NULL;
							if(var->properties & STK_PROPERTY_IS_STACKVAR) {
								stk_ins=((tStackElement *)var->varRef);
							}
							else{
								stk_ins=var;
							}

							if(stk_ins->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
								class_obj=(CScriptVariable *)(stk_ins->varRef);
								CScriptClass *sc = CScriptClass::getScriptClassByIdx(((CScriptVariable *)class_obj)->idxScriptClass);
								vec_functions=&sc->metadata_info.object_info.local_symbols.vec_idx_registeredFunction;
							}
							else{
								CASTNode *ast_previous=vec_ast_node[(instruction-1)->idxAstNode];
								CASTNode *ast=vec_ast_node[(int)instruction->idxAstNode];
								writeErrorMsg(GET_AST_FILENAME_LINE(ast->idxAstNode),"Cannot access symbol \"%s\" (type of %s is %s)",ast->symbol_value.c_str(),ast_previous->symbol_value.c_str(),STR_GET_TYPE_VAR_INDEX_INSTRUCTION(stk_ins));
								RETURN_ERROR;
							}
						}else if(scope_type ==INS_PROPERTY_THIS_SCOPE){
							if((si = this_object->getFunctionSymbolByIndex(index_op2))==NULL){
								writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"cannot find function \"this.%s\"",vec_ast_node[instruction->idxAstNode]->symbol_value.c_str());
								RETURN_ERROR;
							}

							function_obj =(CScriptFunctionObject *)si->object.stkValue;

						}else if(scope_type == INS_PROPERTY_SUPER_SCOPE){ // super scope ?
							if((si = this_object->getFunctionSymbolByIndex(index_op2))==NULL){
								writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"cannot find function \"super.%s\"",vec_ast_node[instruction->idxAstNode]->symbol_value.c_str());
								RETURN_ERROR;
							}
							function_obj =(CScriptFunctionObject *)si->object.stkValue;
						}else{ // global
							vec_functions = &(main_function_object->object_info.local_symbols.vec_idx_registeredFunction);
							//function_obj = GET_SCRIPT_FUNCTION_OBJECT(info_function->object_info.local_symbols.vec_idx_registeredFunction[index_op2]);
						}


						if(!(scope_type == INS_PROPERTY_THIS_SCOPE || scope_type == INS_PROPERTY_SUPER_SCOPE)){

							if(index_op2 == ZS_UNDEFINED_IDX || index_op2 == ZS_FUNCTION_NOT_FOUND_IDX){ // is will be processed after in CALL instruction ...
								//function_properties=INS_PROPERTY_UNRESOLVED_FUNCTION;
								function_obj= NULL;//(void *)instruction; // saves current instruction in order to resolve its idx later (in call instruction)
							}else if((index_op2<(int)vec_functions->size())) // get the function ...
							{
								function_obj = vec_script_function_object_node[(*vec_functions)[index_op2]];
							}
							else{
								writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"cannot find symbol global \"%s\"",vec_ast_node[instruction->idxAstNode]->symbol_value.c_str());
								RETURN_ERROR;
							}
						}

						if(index_op2 == ZS_UNDEFINED_IDX){
							*ptrCurrentOp++={(unsigned short)(STK_PROPERTY_IS_INSTRUCTIONVAR|STK_PROPERTY_TYPE_FUNCTION|extra_flags),instruction,class_obj};
						}else{
							PUSH_FUNCTION(extra_flags,function_obj,class_obj);
						}
						continue;

					}else if(index_op1== LOAD_TYPE::LOAD_TYPE_ARGUMENT){
						//ldrVar=&ptrArg[instruction->index_op2];
						*ptrCurrentOp++=ptrArg[instruction->index_op2]; // copy arg directly ...
						continue;
					}
					else{

						writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"runtime error. Try to restart");
						RETURN_ERROR;

					}
					continue;

				case STORE:
				case VPUSH:
				case PUSH_ATTR:

					{
						bool assign_metamethod=false;
						bool push_value=true;

						if(operator_type==VPUSH){
							POP_ONE; // only pops the value, the last is the vector variable itself
							CScriptVariable *vec_obj = NULL;
							if((ptrCurrentOp-1)->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
								vec_obj = (CScriptVariable *)(ptrCurrentOp-1)->varRef;
								if(vec_obj->idxScriptClass == IDX_CLASS_VECTOR){ // push value ...
									// op1 is now the src value ...
									src_ins=ptrResultInstructionOp1;
									dst_ins=((CVectorScriptVariable *)vec_obj)->push();
									ok=true;
								}
							}

							if(!ok){
								writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Expected vector object");
								RETURN_ERROR;
							}

							push_value=false;

						}else if(operator_type==PUSH_ATTR){


							POP_TWO; // first must be the value name and the other the variable name ...
							CScriptVariable *struct_obj = NULL;
							if((ptrCurrentOp-1)->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
								struct_obj = (CScriptVariable *)(ptrCurrentOp-1)->varRef;
								if(struct_obj->idxScriptClass == IDX_CLASS_STRUCT){ // push value ...
									// op1 is now the src value ...
									if(ptrResultInstructionOp2->properties & STK_PROPERTY_TYPE_STRING){
										tSymbolInfo *si=NULL;
										const char *str = (const char *)ptrResultInstructionOp2->stkValue;
										src_ins=ptrResultInstructionOp1;
										if(src_ins->properties&STK_PROPERTY_TYPE_FUNCTION){
											si =((CStructScriptVariable *)struct_obj)->addFunctionSymbol(str, -1,NULL, false );
										}else{
											si =((CStructScriptVariable *)struct_obj)->addVariableSymbol(str, -1,NULL );
										}

										if(si == NULL){
											RETURN_ERROR;
										}

										dst_ins=&si->object;
										ok=true;
									}
									else{
										writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"internal error (operator2 is not string)");
										RETURN_ERROR;
									}
								}else{
									writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Expected struct object");
									RETURN_ERROR;
								}
							}else{
								writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Expected scriptvar");
								RETURN_ERROR;
							}
							push_value=false;
						}
						else{ // pop two parameters nothing ...
							POP_TWO;


							if(ptrResultInstructionOp1->properties & STK_PROPERTY_IS_STACKVAR) {// == CScriptVariable::VAR_TYPE::OBJECT){
								dst_ins=(tStackElement *)ptrResultInstructionOp1->varRef; // stkValue is expect to contents a stack variable
							}else{
								writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Expected object l-value mov");
								RETURN_ERROR;
							}

							src_ins=ptrResultInstructionOp2; // store ptr instruction2 op as src_var_value

							// we need primitive stackelement in order to assign...
							if(src_ins->properties & STK_PROPERTY_IS_STACKVAR) {// == CScriptVariable::VAR_TYPE::OBJECT){
								src_ins=(tStackElement *)src_ins->varRef; // stkValue is expect to contents a stack variable

							}



							// ok load object pointer ...
							if(dst_ins->properties & STK_PROPERTY_TYPE_SCRIPTVAR){

								if(((CScriptVariable *)dst_ins->varRef)->itHasSetMetamethod()){
									APPLY_METAMETHOD(=,SET_METAMETHOD);
									assign_metamethod=true;
								}
							}

						}



						if(! assign_metamethod){

							if(dst_ins->properties & STK_PROPERTY_IS_THIS_VAR){
								writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"\"this\" is not assignable");
								RETURN_ERROR;
							}

							tStackElement old_dst_ins = *dst_ins; // save dst_var to check after assignment...

							ASSIGN_STACK_VAR(dst_ins,src_ins);



							// check old var structure ...
							switch(GET_INS_PROPERTY_VAR_TYPE(old_dst_ins.properties)){
							case STK_PROPERTY_TYPE_NULL:
							case STK_PROPERTY_TYPE_UNDEFINED:
							case STK_PROPERTY_TYPE_INTEGER:
							case STK_PROPERTY_TYPE_NUMBER:
							case STK_PROPERTY_TYPE_BOOLEAN:
							case STK_PROPERTY_TYPE_FUNCTION: // we aren't take care about nothing! :)
								break;
							case STK_PROPERTY_TYPE_STRING: // type string is really a string or variable ?!?!
							case STK_PROPERTY_TYPE_SCRIPTVAR: // we are getting script vars ...
								if(!(((old_dst_ins.properties & (STK_PROPERTY_TYPE_STRING | STK_PROPERTY_IS_C_VAR))==(STK_PROPERTY_TYPE_STRING | STK_PROPERTY_IS_C_VAR)))){
									if(old_dst_ins.varRef!=NULL){ // it had a pointer (no constant)...
										if(old_dst_ins.varRef != dst_ins->varRef){ // unref pointer because new pointer has been attached...
											unrefSharedScriptVar(((CScriptVariable  *)old_dst_ins.varRef)->ptr_shared_pointer_node);
										}
									}
								}
								break;
							}

						}

						if(push_value){ // to be able to do multiple assigns like a=b=c=1 (1 will be pushed in each store instruction)
							*ptrCurrentOp++=*src_ins;
						}

					}

					continue;

				case EQU:  // ==

					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE;
					}else{
						POP_TWO;
					}

					PROCESS_COMPARE_OPERATION(==, EQU_METAMETHOD);
					continue;
				case NOT_EQU:  // !=

					POP_TWO;
					PROCESS_COMPARE_OPERATION(!=, NOT_EQU_METAMETHOD);
					continue;

				case LT:  // <
					POP_TWO;
					PROCESS_COMPARE_OPERATION(<, LT_METAMETHOD);
					continue;
				case LTE:  // <=
					POP_TWO;
					PROCESS_COMPARE_OPERATION(<=, LTE_METAMETHOD);
					continue;
				case GT:  // >

					POP_TWO;
					PROCESS_COMPARE_OPERATION(>,GT_METAMETHOD);
					continue;
				case GTE:  // >=
					POP_TWO;
					PROCESS_COMPARE_OPERATION(>=,GTE_METAMETHOD);
					continue;
				case LOGIC_AND:  // &&
					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE;
					}else{
						POP_TWO;
					}

					PROCESS_LOGIC_OPERATION(&&);
					continue;
				case LOGIC_OR:  // ||

					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE;
					}else{
						POP_TWO;
					}
					PROCESS_LOGIC_OPERATION(||);
					continue;
				case NOT: // !

					POP_ONE;

					if(ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_BOOLEAN){ // operation will result as integer.
						PUSH_BOOLEAN((!((bool)(ptrResultInstructionOp1->stkValue))));
					}else{
						APPLY_METAMETHOD(!,NOT_METAMETHOD);
							//writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Expected operands 1 as boolean!");
							RETURN_ERROR;
					}
					continue;

				case NEG: // -

					POP_ONE;

					if(ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_INTEGER){ // operation will result as integer.
						PUSH_INTEGER((-((intptr_t)(ptrResultInstructionOp1->stkValue))));
					}else if(ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_NUMBER){
						COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);
						PUSH_NUMBER(-f_aux_value1);
					/*}else if(ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_BOOLEAN){
						PUSH_BOOLEAN((!((bool)(ptrResultInstructionOp1->stkValue))));
					*/
					}else{ // try metamethod ...
							APPLY_METAMETHOD(-,NEG_METAMETHOD);
							//#define APPLY_METAMETHOD(__OVERR_OP__, __METAMETHOD__)
					}
					continue;

				case ADD: // +

				{
					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE;
					}else{
						POP_TWO;
					}


					unsigned short mask_properties =GET_INS_PROPERTY_PRIMITIVE_TYPES(ptrResultInstructionOp1->properties&ptrResultInstructionOp2->properties);
					unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);
					if(mask_properties==STK_PROPERTY_TYPE_INTEGER){
							PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) + LOAD_INT_OP(ptrResultInstructionOp2));
					}
					else if(properties==(STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_TYPE_NUMBER)){
							if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
								COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);
								PUSH_NUMBER(LOAD_INT_OP(ptrResultInstructionOp1) + f_aux_value2);
							}else{
								COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);
								PUSH_NUMBER(f_aux_value1 + LOAD_INT_OP(ptrResultInstructionOp2));
							}
					}
					else if(properties==(STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_TYPE_STRING)){
						if (IS_STRING(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
							sprintf(str_aux,"%s%i",((const char *)ptrResultInstructionOp1->stkValue),(int)((intptr_t)ptrResultInstructionOp2->stkValue));
							//PUSH_STRING(str_aux);
							if(ptrCurrentStr==ptrLastStr){writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Error stkString out-stack");RETURN_ERROR;}\
									*ptrCurrentStr++=str_aux;\
									*ptrCurrentOp++={STK_PROPERTY_TYPE_STRING,(void *)((ptrCurrentStr-1)->c_str()),NULL};\

						}else{
							sprintf(str_aux,"%i%s",(int)((intptr_t)ptrResultInstructionOp1->stkValue),((const char *)ptrResultInstructionOp2->stkValue));
							PUSH_STRING(str_aux);
						}
					}else if(properties==(STK_PROPERTY_TYPE_NUMBER|STK_PROPERTY_TYPE_STRING)){
						if (IS_STRING(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
							COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);
							sprintf(str_aux,"%s%f",((const char *)ptrResultInstructionOp1->stkValue),f_aux_value2);
							PUSH_STRING(str_aux);

						}else{
							COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);
							sprintf(str_aux,"%f%s",f_aux_value1,((const char *)ptrResultInstructionOp2->stkValue));
							PUSH_STRING(str_aux);
						}
					}else if(properties==(STK_PROPERTY_TYPE_UNDEFINED|STK_PROPERTY_TYPE_STRING)){
						if (IS_STRING(ptrResultInstructionOp1->properties) && IS_UNDEFINED(ptrResultInstructionOp2->properties)){
							sprintf(str_aux,"%s%s",((const char *)ptrResultInstructionOp1->stkValue),"undefined");
							PUSH_STRING(str_aux);
						}else{
							sprintf(str_aux,"%s%s","undefined",((const char *)ptrResultInstructionOp2->stkValue));
							PUSH_STRING(str_aux);
						}

					}else if(properties==(STK_PROPERTY_TYPE_BOOLEAN|STK_PROPERTY_TYPE_STRING)){
						if (IS_STRING(ptrResultInstructionOp1->properties) && IS_BOOLEAN(ptrResultInstructionOp2->properties)){
							sprintf(str_aux,"%s%s",((const char *)ptrResultInstructionOp1->stkValue),((bool)(ptrResultInstructionOp2->stkValue))?"true":"false");
							PUSH_STRING(str_aux);
						}else{
							sprintf(str_aux,"%s%s",((bool)(ptrResultInstructionOp1->stkValue))?"true":"false",((const char *)ptrResultInstructionOp2->stkValue));
							PUSH_STRING(str_aux);
						}

					}else if (mask_properties== STK_PROPERTY_TYPE_STRING){
							sprintf(str_aux,"%s%s",((const char *)ptrResultInstructionOp1->stkValue),((const char *)(ptrResultInstructionOp2->stkValue)));
							PUSH_STRING(str_aux);


					}else if(mask_properties== STK_PROPERTY_TYPE_NUMBER){
						COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
						COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
						PUSH_NUMBER(f_aux_value1 + f_aux_value2);\

					}
					else{ // try metamethod ...

						APPLY_METAMETHOD(+,ADD_METAMETHOD);
					}
				}
					continue;

				case MUL: // *

					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE
					}else{
						POP_TWO;
					}

					PROCESS_ARITHMETIC_OPERATION(*,MUL_METAMETHOD);
					continue;

				case DIV: // /

					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE
					}else{
						POP_TWO;
					}

					PROCESS_ARITHMETIC_OPERATION(/, DIV_METAMETHOD);
					continue;

			 	 case MOD: // /

					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE
					}else{
						POP_TWO;
					}

					PROCESS_MOD_OPERATION;
					continue;

			 	 case AND: // &
					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE
					}else{
						POP_TWO;
					}

					PROCESS_BINARY_OPERATION(&, AND_METAMETHOD);
					continue;
			 	 case OR: // *
					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE
					}else{
						POP_TWO;
					}

					PROCESS_BINARY_OPERATION(|, OR_METAMETHOD);
					continue;
			 	 case XOR: // ^
					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE
					}else{
						POP_TWO;
					}

					PROCESS_BINARY_OPERATION(^, XOR_METAMETHOD);
					continue;

			 	 case SHR: // >>
					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE
					}else{
						POP_TWO;
					}

					PROCESS_BINARY_OPERATION(>>,SHR_METAMETHOD);
					continue;

			 	 case SHL: // <<
					if(instruction->instruction_properties&STK_PROPERTY_READ_TWO_POP_ONE){
						READ_TWO_POP_ONE
					}else{
						POP_TWO;
					}

					PROCESS_BINARY_OPERATION(<<, SHL_METAMETHOD);
					continue;
			 	 case JMP:
					CHK_JMP;
					continue;
			 	 case INSTANCEOF: // check instance of ...
			 		 POP_TWO;

			 		if(ptrResultInstructionOp1->properties & STK_PROPERTY_IS_STACKVAR) {// == CScriptVariable::VAR_TYPE::OBJECT){
			 			ptrResultInstructionOp1=(tStackElement *)ptrResultInstructionOp1->varRef; // stkValue is expect to contents a stack variable
					}

			 		switch((intptr_t)ptrResultInstructionOp2->stkValue){
						case IDX_CLASS_INT_PTR_C:
							PUSH_BOOLEAN((ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_INTEGER)!=0);
							break;
						case IDX_CLASS_FLOAT_PTR_C:
							PUSH_BOOLEAN((ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_NUMBER)!=0);
							break;
						case IDX_CLASS_BOOL_PTR_C:
							PUSH_BOOLEAN((ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_BOOLEAN)!=0);
							break;
						case IDX_CLASS_STRING_PTR_C:
							PUSH_BOOLEAN((ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_STRING)!=0);
							break;
						case IDX_CLASS_FUNCTOR:
							PUSH_BOOLEAN((ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_FUNCTION)!=0);
							break;
						default:
							if(ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
								bool b = CScriptClass::isIdxClassInstanceOf(((CScriptVariable *)(ptrResultInstructionOp1->varRef))->idxScriptClass, (intptr_t)ptrResultInstructionOp2->stkValue);
								PUSH_BOOLEAN(b);
							}else{
								PUSH_BOOLEAN(false);
							}
							break;
						}


			 		 continue;
			 	 case JNT: // goto if not true ... goes end to conditional.
					POP_ONE;
					if(ptrResultInstructionOp1->stkValue == 0){
						CHK_JMP;
					}
					continue;
			 	 case JT: // goto if true ... goes end to conditional.
					POP_ONE;
					if(ptrResultInstructionOp1->stkValue != 0){
						CHK_JMP;
					}
					continue;
			 	 case  CALL: // calling function after all of args are processed...
					// check whether signatures matches or not ...
					// 1. get function object ...
			 	 {
					aux_function_info = NULL;
					unsigned char n_args=0;//iao->instruction_properties;


					bool is_c = false;
					tStackElement *startArg=ptrCurrentOp;
					tStackElement *callAle=NULL;

					while(n_args <= MAX_N_ARGS && (((startArg-1)->properties&STK_PROPERTY_TYPE_FUNCTION)==0)){
						startArg--;
						n_args++;
					}

					callAle = ((startArg-1));
					calling_object = this_object;
					if(callAle->varRef!=NULL){
						calling_object=(CScriptVariable *)callAle->varRef;
					}


					aux_function_info = NULL;//(CScriptFunctionObject *)callAle->stkValue;


					bool is_constructor = (callAle->properties & STK_PROPERTY_CONSTRUCTOR_FUNCTION)!=0;
					//bool deduce_function = false; //(iao->instruction_properties & INS_PROPERTY_DEDUCE_C_CALL)!=0;

					// try to find the function ...
					if(((callAle)->properties & STK_PROPERTY_IS_INSTRUCTIONVAR)){// || deduce_function){
						tInfoAsmOp *iao = (tInfoAsmOp *)(callAle)->stkValue;
						PASTNode ast_node_call_ale = vec_ast_node[iao->idxAstNode];

						symbol_to_find = ast_node_call_ale->symbol_value;

						//tInfoAsmOp *iao = &(*current_statment)[instruction->index_op1];
						unsigned short scope_type = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);


						// local vars as functions ...
						vec_global_functions=&(main_function_object->object_info.local_symbols.vec_idx_registeredFunction);

						int size_fun_vec = (int)vec_global_functions->size()-1;

						// registered symbols in case is INS_PROPERTY_ACCESS_SCOPE...
						vector<tSymbolInfo> *m_functionSymbol=NULL;
						if(scope_type==INS_PROPERTY_ACCESS_SCOPE){
							calling_object = (CScriptVariable *)callAle->varRef;

							// we have to no to call default constructor...is implicit
							if(is_constructor) {

								is_c=calling_object->is_c_object();

								if(n_args == 0 && is_c){
									aux_function_info = NULL;
									iao->index_op2 = ZS_FUNCTION_NOT_FOUND_IDX;
								}
							}

							m_functionSymbol=calling_object->getVectorFunctionSymbol();

							size_fun_vec = (int)m_functionSymbol->size()-1;

						}

						//bool all_check=true;
						if(iao->index_op2 != ZS_FUNCTION_NOT_FOUND_IDX)
						{
							//#define FIND_FUNCTION(iao, is_constructor, symbol_to_find,size_fun_vec,vec_global_functions,startArgs, n_args,scope_type)
							FIND_FUNCTION(
									m_functionSymbol
									,iao
									,is_constructor
									,symbol_to_find
									,size_fun_vec
									,vec_global_functions
									,startArg
									,n_args
									,NULL);


							// saves function pointer found ...
							callAle->stkValue=aux_function_info;

						}

					}
					else{
						if(((callAle)->properties & STK_PROPERTY_UNRESOLVED_FUNCTION)==0){
							aux_function_info=(CScriptFunctionObject *) (callAle->stkValue);
						}
					}

					if(aux_function_info !=NULL)
					{
						if(n_args > MAX_N_ARGS){
							writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"Max arguments reached function at line XXX");
							RETURN_ERROR;
						}

						if((aux_function_info->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == 0){ // is function script ...
							unsigned aux_function_info_m_arg_size=aux_function_info->m_arg.size();
							if( n_args < aux_function_info_m_arg_size){ // we must push undefined parameters ...
								for(unsigned i = n_args; i < aux_function_info_m_arg_size; i++){
									*ptrCurrentOp++={
										STK_PROPERTY_TYPE_UNDEFINED, // starts undefined.
										0,							 // no value assigned.
										NULL			     // no varref related.
									};
									n_args++;
								}
							}
						}

						// by default virtual machine gets main object class in order to run functions ...
						ret_obj=execute_internal(aux_function_info,calling_object,error,ptrCurrentOp,ptrCurrentStr,n_args,instruction->idxAstNode);

						if(error)
						{
							RETURN_ERROR;
						}

						if(ret_obj.properties & STK_PROPERTY_TYPE_SCRIPTVAR){

							// if c pointer is not from application share ...
							if(!((CScriptVariable *)(ret_obj.varRef))->initSharedPtr()){
								RETURN_ERROR;
							}
						}

						if(cancel_execution) {
							if(custom_error!=NULL){
								writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),custom_error);
							}
							RETURN_ERROR;
						}

					}

					// reset stack (function+asm_op (-1 op less))...
					ptrCurrentOp=startArg-1;

					// ... and push result if not function constructor...
					if(!is_constructor){
						*ptrCurrentOp++ = ret_obj;
					}
			 	 }
					continue;
			 	 case  NEW:
						svar=NEW_CLASS_VAR_BY_IDX(instruction->index_op1);

						if(!svar->initSharedPtr()){
							RETURN_ERROR;
						}
						svar->ast_node_new=instruction->idxAstNode;
						(*ptrCurrentOp++)={STK_PROPERTY_TYPE_SCRIPTVAR,NULL,svar};
						continue;
			 	 case  DELETE_OP:
			 		 	POP_ONE;
			 		 	//svar
						if(ptrResultInstructionOp1->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
							tStackElement *se=ptrResultInstructionOp1;
							if(ptrResultInstructionOp1->properties & STK_PROPERTY_IS_STACKVAR){
								se=(tStackElement *)(ptrResultInstructionOp1->varRef);
							}

							svar = (CScriptVariable *)(se)->varRef;
							if(svar->idxScriptClass >= MAX_BASIC_CLASS_TYPES
							 ||svar->idxScriptClass==IDX_CLASS_VECTOR
							 ||svar->idxScriptClass==IDX_CLASS_STRUCT
							)
							{ // max ...
								svar->unrefSharedPtr();

								if(svar->isCreatedByContructor()){
									svar->setDelete_C_ObjectOnDestroy(true);
								}

								se->stkValue=NULL;
								se->varRef=NULL;
								se->properties=STK_PROPERTY_TYPE_UNDEFINED;

							}
						}
						else{
							writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"delete op: expected scriptvar var but it was \"%s\"",STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp1));
							RETURN_ERROR;
						}
						continue;
			 	 case DECL_VEC: // Create new vector object...
						svar=NEW_VECTOR_VAR;
						//PUSH_VAR(svar,NULL,0,false);

						if(!svar->initSharedPtr()){
							RETURN_ERROR;
						}

						(*ptrCurrentOp++)={STK_PROPERTY_TYPE_SCRIPTVAR,NULL,svar};

						continue;

			 	 case  DECL_STRUCT: // Create new vector object...
					svar=NEW_STRUCT_VAR;
					//PUSH_VAR(svar,NULL,0,false);

					if(!svar->initSharedPtr()){
						RETURN_ERROR;
					}

					(*ptrCurrentOp++)={STK_PROPERTY_TYPE_SCRIPTVAR,NULL,svar};

					continue;


			 	 case  RET:

			 		 if(instruction->index_op1 != 0xff){ // return a value, void else.
						callc_result=*(ptrCurrentOp-1);

						// remove shared pointer if scriptvar ...
						if(callc_result.properties & STK_PROPERTY_TYPE_SCRIPTVAR){
							if(callc_result.properties & STK_PROPERTY_IS_STACKVAR){
								callc_result=*((tStackElement *)((tStackElement *)callc_result.varRef));

							}

							// unref pointer to be deallocated from gc...
							//((CScriptVariable *)callc_result.varRef)->ptr_shared_pointer_node->data.shared_ptr=NULL;
							((CScriptVariable *)callc_result.varRef)->ptr_shared_pointer_node=NULL;
							// share pointer  + 1
						}
			 		 }
					goto lbl_exit_function;
			 case PUSH_SCOPE:

					PUSH_SCOPE(instruction->index_op2,info_function,ptrLocalVar);//instruction->index_op1);
					continue;

			 case POP_SCOPE:
					POP_SCOPE(NULL);//instruction->index_op1);
				 	continue;

				//
				// END OPERATOR MANAGEMENT
				//
				//-----------------------------------------------------------------------------------------------------------------------

				}

				writeErrorMsg(GET_AST_FILENAME_LINE(instruction->idxAstNode),"operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
				RETURN_ERROR;


			 }




	// exit statment (don't remove ;, it gives a compile error)
	lbl_exit_statment:;
		}

	lbl_exit_function:


		//=========================
		// POP STACK
		if(info_function->object_info.idxScriptFunctionObject == 0){ // if main function only remove 0s and preserve variables!)
			REMOVE_0_SHARED_POINTERS(idxCurrentStack,NULL);
		}
		else{
			while(ptrStartScopeInfo<=(current_scope_info_ptr)){
				POP_SCOPE(callc_result.varRef);
			}
		}

		idxCurrentStack--;
		// POP STACK
		//=========================


		//popStack();
		return callc_result;
	}


	tStackElement  * CVirtualMachine::getLastStackValue(){
		return (ptrCurrentOp-1);
	}


	CVirtualMachine::~CVirtualMachine(){
		destroyCache();
	}
}
