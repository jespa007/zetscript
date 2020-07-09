/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "ZetScript.h"

#define RETURN_ERROR\
	error=true;\
	return callc_result;




namespace zetscript{

	void  WriteError(const char *filename, int line, const  char  *string_text, ...);



	#define COPY_NUMBER(d,s)  memcpy((d),(s),sizeof(float))

	#define LOAD_INT_OP(ptr_result_instruction) \
			((int)((intptr_t)(ptr_result_instruction->stk_value)))

	#define LOAD_BOOL_OP(ptr_result_instruction) \
			(((bool)(ptr_result_instruction->stk_value)))


	#define LOAD_STRING_OP(ptr_result_instruction) \
			(((const char *)(ptr_result_instruction->stk_value)))

	// Check types
	#define IS_NUMBER(properties) \
	(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)


	#define IS_INT(properties) \
	(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER)


	#define IS_STRING(properties) \
	(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)

	#define IS_CHAR_PTR(properties) \
	(properties & STK_PROPERTY_TYPE_CHAR_PTR)


	#define IS_BOOLEAN(properties) \
	(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN)

	#define IS_UNDEFINED(properties) \
	(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED)

	#define IS_FUNCTION(properties) \
	(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)

	#define IS_VAR(properties) \
	(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)

	#define IS_VECTOR(ptr_result_instruction) \
	(( ptr_result_instruction->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR) &&\
	 (((ScriptVar *)(ptr_result_instruction->stk_value))->idx_class==IDX_BUILTIN_TYPE_CLASS_VECTOR))

	#define IS_GENERIC_NUMBER(properties) \
	((properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER) ||\
	(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT))


	#define OP1_AND_OP2_ARE_NUMBERS \
	(IS_GENERIC_NUMBER(stk_result_op1->properties) && IS_GENERIC_NUMBER(stk_result_op2->properties))

	#define OP1_IS_STRING_AND_OP2_IS_NUMBER \
	(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING) && \
	IS_GENERIC_NUMBER(stk_result_op2->type_var)

	#define OP1_IS_NUMBER_AND_OP2_IS_STRING \
	(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING) && \
	IS_GENERIC_NUMBER(stk_result_op1->type_var)

	#define OP1_IS_STRING_AND_OP2_IS_BOOLEAN \
	(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING) && \
	(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN)


	#define OP1_AND_OP2_ARE_BOOLEANS \
	(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN) && \
	(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN)

	#define OP1_AND_OP2_ARE_STRINGS \
	(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING) && \
	(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)

	#define OP1_AND_OP2_ARE_UNDEFINED \
			(stk_result_op1->stk_value == VM_UNDEFINED) && \
			(stk_result_op2->stk_value == VM_UNDEFINED)

	#define OP1_AND_OP2_ARE_NULL \
			(stk_result_op1->stk_value == VM_NULL) && \
			(stk_result_op2->stk_value == VM_NULL)



	#define OP1_OR_OP2_IS_UNDEFINED \
			(stk_result_op1->stk_value == VM_UNDEFINED) || \
			(stk_result_op2->stk_value == VM_UNDEFINED)

	#define OP1_OR_OP2_ARE_NULL \
			(stk_result_op1->stk_value == VM_NULL) || \
			(stk_result_op2->stk_value == VM_NULL)


	#define PUSH_UNDEFINED \
	*stk_current_data++={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

	#define PUSH_NULL \
	*stk_current_data++={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL};

	#define PUSH_BOOLEAN(init_value) \
	*stk_current_data++={(void *)((intptr_t)(init_value)),NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN};

	#define PUSH_INTEGER(init_value) \
	*stk_current_data++={(void *)((intptr_t)(init_value)),NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER};

	#define PUSH_NUMBER(init_value) \
	{\
		float aux=(float)(init_value);\
		COPY_NUMBER(&stk_current_data->stk_value,&aux);\
		stk_current_data->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;\
		stk_current_data++;\
	}

	#define PUSH_STRING(init_value)\
		if(ptr_current_str==ptr_last_str){WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Error stk_string out-stack");RETURN_ERROR;}\
		*ptr_current_str++=init_value;\
		*stk_current_data++={(void *)((ptr_current_str-1)->c_str()),NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};\



	#define PUSH_FUNCTION(fun_obj,class_obj,function_prop) \
	*stk_current_data++={(void *)(fun_obj),class_obj,(unsigned short)(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION|function_prop)};

	#define PUSH_SCRIPTVAR(var_ref) \
	*stk_current_data++={NULL,var_ref,INS_PROPERTY_SCRIPTVAR};


	// Push stack var value (as varValue and put ptr as ref second value)...
	#define PUSH_STACK_VAR(stack_ref_var) \
		*stk_current_data++={(stack_ref_var)->stk_value,stack_ref_var,(unsigned short)(((stack_ref_var)->properties)|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT)};


	#define PROCESS_MOD_OPERATION \
	{ \
		unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
		if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
				PUSH_INTEGER(LOAD_INT_OP(stk_result_op1) % LOAD_INT_OP(stk_result_op2));\
		}\
		else if(properties == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
				if (IS_INT(stk_result_op1->properties) && IS_NUMBER(stk_result_op2->properties)){\
					COPY_NUMBER(&f_aux_value2,&stk_result_op2->stk_value);\
					PUSH_NUMBER(fmod(LOAD_INT_OP(stk_result_op1) , f_aux_value2));\
				}else{\
					COPY_NUMBER(&f_aux_value1,&stk_result_op1->stk_value);\
					PUSH_NUMBER(fmod(f_aux_value1 , LOAD_INT_OP(stk_result_op2)));\
				}\
		}\
		else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
				COPY_NUMBER(&f_aux_value1,&stk_result_op1->stk_value);\
				COPY_NUMBER(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_NUMBER(fmod(f_aux_value1 , f_aux_value2));\
		}\
		else{\
			if(!ApplyMetamethod(\
							 calling_object\
							,info_function\
  						    ,instruction\
							,STR(%)\
							,BYTE_CODE_METAMETHOD_MOD\
							,stk_result_op1\
							,stk_result_op2\
			)){\
				RETURN_ERROR;\
			}\
		}\
	}

	#define PROCESS_ARITHMETIC_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
		if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
				PUSH_INTEGER(LOAD_INT_OP(stk_result_op1) __OVERR_OP__ LOAD_INT_OP(stk_result_op2));\
		}\
		else if(properties == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
				if (IS_INT(stk_result_op1->properties) && IS_NUMBER(stk_result_op2->properties)){\
					COPY_NUMBER(&f_aux_value2,&stk_result_op2->stk_value);\
					PUSH_NUMBER(LOAD_INT_OP(stk_result_op1) __OVERR_OP__ f_aux_value2);\
				}else{\
					COPY_NUMBER(&f_aux_value1,&stk_result_op1->stk_value);\
					PUSH_NUMBER(f_aux_value1 __OVERR_OP__ LOAD_INT_OP(stk_result_op2));\
				}\
		}\
		else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
				COPY_NUMBER(&f_aux_value1,&stk_result_op1->stk_value);\
				COPY_NUMBER(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_NUMBER(f_aux_value1 __OVERR_OP__ f_aux_value2);\
		}\
		else{\
			if(!ApplyMetamethod(\
							calling_object\
							,info_function\
							,instruction\
							,STR(__OVERR_OP__)\
							,__METAMETHOD__\
							,stk_result_op1\
							,stk_result_op2\
			)){\
				RETURN_ERROR;\
			}\
		}\
	}

	#define PROCESS_COMPARE_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
		if(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
				PUSH_BOOLEAN(LOAD_INT_OP(stk_result_op1) __OVERR_OP__ LOAD_INT_OP(stk_result_op2));\
		}\
		else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){\
			PUSH_BOOLEAN(LOAD_BOOL_OP(stk_result_op1) __OVERR_OP__ LOAD_BOOL_OP(stk_result_op2));\
		}\
		else if(properties == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
				if (IS_INT(stk_result_op1->properties) && IS_NUMBER(stk_result_op2->properties)){\
					COPY_NUMBER(&f_aux_value2,&stk_result_op2->stk_value);\
					PUSH_BOOLEAN(LOAD_INT_OP(stk_result_op1) __OVERR_OP__ f_aux_value2);\
				}else{\
					COPY_NUMBER(&f_aux_value1,&stk_result_op1->stk_value);\
					PUSH_BOOLEAN(f_aux_value1 __OVERR_OP__ LOAD_INT_OP(stk_result_op2));\
				}\
		}\
		else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
				COPY_NUMBER(&f_aux_value1,&stk_result_op1->stk_value);\
				COPY_NUMBER(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_BOOLEAN(f_aux_value1 __OVERR_OP__ f_aux_value2);\
		}\
		else if((stk_result_op1->properties&stk_result_op2->properties) == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){\
			PUSH_BOOLEAN(ZS_STRCMP(LOAD_STRING_OP(stk_result_op1), __OVERR_OP__ ,LOAD_STRING_OP(stk_result_op2)));\
		}else{\
			if(!ApplyMetamethod(\
						 calling_object\
						,info_function\
						,instruction\
						,STR(__OVERR_OP__)\
						, __METAMETHOD__\
						,stk_result_op1\
						,stk_result_op2\
			)){\
				RETURN_ERROR;\
			}\
		}\
	}

	#define PROCESS_LOGIC_OPERATION(__OVERR_OP__)\
	{\
		unsigned short properties = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
		if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){\
			PUSH_BOOLEAN(LOAD_BOOL_OP(stk_result_op1) __OVERR_OP__ LOAD_BOOL_OP(stk_result_op2));\
		}else{\
			PRINT_DUAL_ERROR_OP(STR(__OVERR_OP__));\
		}\
	}

	#define PROCESS_BINARY_OPERATION(__OVERR_OP__, __METAMETHOD__)\
	{\
		unsigned short properties = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
		if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
			PUSH_INTEGER(LOAD_INT_OP(stk_result_op1) __OVERR_OP__ LOAD_INT_OP(stk_result_op2));\
		}else{\
			if(!ApplyMetamethod(\
							 calling_object\
							,info_function\
							,instruction\
							,STR(__OVERR_OP__)\
							, __METAMETHOD__\
							,stk_result_op1\
							,stk_result_op2\
			)){\
				RETURN_ERROR;\
			}\
		}\
	}

	#define PERFORM_PRE_POST_OPERATOR(ldrOp, __OPERATOR__) \
	{\
		void **ref=(void **)(&((ldrOp)->stk_value));\
		if(ldrOp->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){\
			ref=(void **)((ldrOp)->var_ref);\
		}\
		switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES((ldrOp)->properties)){\
		case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:\
				(*((int *)(ref)))__OPERATOR__;\
				break;\
		case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:\
				(*((float *)(ref)))__OPERATOR__;\
				break;\
		default:\
			WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction)," Cannot perform pre/post operator (%s)",IndexInstructionVarTypeToStr(ldrOp));\
			RETURN_ERROR;\
			break;\
		}\
	}


	#define AssignStackVar(dst_ins, src_ins) \
	{\
		ScriptVar *script_var=NULL;\
		std::string *aux_str=NULL;\
		void *copy_aux=NULL;/*copy aux in case of the var is c and primitive (we have to update stk_value on save) */\
		void **src_ref=&src_ins->stk_value;\
		void **dst_ref=&dst_ins->stk_value;\
		if(src_ins->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){\
			src_ref=(void **)((src_ins)->var_ref);\
		}\
		if(dst_ins->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){\
			if(!IS_NUMBER_OR_INT(src_ins->properties) && IS_NUMBER_OR_INT(dst_ins->properties)){\
				if(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(src_ins->properties) != GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(dst_ins->properties)\
				){\
					WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"different types! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals",INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction));\
					RETURN_ERROR;\
				}else{\
					if(\
						(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(src_ins->properties) == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)\
					){\
							ZS_PRINT_INFO("Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator in order to make the proper operation.");\
							RETURN_ERROR;\
					}\
				}\
			}\
			dst_ref=(void **)((dst_ins)->var_ref);\
			copy_aux=&((dst_ins)->stk_value);\
		}\
		unsigned short type_var=src_ins->properties;\
		unsigned short runtime_var=0; /* there's no reason to heredate runtime_props ?!? GET_MSK_STACK_ELEMENT_PROPERTY_RUNTIME(type_var);*/\
		if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED){\
			dst_ins->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;\
		}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL){\
			dst_ins->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL;\
		}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
			dst_ins->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER;\
			*((int *)dst_ref)=*((int *)src_ref);\
			if(copy_aux!=NULL)(*(int *)copy_aux)=*((int *)src_ref);\
		}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
			dst_ins->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;\
			*((float *)dst_ref)=*((float *)src_ref);\
			if(copy_aux!=NULL)(*(float *)copy_aux)=*((float *)src_ref);\
		}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){\
			dst_ins->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN;\
			*((bool *)dst_ref)=*((bool *)src_ref);\
			if(copy_aux!=NULL)(*(bool *)copy_aux)=*((bool *)src_ref);\
		}else if(type_var  &  MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){\
			*dst_ins={	src_ins->stk_value,\
						NULL,\
						(unsigned short)(runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)\
						};\
		}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){\
			if(dst_ins->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){\
				*((std::string *)dst_ins->var_ref)=((const char *)src_ins->stk_value);/* Assign std::string */\
				dst_ins->stk_value=(void *)(((std::string *)dst_ins->var_ref)->c_str());/* Because std::string assignment implies reallocs ptr char it changes, so reassing const char pointer */\
			}else{\
				if(((dst_ins->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)==0) || (dst_ins->var_ref==NULL)){/* Generates a std::string var */  \
					script_var= NEW_STRING_VAR;\
					dst_ins->var_ref=script_var;\
					aux_str=&(((ScriptVarString *)script_var)->m_strValue);\
					dst_ins->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;\
					script_var->initSharedPtr(true);\
				}\
				(*aux_str)=((const char *)src_ins->stk_value);\
				dst_ins->stk_value=(void *)aux_str->c_str();/* Because std::string assignment implies reallocs ptr char it changes, so reassing const char pointer */ \
			}\
		}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){\
			script_var=(ScriptVar *)src_ins->var_ref;\
			dst_ins->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;\
			dst_ins->stk_value=NULL;\
			dst_ins->var_ref=script_var;\
			if((dst_ins->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS) !=  MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS){\
				SharePointer(script_var->ptr_shared_pointer_node);\
			}\
		}else{\
			WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"(internal) cannot determine var type %i",GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(src_ins->properties));\
			RETURN_ERROR;\
		}\
		if(copy_aux!=NULL)dst_ins->properties|=MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C;\
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

	#define BYTE_CODE_PUSH_SCOPE(_index,_ptr_info_function, _ptr_local_var,_properties) {\
		if(current_scope_info_ptr >=  max_scope_info){WriteError(NULL,0,"reached max scope"); RETURN_ERROR;}\
		*current_scope_info_ptr++={(short)(_index),_ptr_info_function,_ptr_local_var,_properties};\
	}


	#define CALL_GC SHARED_LIST_DESTROY(zero_shares[idx_current_stack])



	const char * VirtualMachine::IndexInstructionVarTypeToStr(StackElement *ptr_info_ale){
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
			if(ptr_info_ale->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT){
				var=(StackElement *)var->var_ref;
			}
			result=((ScriptVar *)var->var_ref)->GetClassName().c_str();
		}

		return result;
	}

	void VirtualMachine::DoStackDump(){
		// derefer all variables in all scopes (except main )...
		while(scope_info<(current_scope_info_ptr)){
			PopScopeCall(idx_current_stack,NULL,0);
		}
		idx_current_stack=0;
	}

	VirtualMachine::VirtualMachine(ZetScript *_zs){

		//-----------------------------------------------------------
		// set memory manager

		memset(zero_shares,0,sizeof(zero_shares));
		memset(shared_var,0,sizeof(shared_var));
		//memset(stack,0,sizeof(stack));

		StackElement *aux=stack;

		for(int i=0; i < VM_LOCAL_VAR_MAX_STACK;i++){
			*aux++={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		}

		idx_current_stack=0;
		//------------------

		f_return_value=0;
		s_return_value="unknow";

		cancel_execution=false;

		stk_current_data=NULL;
		current_scope_info_ptr = scope_info;

		f_aux_value1=0;
		f_aux_value2=0;

		custom_error = "unknow_error";

		idx_last_statment=0;

		ptr_last_str=&stk_string[VM_MAX_AUX_STRINGS-1]; // aux values for std::string ...
		ptr_current_str=NULL;

		max_scope_info = &scope_info[VM_MAX_SCOPES-1];

		//current_ast_node_call_c_function=-1;

		main_function_object = NULL;

		script_functions = NULL;
		//vec_ast_node = NULL;

		size_vec_script_function_object_node = 0;
		//size_vec_ast_node = 0;
		current_foreach=NULL;
		current_call_c_function = NULL;
		n_globals=0;
		zs=_zs;
		script_function_factory=this->zs->GetScriptFunctionFactory();
		script_class_factory=this->zs->GetScriptClassFactory();

	}


	#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

	#define print_vm_cr ZS_PRINT_INFO
	#else
	#define print_vm_cr(s,...)
	#endif


	//============================================================================================================================================
	// POINTER MANANAGER


	PInfoSharedPointerNode VirtualMachine::NewSharedPointer(ScriptVar *_var_ptr){
		//int index = VirtualMachine::getFreeCell();
		PInfoSharedPointerNode _node = (PInfoSharedPointerNode)malloc(sizeof(tInfoSharedPointerNode));
		_node->data.n_shares=0;
		_node->data.shared_ptr=_var_ptr;
		_node->currentStack = idx_current_stack;

		// insert node into shared nodes ...
		SHARE_LIST_INSERT(zero_shares[idx_current_stack],_node);
		//zero_shares[idx_current_stack].InsertNode(_node);
		return _node;
	}


	bool VirtualMachine::SharePointer(PInfoSharedPointerNode _node){

		unsigned char *n_shares = &_node->data.n_shares;

		bool move_to_shared_list=*n_shares==0;

		if(*n_shares >= MAX_SHARES_VARIABLE){
			THROW_RUNTIME_ERROR(zs_strutils::Format("MAX SHARED VARIABLES (Max. %i)",MAX_SHARES_VARIABLE));
			return false;

		}

		(*n_shares)++;

		if(move_to_shared_list){

			// Mov to shared pointer...
			SHARE_LIST_DEATTACH(zero_shares[_node->currentStack],_node);
			// update current stack due different levels from functions!
			_node->currentStack=idx_current_stack;
			SHARE_LIST_INSERT(shared_var[idx_current_stack],_node);
		}

		return true;

	}

	void VirtualMachine::UnrefSharedScriptVar(PInfoSharedPointerNode _node, bool remove_if_0){

		unsigned char *n_shares = &_node->data.n_shares;
		if(*n_shares > 0){
			if(--(*n_shares)==0){ // mov back to 0s shares (candidate to be deleted on GC check)

				SHARE_LIST_DEATTACH(shared_var[_node->currentStack],_node);

				if(remove_if_0){ // Remove node and data instead...
					delete _node->data.shared_ptr;
					free(_node);
				}
				else{ // insert into zero array.. if not referenced anymore will be removed by REMOVE_0_SHARED
					// update current stack due different levels from functions!
					_node->currentStack=idx_current_stack;
					SHARE_LIST_INSERT(zero_shares[idx_current_stack],_node);
				}
			}
		}
	}


	#define POP_TWO \
	stk_result_op2=--stk_current_data;\
	stk_result_op1=--stk_current_data;

	#define READ_TWO_POP_ONE \
	stk_result_op2=--stk_current_data;\
	stk_result_op1=(stk_current_data-1);


	#define POP_ONE \
	stk_result_op1=--stk_current_data;

	const ScriptFunction * VirtualMachine::GetCurrent_C_FunctionCall(){
		return current_call_c_function;
	}

	#define PTR_FUNCTION_VOID_PARAM0(f) ((void (*)())(f))
	#define PTR_FUNCTION_VOID_PARAM1(f) ((void (*)(intptr_t))(f))
	#define PTR_FUNCTION_VOID_PARAM2(f) ((void (*)(intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_VOID_PARAM3(f) ((void (*)(intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_VOID_PARAM4(f) ((void (*)(intptr_t,intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_VOID_PARAM5(f) ((void (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_VOID_PARAM6(f) ((void (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))

	#define PTR_FUNCTION_RET_PARAM0(f) ((intptr_t (*)())(f))
	#define PTR_FUNCTION_RET_PARAM1(f) ((intptr_t (*)(intptr_t))(f))
	#define PTR_FUNCTION_RET_PARAM2(f) ((intptr_t (*)(intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_PARAM3(f) ((intptr_t (*)(intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_PARAM4(f) ((intptr_t (*)(intptr_t,intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_PARAM5(f) ((intptr_t (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_PARAM6(f) ((intptr_t (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))


	#define PTR_FUNCTION_RET_FLOAT_PARAM0(f) ((float (*)())(f))
	#define PTR_FUNCTION_RET_FLOAT_PARAM1(f) ((float (*)(intptr_t))(f))
	#define PTR_FUNCTION_RET_FLOAT_PARAM2(f) ((float (*)(intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_FLOAT_PARAM3(f) ((float (*)(intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_FLOAT_PARAM4(f) ((float (*)(intptr_t,intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_FLOAT_PARAM5(f) ((float (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_FLOAT_PARAM6(f) ((float (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))

	#define PTR_FUNCTION_RET_BOOL_PARAM0(f) ((bool (*)())(f))
	#define PTR_FUNCTION_RET_BOOL_PARAM1(f) ((bool (*)(intptr_t))(f))
	#define PTR_FUNCTION_RET_BOOL_PARAM2(f) ((bool (*)(intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_BOOL_PARAM3(f) ((bool (*)(intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_BOOL_PARAM4(f)	((bool (*)(intptr_t,intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_BOOL_PARAM5(f) ((bool (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))
	#define PTR_FUNCTION_RET_BOOL_PARAM6(f) ((bool (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))


	//============================================================================================================================================
	StackElement  VirtualMachine::Call_C_Function(
			intptr_t fun_ptr,
			const ScriptFunction *irfs,
			bool & error,
			StackElement *ptrArg,
			unsigned char n_args,
			Instruction *ins,
			ScriptVar  * this_object
			){

		StackElement callc_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		intptr_t converted_param[MAX_N_ARGS];
		intptr_t result=0;
		StackElement *currentArg;
		current_call_c_function = irfs;
		bool static_ref=irfs->symbol_info.symbol_info_properties&SYMBOL_INFO_PROPERTY_STATIC_REF;
		//float aux_float=0;

		if(n_args>MAX_N_ARGS){
			WriteError(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Max run-time args! (Max:%i Provided:%i)",MAX_N_ARGS,n_args);
			RETURN_ERROR;
		}


		if((irfs->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF) {
			WriteError(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Function is not registered as C");
			RETURN_ERROR;
		}

		if(fun_ptr==0){
			WriteError(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Null function");
			//return &callc_result;//ScriptVar::UndefinedSymbol;
			RETURN_ERROR;
		}

		if((char)irfs->arg_info.size() != n_args){
			WriteError(INSTRUCTION_GET_FILE_LINE(irfs,ins),"C argument VS scrip argument doestn't match sizes");
			RETURN_ERROR;
		}

		if(irfs->arg_info.size() > MAX_N_ARGS){
			WriteError(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Reached max param for C function (Current: %i Max Allowed: %i)",irfs->arg_info.size(),MAX_N_ARGS);
			RETURN_ERROR;
			//return &callc_result;//ScriptVar::UndefinedSymbol;
		}

		// convert parameters script to c...
		for(unsigned char  i = 0; i < n_args;i++){

			if( i==0 && (irfs->symbol_info.symbol_info_properties&(SYMBOL_INFO_PROPERTY_SET_FIRST_PARAMETER_AS_THIS))){
				if(!static_ref){
					WriteError(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Internal error: Cannot set parameter as this object due is not static");
					RETURN_ERROR;
				}

				if(this_object==NULL){
					WriteError(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Internal error: Cannot set parameter as this object due this object is NULL");
					RETURN_ERROR;
				}

				converted_param[0]=(intptr_t)this_object->get_C_Object();
			}
			else{
				currentArg=&ptrArg[i];

				if(!zs->stkToVar(currentArg,irfs->arg_info[i].idx_type,(intptr_t *)&converted_param[i],error_str)){
					WriteError(INSTRUCTION_GET_FILE_LINE(irfs,ins),"Function %s, param %i: %s. The function C %s that was found for first time it has different argument types now.",
																	irfs->symbol_info.symbol->name.c_str(),
																	i,
																	error_str.c_str(),
																	irfs->symbol_info.symbol->name.c_str()
																	);
					RETURN_ERROR;
				}
			}
		}

		ZS_PRINT_DEBUG("pre_call %i",n_args);

		if(irfs->idx_return_type == IDX_BUILTIN_TYPE_VOID_C){ // getInstance()->getIdxClassVoid()){

			switch(n_args){
			case 0:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM0(fun_ptr)();
				}else{
					(*((CFunctionMemberPointerVoidParam0 *)fun_ptr))();
				}
				break;
			case 1:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM1(
						converted_param[0]
					);
				}else{
					(*((CFunctionMemberPointerVoidParam1 *)fun_ptr))(
						converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					(*((CFunctionMemberPointerVoidParam2 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					(*((CFunctionMemberPointerVoidParam3 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					(*((CFunctionMemberPointerVoidParam4 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM5(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					(*((CFunctionMemberPointerVoidParam5 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					(*((CFunctionMemberPointerVoidParam6 *)fun_ptr))(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
					);
				}
				break;
			}

		}else if(irfs->idx_return_type==IDX_BUILTIN_TYPE_BOOL_C){  // we must do a bool cast in order to get float return.
			switch(n_args){
			case 0:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM0(fun_ptr)();
				}else{
					result=(*((CFunctionMemberPointerRetParam0 *)fun_ptr)).ret_bool();
				}
				break;
			case 1:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					result=(*((CFunctionMemberPointerRetParam1 *)fun_ptr)).ret_bool(
						converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					result=(*((CFunctionMemberPointerRetParam2 *)fun_ptr)).ret_bool(
						converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					result=(*((CFunctionMemberPointerRetParam3 *)fun_ptr)).ret_bool(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					result=(*((CFunctionMemberPointerRetParam4 *)fun_ptr)).ret_bool(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM5(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					result=(*((CFunctionMemberPointerRetParam5 *)fun_ptr)).ret_bool(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					result=(*((CFunctionMemberPointerRetParam6 *)fun_ptr))(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
					);
				}
				break;
			}
		}else if(irfs->idx_return_type==IDX_BUILTIN_TYPE_FLOAT_C){ // we must do a float cast in order to get float return.
			float aux_flt;
			switch(n_args){
			case 0:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM0(fun_ptr)();
				}else{
					aux_flt=(*((CFunctionMemberPointerRetParam0 *)fun_ptr)).ret_float();
				}
				break;
			case 1:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					aux_flt=(*((CFunctionMemberPointerRetParam1 *)fun_ptr)).ret_float(
						converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					aux_flt=(*((CFunctionMemberPointerRetParam2 *)fun_ptr)).ret_float(
						converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					aux_flt=(*((CFunctionMemberPointerRetParam3 *)fun_ptr)).ret_float(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					aux_flt=(*((CFunctionMemberPointerRetParam4 *)fun_ptr)).ret_float(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM5(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					aux_flt=(*((CFunctionMemberPointerRetParam5 *)fun_ptr)).ret_float(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					aux_flt=(*((CFunctionMemberPointerRetParam6 *)fun_ptr))(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
					);
				}
				break;
			}

			memcpy(&result, &aux_flt, sizeof(float));
		}else{ // generic pointer or int

			switch(n_args){
			case 0:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM0(fun_ptr)();
				}else{
					result=(*((CFunctionMemberPointerRetParam0 *)fun_ptr))();
				}
				break;
			case 1:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					result=(*((CFunctionMemberPointerRetParam1 *)fun_ptr))(
						converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					result=(*((CFunctionMemberPointerRetParam2 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					result=(*((CFunctionMemberPointerRetParam3 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					result=(*((CFunctionMemberPointerRetParam4 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM5(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					result=(*((CFunctionMemberPointerRetParam5 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					result=(*((CFunctionMemberPointerRetParam6 *)fun_ptr))(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
					);
				}
				break;
			}
		}

		callc_result=this->zs->varToStk(result,irfs->idx_return_type);

		return callc_result;
	}

	StackElement * VirtualMachine::GetStackElement(unsigned int idx_glb_element){
		ScriptFunction  *main_function = GET_SCRIPT_FUNCTION(0);

		if(idx_glb_element < main_function->local_variable.size()){
			return &stack[idx_glb_element];
		}

		return NULL;
	}

	void VirtualMachine::SetError(const char *str){

		WriteError("custom_user",-1,str);

		custom_error = str;
		cancel_execution = true;
	}

	void VirtualMachine::CancelExecution(){
		custom_error=NULL;
		cancel_execution = true;
	}

	void VirtualMachine::destroyCache(){
		main_function_object=NULL;
		if(script_functions!= NULL){
			free(script_functions);
			script_functions=NULL;
			size_vec_script_function_object_node=0;
		}

		/*if(vec_ast_node!= NULL){
			free(vec_ast_node);
			vec_ast_node=NULL;
			size_vec_ast_node=0;
		}*/

	}

	/*void VirtualMachine::buildCache(){

		destroyCache();

		main_function_object = MAIN_FUNCTION;
		std::vector<ScriptFunction *> *vec_script_function_object_node_aux=script_function_factory->GetScriptFunctions();
		size_vec_script_function_object_node=vec_script_function_object_node_aux->size();
		script_functions=(ScriptFunction **)malloc(sizeof(ScriptFunction *)*size_vec_script_function_object_node);
		for(unsigned i=0; i < size_vec_script_function_object_node; i++){
			script_functions[i]=vec_script_function_object_node_aux->at(i);
		}
	}*/

/*	void VirtualMachine::AddGlobalVar(StackElement se){
		global_var.push_back(se);
	}*/

	void VirtualMachine::AddGlobalVar(const StackElement & stk){

		if(n_globals < VM_LOCAL_VAR_MAX_STACK){
			stack[n_globals++]=stk;
		}else{
			WriteError(NULL,-1,"Max stack element over limit (%i)",VM_LOCAL_VAR_MAX_STACK);
		}
	}

	void VirtualMachine::ClearGlobalVars(){
		ScriptFunction  *main_function = MAIN_FUNCTION;

		// zero shares have a late loading so it can be null at first time...
		if(zero_shares == NULL){
			return;
		}



		if(n_globals!=(int)main_function->local_variable.size()){
			THROW_RUNTIME_ERROR("n_globals != main variables");
			return;
		}

		bool end=false;
		for(int i =  (int)(main_function->local_variable.size())-1; i >= 0 && !end; i--){
			//switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(ptr_ale->properties)){
			//case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
			end=(main_function->local_variable[i].symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF;
			if(!end){
				StackElement *ptr_ale =&stack[i];
				ScriptVar *var = NULL;

				if(ptr_ale->properties &MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
					var =((ScriptVar *)(ptr_ale->var_ref));
					if(var){
						if(var->ptr_shared_pointer_node != NULL){
							var->unrefSharedPtr();
						}
					}
				}
			}
			//}
		}

		RemoveEmptySharedPointers(0,NULL);


		// clear all stack variables
		StackElement *aux=stack;
		for(int i=0; i < VM_LOCAL_VAR_MAX_STACK;i++){
			*aux++={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		}

		memset(zero_shares,0,sizeof(zero_shares));
		memset(shared_var,0,sizeof(shared_var));
		//memset(stack,0,sizeof(stack));

		idx_current_stack=0;
		//int idx_current_stack
	}

	StackElement  VirtualMachine::Execute(
			 ScriptFunction *info_function,
			 ScriptVar *this_object,
			 bool & error,
			const std::vector<StackElement> & arg
			){


		StackElement callc_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		if(info_function==NULL){
			RETURN_ERROR;
		}


		if(idx_current_stack==0){ // set stack and init vars for first call...

			if(main_function_object->instruction == NULL){ // no statments
				RETURN_ERROR;
			}
			cancel_execution=false;

			stk_current_data=stack;
			//*stk_current_data={MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED,0,0}; // ini first op

			if(info_function->idx_script_function != 0){ // calls script function from C : preserve stack space for global vars
				stk_current_data=&stack[main_function_object->local_variable.size()];
			}

			current_foreach=&stk_foreach[0];
		}

		int n_arg=0;
		if(arg.size()>0){ // pass parameters...

			for(unsigned i = 0; i < arg.size(); i++){
				*stk_current_data++=arg[i];
			}
			//advance idxBaseStk...
			//idxBaseStk+=arg->size();
			n_arg=arg.size();
		}

		// Script function starts here.... later script function can call c++ function, but once in c++ function is not possible by now call script function again.


		StackElement info=ExecuteInternal(
				info_function,
				this_object,
				error,
				stk_current_data,
				NULL,
				n_arg);


		if(error){ // it was error so reset stack and stop execution ? ...
			DoStackDump();
		}


		return info;
	}

	StackElement VirtualMachine::ExecuteInternal(
			ScriptFunction 		* info_function,
			ScriptVar       		* this_object,
			bool & error,
			StackElement 		  	* _ptrStartOp,
			std::string 		  	* _ptrStartStr,
			unsigned char n_args,
			Instruction *calling_instruction){

		error=false;
		StackElement callc_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		std::string *ptrStartStr;
		StackElement *ptrStartOp;

		if(info_function == NULL){
			RETURN_ERROR;
		}

		std::vector<SymbolInfo> * local_var=&info_function->local_variable;

		ptrStartOp =_ptrStartOp;
		ptrStartStr =_ptrStartStr;

		if(ptrStartStr == NULL){
			ptrStartStr=stk_string;
		}

		StackElement *ptrLocalVar=NULL;
		StackElement *ptrArg=NULL;
		VM_ScopeInfo * ptrStartScopeInfo=NULL;

		ZS_PRINT_DEBUG("Executing function %s ...",info_function->symbol_info.symbol->name.c_str());
		int idxBaseStk=(ptrStartOp-stack);//>>sizeof(StackElement *);

		if(idxBaseStk<n_args){
			WriteError(INSTRUCTION_GET_FILE_LINE(info_function,calling_instruction),"Internal error (idxBaseStk<n_args) (%i<%i)",idxBaseStk,n_args);
			RETURN_ERROR;
		}

		if(n_args>0){
			ptrArg=(ptrStartOp-n_args);
		}

		ptrLocalVar=ptrStartOp;

		if(idx_current_stack < MAX_FUNCTION_CALL){

			/* VM_ScopeInfo		scope_info[VM_LOCAL_VAR_MAX_STACK];
			std::string 				stk_string[VM_LOCAL_VAR_MAX_STACK]; // aux values for std::string ...
			std::string              *ptr_last_str;
			std::string              *ptr_current_str;

			 StackElement     stack[VM_LOCAL_VAR_MAX_STACK];*/

			//printf("call:%i left:%lu\n",idx_current_stack,max_scope_info-current_scope_info_ptr);// scope_info[VM_LOCAL_VAR_MAX_STACK-1]);
			idx_current_stack++;
		}
		else{
			WriteError(INSTRUCTION_GET_FILE_LINE(info_function,calling_instruction),"Reached max stack");
			RETURN_ERROR;
		}

		if((info_function->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) ){ // C-Call

			if((info_function->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_IS_POLYMORPHIC)){ // cannot call...
				WriteError(INSTRUCTION_GET_FILE_LINE(info_function,calling_instruction),"Function \"%s%s\" derives from polymorphic class and cannot be executed due pointer changes at runtime. You have two options:\n"
						"1. Set register_C_baseSymbols(false) and  re-register the function using REGISTER_C_FUNCTION_MEMBER\n"
						"2. Adapt all virtual functions/classes to no non-virtual\n"
						,this_object==NULL?"":this_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(this_object->GetClassName()+"::").c_str():""
						,info_function->symbol_info.symbol->name.c_str());
				RETURN_ERROR;
			}

			intptr_t  fun_ptr = info_function->symbol_info.ref_ptr;

			if((info_function->symbol_info.symbol_info_properties &  SYMBOL_INFO_PROPERTY_STATIC_REF) == 0){ // if not static then is function depends of object ...

				if(this_object!= NULL){
					fun_ptr = this_object->getFunctionSymbolByIndex(info_function->symbol_info.idx_symbol)->proxy_ptr;
				}
			}

			StackElement se = Call_C_Function(
					fun_ptr
					,info_function
					,error
					,ptrArg
					,n_args
					, calling_instruction
					,this_object
			);

			if(idx_current_stack > 0){
				idx_current_stack--;
			}
			else{
				WriteError(INSTRUCTION_GET_FILE_LINE(info_function,calling_instruction),"Reached min stack");
				RETURN_ERROR;
			}


			return se;
		}

		short scope_index = info_function->symbol_info.symbol->idx_scope;// ast->idx_scope;


		if(info_function->idx_script_function != 0){

			BYTE_CODE_PUSH_SCOPE(scope_index,info_function,ptrLocalVar,0);
			ptrStartScopeInfo = current_scope_info_ptr;
		}

		callc_result ={ NULL,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		//=========================================
		// PUSH STACK
		// reserve vars and assign argv vars ...
		unsigned size_local_var=local_var->size();
		unsigned n_total_vars=n_args+size_local_var;

		if((idxBaseStk+n_total_vars) >=  VM_LOCAL_VAR_MAX_STACK){
			WriteError(INSTRUCTION_GET_FILE_LINE(info_function,calling_instruction),"Error MAXIMUM stack size reached");
			RETURN_ERROR;
		}

		// init local vars ...
		if(info_function->idx_script_function != 0){ // is not main function, so we have to initialize vars.


			if(idx_current_stack > 1){ // not global vars, then initialize variables as undefined...
				StackElement *ptr_aux = ptrLocalVar;
				for(unsigned i = 0; i < size_local_var; i++){

					// if C then pass its properties...
					*ptr_aux++={

							0,							 // no value assigned.
							0, 						     // no varref related.
							MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED // starts undefined.
					};
				}
			}
		}

		ptrStartOp=&ptrLocalVar[size_local_var];

		// PUSH STACK
		//=========================================
		//bool	aux_boolean=false;
		float aux_float=0.0;


		ScriptFunction * aux_function_info=NULL;
		StackElement ret_obj;

		ScriptVar *svar=NULL;
		ScriptVar *calling_object=NULL;
//		ScriptClass *script_class_aux;



		StackElement *stk_result_op1=NULL;
		StackElement *stk_result_op2=NULL;
		StackElement *ldrVar;
		unsigned short pre_post_properties=0;
		unsigned short properties=0;
		StackElement *variable_stack_element;
		//FunctionSymbol *si;
		ScriptVar *var_object = NULL;

		unsigned short scope_type=0;

		StackElement *dst_ins=NULL;
		StackElement *src_ins=NULL;
		bool ok=false;

		Instruction *start_it=info_function->instruction;
		Instruction *instruction_it=start_it;

		ptr_current_str=ptrStartStr;
		stk_current_data=ptrStartOp;


		//-----------------------------------------------------------------------------------------------------------------------
		//
		// START OPERATOR MANAGEMENT
		//

		 for(;;){ // foreach asm instruction ...

			Instruction * instruction = instruction_it++;

			const unsigned char operator_type=instruction->byte_code_op;
			const unsigned char op1_value=instruction->op1_value;

			switch(operator_type){
			//default:
			//	WriteError(INSTRUCTION_GET_FILE_LINE(instruction->idxAstNode),"operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
			//	RETURN_ERROR;
			case BYTE_CODE_END_FUNCTION:
				goto lbl_exit_function;
			case BYTE_CODE_LOAD:
			case BYTE_CODE_VGET:

				if(op1_value==LoadType::LOAD_TYPE_VARIABLE || operator_type==BYTE_CODE_VGET){
					if(operator_type==BYTE_CODE_VGET){

						bool ok=false;

						POP_TWO;

						if( (stk_result_op1->properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT)) == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT)){
							var_object = (ScriptVar *)(((StackElement *)stk_result_op1->var_ref)->var_ref);
						}

						if(var_object != NULL){
							if(var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR){
								ScriptVarVector * vec = (ScriptVarVector *)var_object;

								if(IS_INT(stk_result_op2->properties)){
									// determine object ...
									int v_index = LOAD_INT_OP(stk_result_op2);

									// check indexes ...
									if(v_index < 0){
										WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Negative index std::vector (%i)",v_index);
										RETURN_ERROR;
									}

									if(v_index >= (int)(vec->variable.size())){
										WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Index std::vector out of bounds (%i)",v_index);
										RETURN_ERROR;
									}

									ldrVar = &vec->variable[v_index];;
									ok = true;
								}else{
									WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Expected std::vector-index as integer or std::string");
									RETURN_ERROR;
								}
							}
						}

						if(!ok){
							WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Variable \"%s\" is not type std::vector",
								INSTRUCTION_GET_SYMBOL_NAME(info_function,&info_function->instruction[instruction->op2_value])
							);
							RETURN_ERROR;
						}

					}else{ // load variable ...

						properties=instruction->properties;
						scope_type=GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(properties);

						switch(scope_type){
						default: // global...
							ldrVar = &stack[instruction->op2_value];
							break;
						case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS:
						case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS:

							if(scope_type == MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){
								POP_ONE; // get var op1 and symbol op2

								Instruction *previous_ins= (instruction-1);

								if((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)!= MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)
								{

									WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"var \"%s\" is not scriptvariable",INSTRUCTION_GET_SYMBOL_NAME(info_function,previous_ins));
									RETURN_ERROR;
								}


								ScriptVar  * base_var = (ScriptVar  *)stk_result_op1->var_ref;
								if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) {
									StackElement *stk_ins=((StackElement *)stk_result_op1->var_ref);

									if(stk_ins->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
										base_var=((ScriptVar *)stk_ins->var_ref);
									}
								}

								if(base_var == NULL)
								{
									WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"var \"%s\" is not scriptvariable",INSTRUCTION_GET_SYMBOL_NAME(info_function,previous_ins));

									RETURN_ERROR;
								}

								if((variable_stack_element = base_var->getVariableSymbol(INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)))==NULL){

									WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Variable \"%s\" as type \"%s\" has not symbol \"%s\"",INSTRUCTION_GET_SYMBOL_NAME(info_function,previous_ins),base_var->GetClassName().c_str(), INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction));
									RETURN_ERROR;
								}
							}
							else{ // this scope ...
								if((variable_stack_element = this_object->getVariableSymbolByIndex(instruction->op2_value))==NULL){
									WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot find symbol \"this.%s\"",INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction));
									RETURN_ERROR;
								}
							}
							ldrVar=variable_stack_element;
							break;
						case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS:
							ldrVar = &ptrLocalVar[instruction->op2_value];
							break;
						}

						if(instruction->properties&MSK_INSTRUCTION_PROPERTY_CHECK_IS_FUNCTION){
							if((ldrVar->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION) != MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){
								WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"\"%s\" expected function variable but is \"%s\""
										, INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)
										, IndexInstructionVarTypeToStr(ldrVar));
								RETURN_ERROR;
							}
						}
					}

					pre_post_properties = GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(properties);

					/* all preoperators makes load var as constant ... */
					switch(pre_post_properties){
					case MSK_INSTRUCTION_PROPERTY_PRE_INC:
							PERFORM_PRE_POST_OPERATOR(ldrVar,++);
							(*stk_current_data++)=*ldrVar;
							continue;
					case MSK_INSTRUCTION_PROPERTY_PRE_DEC:
							PERFORM_PRE_POST_OPERATOR(ldrVar,--);
							(*stk_current_data++)=*ldrVar;
							continue;
					case MSK_INSTRUCTION_PROPERTY_POST_DEC:
							(*stk_current_data++)=*ldrVar;
							PERFORM_PRE_POST_OPERATOR(ldrVar,--);
							continue;
					case MSK_INSTRUCTION_PROPERTY_POST_INC:
							(*stk_current_data++)=*ldrVar;
							PERFORM_PRE_POST_OPERATOR(ldrVar,++);
							continue;
					case MSK_INSTRUCTION_PROPERTY_PRE_NOT:
							switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(ldrVar->properties)){
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
								if(ldrVar->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									*stk_current_data++={(void *)(-(*((intptr_t *)ldrVar->var_ref))),ldrVar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C};
								}else{
									*stk_current_data++={(void *)(-(((intptr_t)ldrVar->stk_value))),ldrVar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT};
								}
								break;
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
								if(ldrVar->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									*stk_current_data++={(void *)(!(*((bool *)ldrVar->var_ref))),ldrVar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C};
								}else{
									*stk_current_data++={(void *)(!(((bool)ldrVar->stk_value))),ldrVar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT};
								}
								break;

							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
								if(ldrVar->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									COPY_NUMBER(&aux_float,ldrVar->var_ref);
								}else{
									COPY_NUMBER(&aux_float,&ldrVar->stk_value);
								}
								aux_float=-aux_float;
								COPY_NUMBER(&stk_current_data->stk_value,&aux_float);
								*stk_current_data={stk_current_data->stk_value,ldrVar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT};

								if(ldrVar->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									stk_current_data->properties|=MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C;
								}

								stk_current_data++;
								break;
							default:
								WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"internal error:cannot perform pre operator - because is not number");
								RETURN_ERROR;
							}
							continue;
					default:

						// update var if needed ...
						PUSH_STACK_VAR(ldrVar);
						break;
					}

					// ok in case is C we must udpate stk_value becaus it can be updated from C++. (only primitives)
					if(ldrVar->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
						switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(ldrVar->properties)){
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
							//(stk_current_data-1)->stk_value=(void *)((*((int *)ldrVar->var_ref)));
							memcpy(&((stk_current_data-1)->stk_value),ldrVar->var_ref,sizeof(int));
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
							COPY_NUMBER(&((stk_current_data-1)->stk_value),ldrVar->var_ref);
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
							(stk_current_data-1)->stk_value=(void *)((*((bool *)ldrVar->var_ref)));
							break;
						}
					}



					continue;

				}else if(op1_value==LoadType::LOAD_TYPE_NULL){
					PUSH_NULL;
					continue;
				}else if(op1_value==LoadType::LOAD_TYPE_UNDEFINED){
					PUSH_UNDEFINED;
					continue;
				}else if(op1_value==LoadType::LOAD_TYPE_CONSTANT){
					(*stk_current_data)=*(((ConstantValueInfo *)instruction->op2_value));

					pre_post_properties = GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(instruction->properties);

					// all preoperators makes load var as constant ...
					switch(pre_post_properties){
					case MSK_INSTRUCTION_PROPERTY_PRE_NOT:
							switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_current_data->properties)){
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
								stk_current_data->stk_value=(void *)(-((intptr_t)stk_current_data->stk_value));
								break;
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
								stk_current_data->stk_value=(void *)(!((bool)stk_current_data->stk_value));
								break;
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
								COPY_NUMBER(&aux_float,&stk_current_data->stk_value);
								aux_float=-aux_float;
								COPY_NUMBER(&stk_current_data->stk_value,&aux_float);
								break;
							default:
								WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"internal error:cannot perform pre operator - constant because is not numeric");
								RETURN_ERROR;
							}
							break;
					}
					stk_current_data++;
					continue;
				}else if(op1_value== LoadType::LOAD_TYPE_FUNCTION){

					unsigned short extra_flags=(instruction->properties&MSK_INSTRUCTION_PROPERTY_CONSTRUCT_CALL)?MSK_STACK_ELEMENT_PROPERTY_CONSTRUCTOR_FUNCTION:0;
					extra_flags|=(instruction->properties&MSK_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL) ?MSK_STACK_ELEMENT_PROPERTY_UNRESOLVED_FUNCTION:0;
					//void *function_obj=NULL;
					//std::vector<ScriptFunction *> *vec_functions;
					ScriptVar * class_obj=NULL;
					//intptr_t function_obj =  instruction->op2_value;
					intptr_t op2_value = instruction->op2_value;
					properties=instruction->properties;
					scope_type=GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(properties);

					/*if(scope_type==MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){ // local gets functions from info_function ...
						vec_functions=&info_function->local_function;
					}else*/
					if(scope_type == MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){
						StackElement *var=NULL;
						if(properties & MSK_INSTRUCTION_PROPERTY_CONSTRUCT_CALL){
							var=(stk_current_data-1);
						}else{
							POP_ONE;
							var = stk_result_op1;
						}

						StackElement *stk_ins=NULL;
						if(var->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) {
							stk_ins=((StackElement *)var->var_ref);
						}
						else{
							stk_ins=var;
						}

						if(stk_ins->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							class_obj=(ScriptVar *)(stk_ins->var_ref);
							//ScriptClass *sc =GET_SCRIPT_CLASS(((ScriptVar *)class_obj)->idx_class);
							//vec_functions=&sc->local_function;
						}
						else{
							WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Cannot access symbol \"%s\" (type of %s is %s)",INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction),INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction-1),IndexInstructionVarTypeToStr(stk_ins));
							RETURN_ERROR;
						}


						/*if(op2_value == 0){//ZS_UNDEFINED_IDX || op2_value == ZS_FUNCTION_NOT_FOUND_IDX){ // is will be processed after in BYTE_CODE_CALL instruction ...
							function_obj= 0;//(void *)instruction; // saves current instruction in order to resolve its idx later (in call instruction)
						}else{
							function_obj= op2_value;
						}*/
						/*else if((op2_value<(int)vec_functions->size())) // get the function ...
						{
							function_obj = (*vec_functions)[op2_value];

						}
						else{
							WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot find symbol global \"%s\""
									,INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)
									);
							RETURN_ERROR;
						}*/
					}/*else if(scope_type ==MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS){
						if((si = this_object->getFunctionSymbolByIndex(op2_value))==NULL){
							WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot find function \"this.%s\"",INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction));
							RETURN_ERROR;
						}

						function_obj =(ScriptFunction *)si->object.stk_value;

					}else if(scope_type == MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER){ // super scope ?
						if((si = this_object->getFunctionSymbolByIndex(op2_value))==NULL){
							WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot find function \"super.%s\"",INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction));
							RETURN_ERROR;
						}
						function_obj =(ScriptFunction *)si->object.stk_value;
					}else{ // global
						vec_functions = &(main_function_object->local_function);
					}


					if(!(scope_type == MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS || scope_type == MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER)){

						if(op2_value == ZS_UNDEFINED_IDX || op2_value == ZS_FUNCTION_NOT_FOUND_IDX){ // is will be processed after in BYTE_CODE_CALL instruction ...
							function_obj= NULL;//(void *)instruction; // saves current instruction in order to resolve its idx later (in call instruction)
						}else if((op2_value<(int)vec_functions->size())) // get the function ...
						{
							function_obj = (*vec_functions)[op2_value];

						}
						else{
							WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"cannot find symbol global \"%s\""
									,INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)
									);
							RETURN_ERROR;
						}
					}*/

					if(op2_value == 0){
						*stk_current_data++={instruction,class_obj,(unsigned short)(MSK_STACK_ELEMENT_PROPERTY_IS_VAR_INSTRUCTION|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION|extra_flags)};
					}else{
						PUSH_FUNCTION(op2_value,class_obj,extra_flags);
					}
					continue;

				}else if(op1_value== LoadType::LOAD_TYPE_ARGUMENT){
					*stk_current_data++=ptrArg[instruction->op2_value]; // copy arg directly ...
					continue;
				}
				else{

					WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"runtime error. Try to restart");
					RETURN_ERROR;

				}
				continue;

			case BYTE_CODE_STORE:
			case BYTE_CODE_VPUSH:
			case BYTE_CODE_PUSH_ATTR:

				{
					bool assign_metamethod=false;
					bool push_value=true;

					if(operator_type==BYTE_CODE_VPUSH){
						POP_ONE; // only pops the value, the last is the std::vector variable itself
						ScriptVar *vec_obj = NULL;
						if((stk_current_data-1)->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							vec_obj = (ScriptVar *)(stk_current_data-1)->var_ref;
							if(vec_obj->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR){ // push value ...
								// op1 is now the src value ...
								src_ins=stk_result_op1;
								dst_ins=((ScriptVarVector *)vec_obj)->push();
								ok=true;
							}
						}

						if(!ok){
							WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Expected std::vector object");
							RETURN_ERROR;
						}

						push_value=false;

					}else if(operator_type==BYTE_CODE_PUSH_ATTR){


						POP_TWO; // first must be the value name and the other the variable name ...
						ScriptVar *struct_obj = NULL;
						if((stk_current_data-1)->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							struct_obj = (ScriptVar *)(stk_current_data-1)->var_ref;
							if(struct_obj->idx_class == IDX_BUILTIN_TYPE_CLASS_DICTIONARY){ // push value ...
								// op1 is now the src value ...
								if(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
									StackElement *se=NULL;
									const char *str = (const char *)stk_result_op2->stk_value;
									src_ins=stk_result_op1;
									if(src_ins->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){
										FunctionSymbol *si =((ScriptVarDictionary *)struct_obj)->addFunctionSymbol(str,NULL);
										if(si!=NULL){
											se=&si->object;;
										}
									}else{
										se =((ScriptVarDictionary *)struct_obj)->addVariableSymbol(str);
									}

									if(se == NULL){
										RETURN_ERROR;
									}

									dst_ins=se;
									ok=true;
								}
								else{
									WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"internal error (operator2 is not std::string)");
									RETURN_ERROR;
								}
							}else{
								WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Expected struct object");
								RETURN_ERROR;
							}
						}else{
							WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Expected scriptvar");
							RETURN_ERROR;
						}
						push_value=false;
					}
					else{ // pop two parameters nothing ...
						POP_TWO;

						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) {// == ScriptVar::VAR_TYPE::OBJECT){
							dst_ins=(StackElement *)stk_result_op1->var_ref; // stk_value is expect to contents a stack variable
						}else{
							WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Expected l-value on assignment ('=')");
							RETURN_ERROR;
						}

						if(current_foreach!=NULL){
							if(dst_ins == current_foreach->key){
								WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"for-in key value is read only");
								RETURN_ERROR;
							}
						}

						src_ins=stk_result_op2; // store ptr instruction2 op as src_var_value

						// we need primitive stackelement in order to assign...
						if(src_ins->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) {// == ScriptVar::VAR_TYPE::OBJECT){
							src_ins=(StackElement *)src_ins->var_ref; // stk_value is expect to contents a stack variable

						}

						// ok load object pointer ...
						if(dst_ins->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){

							if(((ScriptVar *)dst_ins->var_ref)->itHasSetMetamethod()){
								if(!ApplyMetamethod(
										calling_object
										,info_function
										,instruction
										,"="
										,BYTE_CODE_METAMETHOD_SET
										,stk_result_op1
										,stk_result_op2
								)){
									RETURN_ERROR;
								}
								assign_metamethod=true;
							}
						}
					}

					if(! assign_metamethod){

						if(dst_ins->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS){
							WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"\"this\" is not assignable");
							RETURN_ERROR;
						}

						StackElement old_dst_ins = *dst_ins; // save dst_var to check after assignment...

						AssignStackVar(dst_ins,src_ins);

						// check old var structure ...
						switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(old_dst_ins.properties)){
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION: // we aren't take care about nothing! :)
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING: // type std::string is really a std::string or variable ?!?!
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR: // we are getting script vars ...
							if(!(((old_dst_ins.properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C))==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C)))){
								if(old_dst_ins.var_ref!=NULL){ // it had a pointer (no constant)...
									if(old_dst_ins.var_ref != dst_ins->var_ref){ // unref pointer because new pointer has been attached...
										UnrefSharedScriptVar(((ScriptVar  *)old_dst_ins.var_ref)->ptr_shared_pointer_node);
									}
								}
							}
							break;
						}
					}

					if(push_value){ // to be able to do multiple assigns like a=b=c=1 (1 will be pushed in each store instruction)
						*stk_current_data++=*src_ins;
					}
				}
				continue;

			case BYTE_CODE_EQU:  // ==

				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE;
				}else{
					POP_TWO;
				}

				PROCESS_COMPARE_OPERATION(==, BYTE_CODE_METAMETHOD_EQU);
				continue;
			case BYTE_CODE_NOT_EQU:  // !=

				POP_TWO;
				PROCESS_COMPARE_OPERATION(!=, BYTE_CODE_METAMETHOD_NOT_EQU);
				continue;

			case BYTE_CODE_LT:  // <
				POP_TWO;
				PROCESS_COMPARE_OPERATION(<, BYTE_CODE_METAMETHOD_LT);
				continue;
			case BYTE_CODE_LTE:  // <=
				POP_TWO;
				PROCESS_COMPARE_OPERATION(<=, BYTE_CODE_METAMETHOD_LTE);
				continue;
			case BYTE_CODE_GT:  // >

				POP_TWO;
				PROCESS_COMPARE_OPERATION(>,BYTE_CODE_METAMETHOD_GT);
				continue;
			case BYTE_CODE_GTE:  // >=
				POP_TWO;
				PROCESS_COMPARE_OPERATION(>=,BYTE_CODE_METAMETHOD_GTE);
				continue;
			case BYTE_CODE_LOGIC_AND:  // &&
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE;
				}else{
					POP_TWO;
				}

				PROCESS_LOGIC_OPERATION(&&);
				continue;
			case BYTE_CODE_LOGIC_OR:  // ||

				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE;
				}else{
					POP_TWO;
				}
				PROCESS_LOGIC_OPERATION(||);
				continue;
			case BYTE_CODE_NOT: // !

				POP_ONE;

				if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){ // operation will result as integer.
					PUSH_BOOLEAN((!((bool)(stk_result_op1->stk_value))));
				}else{
					if(!ApplyMetamethod(
							calling_object
							,info_function
							,instruction
							,"!"
							,BYTE_CODE_METAMETHOD_NOT
							,stk_result_op1
							,stk_result_op2
							)){
						//WriteError(INSTRUCTION_GET_FILE_LINE(instruction->idxAstNode),"Expected operands 1 as boolean!");
						RETURN_ERROR;
					}
				}
				continue;

			case BYTE_CODE_NEG: // -

				POP_ONE;

				if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){ // operation will result as integer.
					PUSH_INTEGER((-((intptr_t)(stk_result_op1->stk_value))));
				}else if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
					COPY_NUMBER(&f_aux_value1,&stk_result_op1->stk_value);
					PUSH_NUMBER(-f_aux_value1);
				/*}else if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){
					PUSH_BOOLEAN((!((bool)(stk_result_op1->stk_value))));
				*/
				}else{ // try metamethod ...
						if(!ApplyMetamethod(
								 calling_object
								,info_function
								,instruction
								,"-"
								,BYTE_CODE_METAMETHOD_NEG
								,stk_result_op1
								,stk_result_op2
								)){
							RETURN_ERROR;
						}

				}
				continue;

			case BYTE_CODE_ADD: // +

			{
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE;
				}else{
					POP_TWO;
				}


				unsigned short mask_properties =GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties);
				unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);
				if(mask_properties==MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){
						PUSH_INTEGER(LOAD_INT_OP(stk_result_op1) + LOAD_INT_OP(stk_result_op2));
				}
				else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){
						if (IS_INT(stk_result_op1->properties) && IS_NUMBER(stk_result_op2->properties)){
							COPY_NUMBER(&f_aux_value2,&stk_result_op2->stk_value);
							PUSH_NUMBER(LOAD_INT_OP(stk_result_op1) + f_aux_value2);
						}else{
							COPY_NUMBER(&f_aux_value1,&stk_result_op1->stk_value);
							PUSH_NUMBER(f_aux_value1 + LOAD_INT_OP(stk_result_op2));
						}
				}
				else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)){
					if (IS_STRING(stk_result_op1->properties) && IS_INT(stk_result_op2->properties)){
						sprintf(str_aux,"%s%i",((const char *)stk_result_op1->stk_value),(int)((intptr_t)stk_result_op2->stk_value));
						//PUSH_STRING(str_aux);
						if(ptr_current_str==ptr_last_str){
								WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Error stk_string out-stack");RETURN_ERROR;}\
								*ptr_current_str++=str_aux;\
								*stk_current_data++={(void *)((ptr_current_str-1)->c_str()),NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};\

					}else{
						sprintf(str_aux,"%i%s",(int)((intptr_t)stk_result_op1->stk_value),((const char *)stk_result_op2->stk_value));
						PUSH_STRING(str_aux);
					}
				}else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)){
					if (IS_STRING(stk_result_op1->properties) && IS_NUMBER(stk_result_op2->properties)){
						COPY_NUMBER(&f_aux_value2,&stk_result_op2->stk_value);
						sprintf(str_aux,"%s%f",((const char *)stk_result_op1->stk_value),f_aux_value2);
						PUSH_STRING(str_aux);

					}else{
						COPY_NUMBER(&f_aux_value1,&stk_result_op1->stk_value);
						sprintf(str_aux,"%f%s",f_aux_value1,((const char *)stk_result_op2->stk_value));
						PUSH_STRING(str_aux);
					}
				}else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)){
					if (IS_STRING(stk_result_op1->properties) && IS_UNDEFINED(stk_result_op2->properties)){
						sprintf(str_aux,"%s%s",((const char *)stk_result_op1->stk_value),"undefined");
						PUSH_STRING(str_aux);
					}else{
						sprintf(str_aux,"%s%s","undefined",((const char *)stk_result_op2->stk_value));
						PUSH_STRING(str_aux);
					}

				}else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)){
					if (IS_STRING(stk_result_op1->properties) && IS_BOOLEAN(stk_result_op2->properties)){
						sprintf(str_aux,"%s%s",((const char *)stk_result_op1->stk_value),((bool)(stk_result_op2->stk_value))?"true":"false");
						PUSH_STRING(str_aux);
					}else{
						sprintf(str_aux,"%s%s",((bool)(stk_result_op1->stk_value))?"true":"false",((const char *)stk_result_op2->stk_value));
						PUSH_STRING(str_aux);
					}

				}else if (mask_properties== MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
						sprintf(str_aux,"%s%s",((const char *)stk_result_op1->stk_value),((const char *)(stk_result_op2->stk_value)));
						PUSH_STRING(str_aux);


				}else if(mask_properties== MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
					COPY_NUMBER(&f_aux_value1,&stk_result_op1->stk_value);\
					COPY_NUMBER(&f_aux_value2,&stk_result_op2->stk_value);\
					PUSH_NUMBER(f_aux_value1 + f_aux_value2);\

				}
				else{ // try metamethod ...

					if(!ApplyMetamethod(
							 calling_object
							,info_function
							,instruction
							,"+"
							,BYTE_CODE_METAMETHOD_ADD
							,stk_result_op1
							,stk_result_op2
							)){
						RETURN_ERROR;
					}
				}
			}
				continue;

			case BYTE_CODE_MUL: // *

				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_ARITHMETIC_OPERATION(*,BYTE_CODE_METAMETHOD_MUL);
				continue;

			case BYTE_CODE_DIV: // /

				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_ARITHMETIC_OPERATION(/, BYTE_CODE_METAMETHOD_DIV);
				continue;

			 case BYTE_CODE_MOD: // /

				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_MOD_OPERATION;
				continue;

			 case BYTE_CODE_AND: // &
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_BINARY_OPERATION(&, BYTE_CODE_METAMETHOD_AND);
				continue;
			 case BYTE_CODE_OR: // *
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_BINARY_OPERATION(|, BYTE_CODE_METAMETHOD_OR);
				continue;
			 case BYTE_CODE_XOR: // ^
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_BINARY_OPERATION(^, BYTE_CODE_METAMETHOD_XOR);
				continue;

			 case BYTE_CODE_SHR: // >>
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_BINARY_OPERATION(>>,BYTE_CODE_METAMETHOD_SHR);
				continue;

			 case BYTE_CODE_SHL: // <<
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_BINARY_OPERATION(<<, BYTE_CODE_METAMETHOD_SHL);
				continue;
			 case BYTE_CODE_JMP:
				instruction_it=&start_it[instruction->op2_value];
				continue;
			 case BYTE_CODE_INSTANCEOF: // check instance of ...
				 POP_TWO;

				if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) {// == ScriptVar::VAR_TYPE::OBJECT){
					stk_result_op1=(StackElement *)stk_result_op1->var_ref; // stk_value is expect to contents a stack variable
				}

				switch((intptr_t)stk_result_op2->stk_value){
					case IDX_BUILTIN_TYPE_INT_PTR_C:
						PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER)!=0);
						break;
					case IDX_BUILTIN_TYPE_FLOAT_PTR_C:
						PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)!=0);
						break;
					case IDX_BUILTIN_TYPE_BOOL_PTR_C:
						PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN)!=0);
						break;
					case IDX_BUILTIN_TYPE_STRING_PTR_C:
						PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)!=0);
						break;
					case IDX_BUILTIN_TYPE_CLASS_FUNCTOR:
						PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)!=0);
						break;
					default:
						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							bool b = this->script_class_factory->isIdxClassInstanceOf(((ScriptVar *)(stk_result_op1->var_ref))->idx_class, (intptr_t)stk_result_op2->stk_value);
							PUSH_BOOLEAN(b);
						}else{
							PUSH_BOOLEAN(false);
						}
						break;
					}


				 continue;
			 case BYTE_CODE_JNT: // goto if not true ... goes end to conditional.
				POP_ONE;
				if(stk_result_op1->stk_value == 0){
					instruction_it=&start_it[instruction->op2_value];
				}
				continue;
			 case BYTE_CODE_JT: // goto if true ... goes end to conditional.
				POP_ONE;
				if(stk_result_op1->stk_value != 0){
					instruction_it=&start_it[instruction->op2_value];
				}
				continue;
			 case  BYTE_CODE_CALL: // calling function after all of args are processed...
				// check whether signatures matches or not ...
				// 1. get function object ...
			 	 {
					aux_function_info = NULL;
					unsigned char n_args=0;


					bool is_c = false;
					StackElement *start_arg=stk_current_data;
					StackElement *callAle=NULL;

					while(n_args <= MAX_N_ARGS && (((start_arg-1)->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)==0)){
						start_arg--;
						n_args++;
					}

					callAle = ((start_arg-1));
					calling_object = this_object;
					if(callAle->var_ref!=NULL){
						calling_object=(ScriptVar *)callAle->var_ref;
					}


					aux_function_info = NULL;//(ScriptFunction *)callAle->stk_value;


					bool is_constructor = (callAle->properties & MSK_STACK_ELEMENT_PROPERTY_CONSTRUCTOR_FUNCTION)!=0;

					// try to find the function ...
					if(((callAle)->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_INSTRUCTION)){// || deduce_function){
						Instruction *callAleInstruction = (Instruction *)(callAle)->stk_value;
						//PASTNode ast_node_call_ale = vec_ast_node[iao->idxAstNode];

						symbol_to_find = INSTRUCTION_GET_SYMBOL_NAME(info_function,callAleInstruction);
						unsigned short scope_type = GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(callAleInstruction->properties);


						// local vars as functions ...

						// registered symbols in case is MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS...
						std::vector<FunctionSymbol> *function_symbol=NULL;
						if(scope_type==MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){
							calling_object = (ScriptVar *)callAle->var_ref;

							// we have to no to call default constructor...is implicit
							if(is_constructor) {

								is_c=calling_object->is_c_object();

								if(n_args == 0 && is_c){
									aux_function_info = NULL;
									callAleInstruction->properties |= MSK_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL;
								}
							}

							function_symbol=calling_object->getVectorFunctionSymbol();
						}

						//bool all_check=true;
						if((callAleInstruction->properties & MSK_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL)==0)
						{
							std::vector<ScriptFunction *> *global_functions=&(main_function_object->local_function);
							//#define FindFunction(iao, is_constructor, symbol_to_find,size_fun_vec,global_functions,startArgs, n_args,scope_type)
							if((aux_function_info=FindFunction(
									 calling_object
									,info_function
									,instruction
									,callAleInstruction

									,function_symbol
									,global_functions
									,is_constructor
									,symbol_to_find
									,stk_result_op1
									,stk_result_op2
									,start_arg
									,n_args
									,NULL))==NULL){

								if((callAleInstruction->properties & MSK_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL)==0){
									RETURN_ERROR;
								}
							}


							// saves function pointer found ...
							callAle->stk_value=aux_function_info;

						}

					}
					else{
						if(((callAle)->properties & MSK_STACK_ELEMENT_PROPERTY_UNRESOLVED_FUNCTION)==0){
							aux_function_info=(ScriptFunction *) (callAle->stk_value);
						}
					}

					if(aux_function_info !=NULL)
					{
						if(n_args > MAX_N_ARGS){
							WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Max arguments reached function at line XXX");
							RETURN_ERROR;
						}

						if((aux_function_info->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) == 0){ // is function script ...
							unsigned aux_function_info_m_arg_size=aux_function_info->arg_info.size();
							if( n_args < aux_function_info_m_arg_size){ // we must push undefined parameters ...
								for(unsigned i = n_args; i < aux_function_info_m_arg_size; i++){
									*stk_current_data++={

										0,							 // no value assigned.
										NULL,			     // no varref related.
										MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED // starts undefined.
									};
									n_args++;
								}
							}
						}

						// by default virtual machine gets main object class in order to run functions ...
						ret_obj=ExecuteInternal(aux_function_info,calling_object,error,stk_current_data,ptr_current_str,n_args,instruction);

						if(error)
						{
							RETURN_ERROR;
						}

						if(ret_obj.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){

							// if c pointer is not from application share ...
							if(!((ScriptVar *)(ret_obj.var_ref))->initSharedPtr()){
								RETURN_ERROR;
							}
						}

						if(cancel_execution) {
							if(custom_error!=NULL){
								WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),custom_error);
							}
							RETURN_ERROR;
						}

					}

					// reset stack (function+instruction (-1 op less))...
					stk_current_data=start_arg-1;

					// ... and push result if not function constructor...
					if(!is_constructor){
						*stk_current_data++ = ret_obj;
					}
			 	 }
				continue;
			 case  BYTE_CODE_NEW:
					svar=NEW_CLASS_VAR_BY_IDX(op1_value);

					if(!svar->initSharedPtr()){
						RETURN_ERROR;
					}
					svar->info_function_new=info_function;
					svar->instruction_new=instruction;
					(*stk_current_data++)={NULL,svar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR};
					continue;
			 case  BYTE_CODE_DELETE:
					POP_ONE;
					//svar
					if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
						StackElement *se=stk_result_op1;
						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT){
							se=(StackElement *)(stk_result_op1->var_ref);
						}

						svar = (ScriptVar *)(se)->var_ref;
						if(svar->idx_class >= IDX_BUILTIN_TYPE_MAX
						 ||svar->idx_class==IDX_BUILTIN_TYPE_CLASS_VECTOR
						 ||svar->idx_class==IDX_BUILTIN_TYPE_CLASS_DICTIONARY
						)
						{ // max ...
							svar->unrefSharedPtr();

							if(svar->isCreatedByContructor()){
								svar->setDelete_C_ObjectOnDestroy(true);
							}

							se->stk_value=NULL;
							se->var_ref=NULL;
							se->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;

						}
					}
					else{
						WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"delete op: expected scriptvar var but it was \"%s\"",IndexInstructionVarTypeToStr(stk_result_op1));
						RETURN_ERROR;
					}
					continue;
			 case BYTE_CODE_DECL_VEC: // Create new std::vector object...
					svar=NEW_VECTOR_VAR;
					//PUSH_VAR(svar,NULL,0,false);

					if(!svar->initSharedPtr()){
						RETURN_ERROR;
					}

					(*stk_current_data++)={NULL,svar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR};

					continue;

			 case  BYTE_CODE_DECL_DICTIONARY: // Create new std::vector object...
				svar=NEW_STRUCT_VAR;
				//PUSH_VAR(svar,NULL,0,false);

				if(!svar->initSharedPtr()){
					RETURN_ERROR;
				}

				(*stk_current_data++)={NULL,svar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR};

				continue;


			 case  BYTE_CODE_RET:

				if(stk_current_data>ptrStartOp){ // can return something
					callc_result=*(stk_current_data-1);

					// Remove shared pointer if scriptvar ...
					if(callc_result.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
						if(callc_result.properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT){
							callc_result=*((StackElement *)((StackElement *)callc_result.var_ref));

						}

						// unref pointer to be deallocated from gc...
						//((ScriptVar *)callc_result.var_ref)->ptr_shared_pointer_node->data.shared_ptr=NULL;
						if(((ScriptVar *)callc_result.var_ref)->ptr_shared_pointer_node!=NULL){
							free(((ScriptVar *)callc_result.var_ref)->ptr_shared_pointer_node);
							((ScriptVar *)callc_result.var_ref)->ptr_shared_pointer_node=NULL;
						}
						// share pointer  + 1
					}
					else{ // Remove all involved inforamtion
						callc_result.properties&=~MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT; // Remove stack var if any...
						callc_result.var_ref =NULL;
					}
				}
				// }
				goto lbl_exit_function;
			 case BYTE_CODE_PUSH_SCOPE:

				BYTE_CODE_PUSH_SCOPE(instruction->op2_value,info_function,ptrLocalVar,op1_value);

				if(op1_value & ScopeProperty::FOR_IN){
					if(current_foreach == &stk_foreach[VM_MAX_FOREACH-1]){
						WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Max foreach reached");
						RETURN_ERROR;
					}

					current_foreach++;
				}
				continue;

			 case BYTE_CODE_POP_SCOPE:
				ptr_current_str=ptrStartStr; // reset op ptr
				stk_current_data=ptrStartOp;
				if(!PopScopeCall(idx_current_stack,NULL,op1_value)){
					WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Error pop scope");
					RETURN_ERROR;
				}

				if(op1_value & ScopeProperty::FOR_IN){
					if(current_foreach == &stk_foreach[0]){
						WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Min foreach reached");
						RETURN_ERROR;
					}

					memset(current_foreach,0,sizeof(ForeachInfo));
					current_foreach--;
				}
				continue;

			 case BYTE_CODE_IT_INI:
				 POP_TWO; // op2:std::vector or struct op1:key iterator


				 if((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) == 0){
						WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Internal error: Expected stackvar");
						RETURN_ERROR;

				 }
				 current_foreach->key=(StackElement *)stk_result_op1->var_ref;
				 current_foreach->ptr=NULL;
				 current_foreach->idx_current=0;

				 var_object = (ScriptVar *)stk_result_op2->var_ref;
				 if( (stk_result_op2->properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT)) == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT)){
				 		var_object = (ScriptVar *)(((StackElement *)stk_result_op2->var_ref)->var_ref);
				 }

				if(var_object != NULL && (var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR || var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_DICTIONARY)){

					if(var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR){ // integer as iterator...
						*current_foreach->key={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER};
					}
					else{ // struct -> std::string as iterator...
						*current_foreach->key={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};
					}

					current_foreach->ptr = var_object;
				}else{
					WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"Variable \"%s\" is not type std::vector or struct",
						INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction-1)
					);
					RETURN_ERROR;
				}
				 continue;
			 case BYTE_CODE_SET_AND_NEXT:
				 WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"TODOOOOO!",
						 	 	 	 INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)
				 					);
				 					RETURN_ERROR;
				 //*((StackElement *)current_foreach->key)=((ScriptVarVector *)current_foreach->ptr)->m_objVector[current_foreach->idx_current++];
				 continue;
			 case BYTE_CODE_IT_CHK_END:
				 WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"TODOOOOO!",
						 	 	 	 INSTRUCTION_GET_SYMBOL_NAME(info_function,instruction)
				 					);
				 					RETURN_ERROR;

				 /*if(current_foreach->idx_current>=((ScriptVarVector *)current_foreach->ptr)->m_objVector.size()){ // set true...
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

			WriteError(INSTRUCTION_GET_FILE_LINE(info_function,instruction),"operator type(%s) not implemented",Instruction::ByteCodeOpToStr(instruction->byte_code_op));
			RETURN_ERROR;


		 }

	lbl_exit_function:


		//=========================
		// POP STACK
		if(info_function->idx_script_function == 0){ // if main function only Remove 0s and preserve variables!)
			RemoveEmptySharedPointers(idx_current_stack,NULL);
		}
		else{
			while(ptrStartScopeInfo<=(current_scope_info_ptr)){
				PopScopeCall(idx_current_stack,callc_result.var_ref,0);
			}
		}

		idx_current_stack--;
		// POP STACK
		//=========================


		//popStack();
		return callc_result;
	}


	StackElement  * VirtualMachine::GetLastStackValue(){
		return (stk_current_data-1);
	}


	VirtualMachine::~VirtualMachine(){
		destroyCache();
	}
}
