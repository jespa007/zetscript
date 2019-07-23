/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define RETURN_ERROR\
	error=true;\
	return callc_result;




namespace zetscript{

	void  write_error(const char *filename, int line, const  char  *string_text, ...);



	#define COPY_NUMBER(d,s)  memcpy((d),(s),sizeof(float))

	#define LOAD_INT_OP(ptr_result_instruction) \
			((int)((intptr_t)(ptr_result_instruction->stkValue)))

	#define LOAD_BOOL_OP(ptr_result_instruction) \
			(((bool)(ptr_result_instruction->stkValue)))


	#define LOAD_STRING_OP(ptr_result_instruction) \
			(((const char *)(ptr_result_instruction->stkValue)))

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
	 (((CScriptVariable *)(ptr_result_instruction->stkValue))->idx_class==IDX_CLASS_VECTOR))

	#define IS_GENERIC_NUMBER(properties) \
	((properties & STK_PROPERTY_TYPE_INTEGER) ||\
	(properties & STK_PROPERTY_TYPE_NUMBER))


	#define OP1_AND_OP2_ARE_NUMBERS \
	(IS_GENERIC_NUMBER(stkResultOp1->properties) && IS_GENERIC_NUMBER(stkResultOp2->properties))

	#define OP1_IS_STRING_AND_OP2_IS_NUMBER \
	(stkResultOp1->properties & STK_PROPERTY_TYPE_STRING) && \
	IS_GENERIC_NUMBER(stkResultOp2->type_var)

	#define OP1_IS_NUMBER_AND_OP2_IS_STRING \
	(stkResultOp2->properties & STK_PROPERTY_TYPE_STRING) && \
	IS_GENERIC_NUMBER(stkResultOp1->type_var)

	#define OP1_IS_STRING_AND_OP2_IS_BOOLEAN \
	(stkResultOp1->properties & STK_PROPERTY_TYPE_STRING) && \
	(stkResultOp2->properties & STK_PROPERTY_TYPE_BOOLEAN)


	#define OP1_AND_OP2_ARE_BOOLEANS \
	(stkResultOp1->properties & STK_PROPERTY_TYPE_BOOLEAN) && \
	(stkResultOp2->properties & STK_PROPERTY_TYPE_BOOLEAN)

	#define OP1_AND_OP2_ARE_STRINGS \
	(stkResultOp1->properties & STK_PROPERTY_TYPE_STRING) && \
	(stkResultOp2->properties & STK_PROPERTY_TYPE_STRING)

	#define OP1_AND_OP2_ARE_UNDEFINED \
			(stkResultOp1->stkValue == VM_UNDEFINED) && \
			(stkResultOp2->stkValue == VM_UNDEFINED)

	#define OP1_AND_OP2_ARE_NULL \
			(stkResultOp1->stkValue == VM_NULL) && \
			(stkResultOp2->stkValue == VM_NULL)



	#define OP1_OR_OP2_IS_UNDEFINED \
			(stkResultOp1->stkValue == VM_UNDEFINED) || \
			(stkResultOp2->stkValue == VM_UNDEFINED)

	#define OP1_OR_OP2_ARE_NULL \
			(stkResultOp1->stkValue == VM_NULL) || \
			(stkResultOp2->stkValue == VM_NULL)


	#define PUSH_UNDEFINED \
	*stkCurrentData++={0,NULL,STK_PROPERTY_TYPE_UNDEFINED};

	#define PUSH_NULL \
	*stkCurrentData++={0,NULL,STK_PROPERTY_TYPE_NULL};

	#define PUSH_BOOLEAN(init_value) \
	*stkCurrentData++={(void *)((intptr_t)(init_value)),NULL,STK_PROPERTY_TYPE_BOOLEAN};

	#define PUSH_INTEGER(init_value) \
	*stkCurrentData++={(void *)((intptr_t)(init_value)),NULL,STK_PROPERTY_TYPE_INTEGER};

	#define PUSH_NUMBER(init_value) \
	{\
		float aux=(float)(init_value);\
		COPY_NUMBER(&stkCurrentData->stkValue,&aux);\
		stkCurrentData->properties=STK_PROPERTY_TYPE_NUMBER;\
		stkCurrentData++;\
	}

	#define PUSH_STRING(init_value)\
		if(ptrCurrentStr==ptrLastStr){write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Error stkString out-stack");RETURN_ERROR;}\
		*ptrCurrentStr++=init_value;\
		*stkCurrentData++={(void *)((ptrCurrentStr-1)->c_str()),NULL,STK_PROPERTY_TYPE_STRING};\



	#define PUSH_FUNCTION(fun_obj,class_obj,function_prop) \
	*stkCurrentData++={(void *)(fun_obj),class_obj,(unsigned short)(STK_PROPERTY_TYPE_FUNCTION|function_prop)};

	#define PUSH_SCRIPTVAR(var_ref) \
	*stkCurrentData++={NULL,var_ref,INS_PROPERTY_SCRIPTVAR};


	// Push stack var value (as varValue and put ptr as ref second value)...
	#define PUSH_STACK_VAR(stack_ref_var) \
		*stkCurrentData++={(stack_ref_var)->stkValue,stack_ref_var,(unsigned short)(((stack_ref_var)->properties)|STK_PROPERTY_IS_STACKVAR)};


	#define PROCESS_MOD_OPERATION \
	{ \
		unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stkResultOp1->properties|stkResultOp2->properties);\
		if(properties == STK_PROPERTY_TYPE_INTEGER){\
				PUSH_INTEGER(LOAD_INT_OP(stkResultOp1) % LOAD_INT_OP(stkResultOp2));\
		}\
		else if(properties == (STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_TYPE_NUMBER)){\
				if (IS_INT(stkResultOp1->properties) && IS_NUMBER(stkResultOp2->properties)){\
					COPY_NUMBER(&f_aux_value2,&stkResultOp2->stkValue);\
					PUSH_NUMBER(fmod(LOAD_INT_OP(stkResultOp1) , f_aux_value2));\
				}else{\
					COPY_NUMBER(&f_aux_value1,&stkResultOp1->stkValue);\
					PUSH_NUMBER(fmod(f_aux_value1 , LOAD_INT_OP(stkResultOp2)));\
				}\
		}\
		else if(properties == STK_PROPERTY_TYPE_NUMBER){\
				COPY_NUMBER(&f_aux_value1,&stkResultOp1->stkValue);\
				COPY_NUMBER(&f_aux_value2,&stkResultOp2->stkValue);\
				PUSH_NUMBER(fmod(f_aux_value1 , f_aux_value2));\
		}\
		else{\
			if(!APPLY_METAMETHOD(\
							 calling_object\
							,info_function\
  						    ,instruction\
							,STR(%)\
							,MOD_METAMETHOD\
							,stkResultOp1\
							,stkResultOp2\
			)){\
				RETURN_ERROR;\
			}\
		}\
	}

	#define PROCESS_ARITHMETIC_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stkResultOp1->properties|stkResultOp2->properties);\
		if(properties == STK_PROPERTY_TYPE_INTEGER){\
				PUSH_INTEGER(LOAD_INT_OP(stkResultOp1) __OVERR_OP__ LOAD_INT_OP(stkResultOp2));\
		}\
		else if(properties == (STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_TYPE_NUMBER)){\
				if (IS_INT(stkResultOp1->properties) && IS_NUMBER(stkResultOp2->properties)){\
					COPY_NUMBER(&f_aux_value2,&stkResultOp2->stkValue);\
					PUSH_NUMBER(LOAD_INT_OP(stkResultOp1) __OVERR_OP__ f_aux_value2);\
				}else{\
					COPY_NUMBER(&f_aux_value1,&stkResultOp1->stkValue);\
					PUSH_NUMBER(f_aux_value1 __OVERR_OP__ LOAD_INT_OP(stkResultOp2));\
				}\
		}\
		else if(properties == STK_PROPERTY_TYPE_NUMBER){\
				COPY_NUMBER(&f_aux_value1,&stkResultOp1->stkValue);\
				COPY_NUMBER(&f_aux_value2,&stkResultOp2->stkValue);\
				PUSH_NUMBER(f_aux_value1 __OVERR_OP__ f_aux_value2);\
		}\
		else{\
			if(!APPLY_METAMETHOD(\
							calling_object\
							,info_function\
							,instruction\
							,STR(__OVERR_OP__)\
							,__METAMETHOD__\
							,stkResultOp1\
							,stkResultOp2\
			)){\
				RETURN_ERROR;\
			}\
		}\
	}

	#define PROCESS_COMPARE_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_VAR_TYPE(stkResultOp1->properties|stkResultOp2->properties);\
		if(properties & STK_PROPERTY_TYPE_INTEGER){\
				PUSH_BOOLEAN(LOAD_INT_OP(stkResultOp1) __OVERR_OP__ LOAD_INT_OP(stkResultOp2));\
		}\
		else if(properties == STK_PROPERTY_TYPE_BOOLEAN){\
			PUSH_BOOLEAN(LOAD_BOOL_OP(stkResultOp1) __OVERR_OP__ LOAD_BOOL_OP(stkResultOp2));\
		}\
		else if(properties == (STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_TYPE_NUMBER)){\
				if (IS_INT(stkResultOp1->properties) && IS_NUMBER(stkResultOp2->properties)){\
					COPY_NUMBER(&f_aux_value2,&stkResultOp2->stkValue);\
					PUSH_BOOLEAN(LOAD_INT_OP(stkResultOp1) __OVERR_OP__ f_aux_value2);\
				}else{\
					COPY_NUMBER(&f_aux_value1,&stkResultOp1->stkValue);\
					PUSH_BOOLEAN(f_aux_value1 __OVERR_OP__ LOAD_INT_OP(stkResultOp2));\
				}\
		}\
		else if(properties == STK_PROPERTY_TYPE_NUMBER){\
				COPY_NUMBER(&f_aux_value1,&stkResultOp1->stkValue);\
				COPY_NUMBER(&f_aux_value2,&stkResultOp2->stkValue);\
				PUSH_BOOLEAN(f_aux_value1 __OVERR_OP__ f_aux_value2);\
		}\
		else if((stkResultOp1->properties&stkResultOp2->properties) == STK_PROPERTY_TYPE_STRING){\
			PUSH_BOOLEAN(STRCMP(LOAD_STRING_OP(stkResultOp1), __OVERR_OP__ ,LOAD_STRING_OP(stkResultOp2)));\
		}else{\
			if(!APPLY_METAMETHOD(\
						 calling_object\
						,info_function\
						,instruction\
						,STR(__OVERR_OP__)\
						, __METAMETHOD__\
						,stkResultOp1\
						,stkResultOp2\
			)){\
				RETURN_ERROR;\
			}\
		}\
	}

	#define PROCESS_LOGIC_OPERATION(__OVERR_OP__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_VAR_TYPE(stkResultOp1->properties|stkResultOp2->properties);\
		if(properties == STK_PROPERTY_TYPE_BOOLEAN){\
			PUSH_BOOLEAN(LOAD_BOOL_OP(stkResultOp1) __OVERR_OP__ LOAD_BOOL_OP(stkResultOp2));\
		}else{\
			PRINT_DUAL_ERROR_OP(STR(__OVERR_OP__));\
		}\
	}

	#define PROCESS_BINARY_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_VAR_TYPE(stkResultOp1->properties|stkResultOp2->properties);\
		if(properties == STK_PROPERTY_TYPE_INTEGER){\
			PUSH_INTEGER(LOAD_INT_OP(stkResultOp1) __OVERR_OP__ LOAD_INT_OP(stkResultOp2));\
		}else{\
			if(!APPLY_METAMETHOD(\
							 calling_object\
							,info_function\
							,instruction\
							,STR(__OVERR_OP__)\
							, __METAMETHOD__\
							,stkResultOp1\
							,stkResultOp2\
			)){\
				RETURN_ERROR;\
			}\
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
			write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction)," Cannot perform pre/post operator (%s)",STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ldrOp));\
			RETURN_ERROR;\
			break;\
		}\
	}


	#define ASSIGN_STACK_VAR(dst_ins, src_ins) \
	{\
		CScriptVariable *script_var=NULL;\
		std::string *aux_str=NULL;\
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
					write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"different types! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals",INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction));\
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
			*dst_ins={	src_ins->stkValue,\
						NULL,\
						(unsigned short)(runtime_var | STK_PROPERTY_TYPE_FUNCTION)\
						};\
		}else if(type_var & STK_PROPERTY_TYPE_STRING){\
			if(dst_ins->properties & STK_PROPERTY_IS_C_VAR){\
				*((std::string *)dst_ins->varRef)=((const char *)src_ins->stkValue);/* Assign std::string */\
				dst_ins->stkValue=(void *)(((std::string *)dst_ins->varRef)->c_str());/* Because std::string assignment implies reallocs ptr char it changes, so reassing const char pointer */\
			}else{\
				if(((dst_ins->properties & STK_PROPERTY_TYPE_STRING)==0) || (dst_ins->varRef==NULL)){/* Generates a std::string var */  \
					script_var= NEW_STRING_VAR;\
					dst_ins->varRef=script_var;\
					aux_str=&(((CStringScriptVariable *)script_var)->m_strValue);\
					dst_ins->properties=runtime_var | STK_PROPERTY_TYPE_STRING | STK_PROPERTY_TYPE_SCRIPTVAR;\
					script_var->initSharedPtr(true);\
				}\
				(*aux_str)=((const char *)src_ins->stkValue);\
				dst_ins->stkValue=(void *)aux_str->c_str();/* Because std::string assignment implies reallocs ptr char it changes, so reassing const char pointer */ \
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
			write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"(internal) cannot determine var type %i",GET_INS_PROPERTY_VAR_TYPE(src_ins->properties));\
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

	#define PUSH_SCOPE(_index,_ptr_info_function, _ptr_local_var,_properties) {\
		if(current_scope_info_ptr >=  MAX_SCOPE_INFO){write_error(NULL,0,"reached max scope"); RETURN_ERROR;}\
		*current_scope_info_ptr++={(short)(_index),_ptr_info_function,_ptr_local_var,_properties};\
	}


	#define CALL_GC SHARED_LIST_DESTROY(zero_shares[idxCurrentStack])



	const char * CVirtualMachine::STR_GET_TYPE_VAR_INDEX_INSTRUCTION(StackElement *ptr_info_ale){
		const char * result="undefined";
		if(IS_INT(ptr_info_ale->properties))
			result= "int";
		else if(IS_NUMBER(ptr_info_ale->properties))
			result= "float";
		else if(IS_BOOLEAN(ptr_info_ale->properties))
			result= "bool";
		else if(IS_STRING(ptr_info_ale->properties))
			result= "std::string";
		else if(IS_FUNCTION(ptr_info_ale->properties))
			result= "function";

		else if(IS_VAR(ptr_info_ale->properties)){
			StackElement *var=ptr_info_ale;
			if(ptr_info_ale->properties & STK_PROPERTY_IS_STACKVAR){
				var=(StackElement *)var->varRef;
			}
			result=((CScriptVariable *)var->varRef)->getClassName().c_str();
		}

		return result;
	}



	const char * 		CVirtualMachine::getOpCodeStr(OP_CODE  op){
		if(op < OP_CODE::MAX_OP_CODES){
			return defined_opcode[op].str;

		}

		return "unknow_op";
	}

	void CVirtualMachine::stackDumped(){
		// derefer all variables in all scopes (except main )...
		while(scope_info<(current_scope_info_ptr)){
			POP_SCOPE_CALL(idxCurrentStack,NULL,0);
		}

		idxCurrentStack=0;
	}



	CVirtualMachine::CVirtualMachine(CZetScript *zs){

		//-----------------------------------------------------------
		// set memory manager

		memset(zero_shares,0,sizeof(zero_shares));
		memset(shared_var,0,sizeof(shared_var));
		//memset(stack,0,sizeof(stack));

		StackElement *aux=stack;

		for(int i=0; i < VM_LOCAL_VAR_MAX_STACK;i++){
			*aux++={0,NULL,STK_PROPERTY_TYPE_UNDEFINED};
		}

		idxCurrentStack=0;
		//------------------

		f_return_value=0;
		s_return_value="unknow";

		cancel_execution=false;

		stkCurrentData=NULL;
		current_scope_info_ptr = scope_info;

		f_aux_value1=0;
		f_aux_value2=0;

		custom_error = "unknow_error";

		idx_laststatment=0;

		ptrLastStr=&stkString[VM_MAX_AUX_STRINGS-1]; // aux values for std::string ...
		ptrCurrentStr=NULL;

		MAX_SCOPE_INFO = &scope_info[VM_MAX_SCOPES-1];

		//current_ast_node_call_c_function=-1;

		main_function_object = NULL;

		vec_script_function_node = NULL;
		//vec_ast_node = NULL;

		size_vec_script_function_object_node = 0;
		//size_vec_ast_node = 0;
		current_foreach=NULL;
		current_call_c_function = NULL;
		n_globals=0;
		_zs=zs;

		memset(defined_opcode,0,sizeof(defined_opcode));


		// OP CODES
		//		VAR  			|	STR   | ID | NUM OP
		//----------------------+---------+----+-------

		defined_opcode[EQU]         ={EQU,"EQU" };  // ==
		defined_opcode[INSTANCEOF]  ={INSTANCEOF,"INSTANCEOF"};  // ==
		defined_opcode[NOT_EQU]     ={NOT_EQU,"NOT_EQU" };  // !=
		defined_opcode[LT]          ={LT,"LT"  };  // <
		defined_opcode[LTE]         ={LTE,"LTE"};  // <=
		defined_opcode[NOT]         ={NOT,"NOT"}; // !
		defined_opcode[GT]          ={GT,"GT"};  // >
		defined_opcode[GTE]         ={GTE,"GTE"}; // >=
		defined_opcode[NEG]         ={NEG,"NEG"}; // !
		defined_opcode[ADD]         ={ADD,"ADD"}; // +
		defined_opcode[LOGIC_AND]   ={LOGIC_AND,"LOGIC_AND"}; // &&
		defined_opcode[LOGIC_OR]    ={LOGIC_OR,"LOGIC_OR"};  // ||
		defined_opcode[DIV]         ={DIV,"DIV"}; // /
		defined_opcode[MUL]         ={MUL,"MUL"}; // *
		defined_opcode[MOD]         ={MOD,"MOD"};  // %

		defined_opcode[AND]         ={AND,"AND"}; // bitwise logic and
		defined_opcode[OR]          ={OR,"OR"}; // bitwise logic or
		defined_opcode[XOR]         ={XOR,"XOR"}; // logic xor
		defined_opcode[SHL]         ={SHL,"SHL"}; // shift left
		defined_opcode[SHR]         ={SHR,"SHR"}; // shift right

		defined_opcode[STORE]       ={STORE,"STORE"}; // mov expression to var
		defined_opcode[LOAD]        ={LOAD,"LOAD"}; // primitive value like number/std::string or boolean...

		defined_opcode[JMP]         ={JMP,"JMP"}; // Unconditional jump.
		defined_opcode[JNT]         ={JNT,"JNT"}; // goto if not true ... goes end to conditional.
		defined_opcode[JT]          ={JT,"JT"}; // goto if true ... goes end to conditional.

		defined_opcode[CALL]={CALL,"CALL"}; // calling function after all of arguments are processed...


		defined_opcode[VGET]={VGET,"VGET"}; // std::vector access after each index is processed...

		defined_opcode[DECL_VEC]={DECL_VEC,"DECL_VEC"}; // Vector object (CREATE)

		defined_opcode[VPUSH]={VPUSH,"VPUSH"}; // Value push for std::vector
		defined_opcode[RET]={RET,"RET"}; // Value pop for std::vector

		defined_opcode[NEW]={NEW,"NEW"}; // New object (CREATE)
		defined_opcode[DELETE_OP]={DELETE_OP,"DELETE_OP"};

		defined_opcode[POP_SCOPE]={POP_SCOPE,"POP_SCOPE"}; // New object (CREATE)
		defined_opcode[PUSH_SCOPE]={PUSH_SCOPE,"PUSH_SCOPE"}; // New object (CREATE)
		defined_opcode[PUSH_ATTR]={PUSH_ATTR,"PUSH_ATTR"}; // New object (CREATE)
		defined_opcode[DECL_STRUCT]={DECL_STRUCT,"DECL_STRUCT"}; // New object (CREATE)

		defined_opcode[IT_INI]={IT_INI,"IT_INI"}; // IT_INI
		defined_opcode[IT_CHK_END]={IT_CHK_END,"IT_CHK_END"}; // IT_CHK_END
		defined_opcode[IT_SET_AND_NEXT]={IT_SET_AND_NEXT,"IT_SET_AND_NEXT"}; // IT_SET_AND_NEXT

	}







	#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

	#define print_vm_cr zs_print_info_cr
	#else
	#define print_vm_cr(s,...)
	#endif


	//============================================================================================================================================
	// POINTER MANANAGER
	std::string CZetScript::stk_C_TypeStr(const StackElement & stk_v){
		if(stk_v.properties & STK_PROPERTY_TYPE_INTEGER){
			return rtti::demangle(typeid(int).name());
		}else if(stk_v.properties & STK_PROPERTY_TYPE_NUMBER){
			return rtti::demangle(typeid(float).name());
		}else if(stk_v.properties & STK_PROPERTY_TYPE_BOOLEAN){
			return rtti::demangle(typeid(bool).name());
		}else if(stk_v.properties & STK_PROPERTY_TYPE_STRING){
			return rtti::demangle(typeid(std::string).name());
		}else if(stk_v.properties & STK_PROPERTY_TYPE_SCRIPTVAR){


			CScriptClass *c = GET_SCRIPT_CLASS(((CScriptVariable *)(stk_v.varRef))->idx_class);

			if(c!=NULL){
				return rtti::demangle(c->classPtrType);
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
			THROW_RUNTIME_ERROR(string::sformat("MAX SHARED VARIABLES (Max. %i)",MAX_SHARES_VARIABLE));
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
					delete _node->data.shared_ptr;
					free(_node);
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
	stkResultOp2=--stkCurrentData;\
	stkResultOp1=--stkCurrentData;

	#define READ_TWO_POP_ONE \
	stkResultOp2=--stkCurrentData;\
	stkResultOp1=(stkCurrentData-1);


	#define POP_ONE \
	stkResultOp1=--stkCurrentData;

	const CScriptFunction * CVirtualMachine::getCurrent_C_FunctionCall(){
		return current_call_c_function;
	}

	//============================================================================================================================================
	StackElement  CVirtualMachine::call_C_function(
			intptr_t fun_ptr,
			const CScriptFunction *irfs,
			bool & error,
			StackElement *ptrArg,
			unsigned char n_args,
			OpCodeInstruction *ins){

		StackElement callc_result={0,0,STK_PROPERTY_TYPE_UNDEFINED};

		intptr_t converted_param[MAX_N_ARGS];
		intptr_t result=0;
		StackElement *currentArg;
		current_call_c_function = irfs;
		//float aux_float=0;

		if(n_args>MAX_N_ARGS){
			write_error(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Max run-time args! (Max:%i Provided:%i)",MAX_N_ARGS,n_args);
			RETURN_ERROR;
		}


		if((irfs->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) {
			write_error(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Function is not registered as C");
			RETURN_ERROR;
		}

		if(fun_ptr==0){
			write_error(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Null function");
			//return &callc_result;//CScriptVariable::UndefinedSymbol;
			RETURN_ERROR;
		}

		if((char)irfs->m_arg.size() != n_args){
			write_error(INSTRUCTION_GET_FILE_LINE(irfs,ins),"C argument VS scrip argument doestn't match sizes");
			RETURN_ERROR;
		}

		if(irfs->m_arg.size() > MAX_N_ARGS){
			write_error(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Reached max param for C function (Current: %i Max Allowed: %i)",irfs->m_arg.size(),MAX_N_ARGS);
			RETURN_ERROR;
			//return &callc_result;//CScriptVariable::UndefinedSymbol;
		}

		// convert parameters script to c...
		for(unsigned char  i = 0; i < n_args;i++){

			currentArg=&ptrArg[i];

			if(!zs->stk_2_var(currentArg,irfs->m_arg[i].idx_type,(intptr_t *)&converted_param[i],error_str)){
				write_error(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Function %s, param %i: %s. The function C %s that was found for first time it has different argument types now.",
																irfs->symbol_info.symbol->name.c_str(),
																i,
																error_str.c_str(),
																irfs->symbol_info.symbol->name.c_str()
																);
				RETURN_ERROR;
			}
		}

		zs_print_debug_cr("pre_call %i",n_args);

		if(irfs->idx_return_type == IDX_TYPE_VOID_C){ // getInstance()->getIdxClassVoid()){

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

			if(irfs->idx_return_type==IDX_TYPE_BOOL_C){  // we must do a bool cast in order to get float return.
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
			}else if(irfs->idx_return_type==IDX_TYPE_FLOAT_C){ // we must do a float cast in order to get float return.
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

			callc_result=var_2_stk(result,irfs->idx_return_type);
		}





		return callc_result;
	}

	StackElement * CVirtualMachine::getStackElement(unsigned int idx_glb_element){
		CScriptFunction  *main_function = GET_SCRIPT_FUNCTION(0);

		if(idx_glb_element < main_function->m_variable.size()){
			return &stack[idx_glb_element];
		}

		return NULL;
	}

	void CVirtualMachine::setError(const char *str){

		write_error("custom_user",-1,str);

		custom_error = str;
		cancel_execution = true;
	}

	void CVirtualMachine::cancelExecution(){
		custom_error=NULL;
		cancel_execution = true;
	}

	void CVirtualMachine::destroyCache(){
		main_function_object=NULL;
		if(vec_script_function_node!= NULL){
			free(vec_script_function_node);
			vec_script_function_node=NULL;
			size_vec_script_function_object_node=0;
		}

		/*if(vec_ast_node!= NULL){
			free(vec_ast_node);
			vec_ast_node=NULL;
			size_vec_ast_node=0;
		}*/

	}

	void CVirtualMachine::buildCache(){

		destroyCache();

		main_function_object = MAIN_FUNCTION;
		std::vector<CScriptFunction *> *vec_script_function_object_node_aux=_script_function_factory->getVectorScriptFunctionNode();
		size_vec_script_function_object_node=vec_script_function_object_node_aux->size();
		vec_script_function_node=(CScriptFunction **)malloc(sizeof(CScriptFunction *)*size_vec_script_function_object_node);
		for(unsigned i=0; i < size_vec_script_function_object_node; i++){
			vec_script_function_node[i]=vec_script_function_object_node_aux->at(i);
		}
	}

/*	void CVirtualMachine::addGlobalVar(StackElement se){
		global_var.push_back(se);
	}*/

	void CVirtualMachine::addGlobalVar(const StackElement & stk){

		if(n_globals < VM_LOCAL_VAR_MAX_STACK){
			stack[n_globals++]=stk;
		}else{
			write_error(NULL,-1,"Max stack element over limit (%i)",VM_LOCAL_VAR_MAX_STACK);
		}
	}

	void CVirtualMachine::clearGlobalVars(){
		CScriptFunction  *main_function = MAIN_FUNCTION;

		// zero shares have a late loading so it can be null at first time...
		if(zero_shares == NULL){
			return;
		}



		if(n_globals!=main_function->m_variable.size()){
			THROW_RUNTIME_ERROR("n_globals != main variables");
			return;
		}

		bool end=false;
		for(int i =  (int)(main_function->m_variable.size())-1; i >= 0 && !end; i--){
			//switch(GET_INS_PROPERTY_VAR_TYPE(ptr_ale->properties)){
			//case STK_PROPERTY_TYPE_STRING:
			end=(main_function->m_variable[i].properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF;
			if(!end){
				StackElement *ptr_ale =&stack[i];
				CScriptVariable *var = NULL;

				if(ptr_ale->properties &STK_PROPERTY_TYPE_SCRIPTVAR){
					var =((CScriptVariable *)(ptr_ale->varRef));
					if(var){
						if(var->ptr_shared_pointer_node != NULL){
							var->unrefSharedPtr();
						}
					}
				}
			}
			//}
		}

		REMOVE_0_SHARED_POINTERS(0,NULL);


		// clear all stack variables
		StackElement *aux=stack;
		for(int i=0; i < VM_LOCAL_VAR_MAX_STACK;i++){
			*aux++={0,NULL,STK_PROPERTY_TYPE_UNDEFINED};
		}

		memset(zero_shares,0,sizeof(zero_shares));
		memset(shared_var,0,sizeof(shared_var));
		//memset(stack,0,sizeof(stack));

		idxCurrentStack=0;
		//int idxCurrentStack
	}

	StackElement  CVirtualMachine::execute(
			 CScriptFunction *info_function,
			 CScriptVariable *this_object,
			 bool & error,
			const std::vector<StackElement> & arg
			){


		StackElement callc_result={0,0,STK_PROPERTY_TYPE_UNDEFINED};

		if(info_function==NULL){
			RETURN_ERROR;
		}


		if(idxCurrentStack==0){ // set stack and init vars for first call...

			if(main_function_object->instruction == NULL){ // no statments
				RETURN_ERROR;
			}
			cancel_execution=false;

			stkCurrentData=stack;
			//*stkCurrentData={STK_PROPERTY_TYPE_UNDEFINED,0,0}; // ini first op

			if(info_function->idxScriptFunction != 0){ // calls script function from C : preserve stack space for global vars
				stkCurrentData=&stack[main_function_object->m_variable.size()];
			}

			current_foreach=&stkForeach[0];
		}

		int n_arg=0;
		if(arg.size()>0){ // pass parameters...

			for(unsigned i = 0; i < arg.size(); i++){
				*stkCurrentData++=arg[i];
			}
			//advance idxBaseStk...
			//idxBaseStk+=arg->size();
			n_arg=arg.size();
		}

		// Script function starts here.... later script function can call c++ function, but once in c++ function is not possible by now call script function again.


		StackElement info=execute_internal(
				info_function,
				this_object,
				error,
				stkCurrentData,
				NULL,
				n_arg);


		if(error){ // it was error so reset stack and stop execution ? ...
			stackDumped();
		}


		return info;
	}

	StackElement CVirtualMachine::execute_internal(
			CScriptFunction 		* info_function,
			CScriptVariable       	* this_object,
			bool & error,
			StackElement 		  	* _ptrStartOp,
			std::string 		  		  	* _ptrStartStr,
			unsigned char n_args,
			OpCodeInstruction *calling_instruction){

		error=false;
		StackElement callc_result={0,0,STK_PROPERTY_TYPE_UNDEFINED};

		std::string *ptrStartStr;
		StackElement *ptrStartOp;

		if(info_function == NULL){
			RETURN_ERROR;
		}

		std::vector<VariableSymbolInfo> * local_var=&info_function->m_variable;

		ptrStartOp =_ptrStartOp;
		ptrStartStr =_ptrStartStr;

		if(ptrStartStr == NULL){
			ptrStartStr=stkString;
		}

		StackElement *ptrLocalVar=NULL;
		StackElement *ptrArg=NULL;
		tVM_ScopeInfo * ptrStartScopeInfo=NULL;

		zs_print_debug_cr("Executing function %s ...",info_function->symbol_info.symbol->name.c_str());
		int idxBaseStk=(ptrStartOp-stack);//>>sizeof(StackElement *);

		if(idxBaseStk<n_args){
			write_error(INSTRUCTION_GET_FILE_LINE(info_function,calling_instruction),"Internal error (idxBaseStk<n_args) (%i<%i)",idxBaseStk,n_args);
			RETURN_ERROR;
		}

		if(n_args>0){
			ptrArg=(ptrStartOp-n_args);
		}

		ptrLocalVar=ptrStartOp;

		if(idxCurrentStack < MAX_FUNCTION_CALL){

			/* tVM_ScopeInfo		scope_info[VM_LOCAL_VAR_MAX_STACK];
			std::string 				stkString[VM_LOCAL_VAR_MAX_STACK]; // aux values for std::string ...
			std::string              *ptrLastStr;
			std::string              *ptrCurrentStr;

			 StackElement     stack[VM_LOCAL_VAR_MAX_STACK];*/

			//printf("call:%i left:%lu\n",idxCurrentStack,MAX_SCOPE_INFO-current_scope_info_ptr);// scope_info[VM_LOCAL_VAR_MAX_STACK-1]);
			idxCurrentStack++;
		}
		else{
			write_error(INSTRUCTION_GET_FILE_LINE(info_function,calling_instruction),"Reached max stack");
			RETURN_ERROR;
		}

		if((info_function->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) ){ // C-Call

			if((info_function->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_IS_POLYMORPHIC)){ // cannot call...
				write_error(INSTRUCTION_GET_FILE_LINE(info_function,calling_instruction),"Function \"%s%s\" derives from polymorphic class and cannot be executed due pointer changes at runtime. You have two options:\n"
						"1. Set register_C_baseSymbols(false) and  re-register the function using REGISTER_C_FUNCTION_MEMBER\n"
						"2. Adapt all virtual functions/classes to no non-virtual\n"
						,this_object==NULL?"":this_object->idx_class!=IDX_CLASS_MAIN?(this_object->getClassName()+"::").c_str():""
						,info_function->symbol_info.symbol->name.c_str());
				RETURN_ERROR;
			}

			intptr_t  fun_ptr = info_function->symbol_info.ref_ptr;

			if((info_function->symbol_info.properties &  SYMBOL_INFO_PROPERTY::PROPERTY_STATIC_REF) != SYMBOL_INFO_PROPERTY::PROPERTY_STATIC_REF){ // if not static then is function depends of object ...

				if(this_object!= NULL){// && this_object != NULL){//CZetScript::getInstance()->getMainObject()){
					fun_ptr = this_object->getFunctionSymbolByIndex(info_function->symbol_info.idxSymbol)->proxy_ptr;
				}
			}

			StackElement se = call_C_function(fun_ptr,info_function,error,ptrArg,n_args, calling_instruction);

			if(idxCurrentStack > 0){
				idxCurrentStack--;
			}
			else{
				write_error(INSTRUCTION_GET_FILE_LINE(info_function,calling_instruction),"Reached min stack");
				RETURN_ERROR;
			}


			return se;
		}

		short scope_index = info_function->symbol_info.symbol->idxScope;// ast->idxScope;


		if(info_function->idxScriptFunction != 0){

			PUSH_SCOPE(scope_index,info_function,ptrLocalVar,0);
			ptrStartScopeInfo = current_scope_info_ptr;
		}

		callc_result ={ NULL,NULL,STK_PROPERTY_TYPE_UNDEFINED};

		//=========================================
		// PUSH STACK
		// reserve vars and assign argv vars ...
		unsigned size_local_var=local_var->size();
		unsigned n_total_vars=n_args+size_local_var;

		if((idxBaseStk+n_total_vars) >=  VM_LOCAL_VAR_MAX_STACK){
			write_error(INSTRUCTION_GET_FILE_LINE(info_function,calling_instruction),"Error MAXIMUM stack size reached");
			RETURN_ERROR;
		}

		// init local vars ...
		if(info_function->idxScriptFunction != 0){ // is not main function, so we have to initialize vars.


			if(idxCurrentStack > 1){ // not global vars, then initialize variables as undefined...
				StackElement *ptr_aux = ptrLocalVar;
				for(unsigned i = 0; i < size_local_var; i++){

					// if C then pass its properties...
					*ptr_aux++={

							0,							 // no value assigned.
							0, 						     // no varref related.
							STK_PROPERTY_TYPE_UNDEFINED // starts undefined.
					};
				}
			}
		}

		ptrStartOp=&ptrLocalVar[size_local_var];

		// PUSH STACK
		//=========================================
		//bool	aux_boolean=false;
		float aux_float=0.0;


		CScriptFunction * aux_function_info=NULL;
		StackElement ret_obj;

		CScriptVariable *svar=NULL;
		CScriptVariable *calling_object=NULL;
//		CScriptClass *script_class_aux;



		StackElement *stkResultOp1=NULL;
		StackElement *stkResultOp2=NULL;
		StackElement *ldrVar;
		unsigned short pre_post_properties=0;
		unsigned short properties=0;
		StackElement *variable_stack_element;
		FunctionSymbol *si;
		CScriptVariable *var_object = NULL;

		unsigned short scope_type=0;

		StackElement *dst_ins=NULL;
		StackElement *src_ins=NULL;
		bool ok=false;

		OpCodeInstruction *start_it=info_function->instruction;
		OpCodeInstruction *instruction_it=start_it;

		ptrCurrentStr=ptrStartStr;
		stkCurrentData=ptrStartOp;


		//-----------------------------------------------------------------------------------------------------------------------
		//
		// START OPERATOR MANAGEMENT
		//

		 for(;;){ // foreach asm instruction ...

			OpCodeInstruction * instruction = instruction_it++;

			const unsigned char operator_type=instruction->op_code;
			const unsigned char index_op1=instruction->index_op1;

			switch(operator_type){
			//default:
			//	write_error(INSTRUCTION_GET_FILE_LINE(instruction->idxAstNode),"operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
			//	RETURN_ERROR;
			case END_FUNCTION:
				goto lbl_exit_function;
			case LOAD:
			case VGET:

				if(index_op1==LOAD_TYPE::LOAD_TYPE_VARIABLE || operator_type==VGET){
					if(operator_type==VGET){

						bool ok=false;

						POP_TWO;

						if( (stkResultOp1->properties & (STK_PROPERTY_TYPE_SCRIPTVAR | STK_PROPERTY_IS_STACKVAR)) == (STK_PROPERTY_TYPE_SCRIPTVAR | STK_PROPERTY_IS_STACKVAR)){
							var_object = (CScriptVariable *)(((StackElement *)stkResultOp1->varRef)->varRef);
						}

						if(var_object != NULL){
							if(var_object->idx_class == IDX_CLASS_VECTOR){
								CVectorScriptVariable * vec = (CVectorScriptVariable *)var_object;

								if(IS_INT(stkResultOp2->properties)){
									// determine object ...
									int v_index = LOAD_INT_OP(stkResultOp2);

									// check indexes ...
									if(v_index < 0){
										write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Negative index std::vector (%i)",v_index);
										RETURN_ERROR;
									}

									if(v_index >= (int)(vec->m_variable.size())){
										write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Index std::vector out of bounds (%i)",v_index);
										RETURN_ERROR;
									}

									ldrVar = &vec->m_variable[v_index];;
									ok = true;
								}else{
									write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Expected std::vector-index as integer or std::string");
									RETURN_ERROR;
								}
							}
						}

						if(!ok){
							write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Variable \"%s\" is not type std::vector",
								INSTRUCTION_GET_SYMBOL_NAME(info_function,&info_function->instruction[instruction->index_op2])
							);
							RETURN_ERROR;
						}

					}else{ // load variable ...

						properties=instruction->properties;
						scope_type=GET_INS_PROPERTY_SCOPE_TYPE(properties);

						switch(scope_type){
						default: // global...
							ldrVar = &stack[instruction->index_op2];
							break;
						case INS_PROPERTY_ACCESS_SCOPE:
						case INS_PROPERTY_THIS_SCOPE:

							if(scope_type == INS_PROPERTY_ACCESS_SCOPE){
								POP_ONE; // get var op1 and symbol op2

								OpCodeInstruction *previous_ins= (instruction-1);

								if((stkResultOp1->properties & STK_PROPERTY_TYPE_SCRIPTVAR)!= STK_PROPERTY_TYPE_SCRIPTVAR)
								{

									write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"var \"%s\" is not scriptvariable",INSTRUCTION_GET_SYMBOL_NAME(info_function,previous_ins));
									RETURN_ERROR;
								}


								CScriptVariable  * base_var = (CScriptVariable  *)stkResultOp1->varRef;
								if(stkResultOp1->properties & STK_PROPERTY_IS_STACKVAR) {
									StackElement *stk_ins=((StackElement *)stkResultOp1->varRef);

									if(stk_ins->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
										base_var=((CScriptVariable *)stk_ins->varRef);
									}
								}

								if(base_var == NULL)
								{
									write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"var \"%s\" is not scriptvariable",INSTRUCTION_GET_SYMBOL_NAME(info_function,previous_ins));

									RETURN_ERROR;
								}

								if((variable_stack_element = base_var->getVariableSymbol(INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)))==NULL){

									write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Variable \"%s\" as type \"%s\" has not symbol \"%s\"",INSTRUCTION_GET_SYMBOL_NAME(info_function,previous_ins),base_var->getClassName().c_str(), INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction));
									RETURN_ERROR;
								}
							}
							else{ // this scope ...
								if((variable_stack_element = this_object->getVariableSymbolByIndex(instruction->index_op2))==NULL){
									write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot find symbol \"this.%s\"",INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction));
									RETURN_ERROR;
								}
							}
							ldrVar=variable_stack_element;
							break;
						case INS_PROPERTY_LOCAL_SCOPE:
							ldrVar = &ptrLocalVar[instruction->index_op2];
							break;
						}

						if(instruction->properties&INS_CHECK_IS_FUNCTION){
							if((ldrVar->properties & STK_PROPERTY_TYPE_FUNCTION) != STK_PROPERTY_TYPE_FUNCTION){
								write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"\"%s\" expected function variable but is \"%s\""
										, INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)
										, STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ldrVar));
								RETURN_ERROR;
							}
						}
					}

					pre_post_properties = GET_INS_PROPERTY_PRE_POST_OP(properties);

					/* all preoperators makes load var as constant ... */
					switch(pre_post_properties){
					case INS_PROPERTY_PRE_INC:
							PERFORM_PRE_POST_OPERATOR(ldrVar,++);
							(*stkCurrentData++)=*ldrVar;
							continue;
					case INS_PROPERTY_PRE_DEC:
							PERFORM_PRE_POST_OPERATOR(ldrVar,--);
							(*stkCurrentData++)=*ldrVar;
							continue;
					case INS_PROPERTY_POST_DEC:
							(*stkCurrentData++)=*ldrVar;
							PERFORM_PRE_POST_OPERATOR(ldrVar,--);
							continue;
					case INS_PROPERTY_POST_INC:
							(*stkCurrentData++)=*ldrVar;
							PERFORM_PRE_POST_OPERATOR(ldrVar,++);
							continue;
					case INS_PROPERTY_PRE_NEG:
							switch(GET_INS_PROPERTY_VAR_TYPE(ldrVar->properties)){
							case STK_PROPERTY_TYPE_INTEGER:
								if(ldrVar->properties& STK_PROPERTY_IS_C_VAR){
									*stkCurrentData++={(void *)(-(*((intptr_t *)ldrVar->varRef))),ldrVar,STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_IS_STACKVAR|STK_PROPERTY_IS_C_VAR};
								}else{
									*stkCurrentData++={(void *)(-(((intptr_t)ldrVar->stkValue))),ldrVar,STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_IS_STACKVAR};
								}
								break;
							case STK_PROPERTY_TYPE_BOOLEAN:
								if(ldrVar->properties& STK_PROPERTY_IS_C_VAR){
									*stkCurrentData++={(void *)(!(*((bool *)ldrVar->varRef))),ldrVar,STK_PROPERTY_TYPE_BOOLEAN|STK_PROPERTY_IS_STACKVAR|STK_PROPERTY_IS_C_VAR};
								}else{
									*stkCurrentData++={(void *)(!(((bool)ldrVar->stkValue))),ldrVar,STK_PROPERTY_TYPE_BOOLEAN|STK_PROPERTY_IS_STACKVAR};
								}
								break;

							case STK_PROPERTY_TYPE_NUMBER:
								if(ldrVar->properties& STK_PROPERTY_IS_C_VAR){
									COPY_NUMBER(&aux_float,ldrVar->varRef);
								}else{
									COPY_NUMBER(&aux_float,&ldrVar->stkValue);
								}
								aux_float=-aux_float;
								COPY_NUMBER(&stkCurrentData->stkValue,&aux_float);
								*stkCurrentData={stkCurrentData->stkValue,ldrVar,STK_PROPERTY_TYPE_NUMBER|STK_PROPERTY_IS_STACKVAR};

								if(ldrVar->properties& STK_PROPERTY_IS_C_VAR){
									stkCurrentData->properties|=STK_PROPERTY_IS_C_VAR;
								}

								stkCurrentData++;
								break;
							default:
								write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"internal error:cannot perform pre operator - because is not number");
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
							//(stkCurrentData-1)->stkValue=(void *)((*((int *)ldrVar->varRef)));
							memcpy(&((stkCurrentData-1)->stkValue),ldrVar->varRef,sizeof(int));
							break;
						case STK_PROPERTY_TYPE_NUMBER:
							COPY_NUMBER(&((stkCurrentData-1)->stkValue),ldrVar->varRef);
							break;
						case STK_PROPERTY_TYPE_BOOLEAN:
							(stkCurrentData-1)->stkValue=(void *)((*((bool *)ldrVar->varRef)));
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
					(*stkCurrentData)=*(((ConstantValueInfo *)instruction->index_op2));

					pre_post_properties = GET_INS_PROPERTY_PRE_POST_OP(instruction->properties);

					// all preoperators makes load var as constant ...
					switch(pre_post_properties){
					case INS_PROPERTY_PRE_NEG:
							switch(GET_INS_PROPERTY_VAR_TYPE(stkCurrentData->properties)){
							case STK_PROPERTY_TYPE_INTEGER:
								stkCurrentData->stkValue=(void *)(-((intptr_t)stkCurrentData->stkValue));
								break;
							case STK_PROPERTY_TYPE_BOOLEAN:
								stkCurrentData->stkValue=(void *)(!((bool)stkCurrentData->stkValue));
								break;
							case STK_PROPERTY_TYPE_NUMBER:
								COPY_NUMBER(&aux_float,&stkCurrentData->stkValue);
								aux_float=-aux_float;
								COPY_NUMBER(&stkCurrentData->stkValue,&aux_float);
								break;
							default:
								write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"internal error:cannot perform pre operator - constant because is not numeric");
								RETURN_ERROR;
							}
							break;
					}
					stkCurrentData++;
					continue;
				}else if(index_op1== LOAD_TYPE::LOAD_TYPE_FUNCTION){

					unsigned short extra_flags=(instruction->properties&INS_PROPERTY_CONSTRUCT_CALL)?STK_PROPERTY_CONSTRUCTOR_FUNCTION:0;
					extra_flags|=(instruction->properties&INS_PROPERTY_NO_FUNCTION_CALL) ?STK_PROPERTY_UNRESOLVED_FUNCTION:0;
					//void *function_obj=NULL;
					std::vector<CScriptFunction *> *vec_functions;
					CScriptVariable * class_obj=NULL;
					intptr_t function_obj =  instruction->index_op2;
					intptr_t index_op2 = instruction->index_op2;
					properties=instruction->properties;
					scope_type=GET_INS_PROPERTY_SCOPE_TYPE(properties);

					/*if(scope_type==INS_PROPERTY_LOCAL_SCOPE){ // local gets functions from info_function ...
						vec_functions=&info_function->m_function;
					}else*/
					if(scope_type == INS_PROPERTY_ACCESS_SCOPE){
						StackElement *var=NULL;
						if(properties & INS_PROPERTY_CONSTRUCT_CALL){
							var=(stkCurrentData-1);
						}else{
							POP_ONE;
							var = stkResultOp1;
						}

						StackElement *stk_ins=NULL;
						if(var->properties & STK_PROPERTY_IS_STACKVAR) {
							stk_ins=((StackElement *)var->varRef);
						}
						else{
							stk_ins=var;
						}

						if(stk_ins->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
							class_obj=(CScriptVariable *)(stk_ins->varRef);
							CScriptClass *sc =GET_SCRIPT_CLASS(((CScriptVariable *)class_obj)->idx_class);
							//vec_functions=&sc->m_function;
						}
						else{
							write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Cannot access symbol \"%s\" (type of %s is %s)",INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction),INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction-1),STR_GET_TYPE_VAR_INDEX_INSTRUCTION(stk_ins));
							RETURN_ERROR;
						}


						/*if(index_op2 == 0){//ZS_UNDEFINED_IDX || index_op2 == ZS_FUNCTION_NOT_FOUND_IDX){ // is will be processed after in CALL instruction ...
							function_obj= 0;//(void *)instruction; // saves current instruction in order to resolve its idx later (in call instruction)
						}else{
							function_obj= index_op2;
						}*/
						/*else if((index_op2<(int)vec_functions->size())) // get the function ...
						{
							function_obj = (*vec_functions)[index_op2];

						}
						else{
							write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot find symbol global \"%s\""
									,INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)
									);
							RETURN_ERROR;
						}*/
					}/*else if(scope_type ==INS_PROPERTY_THIS_SCOPE){
						if((si = this_object->getFunctionSymbolByIndex(index_op2))==NULL){
							write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot find function \"this.%s\"",INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction));
							RETURN_ERROR;
						}

						function_obj =(CScriptFunction *)si->object.stkValue;

					}else if(scope_type == INS_PROPERTY_SUPER_SCOPE){ // super scope ?
						if((si = this_object->getFunctionSymbolByIndex(index_op2))==NULL){
							write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot find function \"super.%s\"",INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction));
							RETURN_ERROR;
						}
						function_obj =(CScriptFunction *)si->object.stkValue;
					}else{ // global
						vec_functions = &(main_function_object->m_function);
					}


					if(!(scope_type == INS_PROPERTY_THIS_SCOPE || scope_type == INS_PROPERTY_SUPER_SCOPE)){

						if(index_op2 == ZS_UNDEFINED_IDX || index_op2 == ZS_FUNCTION_NOT_FOUND_IDX){ // is will be processed after in CALL instruction ...
							function_obj= NULL;//(void *)instruction; // saves current instruction in order to resolve its idx later (in call instruction)
						}else if((index_op2<(int)vec_functions->size())) // get the function ...
						{
							function_obj = (*vec_functions)[index_op2];

						}
						else{
							write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot find symbol global \"%s\""
									,INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)
									);
							RETURN_ERROR;
						}
					}*/

					if(index_op2 == 0){
						*stkCurrentData++={instruction,class_obj,(unsigned short)(STK_PROPERTY_IS_INSTRUCTIONVAR|STK_PROPERTY_TYPE_FUNCTION|extra_flags)};
					}else{
						PUSH_FUNCTION(index_op2,class_obj,extra_flags);
					}
					continue;

				}else if(index_op1== LOAD_TYPE::LOAD_TYPE_ARGUMENT){
					*stkCurrentData++=ptrArg[instruction->index_op2]; // copy arg directly ...
					continue;
				}
				else{

					write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"runtime error. Try to restart");
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
						POP_ONE; // only pops the value, the last is the std::vector variable itself
						CScriptVariable *vec_obj = NULL;
						if((stkCurrentData-1)->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
							vec_obj = (CScriptVariable *)(stkCurrentData-1)->varRef;
							if(vec_obj->idx_class == IDX_CLASS_VECTOR){ // push value ...
								// op1 is now the src value ...
								src_ins=stkResultOp1;
								dst_ins=((CVectorScriptVariable *)vec_obj)->push();
								ok=true;
							}
						}

						if(!ok){
							write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Expected std::vector object");
							RETURN_ERROR;
						}

						push_value=false;

					}else if(operator_type==PUSH_ATTR){


						POP_TWO; // first must be the value name and the other the variable name ...
						CScriptVariable *struct_obj = NULL;
						if((stkCurrentData-1)->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
							struct_obj = (CScriptVariable *)(stkCurrentData-1)->varRef;
							if(struct_obj->idx_class == IDX_CLASS_STRUCT){ // push value ...
								// op1 is now the src value ...
								if(stkResultOp2->properties & STK_PROPERTY_TYPE_STRING){
									StackElement *se=NULL;
									const char *str = (const char *)stkResultOp2->stkValue;
									src_ins=stkResultOp1;
									if(src_ins->properties&STK_PROPERTY_TYPE_FUNCTION){
										FunctionSymbol *si =((CStructScriptVariable *)struct_obj)->addFunctionSymbol(str,NULL);
										if(si!=NULL){
											se=&si->object;;
										}
									}else{
										se =((CStructScriptVariable *)struct_obj)->addVariableSymbol(str);
									}

									if(se == NULL){
										RETURN_ERROR;
									}

									dst_ins=se;
									ok=true;
								}
								else{
									write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"internal error (operator2 is not std::string)");
									RETURN_ERROR;
								}
							}else{
								write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Expected struct object");
								RETURN_ERROR;
							}
						}else{
							write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Expected scriptvar");
							RETURN_ERROR;
						}
						push_value=false;
					}
					else{ // pop two parameters nothing ...
						POP_TWO;

						if(stkResultOp1->properties & STK_PROPERTY_IS_STACKVAR) {// == CScriptVariable::VAR_TYPE::OBJECT){
							dst_ins=(StackElement *)stkResultOp1->varRef; // stkValue is expect to contents a stack variable
						}else{
							write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Expected l-value on assignment ('=')");
							RETURN_ERROR;
						}

						if(current_foreach!=NULL){
							if(dst_ins == current_foreach->key){
								write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"for-in key value is read only");
								RETURN_ERROR;
							}
						}

						src_ins=stkResultOp2; // store ptr instruction2 op as src_var_value

						// we need primitive stackelement in order to assign...
						if(src_ins->properties & STK_PROPERTY_IS_STACKVAR) {// == CScriptVariable::VAR_TYPE::OBJECT){
							src_ins=(StackElement *)src_ins->varRef; // stkValue is expect to contents a stack variable

						}

						// ok load object pointer ...
						if(dst_ins->properties & STK_PROPERTY_TYPE_SCRIPTVAR){

							if(((CScriptVariable *)dst_ins->varRef)->itHasSetMetamethod()){
								if(!APPLY_METAMETHOD(
										calling_object
										,info_function
										,instruction
										,"="
										,SET_METAMETHOD
										,stkResultOp1
										,stkResultOp2
								)){
									RETURN_ERROR;
								}
								assign_metamethod=true;
							}
						}
					}

					if(! assign_metamethod){

						if(dst_ins->properties & STK_PROPERTY_IS_THIS_VAR){
							write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"\"this\" is not assignable");
							RETURN_ERROR;
						}

						StackElement old_dst_ins = *dst_ins; // save dst_var to check after assignment...

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
						case STK_PROPERTY_TYPE_STRING: // type std::string is really a std::string or variable ?!?!
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
						*stkCurrentData++=*src_ins;
					}
				}
				continue;

			case EQU:  // ==

				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
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
				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
					READ_TWO_POP_ONE;
				}else{
					POP_TWO;
				}

				PROCESS_LOGIC_OPERATION(&&);
				continue;
			case LOGIC_OR:  // ||

				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
					READ_TWO_POP_ONE;
				}else{
					POP_TWO;
				}
				PROCESS_LOGIC_OPERATION(||);
				continue;
			case NOT: // !

				POP_ONE;

				if(stkResultOp1->properties & STK_PROPERTY_TYPE_BOOLEAN){ // operation will result as integer.
					PUSH_BOOLEAN((!((bool)(stkResultOp1->stkValue))));
				}else{
					if(!APPLY_METAMETHOD(
							calling_object
							,info_function
							,instruction
							,"!"
							,NOT_METAMETHOD
							,stkResultOp1
							,stkResultOp2
							)){
						//write_error(INSTRUCTION_GET_FILE_LINE(instruction->idxAstNode),"Expected operands 1 as boolean!");
						RETURN_ERROR;
					}
				}
				continue;

			case NEG: // -

				POP_ONE;

				if(stkResultOp1->properties & STK_PROPERTY_TYPE_INTEGER){ // operation will result as integer.
					PUSH_INTEGER((-((intptr_t)(stkResultOp1->stkValue))));
				}else if(stkResultOp1->properties & STK_PROPERTY_TYPE_NUMBER){
					COPY_NUMBER(&f_aux_value1,&stkResultOp1->stkValue);
					PUSH_NUMBER(-f_aux_value1);
				/*}else if(stkResultOp1->properties & STK_PROPERTY_TYPE_BOOLEAN){
					PUSH_BOOLEAN((!((bool)(stkResultOp1->stkValue))));
				*/
				}else{ // try metamethod ...
						if(!APPLY_METAMETHOD(
								 calling_object
								,info_function
								,instruction
								,"-"
								,NEG_METAMETHOD
								,stkResultOp1
								,stkResultOp2
								)){
							RETURN_ERROR;
						}

				}
				continue;

			case ADD: // +

			{
				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
					READ_TWO_POP_ONE;
				}else{
					POP_TWO;
				}


				unsigned short mask_properties =GET_INS_PROPERTY_PRIMITIVE_TYPES(stkResultOp1->properties&stkResultOp2->properties);
				unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stkResultOp1->properties|stkResultOp2->properties);
				if(mask_properties==STK_PROPERTY_TYPE_INTEGER){
						PUSH_INTEGER(LOAD_INT_OP(stkResultOp1) + LOAD_INT_OP(stkResultOp2));
				}
				else if(properties==(STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_TYPE_NUMBER)){
						if (IS_INT(stkResultOp1->properties) && IS_NUMBER(stkResultOp2->properties)){
							COPY_NUMBER(&f_aux_value2,&stkResultOp2->stkValue);
							PUSH_NUMBER(LOAD_INT_OP(stkResultOp1) + f_aux_value2);
						}else{
							COPY_NUMBER(&f_aux_value1,&stkResultOp1->stkValue);
							PUSH_NUMBER(f_aux_value1 + LOAD_INT_OP(stkResultOp2));
						}
				}
				else if(properties==(STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_TYPE_STRING)){
					if (IS_STRING(stkResultOp1->properties) && IS_INT(stkResultOp2->properties)){
						sprintf(str_aux,"%s%i",((const char *)stkResultOp1->stkValue),(int)((intptr_t)stkResultOp2->stkValue));
						//PUSH_STRING(str_aux);
						if(ptrCurrentStr==ptrLastStr){
								write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Error stkString out-stack");RETURN_ERROR;}\
								*ptrCurrentStr++=str_aux;\
								*stkCurrentData++={(void *)((ptrCurrentStr-1)->c_str()),NULL,STK_PROPERTY_TYPE_STRING};\

					}else{
						sprintf(str_aux,"%i%s",(int)((intptr_t)stkResultOp1->stkValue),((const char *)stkResultOp2->stkValue));
						PUSH_STRING(str_aux);
					}
				}else if(properties==(STK_PROPERTY_TYPE_NUMBER|STK_PROPERTY_TYPE_STRING)){
					if (IS_STRING(stkResultOp1->properties) && IS_NUMBER(stkResultOp2->properties)){
						COPY_NUMBER(&f_aux_value2,&stkResultOp2->stkValue);
						sprintf(str_aux,"%s%f",((const char *)stkResultOp1->stkValue),f_aux_value2);
						PUSH_STRING(str_aux);

					}else{
						COPY_NUMBER(&f_aux_value1,&stkResultOp1->stkValue);
						sprintf(str_aux,"%f%s",f_aux_value1,((const char *)stkResultOp2->stkValue));
						PUSH_STRING(str_aux);
					}
				}else if(properties==(STK_PROPERTY_TYPE_UNDEFINED|STK_PROPERTY_TYPE_STRING)){
					if (IS_STRING(stkResultOp1->properties) && IS_UNDEFINED(stkResultOp2->properties)){
						sprintf(str_aux,"%s%s",((const char *)stkResultOp1->stkValue),"undefined");
						PUSH_STRING(str_aux);
					}else{
						sprintf(str_aux,"%s%s","undefined",((const char *)stkResultOp2->stkValue));
						PUSH_STRING(str_aux);
					}

				}else if(properties==(STK_PROPERTY_TYPE_BOOLEAN|STK_PROPERTY_TYPE_STRING)){
					if (IS_STRING(stkResultOp1->properties) && IS_BOOLEAN(stkResultOp2->properties)){
						sprintf(str_aux,"%s%s",((const char *)stkResultOp1->stkValue),((bool)(stkResultOp2->stkValue))?"true":"false");
						PUSH_STRING(str_aux);
					}else{
						sprintf(str_aux,"%s%s",((bool)(stkResultOp1->stkValue))?"true":"false",((const char *)stkResultOp2->stkValue));
						PUSH_STRING(str_aux);
					}

				}else if (mask_properties== STK_PROPERTY_TYPE_STRING){
						sprintf(str_aux,"%s%s",((const char *)stkResultOp1->stkValue),((const char *)(stkResultOp2->stkValue)));
						PUSH_STRING(str_aux);


				}else if(mask_properties== STK_PROPERTY_TYPE_NUMBER){
					COPY_NUMBER(&f_aux_value1,&stkResultOp1->stkValue);\
					COPY_NUMBER(&f_aux_value2,&stkResultOp2->stkValue);\
					PUSH_NUMBER(f_aux_value1 + f_aux_value2);\

				}
				else{ // try metamethod ...

					if(!APPLY_METAMETHOD(
							 calling_object
							,info_function
							,instruction
							,"+"
							,ADD_METAMETHOD
							,stkResultOp1
							,stkResultOp2
							)){
						RETURN_ERROR;
					}
				}
			}
				continue;

			case MUL: // *

				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_ARITHMETIC_OPERATION(*,MUL_METAMETHOD);
				continue;

			case DIV: // /

				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_ARITHMETIC_OPERATION(/, DIV_METAMETHOD);
				continue;

			 case MOD: // /

				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_MOD_OPERATION;
				continue;

			 case AND: // &
				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_BINARY_OPERATION(&, AND_METAMETHOD);
				continue;
			 case OR: // *
				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_BINARY_OPERATION(|, OR_METAMETHOD);
				continue;
			 case XOR: // ^
				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_BINARY_OPERATION(^, XOR_METAMETHOD);
				continue;

			 case SHR: // >>
				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_BINARY_OPERATION(>>,SHR_METAMETHOD);
				continue;

			 case SHL: // <<
				if(instruction->properties&STK_PROPERTY_READ_TWO_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_BINARY_OPERATION(<<, SHL_METAMETHOD);
				continue;
			 case JMP:
				instruction_it=&start_it[instruction->index_op2];
				continue;
			 case INSTANCEOF: // check instance of ...
				 POP_TWO;

				if(stkResultOp1->properties & STK_PROPERTY_IS_STACKVAR) {// == CScriptVariable::VAR_TYPE::OBJECT){
					stkResultOp1=(StackElement *)stkResultOp1->varRef; // stkValue is expect to contents a stack variable
				}

				switch((intptr_t)stkResultOp2->stkValue){
					case IDX_TYPE_INT_PTR_C:
						PUSH_BOOLEAN((stkResultOp1->properties & STK_PROPERTY_TYPE_INTEGER)!=0);
						break;
					case IDX_TYPE_FLOAT_PTR_C:
						PUSH_BOOLEAN((stkResultOp1->properties & STK_PROPERTY_TYPE_NUMBER)!=0);
						break;
					case IDX_TYPE_BOOL_PTR_C:
						PUSH_BOOLEAN((stkResultOp1->properties & STK_PROPERTY_TYPE_BOOLEAN)!=0);
						break;
					case IDX_TYPE_STRING_PTR_C:
						PUSH_BOOLEAN((stkResultOp1->properties & STK_PROPERTY_TYPE_STRING)!=0);
						break;
					case IDX_CLASS_FUNCTOR:
						PUSH_BOOLEAN((stkResultOp1->properties & STK_PROPERTY_TYPE_FUNCTION)!=0);
						break;
					default:
						if(stkResultOp1->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
							bool b = SCRIPT_CLASS_FACTORY->isIdxClassInstanceOf(((CScriptVariable *)(stkResultOp1->varRef))->idx_class, (intptr_t)stkResultOp2->stkValue);
							PUSH_BOOLEAN(b);
						}else{
							PUSH_BOOLEAN(false);
						}
						break;
					}


				 continue;
			 case JNT: // goto if not true ... goes end to conditional.
				POP_ONE;
				if(stkResultOp1->stkValue == 0){
					instruction_it=&start_it[instruction->index_op2];
				}
				continue;
			 case JT: // goto if true ... goes end to conditional.
				POP_ONE;
				if(stkResultOp1->stkValue != 0){
					instruction_it=&start_it[instruction->index_op2];
				}
				continue;
			 case  CALL: // calling function after all of args are processed...
				// check whether signatures matches or not ...
				// 1. get function object ...
			 	 {
					aux_function_info = NULL;
					unsigned char n_args=0;


					bool is_c = false;
					StackElement *startArg=stkCurrentData;
					StackElement *callAle=NULL;

					while(n_args <= MAX_N_ARGS && (((startArg-1)->properties&STK_PROPERTY_TYPE_FUNCTION)==0)){
						startArg--;
						n_args++;
					}

					callAle = ((startArg-1));
					calling_object = this_object;
					if(callAle->varRef!=NULL){
						calling_object=(CScriptVariable *)callAle->varRef;
					}


					aux_function_info = NULL;//(CScriptFunction *)callAle->stkValue;


					bool is_constructor = (callAle->properties & STK_PROPERTY_CONSTRUCTOR_FUNCTION)!=0;

					// try to find the function ...
					if(((callAle)->properties & STK_PROPERTY_IS_INSTRUCTIONVAR)){// || deduce_function){
						OpCodeInstruction *callAleInstruction = (OpCodeInstruction *)(callAle)->stkValue;
						//PASTNode ast_node_call_ale = vec_ast_node[iao->idxAstNode];

						symbol_to_find = INSTRUCTION_GET_SYMBOL_NAME(info_function,callAleInstruction);
						unsigned short scope_type = GET_INS_PROPERTY_SCOPE_TYPE(callAleInstruction->properties);


						// local vars as functions ...

						// registered symbols in case is INS_PROPERTY_ACCESS_SCOPE...
						std::vector<FunctionSymbol> *m_functionSymbol=NULL;
						if(scope_type==INS_PROPERTY_ACCESS_SCOPE){
							calling_object = (CScriptVariable *)callAle->varRef;

							// we have to no to call default constructor...is implicit
							if(is_constructor) {

								is_c=calling_object->is_c_object();

								if(n_args == 0 && is_c){
									aux_function_info = NULL;
									callAleInstruction->properties |= INS_PROPERTY_NO_FUNCTION_CALL;
								}
							}

							m_functionSymbol=calling_object->getVectorFunctionSymbol();
						}

						//bool all_check=true;
						if((callAleInstruction->properties & INS_PROPERTY_NO_FUNCTION_CALL)==0)
						{
							std::vector<CScriptFunction *> *vec_global_functions=&(main_function_object->m_function);
							//#define FIND_FUNCTION(iao, is_constructor, symbol_to_find,size_fun_vec,vec_global_functions,startArgs, n_args,scope_type)
							if((aux_function_info=FIND_FUNCTION(
									 calling_object
									,info_function
									,instruction
									,callAleInstruction

									,m_functionSymbol
									,vec_global_functions
									,is_constructor
									,symbol_to_find
									,stkResultOp1
									,stkResultOp2
									,startArg
									,n_args
									,NULL))==NULL){

								if((callAleInstruction->properties & INS_PROPERTY_NO_FUNCTION_CALL)==0){
									RETURN_ERROR;
								}
							}


							// saves function pointer found ...
							callAle->stkValue=aux_function_info;

						}

					}
					else{
						if(((callAle)->properties & STK_PROPERTY_UNRESOLVED_FUNCTION)==0){
							aux_function_info=(CScriptFunction *) (callAle->stkValue);
						}
					}

					if(aux_function_info !=NULL)
					{
						if(n_args > MAX_N_ARGS){
							write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Max arguments reached function at line XXX");
							RETURN_ERROR;
						}

						if((aux_function_info->symbol_info.properties & PROPERTY_C_OBJECT_REF) == 0){ // is function script ...
							unsigned aux_function_info_m_arg_size=aux_function_info->m_arg.size();
							if( n_args < aux_function_info_m_arg_size){ // we must push undefined parameters ...
								for(unsigned i = n_args; i < aux_function_info_m_arg_size; i++){
									*stkCurrentData++={

										0,							 // no value assigned.
										NULL,			     // no varref related.
										STK_PROPERTY_TYPE_UNDEFINED // starts undefined.
									};
									n_args++;
								}
							}
						}

						// by default virtual machine gets main object class in order to run functions ...
						ret_obj=execute_internal(aux_function_info,calling_object,error,stkCurrentData,ptrCurrentStr,n_args,instruction);

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
								write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),custom_error);
							}
							RETURN_ERROR;
						}

					}

					// reset stack (function+instruction (-1 op less))...
					stkCurrentData=startArg-1;

					// ... and push result if not function constructor...
					if(!is_constructor){
						*stkCurrentData++ = ret_obj;
					}
			 	 }
				continue;
			 case  NEW:
					svar=NEW_CLASS_VAR_BY_IDX(index_op1);

					if(!svar->initSharedPtr()){
						RETURN_ERROR;
					}
					svar->info_function_new=info_function;
					svar->instruction_new=instruction;
					(*stkCurrentData++)={NULL,svar,STK_PROPERTY_TYPE_SCRIPTVAR};
					continue;
			 case  DELETE_OP:
					POP_ONE;
					//svar
					if(stkResultOp1->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
						StackElement *se=stkResultOp1;
						if(stkResultOp1->properties & STK_PROPERTY_IS_STACKVAR){
							se=(StackElement *)(stkResultOp1->varRef);
						}

						svar = (CScriptVariable *)(se)->varRef;
						if(svar->idx_class >= MAX_BUILT_IN_TYPES
						 ||svar->idx_class==IDX_CLASS_VECTOR
						 ||svar->idx_class==IDX_CLASS_STRUCT
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
						write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"delete op: expected scriptvar var but it was \"%s\"",STR_GET_TYPE_VAR_INDEX_INSTRUCTION(stkResultOp1));
						RETURN_ERROR;
					}
					continue;
			 case DECL_VEC: // Create new std::vector object...
					svar=NEW_VECTOR_VAR;
					//PUSH_VAR(svar,NULL,0,false);

					if(!svar->initSharedPtr()){
						RETURN_ERROR;
					}

					(*stkCurrentData++)={NULL,svar,STK_PROPERTY_TYPE_SCRIPTVAR};

					continue;

			 case  DECL_STRUCT: // Create new std::vector object...
				svar=NEW_STRUCT_VAR;
				//PUSH_VAR(svar,NULL,0,false);

				if(!svar->initSharedPtr()){
					RETURN_ERROR;
				}

				(*stkCurrentData++)={NULL,svar,STK_PROPERTY_TYPE_SCRIPTVAR};

				continue;


			 case  RET:

				if(stkCurrentData>ptrStartOp){ // can return something
					callc_result=*(stkCurrentData-1);

					// remove shared pointer if scriptvar ...
					if(callc_result.properties & STK_PROPERTY_TYPE_SCRIPTVAR){
						if(callc_result.properties & STK_PROPERTY_IS_STACKVAR){
							callc_result=*((StackElement *)((StackElement *)callc_result.varRef));

						}

						// unref pointer to be deallocated from gc...
						//((CScriptVariable *)callc_result.varRef)->ptr_shared_pointer_node->data.shared_ptr=NULL;
						if(((CScriptVariable *)callc_result.varRef)->ptr_shared_pointer_node!=NULL){
							free(((CScriptVariable *)callc_result.varRef)->ptr_shared_pointer_node);
							((CScriptVariable *)callc_result.varRef)->ptr_shared_pointer_node=NULL;
						}
						// share pointer  + 1
					}
					else{ // remove all involved inforamtion
						callc_result.properties&=~STK_PROPERTY_IS_STACKVAR; // remove stack var if any...
						callc_result.varRef =NULL;
					}
				}
				// }
				goto lbl_exit_function;
			 case PUSH_SCOPE:

				PUSH_SCOPE(instruction->index_op2,info_function,ptrLocalVar,index_op1);

				if(index_op1 & SCOPE_PROPERTY::FOR_IN){
					if(current_foreach == &stkForeach[VM_MAX_FOREACH-1]){
						write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Max foreach reached");
						RETURN_ERROR;
					}

					current_foreach++;
				}
				continue;

			 case POP_SCOPE:
				ptrCurrentStr=ptrStartStr; // reset op ptr
				stkCurrentData=ptrStartOp;
				if(!POP_SCOPE_CALL(idxCurrentStack,NULL,index_op1)){
					write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Error pop scope");
					RETURN_ERROR;
				}

				if(index_op1 & SCOPE_PROPERTY::FOR_IN){
					if(current_foreach == &stkForeach[0]){
						write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Min foreach reached");
						RETURN_ERROR;
					}

					memset(current_foreach,0,sizeof(tForeachInfo));
					current_foreach--;
				}
				continue;

			 case IT_INI:
				 POP_TWO; // op2:std::vector or struct op1:key iterator


				 if((stkResultOp1->properties & STK_PROPERTY_IS_STACKVAR) == 0){
						write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Internal error: Expected stackvar");
						RETURN_ERROR;

				 }
				 current_foreach->key=(StackElement *)stkResultOp1->varRef;
				 current_foreach->ptr=NULL;
				 current_foreach->idx_current=0;

				 var_object = (CScriptVariable *)stkResultOp2->varRef;
				 if( (stkResultOp2->properties & (STK_PROPERTY_TYPE_SCRIPTVAR | STK_PROPERTY_IS_STACKVAR)) == (STK_PROPERTY_TYPE_SCRIPTVAR | STK_PROPERTY_IS_STACKVAR)){
				 		var_object = (CScriptVariable *)(((StackElement *)stkResultOp2->varRef)->varRef);
				 }

				if(var_object != NULL && (var_object->idx_class == IDX_CLASS_VECTOR || var_object->idx_class == IDX_CLASS_STRUCT)){

					if(var_object->idx_class == IDX_CLASS_VECTOR){ // integer as iterator...
						*current_foreach->key={0,0,STK_PROPERTY_TYPE_INTEGER};
					}
					else{ // struct -> std::string as iterator...
						*current_foreach->key={0,0,STK_PROPERTY_TYPE_STRING};
					}

					current_foreach->ptr = var_object;
				}else{
					write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Variable \"%s\" is not type std::vector or struct",
						INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction-1)
					);
					RETURN_ERROR;
				}
				 continue;
			 case IT_SET_AND_NEXT:
				 write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"TODOOOOO!",
						 	 	 	 INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)
				 					);
				 					RETURN_ERROR;
				 //*((StackElement *)current_foreach->key)=((CVectorScriptVariable *)current_foreach->ptr)->m_objVector[current_foreach->idx_current++];
				 continue;
			 case IT_CHK_END:
				 write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"TODOOOOO!",
						 	 	 	 INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)
				 					);
				 					RETURN_ERROR;

				 /*if(current_foreach->idx_current>=((CVectorScriptVariable *)current_foreach->ptr)->m_objVector.size()){ // set true...
					 PUSH_BOOLEAN(true);
				 }
				 else{ // set false...
					 PUSH_BOOLEAN(false);
				 }*/
				 continue;
			//
			// END OPERATOR MANAGEMENT
			//
			//-----------------------------------------------------------------------------------------------------------------------

			}

			write_error(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"operator type(%s) not implemented",getOpCodeStr(instruction->op_code));
			RETURN_ERROR;


		 }

	lbl_exit_function:


		//=========================
		// POP STACK
		if(info_function->idxScriptFunction == 0){ // if main function only remove 0s and preserve variables!)
			REMOVE_0_SHARED_POINTERS(idxCurrentStack,NULL);
		}
		else{
			while(ptrStartScopeInfo<=(current_scope_info_ptr)){
				POP_SCOPE_CALL(idxCurrentStack,callc_result.varRef,0);
			}
		}

		idxCurrentStack--;
		// POP STACK
		//=========================


		//popStack();
		return callc_result;
	}


	StackElement  * CVirtualMachine::getLastStackValue(){
		return (stkCurrentData-1);
	}


	CVirtualMachine::~CVirtualMachine(){
		destroyCache();
	}
}
