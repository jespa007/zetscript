#include "../../CZetScript.h"

#define MAX_METAMETHOD_ARGS 2

namespace zetscript{
//CVirtualMachine * CVirtualMachine::m_virtualMachine = NULL;
//vector<CVirtualMachine::CVirtualMachine> CVirtualMachine::ALE;
// static: only defined in this module...

	#define COPY_NUMBER(d,s)  memcpy((d),(s),sizeof(float))


	// general
	#define PRINT_DUAL_ERROR_OP(c)\
	string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp1),\
		   var_type2=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp2);\
	\
	zs_print_error_cr("Error at line %i cannot perform operator \"%s\" %s  \"%s\"",\
			AST_LINE_VALUE(instruction->idxAstNode),\
			var_type1.c_str(),\
			STR(c),\
			var_type2.c_str());\
			return NULL;



	#define LOAD_INT_OP(ptr_result_instruction) \
			(((intptr_t)(ptr_result_instruction->stkValue)))




	#define LOAD_BOOL_OP(ptr_result_instruction) \
			(((bool)(ptr_result_instruction->stkValue)))


	#define LOAD_STRING_OP(ptr_result_instruction) \
			*(((string *)(ptr_result_instruction->stkValue)))



	// Check types
	#define IS_NUMBER(properties) \
	(properties & INS_PROPERTY_TYPE_NUMBER)


	#define IS_INT(properties) \
	(properties & INS_PROPERTY_TYPE_INTEGER)


	#define IS_STRING(properties) \
	(properties & INS_PROPERTY_TYPE_STRING)

	#define IS_BOOLEAN(properties) \
	(properties & INS_PROPERTY_TYPE_BOOLEAN)

	#define IS_UNDEFINED(properties) \
	(properties & INS_PROPERTY_TYPE_UNDEFINED)

	#define IS_FUNCTION(properties) \
	(properties & INS_PROPERTY_TYPE_FUNCTION)

	#define IS_VAR(properties) \
	(properties & INS_PROPERTY_TYPE_SCRIPTVAR)

	#define IS_VECTOR(ptr_result_instruction) \
	(( ptr_result_instruction->properties & INS_PROPERTY_TYPE_SCRIPTVAR) &&\
	 (((CScriptVariable *)(ptr_result_instruction->stkValue))->idxScriptClass==IDX_CLASS_VECTOR))

	#define IS_GENERIC_NUMBER(properties) \
	((properties & INS_PROPERTY_TYPE_INTEGER) ||\
	(properties & INS_PROPERTY_TYPE_NUMBER))


	#define OP1_AND_OP2_ARE_NUMBERS \
	(IS_GENERIC_NUMBER(ptrResultInstructionOp1->properties) && IS_GENERIC_NUMBER(ptrResultInstructionOp2->properties))

	#define OP1_IS_STRING_AND_OP2_IS_NUMBER \
	(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_STRING) && \
	IS_GENERIC_NUMBER(ptrResultInstructionOp2->type_var)

	#define OP1_IS_NUMBER_AND_OP2_IS_STRING \
	(ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_STRING) && \
	IS_GENERIC_NUMBER(ptrResultInstructionOp1->type_var)

	#define OP1_IS_STRING_AND_OP2_IS_BOOLEAN \
	(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_STRING) && \
	(ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_BOOLEAN)


	#define OP1_AND_OP2_ARE_BOOLEANS \
	(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_BOOLEAN) && \
	(ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_BOOLEAN)

	#define OP1_AND_OP2_ARE_STRINGS \
	(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_STRING) && \
	(ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_STRING)

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
	*ptrCurrentOp++={INS_PROPERTY_TYPE_UNDEFINED,0,VM_UNDEFINED};

	#define PUSH_NULL \
	*ptrCurrentOp++={INS_PROPERTY_TYPE_NULL,0,VM_NULL};

	#define PUSH_BOOLEAN(init_value) \
	*ptrCurrentOp++={INS_PROPERTY_TYPE_BOOLEAN,(void *)((int)(init_value)),NULL};

	#define PUSH_INTEGER(init_value) \
	*ptrCurrentOp++={INS_PROPERTY_TYPE_INTEGER,(void *)((int)(init_value)),NULL};

	#define PUSH_NUMBER(init_value) \
	{\
		float aux=(float)(init_value);\
		COPY_NUMBER(&ptrCurrentOp->stkValue,&aux);\
		ptrCurrentOp->properties=INS_PROPERTY_TYPE_NUMBER;\
		ptrCurrentOp++;\
	}

	#define PUSH_STRING(init_value)\
		if(ptrCurrentStr==ptrLastStr){zs_print_error_cr("Error stkString out-stack");return NULL;}\
		*ptrCurrentStr=init_value;\
		*ptrCurrentOp++={INS_PROPERTY_TYPE_STRING,ptrCurrentStr++,NULL};\



	#define PUSH_FUNCTION(function_prop,fun_obj,class_obj) \
	*ptrCurrentOp++={(unsigned short)(INS_PROPERTY_TYPE_FUNCTION|function_prop),(void *)(fun_obj),class_obj};

	#define PUSH_SCRIPTVAR(var_ref) \
	*ptrCurrentOp++={INS_PROPERTY_SCRIPTVAR,NULL,var_ref};



	// Push stack var value (as varValue and put ptr as ref second value)...
	#define PUSH_STACK_VAR(stack_ref_var) \
		*ptrCurrentOp++={(unsigned short)(((stack_ref_var)->properties)|INS_PROPERTY_IS_STACKVAR),(stack_ref_var)->stkValue,stack_ref_var};



	#define PROCESS_MOD_OPERATION \
	{ \
		unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
		if(properties==INS_PROPERTY_TYPE_INTEGER){\
				PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) % LOAD_INT_OP(ptrResultInstructionOp2));\
		}\
		else if(properties==(INS_PROPERTY_TYPE_INTEGER|INS_PROPERTY_TYPE_NUMBER)){\
				if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){\
					COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
					PUSH_NUMBER(fmod(LOAD_INT_OP(ptrResultInstructionOp1) , f_aux_value2));\
				}else{\
					COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
					PUSH_NUMBER(fmod(f_aux_value1 , LOAD_INT_OP(ptrResultInstructionOp2)));\
				}\
		}\
		else if(properties== INS_PROPERTY_TYPE_NUMBER){\
				COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
				COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
				PUSH_NUMBER(fmod(f_aux_value1 , f_aux_value2));\
		}\
		else{\
			APPLY_METAMETHOD(MOD_METAMETHOD);\
			PRINT_DUAL_ERROR_OP(#__OVERR_OP__);\
		}\
	}

	#define PROCESS_ARITHMETIC_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
		if(properties==INS_PROPERTY_TYPE_INTEGER){\
				PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
		}\
		else if(properties==(INS_PROPERTY_TYPE_INTEGER|INS_PROPERTY_TYPE_NUMBER)){\
				if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){\
					COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
					PUSH_NUMBER(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ f_aux_value2);\
				}else{\
					COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
					PUSH_NUMBER(f_aux_value1 __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
				}\
		}\
		else if(properties== INS_PROPERTY_TYPE_NUMBER){\
				COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
				COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
				PUSH_NUMBER(f_aux_value1 __OVERR_OP__ f_aux_value2);\
		}\
		else{\
			APPLY_METAMETHOD(__METAMETHOD__);\
			PRINT_DUAL_ERROR_OP(#__OVERR_OP__);\
		}\
	}

	#define PROCESS_COMPARE_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_VAR_TYPE(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
		if(properties==INS_PROPERTY_TYPE_INTEGER){\
				PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
		}\
		else if(properties==INS_PROPERTY_TYPE_BOOLEAN){\
			PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_BOOL_OP(ptrResultInstructionOp2));\
		}\
		else if(properties==(INS_PROPERTY_TYPE_INTEGER|INS_PROPERTY_TYPE_NUMBER)){\
				if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){\
					COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ f_aux_value2);\
				}else{\
					COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
					PUSH_BOOLEAN(f_aux_value1 __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
				}\
		}\
		else if(properties== INS_PROPERTY_TYPE_NUMBER){\
				COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
				COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
				PUSH_BOOLEAN(f_aux_value1 __OVERR_OP__ f_aux_value2);\
		}\
		else{\
			APPLY_METAMETHOD(__METAMETHOD__);\
			PRINT_DUAL_ERROR_OP(#__OVERR_OP__);\
		}\
	}



	#define PROCESS_LOGIC_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_VAR_TYPE(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
		if(properties==INS_PROPERTY_TYPE_BOOLEAN){\
			PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_BOOL_OP(ptrResultInstructionOp2));\
		}else{\
			APPLY_METAMETHOD(__METAMETHOD__);\
			PRINT_DUAL_ERROR_OP(#__OVERR_OP__);\
		}\
	}

	#define PROCESS_BINARY_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_VAR_TYPE(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
		if(properties==INS_PROPERTY_TYPE_INTEGER){\
			PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
		}else{\
			APPLY_METAMETHOD(__METAMETHOD__);\
			PRINT_DUAL_ERROR_OP(#__OVERR_OP__);\
		}\
	}



	#define PERFORM_PRE_POST_OPERATOR(ldrOp, __OPERATOR__) \
	{\
		void **ref=(void **)(&((ldrOp)->stkValue));\
		if(ldrOp->properties & INS_PROPERTY_IS_C_VAR){\
			ref=(void **)((ldrOp)->stkValue);\
		}\
		switch(GET_INS_PROPERTY_VAR_TYPE((ldrOp)->properties)){\
		case INS_PROPERTY_TYPE_INTEGER:\
				(*((int *)(ref)))__OPERATOR__;\
				break;\
		case INS_PROPERTY_TYPE_NUMBER:\
				(*((float *)(ref)))__OPERATOR__;\
				break;\
		default:\
			zs_print_error_cr("internal error:Cannot perform pre/post operator ?? because is not number");\
			return NULL;\
			break;\
		}\
	}



	//CVirtualMachine::createVarFromResultInstruction(tStackElement * ptr_instruction, bool share_ptr, CScriptVariable * ret)


	#define ASSIGN_STACK_VAR(dst_ins, src_ins) \
	{\
		CScriptVariable *script_var=NULL;\
		void **src_ref=&src_ins->stkValue;\
		void **dst_ref=&dst_ins->stkValue;\
		if(src_ins->properties & INS_PROPERTY_IS_C_VAR){\
			src_ref=(void **)((src_ins)->stkValue);\
		}\
		if(dst_ins->properties & INS_PROPERTY_IS_C_VAR){\
			if(GET_INS_PROPERTY_VAR_TYPE(src_ins->properties) != GET_INS_PROPERTY_VAR_TYPE(dst_ins->properties)\
			){\
				zs_print_info_cr("Primitive type not equal! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals");\
				return NULL;\
			}else{\
				if(\
					(GET_INS_PROPERTY_VAR_TYPE(src_ins->properties) == INS_PROPERTY_TYPE_SCRIPTVAR)\
				){\
						zs_print_info_cr("Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator in order to make the proper operation.");\
						return NULL;\
				}\
			}\
			dst_ref=(void **)((dst_ins)->stkValue);\
		}\
		unsigned short type_var=src_ins->properties;\
		unsigned short runtime_var=GET_INS_PROPERTY_RUNTIME(type_var);\
		if(type_var & INS_PROPERTY_TYPE_UNDEFINED){\
			dst_ins->properties=runtime_var | INS_PROPERTY_TYPE_UNDEFINED;\
		}else if(type_var & INS_PROPERTY_TYPE_UNDEFINED){\
			dst_ins->properties=runtime_var | INS_PROPERTY_TYPE_UNDEFINED;\
		}else if(type_var & INS_PROPERTY_TYPE_INTEGER){\
			dst_ins->properties=runtime_var | INS_PROPERTY_TYPE_INTEGER;\
			*((int *)dst_ref)=*((int *)src_ref);\
		}else if(type_var & INS_PROPERTY_TYPE_NUMBER){\
			dst_ins->properties=runtime_var | INS_PROPERTY_TYPE_NUMBER;\
			*((float *)dst_ref)=*((float *)src_ref);\
		}else if(type_var & INS_PROPERTY_TYPE_BOOLEAN){\
			dst_ins->properties=runtime_var | INS_PROPERTY_TYPE_BOOLEAN;\
			*((int *)dst_ref)=*((int *)src_ref);\
		}else if(type_var  &  INS_PROPERTY_TYPE_FUNCTION){\
			*dst_ins={(unsigned short)(runtime_var | INS_PROPERTY_TYPE_FUNCTION),\
						src_ins->stkValue,\
						NULL};\
		}else if(type_var & INS_PROPERTY_TYPE_STRING){\
			if(((dst_ins->properties & INS_PROPERTY_TYPE_STRING)==0) || (dst_ins->varRef==NULL)){\
				script_var= NEW_STRING_VAR;\
				dst_ins->varRef=script_var;\
				dst_ins->stkValue=&(((CString *)script_var)->m_strValue);\
				dst_ins->properties=runtime_var | INS_PROPERTY_TYPE_STRING | INS_PROPERTY_TYPE_SCRIPTVAR;\
				script_var->initSharedPtr(true);\
			}\
			*((string *)(dst_ins->stkValue))=*((string *)(src_ins->stkValue));\
		}else if(type_var & INS_PROPERTY_TYPE_SCRIPTVAR){\
			script_var=(CScriptVariable *)src_ins->varRef;\
			dst_ins->properties=runtime_var | INS_PROPERTY_TYPE_SCRIPTVAR;\
			dst_ins->stkValue=NULL;\
			dst_ins->varRef=script_var;\
			sharePointer(script_var->ptr_shared_pointer_node);\
		}else{\
			zs_print_error_cr("(internal) cannot determine var type %i",GET_INS_PROPERTY_VAR_TYPE(src_ins->properties));\
			return NULL;\
		}\
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



	#define POP_SCOPE(index) \
	{\
		if(index < 0){\
			zs_print_error_cr("index < 0");\
			return NULL;\
		}\
	\
		if(index >= (int)info_function->object_info.n_info_var_scope){\
			zs_print_error_cr("index >= info_function->object_info.info_var_scope.size()");\
			return NULL;\
		}\
	\
		for(int i = 0; i < info_function->object_info.info_var_scope[index].n_var_index; i++){\
			int idx_local_var = info_function->object_info.info_var_scope[index].var_index[i];\
			tStackElement *ptr_ale =&ptrLocalVar[idx_local_var];\
			CScriptVariable *var = NULL;\
			switch(GET_INS_PROPERTY_VAR_TYPE(ptr_ale->properties)){\
			case INS_PROPERTY_TYPE_STRING:\
			case INS_PROPERTY_TYPE_SCRIPTVAR:\
				var =((CScriptVariable *)(ptr_ale->varRef));\
				if(var != VM_NULL && var !=  VM_UNDEFINED){\
					if(var->ptr_shared_pointer_node != NULL){\
						var->unrefSharedPtr();\
					}\
				}\
			}\
			*ptr_ale={\
					INS_PROPERTY_TYPE_UNDEFINED,\
					0,\
					0\
			};\
		}\
	\
		tInfoSharedList list = zero_shares[idxCurrentStack];\
		PInfoSharedPointerNode first_node,current;\
		first_node=current=(list).first;\
		if(current != NULL){\
			while(current->next !=first_node){\
				PInfoSharedPointerNode node_to_remove=current;\
				if(ret_scriptvariable_node!=node_to_remove->data.shared_ptr){\
					delete node_to_remove->data.shared_ptr;\
				}\
				current=current->next;\
				free(node_to_remove);\
			}\
			delete current->data.shared_ptr;\
			free(current);\
		}\
		(list).first=(list).last=NULL;\
	}


	#define CALL_GC SHARED_LIST_DESTROY(zero_shares[idxCurrentStack])

	#define APPLY_METAMETHOD(DEF_METAMETHOD) \
		script_class_aux=NULL; \
		vec_global_functions=NULL; \
		bool mm_test_is_c = false;\
		tStackElement *mm_test_startArg = ptrCurrentOp+MAX_METAMETHOD_ARGS; \
		aux_function_info = NULL; \
		\
		\
		\
		if((ptrResultInstructionOp1->properties ) == (INS_PROPERTY_TYPE_SCRIPTVAR | INS_PROPERTY_IS_STACKVAR)){ /* check whether the first op1 is a script var ... */ \
			/* 0. Check pre condition ...*/ \
			if(instruction->index_op2 ==ZS_UNDEFINED_IDX){ /* search for first time , else the function is stored in index_op2 */ \
		\
				CScriptVariable *v1 = (CScriptVariable *)(((tStackElement *)(ptrResultInstructionOp1->varRef))->varRef);\
		\
				mm_test_is_c = v1->is_c_object();\
		\
				script_class_aux=REGISTERED_CLASS_NODE(v1->idxScriptClass);\
				vec_global_functions=&script_class_aux->metamethod_operator[DEF_METAMETHOD];\
				int size_fun_vec = vec_global_functions->size()-1;\
		\
				for(int i = size_fun_vec; i>=0 && aux_function_info==NULL; i--){ /* search all function that match symbol ... */ \
					CScriptFunctionObject *sfo=GET_SCRIPT_FUNCTION_OBJECT(vec_global_functions->at(i)); \
					/* we found a function is match ... */ \
					if(mm_test_is_c){ /* C! Must match args... */ \
							bool all_check=true; /* check arguments types ...*/ \
							/* convert parameters script to c...*/ \
								for( int k = 0; k < MAX_METAMETHOD_ARGS && all_check;k++){ \
									tStackElement *currentArg=&(mm_test_startArg-MAX_METAMETHOD_ARGS)[k]; \
									if(currentArg->properties & INS_PROPERTY_IS_STACKVAR){ \
										currentArg = (tStackElement *)currentArg->varRef; \
									} \
									unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(currentArg->properties); \
									switch(var_type){ \
									default: \
										aux_string="unknow"; \
										break; \
									case INS_PROPERTY_TYPE_INTEGER: \
										aux_string=*CScriptClass::INT_PTR_TYPE_STR; \
										break; \
									case INS_PROPERTY_TYPE_NUMBER: \
										aux_string=*CScriptClass::FLOAT_PTR_TYPE_STR; \
										break; \
									case INS_PROPERTY_TYPE_BOOLEAN: \
										aux_string=*CScriptClass::BOOL_PTR_TYPE_STR; \
										break; \
									case INS_PROPERTY_TYPE_STRING: \
										aux_string=*CScriptClass::STRING_PTR_TYPE_STR; \
										break; \
									case INS_PROPERTY_TYPE_NULL: \
									case INS_PROPERTY_TYPE_UNDEFINED: \
									case INS_PROPERTY_TYPE_SCRIPTVAR: \
									case INS_PROPERTY_TYPE_SCRIPTVAR|INS_PROPERTY_TYPE_STRING: \
										aux_string = ((CScriptVariable *)currentArg->varRef)->getPointer_C_ClassName(); \
										break; \
									} \
									all_check = aux_string==sfo->m_arg[k]; \
									if(!all_check){ \
										/* let's see whether it can be coneverted to target signature ... */ \
										all_check =CScriptClass::getConversionType(aux_string,sfo->m_arg[k], false)!=NULL; \
									} \
								} \
								if(all_check){ /* we found the right function (set it up!) ... */ \
									instruction->index_op2 = i; \
									aux_function_info = sfo; \
								} \
					}else{ /* type script function  ... */ \
						instruction->index_op2=(intptr_t)sfo; \
						aux_function_info = sfo; \
					} \
				} \
				\
				if(aux_function_info == NULL){ \
						int n_candidates=0; \
						string str_candidates=""; \
						for(int i = size_fun_vec; i>=0 && aux_function_info==NULL; i--){ /* search all function that match symbol ... */ \
							CScriptFunctionObject *irfs = NULL; \
							irfs=GET_SCRIPT_FUNCTION_OBJECT(vec_global_functions->at(i)); \
							 \
							if(n_candidates == 0){ \
								str_candidates+="\t\tPossible candidates are:\n\n"; \
							} \
							\
							str_candidates+="\t\t-"+irfs->object_info.symbol_info.symbol_name+"("; \
							\
							for(unsigned a = 0; a < irfs->m_arg.size(); a++){ \
								if(a>0){ \
									str_candidates+=","; \
								} \
								\
								if(mm_test_is_c){ \
									str_candidates+=demangle(irfs->m_arg[a]); \
								}else{ /* typic var ... */ \
									str_candidates+="arg"+CStringUtils::intToString(a+1); \
								} \
							} \
							\
							str_candidates+=");\n"; \
							n_candidates++; \
						} \
						  \
						zs_print_error_cr("Cannot find metamethod \"%s\" at line %i.\n\n%s", \
								AST_SYMBOL_VALUE_CONST_CHAR(instruction->idxAstNode), \
								AST_LINE_VALUE(instruction->idxAstNode), \
								str_candidates.c_str()); \
						 \
						return NULL; \
		 		} \
				  \
			}else{ \
				aux_function_info = (CScriptFunctionObject *)instruction->index_op2; \
			} \
			  \
			/* by default virtual machine gets main object class in order to run functions ... */ \
			if((ret_obj=execute_internal(aux_function_info,this_object,mm_test_startArg,ptrCurrentStr,MAX_METAMETHOD_ARGS))==NULL){ \
				return NULL; \
			} \
			  \
			/* restore ptrCurretOp... */ \
			ptrCurrentOp=mm_test_startArg-2; \
			/* if function is C must register pointer ! */ \
			  \
			if(ret_obj->properties & INS_PROPERTY_TYPE_SCRIPTVAR){ \
			  \
				if(!((CScriptVariable *)(ret_obj->varRef))->initSharedPtr()){ \
					return NULL; \
				} \
			} \
			*ptrCurrentOp++ = *ret_obj; \
			continue; \
		}

	string CVirtualMachine::STR_GET_TYPE_VAR_INDEX_INSTRUCTION(tStackElement *ptr_info_ale){
		string result="undefined";
		if(IS_INT(ptr_info_ale->properties))
			result= "int";
		else if(IS_NUMBER(ptr_info_ale->properties))
			result= "number";
		else if(IS_BOOLEAN(ptr_info_ale->properties))
			result= "bool";
		else if(IS_STRING(ptr_info_ale->properties))
			result= "string";
		else if(IS_FUNCTION(ptr_info_ale->properties))
			result= "function";

		else if(IS_VAR(ptr_info_ale->properties)){
			tStackElement *var=ptr_info_ale;
			if(ptr_info_ale->properties & INS_PROPERTY_IS_STACKVAR){
				var=(tStackElement *)var->varRef;
			}
			result=((CScriptVariable *)var->varRef)->getClassName();
		}

		return result;
	}

	CVirtualMachine::CVirtualMachine(){

		//-----------------------------------------------------------
		// set memory manager

		memset(zero_shares,0,sizeof(zero_shares));
		memset(shared_var,0,sizeof(shared_var));


		idxCurrentStack=0;

		//------------------

		f_return_value=0;
		s_return_value="unknow";


		ptrCurrentOp=stack;
		//ptrCurrentOp=ptrBaseOp=ptrLocalVar=stack;

		//instance_gc = NULL;
		//vec_ast_node = NULL;
		//VM_UNDEFINED=NULL;
		VM_UNDEFINED=UNDEFINED_SYMBOL;
		VM_NULL=NULL_SYMBOL;

		f_aux_value1=0;
		f_aux_value2=0;

		ptrLastStr=&stkString[VM_LOCAL_VAR_MAX_STACK-1]; // aux values for string ...
		ptrCurrentStr=NULL;

	}

	#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
	//#define __VERBOSE_MESSAGE__
	#endif



	#ifdef  __VERBOSE_MESSAGE__

	#define print_vm_cr zs_print_info_cr
	#else
	#define print_vm_cr(s,...)
	#endif




	//============================================================================================================================================

	// POINTER MANANAGER

	string stk_C_TypeStr(const tStackElement & stk_v){
		if(stk_v.properties & INS_PROPERTY_TYPE_INTEGER){
			return demangle(typeid(int).name());
		}else if(stk_v.properties & INS_PROPERTY_TYPE_NUMBER){
			return demangle(typeid(float).name());
		}else if(stk_v.properties & INS_PROPERTY_TYPE_BOOLEAN){
			return demangle(typeid(bool).name());
		}else if(stk_v.properties & INS_PROPERTY_TYPE_STRING){
			return demangle(typeid(string).name());
		}else if(stk_v.properties & INS_PROPERTY_TYPE_SCRIPTVAR){


			CScriptClass *c = CScriptClass::getScriptClassByIdx(((CScriptVariable *)(stk_v.varRef))->idxScriptClass);

			if(c!=NULL){
				return demangle(c->classPtrType);
			}
		}
		return "unknow";
	}

		template<typename _T>
		static bool instanceof(const tStackElement & stk_v){
			int idxScriptVar = getIdxClassFromIts_C_Type(typeid(_T).name());
			switch(idxScriptVar){
			case IDX_CLASS_INT_PTR_C:
				if(stk_v.properties & INS_PROPERTY_TYPE_INTEGER){
					return true;
				}
				break;
			case IDX_CLASS_FLOAT_PTR_C:
				if(stk_v.properties & INS_PROPERTY_TYPE_NUMBER){
					return true;
				}
				break;
			case IDX_CLASS_BOOL_PTR_C:
				if(stk_v.properties & INS_PROPERTY_TYPE_BOOLEAN){
					return true;
				}
				break;
			case IDX_CLASS_STRING_PTR_C:
				if(stk_v.properties & INS_PROPERTY_TYPE_STRING){
					return true;
				}
				break;
			default:
				if(stk_v.properties & INS_PROPERTY_TYPE_SCRIPTVAR){
					return idxScriptVar==((CScriptVariable *)(stk_v.varRef))->idxScriptClass;
				}
				break;
			}
			return false;
		}


	PInfoSharedPointerNode CVirtualMachine::newSharedPointer(CScriptVariable *_var_ptr){
		//int index = CVirtualMachine::getFreeCell();
		PInfoSharedPointerNode _node = (PInfoSharedPointerNode)malloc(sizeof(tInfoSharedPointerNode));
		_node->data.n_shares=0;
		_node->data.shared_ptr=_var_ptr;

		// insert node into shared nodes ...

		SHARE_LIST_INSERT(zero_shares[idxCurrentStack],_node);
		//zero_shares[idxCurrentStack].InsertNode(_node);
		return _node;
	}


	bool CVirtualMachine::sharePointer(PInfoSharedPointerNode _node){

		unsigned char *n_shares = &_node->data.n_shares;

		bool move_to_shared_list=*n_shares==0;

		if(*n_shares >= MAX_SHARES_VARIABLE){
			zs_print_error_cr("MAX SHARED VARIABLES (Max. %i)",MAX_SHARES_VARIABLE);
			return false;
		}

		(*n_shares)++;

		if(move_to_shared_list){

			// Mov to shared pointer...
			SHARE_LIST_DEATTACH(zero_shares[idxCurrentStack],_node);
			SHARE_LIST_INSERT(shared_var[idxCurrentStack],_node);
		}

		return true;

	}

	void CVirtualMachine::unrefSharedScriptVar(PInfoSharedPointerNode _node){

		unsigned char *n_shares = &_node->data.n_shares;
		if(*n_shares > 0){
			if(--(*n_shares)==0){ // mov back to 0s shares (candidate to be deleted on GC check)
				SHARE_LIST_DEATTACH(shared_var[idxCurrentStack],_node);
				SHARE_LIST_INSERT(zero_shares[idxCurrentStack],_node);
			}
		}

	}


	#define POP_TWO \
	ptrResultInstructionOp2=--ptrCurrentOp;\
	ptrResultInstructionOp1=--ptrCurrentOp;

	#define POP_TWO_ASSIGN \
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
		if(instruction->index_op1 != ZS_UNDEFINED_IDX){\
			instruction_it=&(*current_statment)[instruction->index_op1];\
		}\
	\
	}

	//============================================================================================================================================
	tStackElement * CVirtualMachine::call_C_function(
			intptr_t fun_ptr,
			const CScriptFunctionObject *irfs,
			tStackElement *ptrArg,
			unsigned char n_args){


		//auto v = argv->at(0)->getPointer_C_ClassName();
		CScriptVariable *script_variable=NULL;
		intptr_t converted_param[MAX_N_ARGS];
		intptr_t result;
		tStackElement *currentArg;

		if((irfs->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) {
			zs_print_error_cr("Function is not registered as C");
			return &callc_result;//CScriptVariable::UndefinedSymbol;
		}

		if(fun_ptr==0){
			zs_print_error_cr("Null function");
			return &callc_result;//CScriptVariable::UndefinedSymbol;
		}

		if((char)irfs->m_arg.size() != n_args){
			zs_print_error_cr("C argument VS scrip argument doestn't match sizes");
			return &callc_result;//CScriptVariable::UndefinedSymbol;
		}

		if(irfs->m_arg.size() >= MAX_N_ARGS){
			zs_print_error_cr("Reached max param for C function (Current: %i Max Allowed: %i)",irfs->m_arg.size(),MAX_N_ARGS);
			return &callc_result;//CScriptVariable::UndefinedSymbol;
		}

		// convert parameters script to c...
		for(unsigned char  i = 0; i < n_args;i++){

			currentArg=&ptrArg[i];

			// due some args are stacked in order to have in/out features it doesn't has any sense through C...
			if(currentArg->properties & INS_PROPERTY_IS_STACKVAR){
				currentArg=((tStackElement *)currentArg->varRef);
			}

			//converted_param[i]= (intptr_t)(script_variable);
			//bool prall_check = false;

			switch(GET_INS_PROPERTY_VAR_TYPE(currentArg->properties)){
			case INS_PROPERTY_TYPE_BOOLEAN:
				if(irfs->m_arg[i] != *CScriptClass::BOOL_PTR_TYPE_STR){
					zs_print_error_cr("Function %s, param %i: cannot convert %s into %s",
											irfs->object_info.symbol_info.symbol_name.c_str(),
											i,
											demangle((*CScriptClass::STRING_PTR_TYPE_STR)).c_str(),
											demangle(irfs->m_arg[i]).c_str()
											);
					return NULL;
				}
				converted_param[i]=(intptr_t)(&currentArg->stkValue);
				break;
			case INS_PROPERTY_TYPE_NUMBER:
				if(irfs->m_arg[i] != *CScriptClass::FLOAT_PTR_TYPE_STR){
					zs_print_error_cr("Function %s, param %i: cannot convert %s into %s",
											irfs->object_info.symbol_info.symbol_name.c_str(),
											i,
											demangle((*CScriptClass::STRING_PTR_TYPE_STR)).c_str(),
											demangle(irfs->m_arg[i]).c_str()
											);
					return NULL;
				}

				converted_param[i]=(intptr_t)(&currentArg->stkValue);
				break;
			case INS_PROPERTY_TYPE_INTEGER:
				if(irfs->m_arg[i] != *CScriptClass::INT_PTR_TYPE_STR){
					zs_print_error_cr("Function %s, param %i: cannot convert %s into %s",
											irfs->object_info.symbol_info.symbol_name.c_str(),
											i,
											demangle((*CScriptClass::STRING_PTR_TYPE_STR)).c_str(),
											demangle(irfs->m_arg[i]).c_str()
											);
					return NULL;
				}

				converted_param[i]=(intptr_t)(&currentArg->stkValue);
				break;

			case INS_PROPERTY_TYPE_STRING:
				if(irfs->m_arg[i] != *CScriptClass::STRING_PTR_TYPE_STR){
					zs_print_error_cr("Function %s, param %i: cannot convert %s into %s",
							irfs->object_info.symbol_info.symbol_name.c_str(),
							i,
							demangle((*CScriptClass::STRING_PTR_TYPE_STR)).c_str(),
							demangle(irfs->m_arg[i]).c_str()
							);
					return NULL;
				}

				converted_param[i]=(intptr_t)(currentArg->stkValue);
				break;
			default: // script variable by default ...
				script_variable=(CScriptVariable *)currentArg->varRef;


				if(script_variable->is_c_object()){ // get the pointer directly ...
					if(script_variable->getPointer_C_ClassName()==irfs->m_arg[i]){
						converted_param[i]=(intptr_t)script_variable->get_C_Object();
					}else{
						fntConversionType paramConv=CScriptClass::getConversionType((script_variable)->getPointer_C_ClassName(),irfs->m_arg[i]);
						if(paramConv==0){
							zs_print_error_cr("Function %s param %i: cannot convert %s into %s",
									irfs->object_info.symbol_info.symbol_name.c_str(),
									i,
									script_variable->getPointer_C_ClassName().c_str(),
									irfs->m_arg[i].c_str()
							);
						}
						converted_param[i] = paramConv(script_variable);
						//converted_param[i]=(intptr_t)script_variable->get_C_Object();
					}

				}else{ // CScriptVariable ?
					zs_print_error_cr("Internal error, no C-object parameter! Unexpected  script variable!");
					return NULL;
				}
				break;
			}
		}

		zs_print_debug_cr("pre_call %i",n_args);

		if(irfs->idx_return_type == IDX_CLASS_VOID_C){ // getInstance()->getIdxClassVoid()){

			switch(n_args){
			default:
				zs_print_error_cr("Max run-time args! (Max:%i Provided:%i)",MAX_N_ARGS,n_args);
				return &callc_result;//CScriptVariable::UndefinedSymbol;
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


			switch(n_args){
			default:
				zs_print_error_cr("Max run-time args! (Max:%i Provided:%i)",MAX_N_ARGS,n_args);
				return NULL;//CScriptVariable::UndefinedSymbol;
			case 0:
				result=(*((std::function<int ()> *)fun_ptr))();
				break;
			case 1:
				result=(*((std::function<int (int)> *)fun_ptr))(converted_param[0]);
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

			// save return type ...
			switch(irfs->idx_return_type){
			 case IDX_CLASS_INT_PTR_C:
				 callc_result={INS_PROPERTY_TYPE_INTEGER,(void *)(*((int *)result)),NULL};
				 break;
			 case IDX_CLASS_FLOAT_PTR_C:
				 callc_result.properties=INS_PROPERTY_TYPE_NUMBER;//{};
				 COPY_NUMBER(&callc_result.stkValue,(float *)result);
				 callc_result.varRef=NULL;
				 break;

			 case IDX_CLASS_BOOL_PTR_C:
				 callc_result={INS_PROPERTY_TYPE_SCRIPTVAR,(void *)(*((bool *)result)),NULL};
				 break;

			 case IDX_CLASS_STRING_PTR_C:
				 s_return_value = *((string *)result);
				 callc_result={INS_PROPERTY_TYPE_STRING,&s_return_value,NULL};//new string(*((string *)result))};
				 break;
			 default:
				 callc_result = {INS_PROPERTY_TYPE_SCRIPTVAR,NULL,CScriptClass::instanceScriptVariableByIdx(irfs->idx_return_type,(void *)result)};
				 break;
			}
		}
		return &callc_result;
	}

	CScriptVariable * CVirtualMachine::execute(
			 CScriptFunctionObject *info_function,
			 CScriptVariable *this_object,
			vector<CScriptVariable *> * arg
			){

		tStackElement *_ptrIniCurrentOp=ptrCurrentOp;
		string * _ptrIniCurrentString=ptrCurrentStr;


		if(_ptrIniCurrentOp == NULL){
			_ptrIniCurrentOp=stack;
		}

		int n_arg=0;

		if(arg!=NULL){

			//advance idxBaseStk...
			//idxBaseStk+=arg->size();
			//n_arg=arg->size();
		}

		tStackElement *info=execute_internal(
				info_function,
				this_object,
				_ptrIniCurrentOp,
				_ptrIniCurrentString,
				n_arg);

		//idxBaseStk-=arg->size();
		if(info==NULL){
			return NULL;
		}

		return UNDEFINED_SYMBOL;
	}


	tStackElement * CVirtualMachine::execute_internal(
			CScriptFunctionObject * info_function,
			CScriptVariable       * this_object,
			tStackElement 		  * _ptrStartOp,
			string 		  		  * _ptrStartStr,
			unsigned char n_args){


		string *ptrStartStr;
		tStackElement *ptrStartOp;

		callc_result ={
				INS_PROPERTY_TYPE_UNDEFINED,
				0,
				NULL};

		unsigned n_local_vars = info_function->object_info.local_symbols.m_registeredVariable.size();
		ptrStartOp =_ptrStartOp;
		if(ptrStartOp == NULL){
			ptrStartOp=stack;
		}

		ptrStartStr =_ptrStartStr;
		if(ptrStartStr == NULL){
			ptrStartStr=stkString;
		}

		tStackElement *ptrLocalVar=NULL;
		tStackElement *ptrArg=NULL;

		zs_print_debug_cr("Executing function %s ...",info_function->object_info.symbol_info.symbol_name.c_str());
		int idxBaseStk=(ptrCurrentOp-stack);//>>sizeof(tStackElement *);

		if(idxBaseStk<n_args){
			zs_print_error_cr("Internal error (idxBaseStk<n_args)");
			exit(EXIT_FAILURE);
		}

		if(n_args>0){
			ptrArg=(ptrStartOp-n_args);
		}

		ptrLocalVar=ptrStartOp;


		if((info_function->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // C-Call

			intptr_t  fun_ptr = info_function->object_info.symbol_info.ref_ptr;

			if((info_function->object_info.symbol_info.properties &  SYMBOL_INFO_PROPERTIES::PROPERTY_STATIC_REF) != SYMBOL_INFO_PROPERTIES::PROPERTY_STATIC_REF){ // if not static then is function depends of object ...

				if(this_object!= NULL && this_object != CZetScript::getInstance()->getMainObject()){
					fun_ptr = this_object->getFunctionSymbolByIndex(info_function->object_info.symbol_info.idxSymbol)->proxy_ptr;
				}
			}

			return call_C_function(fun_ptr,info_function,ptrArg,n_args);
		}

		PASTNode ast = AST_NODE(info_function->object_info.symbol_info.idxAstNode);

		int scope_index =  ast->idxScope;


		if(ast->keyword_info == KEYWORD_TYPE::FUNCTION_KEYWORD){
			scope_index=AST_NODE(ast->children[1])->idxScope;
		}


		//CScriptVariable *ret=VM_UNDEFINED;
		callc_result ={INS_PROPERTY_TYPE_UNDEFINED, UNDEFINED_SYMBOL};
		PtrStatment m_listStatements = info_function->object_info.statment_op;
		//bool end_by_ret=false;

		//=========================================
		// PUSH STACK
		// reserve vars and assign argv vars ...
		//pushStack(info_function, n_args);
		unsigned n_total_vars=n_args+n_local_vars;


		if(idxCurrentStack < MAX_FUNCTION_CALL){
			idxCurrentStack++;
		}
		else{
			zs_print_error_cr("Reached max stack");
			exit(EXIT_FAILURE);
		}

		if((idxBaseStk+n_total_vars) >=  VM_LOCAL_VAR_MAX_STACK){
			zs_print_error_cr("Error MAXIMUM stack size reached");
			exit(EXIT_FAILURE);
		}

		// init local vars ...
		tStackElement *ptr_aux = ptrLocalVar;
		for(unsigned i = 0; i < n_local_vars; i++){
			*ptr_aux++={
					INS_PROPERTY_TYPE_UNDEFINED, // starts undefined.
					0,							 // no value assigned.
					0 						     // no varref related.
				};
		}

		ptrStartOp=&ptrLocalVar[n_local_vars];

		// PUSH STACK
		//=========================================
		//bool	aux_boolean=false;
		float aux_float=0.0;
		CScriptVariable * ret_scriptvariable_node =NULL;

		CScriptFunctionObject * aux_function_info=NULL;
		tStackElement *ret_obj=NULL;

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
				default:
					zs_print_error_cr("operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
					return NULL;
				case END_STATMENT:
					goto lbl_exit_statment;
				case LOAD:
				case VGET:

					if(index_op1==LOAD_TYPE::LOAD_TYPE_VARIABLE || operator_type==VGET){
						if(operator_type==VGET){

							bool ok=false;

							POP_TWO;

							if( (ptrResultInstructionOp1->properties & (INS_PROPERTY_TYPE_SCRIPTVAR | INS_PROPERTY_IS_STACKVAR)) == (INS_PROPERTY_TYPE_SCRIPTVAR | INS_PROPERTY_IS_STACKVAR)){
								var_object = (CScriptVariable *)(((tStackElement *)ptrResultInstructionOp1->varRef)->varRef);
							}

							if(var_object != NULL){
								if(var_object->idxScriptClass == IDX_CLASS_VECTOR){

									if(IS_INT(ptrResultInstructionOp2->properties)){
										// determine object ...
										CVector * vec = (CVector *)var_object;
										int v_index = LOAD_INT_OP(ptrResultInstructionOp2);

										// check indexes ...
										if(v_index < 0 || v_index >= (int)(vec->m_objVector.size())){
											zs_print_error_cr("Line %i. Index vector out of bounds!",AST_LINE_VALUE(instruction->idxAstNode));
											return NULL;
										}

										ldrVar = &vec->m_objVector[v_index];;
										ok = true;
									}else{
										zs_print_error_cr("Expected vector-index as integer");
										return NULL;
									}
								}
							}

							if(!ok){
								zs_print_error_cr("Line %i: Variable \"%s\" is not type vector",
									AST_LINE_VALUE(instruction->idxAstNode),
									AST_SYMBOL_VALUE_CONST_CHAR((*current_statment)[instruction->index_op1].idxAstNode),
									AST_SYMBOL_VALUE_CONST_CHAR(instruction->idxAstNode)
									//base_var->getClassName().c_str(), iao->ast_node->symbol_value.c_str()
									);
								return NULL;
							}

						}else{

							instruction_properties=instruction->instruction_properties;
							scope_type=GET_INS_PROPERTY_SCOPE_TYPE(instruction_properties);

							switch(scope_type){
							default: // global...
								ldrVar = &stack[instruction->index_op2];
								break;
							case INS_PROPERTY_ACCESS_SCOPE:
							case INS_PROPERTY_THIS_SCOPE:
								if(instruction->idxAstNode != -1){
									ast = AST_NODE(instruction->idxAstNode);

								}

								if(scope_type == INS_PROPERTY_ACCESS_SCOPE){
									POP_ONE; // get var op1 and symbol op2
									CScriptVariable  * base_var = NULL;
									if(ptrResultInstructionOp1->properties & INS_PROPERTY_IS_STACKVAR) {
										tStackElement *stk_ins=((tStackElement *)ptrResultInstructionOp1->varRef);

										if(stk_ins->properties & INS_PROPERTY_TYPE_SCRIPTVAR){
											base_var=((CScriptVariable *)stk_ins->varRef);
										}
									}

									if(base_var == NULL)
									{

										zs_print_error_cr("Line %i:var is not scriptvariable",AST_NODE(instruction->idxAstNode)->line_value);
										return NULL;
									}

									if((si = base_var->getVariableSymbol(ast->symbol_value))==NULL){
										zs_print_error_cr("Line %i: Variable %s as type %s has not symbol %s",ast->line_value,AST_SYMBOL_VALUE_CONST_CHAR((*current_statment)[instruction->index_op2].idxAstNode),base_var->getClassName().c_str(), ast->symbol_value.c_str());
										return NULL;
									}
								}
								else{
									if((si = this_object->getVariableSymbolByIndex(instruction->index_op2))==NULL){
										zs_print_error_cr("cannot find symbol this.%s",ast->symbol_value.c_str());
										return NULL;
									}
								}
								ldrVar=&si->object;
								break;
							case INS_PROPERTY_LOCAL_SCOPE:
								ldrVar = &ptrLocalVar[instruction->index_op2];
								break;
							}
						}

						pre_post_properties = GET_INS_PROPERTY_PRE_POST_OP(instruction_properties);
						//bool is_c=(((ldrVar->properties)& INS_PROPERTY_IS_C_VAR)!= 0);

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
								case INS_PROPERTY_TYPE_INTEGER:
									if(ldrVar->properties& INS_PROPERTY_IS_C_VAR){
										*ptrCurrentOp++={INS_PROPERTY_TYPE_INTEGER|INS_PROPERTY_IS_STACKVAR|INS_PROPERTY_IS_C_VAR,(void *)(-(*((int *)ldrVar->varRef))),ldrVar};
									}else{
										*ptrCurrentOp++={INS_PROPERTY_TYPE_INTEGER|INS_PROPERTY_IS_STACKVAR,(void *)(-(((int)ldrVar->stkValue))),ldrVar};
									}
									break;
								case INS_PROPERTY_TYPE_BOOLEAN:
									if(ldrVar->properties& INS_PROPERTY_IS_C_VAR){
										*ptrCurrentOp++={INS_PROPERTY_TYPE_BOOLEAN|INS_PROPERTY_IS_STACKVAR|INS_PROPERTY_IS_C_VAR,(void *)(!(*((bool *)ldrVar->varRef))),ldrVar};
									}else{
										*ptrCurrentOp++={INS_PROPERTY_TYPE_BOOLEAN|INS_PROPERTY_IS_STACKVAR,(void *)(!(((bool)ldrVar->stkValue))),ldrVar};
									}
									break;

								case INS_PROPERTY_TYPE_NUMBER:
									if(ldrVar->properties& INS_PROPERTY_IS_C_VAR){
										COPY_NUMBER(&aux_float,ldrVar->stkValue);
									}else{
										COPY_NUMBER(&aux_float,&ldrVar->stkValue);
									}
									aux_float=-aux_float;
									COPY_NUMBER(&ptrCurrentOp->stkValue,&aux_float);
									*ptrCurrentOp={INS_PROPERTY_TYPE_NUMBER|INS_PROPERTY_IS_STACKVAR,ptrCurrentOp->stkValue,ldrVar};

									if(ldrVar->properties& INS_PROPERTY_IS_C_VAR){
										ptrCurrentOp->properties|=INS_PROPERTY_IS_C_VAR;
									}

									ptrCurrentOp++;
									break;
								default:
									zs_print_error_cr("internal error:cannot perform pre operator - because is not number");
									return NULL;
								}
								break;
						default:
								PUSH_STACK_VAR(ldrVar);
								continue;
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
								case INS_PROPERTY_TYPE_INTEGER:
									ptrCurrentOp->stkValue=(void *)(-((int)ptrCurrentOp->stkValue));
									break;
								case INS_PROPERTY_TYPE_BOOLEAN:
									ptrCurrentOp->stkValue=(void *)(!((bool)ptrCurrentOp->stkValue));
									break;
								case INS_PROPERTY_TYPE_NUMBER:
									COPY_NUMBER(&aux_float,&ptrCurrentOp->stkValue);
									aux_float=-aux_float;
									COPY_NUMBER(&ptrCurrentOp->stkValue,&aux_float);
									break;
								default:
									zs_print_error_cr("internal error:cannot perform pre operator - constant because is not numeric");
									return NULL;
								}
								break;
						}
						ptrCurrentOp++;
						continue;
					}else if(index_op1== LOAD_TYPE::LOAD_TYPE_FUNCTION){
						void *function_obj=NULL;
						vector<int> *vec_functions;
						CScriptVariable * class_obj=NULL;
						int index_op2 = (int)instruction->index_op2;
						instruction_properties=instruction->instruction_properties;
						scope_type=GET_INS_PROPERTY_SCOPE_TYPE(instruction_properties);

						if(scope_type==INS_PROPERTY_LOCAL_SCOPE){ // global! It gets functions from main object ...
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
							if(var->properties & INS_PROPERTY_IS_STACKVAR) {
								stk_ins=((tStackElement *)var->varRef);
							}
							else{
								stk_ins=var;
							}

							if(stk_ins->properties & INS_PROPERTY_TYPE_SCRIPTVAR){
								class_obj=(CScriptVariable *)(stk_ins->varRef);
								CScriptClass *sc = CScriptClass::getScriptClassByIdx(((CScriptVariable *)class_obj)->idxScriptClass);
								vec_functions=&sc->metadata_info.object_info.local_symbols.vec_idx_registeredFunction;
							}
							else{
								zs_print_error_cr("Expected scriptvar");
								return NULL;
							}
						}else if(scope_type == INS_PROPERTY_THIS_SCOPE){
							if((si = this_object->getFunctionSymbolByIndex(index_op2))==NULL){
								zs_print_error_cr("cannot find function \"this.%s\"",AST_NODE(instruction->idxAstNode)->symbol_value.c_str());
								return NULL;
							}

							function_obj =(CScriptFunctionObject *)si->object.stkValue;

						}else if(scope_type == INS_PROPERTY_SUPER_SCOPE){ // super scope ?
							if((si = this_object->getFunctionSymbolByIndex(index_op2))==NULL){
								zs_print_error_cr("cannot find function \"super.%s\"",AST_NODE(instruction->idxAstNode)->symbol_value.c_str());
								return NULL;
							}
							function_obj =(CScriptFunctionObject *)si->object.stkValue;
						}else{ // global
							vec_functions = &(GET_MAIN_FUNCTION_OBJECT->object_info.local_symbols.vec_idx_registeredFunction);
							//function_obj = GET_SCRIPT_FUNCTION_OBJECT(info_function->object_info.local_symbols.vec_idx_registeredFunction[index_op2]);
						}


						if(!(scope_type == INS_PROPERTY_THIS_SCOPE || scope_type == INS_PROPERTY_SUPER_SCOPE)){

							if(index_op2 == ZS_UNDEFINED_IDX){ // is will be processed after in CALL instruction ...
								//function_properties=INS_PROPERTY_UNRESOLVED_FUNCTION;
								function_obj= (void *)instruction; // saves current instruction in order to resolve its idx later (in call instruction)
							}else if((index_op2<(int)vec_functions->size())) // get the function ...
							{
								function_obj =GET_SCRIPT_FUNCTION_OBJECT((*vec_functions)[index_op2]);
							}
							else{
								zs_print_error_cr("cannot find symbol global \"%s\"",AST_NODE(instruction->idxAstNode)->symbol_value.c_str());
								return NULL;
							}
						}
						PUSH_FUNCTION(0,function_obj,class_obj);
						continue;

					}else if(index_op1== LOAD_TYPE::LOAD_TYPE_ARGUMENT){
						//ldrVar=&ptrArg[instruction->index_op2];
						*ptrCurrentOp++=ptrArg[instruction->index_op2]; // copy arg directly ...
						continue;
					}
					else{

						zs_print_error_cr("no load defined type");
						return NULL;
						break;
					}
					continue;

				case STORE:
				case VPUSH:
				case PUSH_ATTR:

				{

					if(operator_type==VPUSH){
						POP_ONE; // only pops the value, the last is the vector variable itself
						CScriptVariable *vec_obj = NULL;
						if((ptrCurrentOp-1)->properties & INS_PROPERTY_TYPE_SCRIPTVAR){
							vec_obj = (CScriptVariable *)(ptrCurrentOp-1)->varRef;
							if(vec_obj->idxScriptClass == IDX_CLASS_VECTOR){ // push value ...
								// op1 is now the src value ...
								src_ins=ptrResultInstructionOp1;
								dst_ins=((CVector *)vec_obj)->push();
								ok=true;
							}
						}

						if(!ok){
							zs_print_error_cr("Expected vector object");
							return NULL;
						}

					}else if(operator_type==PUSH_ATTR){


						POP_TWO; // first must be the value name and the other the variable name ...
						CScriptVariable *struct_obj = NULL;
						if((ptrCurrentOp-1)->properties & INS_PROPERTY_TYPE_SCRIPTVAR){
							struct_obj = (CScriptVariable *)(ptrCurrentOp-1)->varRef;
							if(struct_obj->idxScriptClass == IDX_CLASS_STRUCT){ // push value ...
								// op1 is now the src value ...
								if(ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_STRING){
									tSymbolInfo *si=NULL;
									string *str = (string *)ptrResultInstructionOp2->stkValue;
									src_ins=ptrResultInstructionOp1;
									if(src_ins->properties&INS_PROPERTY_TYPE_FUNCTION){
										si =((CStruct *)struct_obj)->addFunctionSymbol(*str, -1,(CScriptFunctionObject *)src_ins->stkValue );
									}else{
										si =((CStruct *)struct_obj)->addVariableSymbol(*str, -1,src_ins );
									}
									dst_ins=&si->object;
									ok=true;
								}
								else{
									zs_print_error_cr("internal error (operator2 is not string)");
									return NULL;
								}
							}
						}

						if(!ok){
							zs_print_error_cr("Expected struct object");
							return NULL;
						}
					}
					else{ // pop two parameters nothing ...
						POP_TWO;

						if(ptrResultInstructionOp1->properties & INS_PROPERTY_IS_STACKVAR) {// == CScriptVariable::VAR_TYPE::OBJECT){
							dst_ins=(tStackElement *)ptrResultInstructionOp1->varRef; // stkValue is expect to contents a stack variable
							src_ins=ptrResultInstructionOp2; // store ptr instruction2 op as src_var_value
						}else{
							zs_print_error_cr("Expected object l-value mov");
							return NULL;
						}
					}

					tStackElement old_dst_ins = *dst_ins; // save dst_var to check after assignment...

					// ok load object pointer ...
						ASSIGN_STACK_VAR(dst_ins,src_ins);


						// check old var structure ...
						switch(GET_INS_PROPERTY_VAR_TYPE(old_dst_ins.properties)){
						case INS_PROPERTY_TYPE_NULL:
						case INS_PROPERTY_TYPE_UNDEFINED:
						case INS_PROPERTY_TYPE_INTEGER:
						case INS_PROPERTY_TYPE_NUMBER:
						case INS_PROPERTY_TYPE_BOOLEAN:
						case INS_PROPERTY_TYPE_FUNCTION: // we aren't take care about nothing! :)
							break;
						case INS_PROPERTY_TYPE_STRING:
						case INS_PROPERTY_TYPE_SCRIPTVAR: // we are getting script vars ...
							if(old_dst_ins.varRef!=NULL){ // it had a pointer (no constant)...
								if(src_ins->varRef != dst_ins->varRef){ // unref pointer because new pointer has been attached...
									unrefSharedScriptVar(((CScriptVariable  *)old_dst_ins.varRef)->ptr_shared_pointer_node);
								}
							}
							break;
						}
				}

					continue;

				case EQU:  // ==

					POP_TWO;
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
					PROCESS_COMPARE_OPERATION(<, LTE_METAMETHOD);
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
					if(instruction->instruction_properties&INS_PROPERTY_IS_ASSIGN_OP){
						POP_TWO_ASSIGN;
					}else{
						POP_TWO;
					}

					PROCESS_LOGIC_OPERATION(&&, LOGIC_AND_METAMETHOD);
					continue;
				case LOGIC_OR:  // ||

					if(instruction->instruction_properties&INS_PROPERTY_IS_ASSIGN_OP){
						POP_TWO_ASSIGN;
					}else{
						POP_TWO;
					}
					PROCESS_LOGIC_OPERATION(||, LOGIC_OR_METAMETHOD);
					continue;
				case NOT: // !

					POP_ONE;

					if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_BOOLEAN){ // operation will result as integer.
						PUSH_BOOLEAN((!((bool)(ptrResultInstructionOp1->stkValue))));
					}else{
							zs_print_error_cr("Line %i:Expected operands 1 as boolean!",AST_LINE_VALUE(instruction->idxAstNode));
							return NULL;
					}
					continue;

				case NEG: // -

					POP_ONE;

					if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_INTEGER){ // operation will result as integer.
						PUSH_INTEGER((-((int)(ptrResultInstructionOp1->stkValue))));
					}else if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_NUMBER){
						COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);
						PUSH_NUMBER(-f_aux_value1);
					}else{
							zs_print_error_cr("Line %i:Expected operands 1 as number or integer!",AST_LINE_VALUE(instruction->idxAstNode));
							return NULL;
					}
					continue;

				case ADD: // +

				{
					if(instruction->instruction_properties&INS_PROPERTY_IS_ASSIGN_OP){
						POP_TWO_ASSIGN;
					}else{
						POP_TWO;
					}


					unsigned short mask_properties =GET_INS_PROPERTY_PRIMITIVE_TYPES(ptrResultInstructionOp1->properties&ptrResultInstructionOp2->properties);
					unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);
					if(mask_properties==INS_PROPERTY_TYPE_INTEGER){
							PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) + LOAD_INT_OP(ptrResultInstructionOp2));
					}
					else if(properties==(INS_PROPERTY_TYPE_INTEGER|INS_PROPERTY_TYPE_NUMBER)){
							if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
								COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);
								PUSH_NUMBER(LOAD_INT_OP(ptrResultInstructionOp1) + f_aux_value2);
							}else{
								COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);
								PUSH_NUMBER(f_aux_value1 + LOAD_INT_OP(ptrResultInstructionOp2));
							}
					}
					else if(properties==(INS_PROPERTY_TYPE_INTEGER|INS_PROPERTY_TYPE_STRING)){
						if (IS_STRING(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
							sprintf(str_aux,"%s%i",((string *)ptrResultInstructionOp1->stkValue)->c_str(),(int)ptrResultInstructionOp2->stkValue);
							PUSH_STRING(str_aux);
						}else{
							sprintf(str_aux,"%i%s",(int)ptrResultInstructionOp1->stkValue,((string *)ptrResultInstructionOp2->stkValue)->c_str());
							PUSH_STRING(str_aux);
						}
					}else if(properties==(INS_PROPERTY_TYPE_NUMBER|INS_PROPERTY_TYPE_STRING)){
						if (IS_STRING(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
							COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);
							sprintf(str_aux,"%s%f",((string *)ptrResultInstructionOp1->stkValue)->c_str(),f_aux_value2);
							PUSH_STRING(str_aux);

						}else{
							COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);
							sprintf(str_aux,"%f%s",f_aux_value1,((string *)ptrResultInstructionOp2->stkValue)->c_str());
							PUSH_STRING(str_aux);
						}
					}else if(properties==(INS_PROPERTY_TYPE_BOOLEAN|INS_PROPERTY_TYPE_STRING)){
						if (IS_STRING(ptrResultInstructionOp1->properties) && IS_BOOLEAN(ptrResultInstructionOp2->properties)){
							sprintf(str_aux,"%s%s",((string *)ptrResultInstructionOp1->stkValue)->c_str(),((bool)(ptrResultInstructionOp2->stkValue))?"true":"false");
							PUSH_STRING(str_aux);
						}else{
							sprintf(str_aux,"%s%s",((bool)(ptrResultInstructionOp1->stkValue))?"true":"false",((string *)ptrResultInstructionOp2->stkValue)->c_str());
							PUSH_STRING(str_aux);
						}

					}else if (mask_properties== INS_PROPERTY_TYPE_STRING){
							sprintf(str_aux,"%s%s",((string *)ptrResultInstructionOp1->stkValue)->c_str(),((string *)(ptrResultInstructionOp2->stkValue))->c_str());
							PUSH_STRING(str_aux);


					}else if(mask_properties== INS_PROPERTY_TYPE_NUMBER){
						COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
						COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
						PUSH_NUMBER(f_aux_value1 + f_aux_value2);\

					}
					else{ // try metamethod ...

						APPLY_METAMETHOD(ADD_METAMETHOD);

						PRINT_DUAL_ERROR_OP(+);
					}
				}
					continue;

				case MUL: // *

					if(instruction->instruction_properties&INS_PROPERTY_IS_ASSIGN_OP){
						POP_TWO_ASSIGN
					}else{
						POP_TWO;
					}

					PROCESS_ARITHMETIC_OPERATION(*,MUL_METAMETHOD);
					continue;

				case DIV: // /

					if(instruction->instruction_properties&INS_PROPERTY_IS_ASSIGN_OP){
						POP_TWO_ASSIGN
					}else{
						POP_TWO;
					}

					PROCESS_ARITHMETIC_OPERATION(/, DIV_METAMETHOD);
					continue;

			 	 case MOD: // /

					if(instruction->instruction_properties&INS_PROPERTY_IS_ASSIGN_OP){
						POP_TWO_ASSIGN
					}else{
						POP_TWO;
					}

					PROCESS_MOD_OPERATION;
					continue;

			 	 case AND: // &
					if(instruction->instruction_properties&INS_PROPERTY_IS_ASSIGN_OP){
						POP_TWO_ASSIGN
					}else{
						POP_TWO;
					}

					PROCESS_BINARY_OPERATION(&, AND_METAMETHOD);
					continue;
			 	 case OR: // *
					if(instruction->instruction_properties&INS_PROPERTY_IS_ASSIGN_OP){
						POP_TWO_ASSIGN
					}else{
						POP_TWO;
					}

					PROCESS_BINARY_OPERATION(|, OR_METAMETHOD);
					continue;
			 	 case XOR: // ^
					if(instruction->instruction_properties&INS_PROPERTY_IS_ASSIGN_OP){
						POP_TWO_ASSIGN
					}else{
						POP_TWO;
					}

					PROCESS_BINARY_OPERATION(^, XOR_METAMETHOD);
					continue;

			 	 case SHR: // >>
					if(instruction->instruction_properties&INS_PROPERTY_IS_ASSIGN_OP){
						POP_TWO_ASSIGN
					}else{
						POP_TWO;
					}

					PROCESS_BINARY_OPERATION(>>,SHR_METAMETHOD);
					continue;

			 	 case SHL: // <<
					if(instruction->instruction_properties&INS_PROPERTY_IS_ASSIGN_OP){
						POP_TWO_ASSIGN
					}else{
						POP_TWO;
					}

					PROCESS_BINARY_OPERATION(<<, SHL_METAMETHOD);
					continue;
			 	 case JMP:
					CHK_JMP;
					continue;
			 	 case JNT: // goto if not true ... goes end to conditional.
					POP_ONE;
					if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_BOOLEAN){

						if(!((bool)(ptrResultInstructionOp1->stkValue))){
							CHK_JMP;
						}
					}else{
						zs_print_error_cr("Expected boolean element");
						return NULL;
					}

					continue;
			 	 case JT: // goto if true ... goes end to conditional.
					POP_ONE;
					if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_BOOLEAN){

						if(((bool)(ptrResultInstructionOp1->stkValue))){
							CHK_JMP;
						}
					}else{
						zs_print_error_cr("Expected boolean element");
						return NULL;
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

					while(n_args <= MAX_N_ARGS && (((startArg-1)->properties&INS_PROPERTY_TYPE_FUNCTION)==0)){
						startArg--;
						n_args++;
					}

					callAle = ((startArg-1));
					calling_object = this_object;
					tInfoAsmOp *iao = (tInfoAsmOp *)callAle->stkValue;
					bool is_constructor = (iao->instruction_properties & INS_PROPERTY_CONSTRUCT_CALL)!=0;
					bool deduce_function = false; //(iao->instruction_properties & INS_PROPERTY_DEDUCE_C_CALL)!=0;

					// try to find the function ...
					if(iao->index_op2 == ZS_UNDEFINED_IDX || deduce_function){


						symbol_to_find = AST_NODE(iao->idxAstNode)->symbol_value;
						//tInfoAsmOp *iao = &(*current_statment)[instruction->index_op1];
						unsigned short scope_type = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

						{
							// local vars as functions ...
							vec_global_functions=&(GET_MAIN_FUNCTION_OBJECT->object_info.local_symbols.vec_idx_registeredFunction);

							int size_fun_vec = (int)vec_global_functions->size()-1;

							// registered symbols in case is INS_PROPERTY_ACCESS_SCOPE...
							vector<tSymbolInfo> *m_functionSymbol=NULL;
							if(scope_type==INS_PROPERTY_ACCESS_SCOPE){
								calling_object = (CScriptVariable *)callAle->varRef;

								// we have to no to call default constructor...is implicit
								if(is_constructor) {

									is_c=calling_object->is_c_object();

									if(n_args == 0 && is_c){
										iao->index_op2 = ZS_FUNCTION_NOT_FOUND_IDX;
									}
								}

								m_functionSymbol=calling_object->getVectorFunctionSymbol();
								size_fun_vec = (int)m_functionSymbol->size()-1;

							}else if (deduce_function){ // function C deduce
								size_fun_vec = 0;
							}

							bool all_check=true;

							if(iao->index_op2 != ZS_FUNCTION_NOT_FOUND_IDX)
							{

								for(int i = size_fun_vec; i>=0 && aux_function_info==NULL; i--){ // search all function that match symbol ...
									CScriptFunctionObject *irfs = NULL;
									if(scope_type==INS_PROPERTY_ACCESS_SCOPE){
										irfs = (CScriptFunctionObject *)m_functionSymbol->at(i).object.stkValue;
										aux_string=m_functionSymbol->at(i).symbol_value;
									}else if(deduce_function){
										irfs = (CScriptFunctionObject *)callAle->stkValue;
									}else{
										irfs=GET_SCRIPT_FUNCTION_OBJECT(vec_global_functions->at(i));
										aux_string=irfs->object_info.symbol_info.symbol_name;
									}
									// we found a function is match ...
									if(aux_string == symbol_to_find && irfs->m_arg.size() == n_args){
										if(irfs->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF){ // C! Must match args...

												all_check=true; // check arguments types ...
												// convert parameters script to c...
												//for( int h = 0 ; h < 2; h++){
													for( int k = 0; k < n_args && all_check;k++){
														tStackElement *currentArg=&startArg[k];\
														if(currentArg->properties & INS_PROPERTY_IS_STACKVAR){
															currentArg = (tStackElement *)currentArg->varRef;
														}
														unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(currentArg->properties);
														switch(var_type){
														default:
															aux_string="unknow";
															break;
														case INS_PROPERTY_TYPE_INTEGER:
															aux_string=*CScriptClass::INT_PTR_TYPE_STR;
															break;
														case INS_PROPERTY_TYPE_NUMBER:
															aux_string=*CScriptClass::FLOAT_PTR_TYPE_STR;
															break;
														case INS_PROPERTY_TYPE_BOOLEAN:
															aux_string=*CScriptClass::BOOL_PTR_TYPE_STR;
															break;
														case INS_PROPERTY_TYPE_STRING:
															aux_string=*CScriptClass::STRING_PTR_TYPE_STR;
															break;
														case INS_PROPERTY_TYPE_NULL:
														case INS_PROPERTY_TYPE_UNDEFINED:
														case INS_PROPERTY_TYPE_SCRIPTVAR:
														case INS_PROPERTY_TYPE_SCRIPTVAR|INS_PROPERTY_TYPE_STRING:
															aux_string = ((CScriptVariable *)currentArg->varRef)->getPointer_C_ClassName();
															break;
														}
														//converted_param[i]= (int)(argv->at(i);
														//if(h==0){ // match the all parameters
														all_check = aux_string==irfs->m_arg[k];
														//}
														if(!all_check){
															// let's see whether it can be coneverted to target signature ...
															all_check =CScriptClass::getConversionType(aux_string,irfs->m_arg[k], NULL)!=NULL;
														}
													}
													if(all_check){ // we found the right function (set it up!) ...

														if(!deduce_function){
															iao->index_op2 = i;
														}
														aux_function_info = irfs;
													}
										}else{ // type script function  ...

											iao->index_op2=i;
											aux_function_info = irfs;
										}
									}
								}
								{
									if(aux_function_info == NULL){
										if(is_constructor && n_args == 0){ // default constructor not found --> set as not found...
											iao->index_op2 = ZS_FUNCTION_NOT_FOUND_IDX;
										}
										else{
											int n_candidates=0;
											string str_candidates="";
											for(int i = size_fun_vec; i>=0 && aux_function_info==NULL; i--){ // search all function that match symbol ...
												CScriptFunctionObject *irfs = NULL;
												if(deduce_function){
													irfs = (CScriptFunctionObject *)callAle->stkValue;
												}else if(scope_type==INS_PROPERTY_ACCESS_SCOPE){
													irfs = (CScriptFunctionObject *)m_functionSymbol->at(i).object.stkValue;
													//aux_string=m_functionSymbol->at(i).symbol_value;
												}else{
													irfs=GET_SCRIPT_FUNCTION_OBJECT(vec_global_functions->at(i));
													//aux_string=irfs->object_info.symbol_info.symbol_name;
												}
													if(
														 (irfs->object_info.symbol_info.symbol_name == AST_NODE(iao->idxAstNode)->symbol_value)
														){

															if(n_candidates == 0){
																str_candidates+="\t\tPossible candidates are:\n\n";
															}

															str_candidates+="\t\t-"+irfs->object_info.symbol_info.symbol_name+"(";

															for(unsigned a = 0; a < irfs->m_arg.size(); a++){
																if(a>0){
																	str_candidates+=",";
																}

																if(irfs->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF){
																	str_candidates+=demangle(irfs->m_arg[a]);
																}else{ // typic var ...
																	str_candidates+="arg"+CStringUtils::intToString(a+1);
																}
															}

															str_candidates+=");\n";


															n_candidates++;

														}
											}

											if(n_candidates == 0){
												zs_print_error_cr("Cannot find %s \"%s\" function at line %i.\n\n%s",
														is_constructor ? "constructor":"function",
														AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode),
														AST_LINE_VALUE(iao->idxAstNode));

											}
											else{
												string args_str = "";
												// get arguments...
												for( int k = 0; k < n_args;k++){
													tStackElement *currentArg=&startArg[k];
													if(currentArg->properties & INS_PROPERTY_IS_STACKVAR){
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
													case INS_PROPERTY_TYPE_INTEGER:
														aux_string=*CScriptClass::INT_PTR_TYPE_STR;
														break;
													case INS_PROPERTY_TYPE_NUMBER:
														aux_string=*CScriptClass::FLOAT_PTR_TYPE_STR;
														break;
													case INS_PROPERTY_TYPE_BOOLEAN:
														aux_string=*CScriptClass::BOOL_PTR_TYPE_STR;
														break;
													case INS_PROPERTY_TYPE_STRING:
														aux_string=*CScriptClass::STRING_PTR_TYPE_STR;
														break;
													case INS_PROPERTY_TYPE_NULL:
													case INS_PROPERTY_TYPE_UNDEFINED:
													case INS_PROPERTY_TYPE_SCRIPTVAR:
													case INS_PROPERTY_TYPE_SCRIPTVAR|INS_PROPERTY_TYPE_STRING:
														aux_string = ((CScriptVariable *)currentArg->varRef)->getPointer_C_ClassName();
														break;
													}

													args_str+=demangle(aux_string);
												}

											zs_print_error_cr("Cannot match %s \"%s(%s)\"  at line %i.\n\n%s",
													is_constructor ? "constructor":"function",
													AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode),
													args_str.c_str(),
													AST_LINE_VALUE(iao->idxAstNode),
													str_candidates.c_str());
											}

											return NULL;
										}
									}
								}

							}
						}
					}
					else{
						if(iao->index_op2 != ZS_FUNCTION_NOT_FOUND_IDX){
							aux_function_info=(CScriptFunctionObject *) (callAle->stkValue);
						}
					}

					if(aux_function_info !=NULL)
					{
						if(n_args > MAX_N_ARGS){
							zs_print_error_cr("Max arguments reached function at line XXX");
							return NULL;
						}

						if((aux_function_info->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == 0){ // is function script ...
							if( n_args < aux_function_info->m_arg.size()){ // we must push undefined parameters ...
								for(unsigned i = n_args; i < aux_function_info->m_arg.size(); i++){
									*ptrCurrentOp++={
										INS_PROPERTY_TYPE_UNDEFINED, // starts undefined.
										0,							 // no value assigned.
										0 						     // no varref related.
									};
									n_args++;
								}
							}
						}

						// by default virtual machine gets main object class in order to run functions ...
						if((ret_obj=execute_internal(aux_function_info,calling_object,ptrCurrentOp,ptrCurrentStr,n_args))==NULL){
							return NULL;
						}

						if(ret_obj->properties & INS_PROPERTY_TYPE_SCRIPTVAR){

							if(!((CScriptVariable *)(ret_obj->varRef))->initSharedPtr()){
								return NULL;
							}
						}
					}

					// reset stack ...
					ptrCurrentOp=startArg-1;

					// ... and push result if not function constructor...
					if(!is_constructor){
						*ptrCurrentOp++ = *ret_obj;
					}
			 	 }
					continue;


			 	 case  NEW:
						svar=NEW_CLASS_VAR_BY_IDX(instruction->index_op1);

						if(!svar->initSharedPtr()){
							return NULL;
						}


						(*ptrCurrentOp++)={INS_PROPERTY_TYPE_SCRIPTVAR,NULL,svar};
						continue;

			 	 case DECL_VEC: // Create new vector object...
						svar=NEW_VECTOR_VAR;
						//PUSH_VAR(svar,NULL,0,false);

						if(!svar->initSharedPtr()){
							return NULL;
						}

						(*ptrCurrentOp++)={INS_PROPERTY_TYPE_SCRIPTVAR,NULL,svar};

						continue;

			 	 case  DECL_STRUCT: // Create new vector object...
					svar=NEW_STRUCT_VAR;
					//PUSH_VAR(svar,NULL,0,false);

					if(!svar->initSharedPtr()){
						return NULL;
					}

					(*ptrCurrentOp++)={INS_PROPERTY_TYPE_SCRIPTVAR,NULL,svar};

					continue;


			 	 case  RET:

					callc_result=*(ptrCurrentOp-1);

					// remove shared pointer if scriptvar ...
					if(callc_result.properties & INS_PROPERTY_TYPE_SCRIPTVAR){
						if(callc_result.properties & INS_PROPERTY_IS_STACKVAR){
							callc_result=*((tStackElement *)((tStackElement *)callc_result.varRef));

						}

						// unref pointer to be deallocated from gc...
						((CScriptVariable *)callc_result.varRef)->ptr_shared_pointer_node=NULL;
						ret_scriptvariable_node=((CScriptVariable *)callc_result.varRef);

						// share pointer  + 1
						//this->sharePointer(detach_node);
					}


					goto lbl_exit_function;


			 case POP_SCOPE:
					POP_SCOPE(instruction->index_op1);
					break;


				/*case DECL_STRUCT: // symply creates a variable ...
					svar=NEW_STRUCT;
					PUSH_VAR(svar,NULL,0,false);
					if(!svar->initSharedPtr())
						return NULL;
					break;
				case PUSH_ATTR:
					POP_TWO;
					// get symbol ... (current instruction -1)
					if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_STRING){

						CScriptVariable::tSymbolInfo *si;
						string *variable_name = (string *)ptrResultInstructionOp1->stkValue;

						zs_print_error_cr("PUSH ATTRIB NOT IMPLEMENTED +++");
						return NULL;

					}
					else{
						zs_print_error_cr("Struct: Expected symbol at line %i",AST_LINE_VALUE(instruction->idxAstNode));
						return NULL;
					}
					break;*/





				//
				// END OPERATOR MANAGEMENT
				//
				//-----------------------------------------------------------------------------------------------------------------------



				}
			 }



	// exit statment (don't remove ;, it gives a compile error)
	lbl_exit_statment:;

		}//while(ptr_statment_iteration->asm_op!=NULL && !end_by_ret);

	lbl_exit_function:

		POP_SCOPE(scope_index);



		//=========================
		// POP STACK
		// unref 1st scope ...

		if(idxCurrentStack > 0){
			idxCurrentStack--;
		}
		else{
			zs_print_error_cr("Reached min stack");
			return NULL;
		}

		// POP STACK
		//=========================


		//popStack();
		return &callc_result;
	}


	tStackElement  * CVirtualMachine::getLastStackValue(){
		return (ptrCurrentOp-1);
	}


	CVirtualMachine::~CVirtualMachine(){

	}

}
