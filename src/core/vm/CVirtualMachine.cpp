#include "../../CZetScript.h"

//CVirtualMachine * CVirtualMachine::m_virtualMachine = NULL;
//vector<CVirtualMachine::CVirtualMachine> CVirtualMachine::ALE;
// static: only defined in this module...


// general
#define PRINT_DUAL_ERROR_OP(c)\
string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp1),\
	   var_type2=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp2);\
\
print_error_cr("Error at line %i cannot perform operator \"%s\" %s  \"%s\"",\
		AST_LINE_VALUE(instruction->idxAstNode),\
		var_type1.c_str(),\
		c,\
		var_type2.c_str());



#define LOAD_NUMBER_OP(ptr_result_instruction) \
		*(((float *)(ptr_result_instruction->stkResultObject)))

#define LOAD_INT_OP(ptr_result_instruction) \
		(((int)(ptr_result_instruction->stkResultObject)))




#define LOAD_BOOL_OP(ptr_result_instruction) \
		(((bool)(ptr_result_instruction->stkResultObject)))


#define LOAD_STRING_OP(ptr_result_instruction) \
		*(((string *)(ptr_result_instruction->stkResultObject)))



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
 (((CScriptVariable *)(ptr_result_instruction->stkResultObject))->idxScriptClass==IDX_CLASS_VECTOR))

#define IS_GENERIC_NUMBER(properties) \
((properties & INS_PROPERTY_TYPE_INTEGER) |\
(properties & INS_PROPERTY_TYPE_NUMBER))


#define OP1_AND_OP2_ARE_NUMBERS \
(IS_GENERIC_NUMBER(properties_result_instruction_1) && IS_GENERIC_NUMBER(properties_result_instruction_2))

#define OP1_IS_STRING_AND_OP2_IS_NUMBER \
(properties_result_instruction_1 & INS_PROPERTY_TYPE_STRING) && \
IS_GENERIC_NUMBER(properties_result_instruction_2)

#define OP1_IS_NUMBER_AND_OP2_IS_STRING \
(properties_result_instruction_2 & INS_PROPERTY_TYPE_STRING) && \
IS_GENERIC_NUMBER(properties_result_instruction_1)

#define OP1_IS_STRING_AND_OP2_IS_BOOLEAN \
(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_STRING) && \
(ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_BOOLEAN)


#define OP1_AND_OP2_ARE_BOOLEANS \
(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_BOOLEAN) && \
(ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_BOOLEAN)

#define OP1_AND_OP2_ARE_STRINGS \
(properties_result_instruction_1 & INS_PROPERTY_TYPE_STRING) && \
(properties_result_instruction_2 & INS_PROPERTY_TYPE_STRING)

#define OP1_AND_OP2_ARE_UNDEFINED \
		(ptrResultInstructionOp1->stkResultObject == VM_UNDEFINED) && \
		(ptrResultInstructionOp2->stkResultObject == VM_UNDEFINED)

#define OP1_AND_OP2_ARE_NULL \
		(ptrResultInstructionOp1->stkResultObject == VM_NULL) && \
		(ptrResultInstructionOp2->stkResultObject == VM_NULL)



#define OP1_OR_OP2_IS_UNDEFINED \
		(ptrResultInstructionOp1->stkResultObject == VM_UNDEFINED) || \
		(ptrResultInstructionOp2->stkResultObject == VM_UNDEFINED)

#define OP1_OR_OP2_ARE_NULL \
		(ptrResultInstructionOp1->stkResultObject == VM_NULL) || \
		(ptrResultInstructionOp2->stkResultObject == VM_NULL)




#define PROCESS_NUM_OPERATION(__OVERR_OP__)\
					if (IS_INT(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){\
						PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);\
					}else if (IS_INT(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){\
						PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);\
					}else if (IS_NUMBER(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){\
						if(!pushNumber(LOAD_NUMBER_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2))) {\
							return NULL;\
						}\
					}else {\
						if(!pushNumber(LOAD_NUMBER_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_NUMBER_OP(ptrResultInstructionOp2))) {\
							return NULL;\
						}\
					}


#define PUSH_INTEGER(init_value,ptrAssignable,properties)\
*ptrStkCurrentResultInstruction={INS_PROPERTY_TYPE_INTEGER|properties,(void*)((int)(init_value)),ptrAssignable};

#define PUSH_BOOLEAN(init_value, ptrAssignable, properties)\
*ptrStkCurrentResultInstruction={INS_PROPERTY_TYPE_BOOLEAN|properties,(void *)((bool)(init_value)),ptrAssignable};

#define PUSH_VAR(init_value, ptrObjectRef,properties,is_new_var)\
{\
	bool push_var = is_new_var;\
	if(!push_var){\
		switch((init_value)->idxScriptClass){\
			case IDX_CLASS_INTEGER:\
				PUSH_INTEGER(*((int *)((CInteger *)init_value)->m_value),ptrObjectRef,properties);\
				break;\
			case IDX_CLASS_NUMBER:\
				pushNumber(*((float *)((CNumber *)(init_value))->m_value),ptrObjectRef,properties);\
				break;\
			case IDX_CLASS_STRING:\
				pushString(*((string *)((CString *)(init_value))->m_value),ptrObjectRef,properties);\
				break;\
			case IDX_CLASS_BOOLEAN:\
				PUSH_BOOLEAN(*((bool *)((CBoolean *)(init_value))->m_value),ptrObjectRef,properties);\
				break;\
			case IDX_CLASS_FUNCTOR:\
				pushFunction((CScriptFunctionObject *)(((CFunctor *)(init_value))->m_value),ptrObjectRef,properties);\
				break;\
			default:\
				push_var=true;\
		}\
	}\
	if(push_var){\
		if(idxStkCurrentLocalVar ==VM_ALE_OPERATIONS_MAX_STACK){\
			print_error_cr("Reached max object operations");\
			return NULL;\
		}\
		*ptrStkCurrentResultInstruction={INS_PROPERTY_TYPE_SCRIPTVAR|properties,init_value, ptrObjectRef};\
	}\
}


#define LOAD_CONSTANT(info_constant)\
	switch((info_constant)->type){\
		default:\
			print_error_cr("No constant value established");\
			return NULL;\
			break;\
		case INS_PROPERTY_TYPE_INTEGER:\
			PUSH_INTEGER(*((int *)(info_constant)->ptr),NULL,0);\
			break;\
		case INS_PROPERTY_TYPE_BOOLEAN:\
			PUSH_BOOLEAN(*((bool *)(info_constant)->ptr),NULL,0);\
			break;\
		case INS_PROPERTY_TYPE_STRING:\
			if(!pushString((*((string *)(info_constant)->ptr)))) return NULL;\
			break;\
		case INS_PROPERTY_TYPE_NUMBER:\
			if(!pushNumber(*((float *)(info_constant)->ptr))) return NULL;\
			break;\
		}


/*bool CVirtualMachine::LOAD_VARIABLE(tInfoAsmOp *iao,
		CScriptFunctionObject *local_function,
		CScriptVariable *this_object,
		tInfoAsmOp *asm_op)*/
#define LOAD_VARIABLE(iao,local_function,this_object,asm_op)\
{\
	CScriptVariable::tSymbolInfo *si;\
	CScriptVariable **ptr_var_object=NULL;\
	CScriptVariable *var_object = NULL;\
	int start_index_local_var = 0;\
	bool push_object=false;\
	unsigned int scope_type=GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);\
	unsigned int pre_post_operator_type=GET_INS_PROPERTY_PRE_POST_OP(iao->instruction_properties);\
	bool is_valid_variable = false;\
	PASTNode ast = NULL;\
	switch(scope_type){\
	default:\
		ptr_var_object = (CScriptVariable **)(&CVirtualMachine::stack[iao->index_op2].stkResultObject);\
		var_object = (CScriptVariable *)(CVirtualMachine::stack[iao->index_op2].stkResultObject);\
		break;\
	case INS_PROPERTY_ACCESS_SCOPE:\
	case INS_PROPERTY_THIS_SCOPE:\
		if(iao->idxAstNode != -1)\
			ast = AST_NODE(iao->idxAstNode);\
		if(scope_type == INS_PROPERTY_ACCESS_SCOPE){\
			CScriptVariable * base_var = *((CScriptVariable **)ptrStartStkResultInstruction[iao->index_op2].ptrObjectRef);\
			if((si = base_var->getVariableSymbol(ast->symbol_value))==NULL){\
				print_error_cr("Line %i: Variable \"%s\" as type \"%s\" has not symbol \"%s\"",ast->line_value,AST_SYMBOL_VALUE_CONST_CHAR(asm_op[iao->index_op2].idxAstNode),base_var->getClassName().c_str(), ast->symbol_value.c_str());\
				return NULL;\
			}\
		}\
		else{\
			if((si = this_object->getVariableSymbolByIndex(iao->index_op2))==NULL){\
				print_error_cr("cannot find symbol \"this.%s\"",ast->symbol_value.c_str());\
				return NULL;\
			}\
		}\
		ptr_var_object = (CScriptVariable **)(&si->object);\
		var_object = (CScriptVariable *)(si->object);\
		break;\
	case INS_PROPERTY_LOCAL_SCOPE:\
		if(iao->index_op2 >= idxStkCurrentLocalVar){\
			print_error_cr("internal error: index out of stack");\
			return NULL;\
		}\
		ptr_var_object = (CScriptVariable **)(&CVirtualMachine::basePtrLocalVar[start_index_local_var+iao->index_op2].stkResultObject);\
		var_object = (CScriptVariable *)(CVirtualMachine::basePtrLocalVar[start_index_local_var+iao->index_op2].stkResultObject);\
		break;\
	}\
	if(var_object != VM_UNDEFINED){\
		is_valid_variable = true;\
	}\
	push_object = true;\
	if(is_valid_variable){\
		int idx_class =  var_object->idxScriptClass;\
		if((pre_post_operator_type & INS_PROPERTY_PRE_DEC) | (pre_post_operator_type & INS_PROPERTY_PRE_INC)){\
			switch(idx_class){\
				case IDX_CLASS_INTEGER:\
					if(pre_post_operator_type & INS_PROPERTY_PRE_INC)\
						(*((int *)((CInteger *)var_object)->m_value))++;\
					else\
						(*((int *)((CInteger *)var_object)->m_value))--;\
					break;\
				case IDX_CLASS_NUMBER:\
					if(pre_post_operator_type & INS_PROPERTY_PRE_INC)\
						(*((float *)((CNumber *)var_object)->m_value))++;\
					else\
						(*((float *)((CNumber *)var_object)->m_value))--;\
					break;\
				default:\
					print_error_cr("internal error:Cannot perform preoperator ?? because is not number");\
					return NULL;\
					break;\
			}\
		}\
\
		if((pre_post_operator_type & INS_PROPERTY_POST_DEC) | (pre_post_operator_type & INS_PROPERTY_POST_INC)){\
			push_object = false;\
			switch(idx_class){\
				case IDX_CLASS_INTEGER:\
					PUSH_INTEGER(*((int *)((CInteger *)var_object)->m_value),NULL,0);\
					if(pre_post_operator_type & INS_PROPERTY_POST_INC)\
						(*((int *)((CInteger *)var_object)->m_value))++;\
					else\
						(*((int *)((CInteger *)var_object)->m_value))--;\
					break;\
				case IDX_CLASS_NUMBER:\
					pushNumber(*((float *)((CNumber *)var_object)->m_value));\
					if(pre_post_operator_type & INS_PROPERTY_POST_INC)\
						(*((float *)((CNumber *)var_object)->m_value))++;\
					else\
						(*((float *)((CNumber *)var_object)->m_value))--;\
					break;\
				default:\
					print_error_cr("internal error:cannot postoperator ?? because is not number");\
					return NULL;\
					break;\
			}\
		}\
	}\
\
	if(push_object){\
		if(pre_post_operator_type & INS_PROPERTY_PRE_NEG){\
			int idx_class = var_object->idxScriptClass;\
			if(idx_class==IDX_CLASS_INTEGER){\
				PUSH_INTEGER(-(*((int *)((CInteger *)var_object)->m_value)),NULL,0);\
			}else if(idx_class==IDX_CLASS_NUMBER){\
				if(!pushNumber(-(*((float *)((CNumber *)var_object)->m_value)))) return NULL;\
			}else{\
				print_error_cr("internal error:cannot perform pre operator - because is not number");\
				return NULL;\
			}\
		}else{\
			PUSH_VAR(var_object,ptr_var_object,0,false);\
		}\
	}\
}



#define ASSIGN_FROM_RESULT_INSTRUCTION(var, ptr_instruction){\
	CScriptVariable *aux_var=NULL;\
	if(*var == NULL){\
		if((*var = createVarFromResultInstruction(ptr_instruction)) == NULL){\
			return NULL;\
		}\
	}\
	int idxScriptClass = (*var)->idxScriptClass;\
	bool create_from_index=false;\
	unsigned int type_var = GET_INS_PROPERTY_TYPE_VAR(ptr_instruction->properties);\
	switch(type_var){\
		case INS_PROPERTY_TYPE_NULL:\
			*var = VM_NULL;\
			break;\
		case INS_PROPERTY_TYPE_UNDEFINED:\
			*var = VM_UNDEFINED;\
			break;\
		case INS_PROPERTY_TYPE_INTEGER:\
			if(idxScriptClass == IDX_CLASS_INTEGER){\
				*((int *)((CInteger *)(*var))->m_value)=((int)(ptr_instruction->stkResultObject));\
			}else\
			{\
				create_from_index=true;\
			}\
			break;\
		case INS_PROPERTY_TYPE_NUMBER:\
			if(idxScriptClass == IDX_CLASS_NUMBER){\
				*((float *)((CNumber *)(*var))->m_value) = *((float *)(ptr_instruction->stkResultObject));\
			}else\
			{\
				create_from_index=true;\
			}\
			break;\
		case INS_PROPERTY_TYPE_STRING:\
			if(idxScriptClass == IDX_CLASS_STRING){\
				*((string *)((CString  *)(*var))->m_value)= *((string *)(ptr_instruction->stkResultObject));\
			}else\
			{\
				create_from_index=true;\
			}\
			break;\
		case INS_PROPERTY_TYPE_BOOLEAN:\
			if(idxScriptClass == IDX_CLASS_BOOLEAN){\
				*((bool *)((CBoolean  *)aux_var)->m_value)= ((bool)(ptr_instruction->stkResultObject));\
			}else\
			{\
				create_from_index=true;\
			}\
			break;\
		case INS_PROPERTY_TYPE_FUNCTION:\
			if(idxScriptClass == IDX_CLASS_FUNCTOR){\
				((CFunctor  *)aux_var)->m_value= ((CScriptFunctionObject *)(ptr_instruction->stkResultObject));\
			}else{\
				create_from_index=true;\
			}\
			break;\
		case INS_PROPERTY_TYPE_SCRIPTVAR:\
			if((*var)->idx_shared_ptr != ZS_UNDEFINED_IDX){\
				unrefSharedPointer((*var)->idx_shared_ptr);\
			}\
			*var = (CScriptVariable *)(ptr_instruction->stkResultObject);\
			if(*var != VM_NULL && *var != VM_UNDEFINED){\
				sharePointer((*var)->idx_shared_ptr);\
			}\
			break;\
		default:\
				print_error_cr("internal error: unknow assignment %i!",type_var);\
				return NULL;\
				break;\
		}\
	if(create_from_index){\
		\
		if((*var)->idx_shared_ptr != ZS_UNDEFINED_IDX){\
			unrefSharedPointer((*var)->idx_shared_ptr);\
		}\
		if((*var = createVarFromResultInstruction(ptr_instruction)) == NULL){\
			return NULL;\
		}\
		sharePointer((*var)->idx_shared_ptr);\
	}\
}

#define LOAD_OP()\
switch(instruction->index_op1){\
case LOAD_TYPE::LOAD_TYPE_NULL:\
	PUSH_VAR(VM_NULL,NULL,0,false);\
	break;\
case LOAD_TYPE::LOAD_TYPE_UNDEFINED:\
	PUSH_VAR(VM_UNDEFINED,NULL,0,false);\
	break;\
case LOAD_TYPE::LOAD_TYPE_CONSTANT:\
	LOAD_CONSTANT((CCompiler::tInfoConstantValue *)instruction->index_op2);\
	break;\
case LOAD_TYPE::LOAD_TYPE_VARIABLE:\
	LOAD_VARIABLE(instruction, info_function,this_object,(*current_statment));\
	break;\
case LOAD_TYPE::LOAD_TYPE_FUNCTION:\
	if(!loadFunctionValue(instruction,info_function, this_object,*current_statment)){\
		return NULL;\
	}\
	break;\
case LOAD_TYPE::LOAD_TYPE_ARGUMENT:\
	PUSH_VAR(\
			(CScriptVariable *)basePtrLocalVar[instruction->index_op2+n_local_vars].stkResultObject,\
			basePtrLocalVar[instruction->index_op2+n_local_vars].ptrObjectRef,\
			INS_PROPERTY_IS_ARG,\
			false);\
	break;\
default:\
	print_error_cr("no load defined type");\
	return NULL;\
	break;\
}


#define CALL_GC\
	if(n_pointers_with_0_shares[idxCurrentStack]>0){\
		short *index = pointers_with_0_shares[idxCurrentStack];\
		do{\
			unrefSharedPointer(*(index++));\
		}while(--n_pointers_with_0_shares[idxCurrentStack]);\
	}\


#define RESET\
	idxStkCurrentNumber=startIdxStkNumber;\
	idxStkCurrentString=startIdxStkString;\
	ptrStkCurrentResultInstruction=ptrStartStkResultInstruction;\



string CVirtualMachine::STR_GET_TYPE_VAR_INDEX_INSTRUCTION(tAleObjectInfo *ptr_info_ale){
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
		result=((CScriptVariable *)ptr_info_ale->stkResultObject)->getClassName();
	}

	return result;
}

CVirtualMachine::CVirtualMachine(){

	//-----------------------------------------------------------
	// set memory manager
	for(int s = 0; s < MAX_STACK; s++){
		n_pointers_with_0_shares[s]=0;
		memset(shared_pointer[s],0,sizeof(shared_pointer[s]));
		n_freeCell[s]=MAX_UNIQUE_OBJECTS_POINTERS-1;
		for(int i = 0; i < MAX_UNIQUE_OBJECTS_POINTERS; i++){
			indexFreeCell[s][i]=i;
		}
	}

	idxCurrentStack=0;

	//------------------

	basePtrLocalVar=NULL;
	idxStkCurrentLocalVar=0;
	//idxSavedInstruction = 0;

	startIdxStkNumber=
	startIdxStkString=0;

	ptrStartStkResultInstruction=&stkResultInstruction[0];

	//instance_gc = NULL;
	//vec_ast_node = NULL;
	//VM_UNDEFINED=NULL;
	VM_UNDEFINED=UNDEFINED_SYMBOL;
	VM_NULL=NULL_SYMBOL;


	// push indexes ...

	RESET
}

#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __VERBOSE_MESSAGE__
#endif



#ifdef  __VERBOSE_MESSAGE__

#define print_vm_cr print_info_cr
#else
#define print_vm_cr(s,...)
#endif


//============================================================================================================================================

// POINTER MANANAGER

int CVirtualMachine::getFreeCell(){
	if(n_freeCell[idxCurrentStack] >= 0){
		int index= indexFreeCell[idxCurrentStack][n_freeCell[idxCurrentStack]];
		indexFreeCell[idxCurrentStack][n_freeCell[idxCurrentStack]] = -1;
		n_freeCell[idxCurrentStack]--;
		return index;
	}else{
		print_error_cr("Reached max assigment pointers (MAX: %i)",MAX_UNIQUE_OBJECTS_POINTERS);
	}

	return -1;

}

void CVirtualMachine::setFreeCell(int index_to_free){
	if(n_freeCell[idxCurrentStack] < (MAX_UNIQUE_OBJECTS_POINTERS-1)){

		n_freeCell[idxCurrentStack]++;
		indexFreeCell[idxCurrentStack][n_freeCell[idxCurrentStack]]=index_to_free;

	}else{
		print_error_cr("Reached min unique pointers (internal error)");
	}
}

int CVirtualMachine::getIdx0Shares(int index){

	if(index < 0){
		print_error_cr("index -1");
		return -1;
	}

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("bad reference shared pointers");
		return -1;
	}

	return shared_pointer[idxCurrentStack][index].idx_0_shares;
}

int CVirtualMachine::getNumShares(int index){

	if(index < 0){
		print_error_cr("index -1");
		return -1;
	}

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("bad reference shared pointers");
		return -1;
	}

	return shared_pointer[idxCurrentStack][index].n_shares;
}


void CVirtualMachine::remove0Shares(int index_0_share_idx){

	if(index_0_share_idx >=n_pointers_with_0_shares[idxCurrentStack]){
		print_error_cr("index_0_share_idx >=n_pointers_with_0_shares");
		return ;
	}

	if(n_pointers_with_0_shares[idxCurrentStack] > 0){

		// erase element ...
		memmove(pointers_with_0_shares[idxCurrentStack] + index_0_share_idx,
				pointers_with_0_shares[idxCurrentStack] + index_0_share_idx + 1,
				sizeof(pointers_with_0_shares[idxCurrentStack][0]) * (MAX_UNIQUE_OBJECTS_POINTERS - index_0_share_idx - 1));

		n_pointers_with_0_shares[idxCurrentStack]--;

	}
	else{
		print_error_cr("n_pointers_with_0_shares <= 0");
	}
}

int CVirtualMachine::insert0Shares(int shared_pointer_idx)
{
	if(n_pointers_with_0_shares[idxCurrentStack] >=MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("n_pointers_with_0_shares >=MAX_UNIQUE_OBJECTS_POINTERS");
		return -1;
	}

	pointers_with_0_shares[idxCurrentStack][n_pointers_with_0_shares[idxCurrentStack]] = shared_pointer_idx;

	n_pointers_with_0_shares[idxCurrentStack]++;

	return (n_pointers_with_0_shares[idxCurrentStack]-1);
}

int CVirtualMachine::newSharedPointer(CScriptVariable *var_ptr){
	int index = CVirtualMachine::getFreeCell();

	if(index != -1){
		shared_pointer[idxCurrentStack][index].n_shares=0;
		shared_pointer[idxCurrentStack][index].shared_ptr=var_ptr;

		shared_pointer[idxCurrentStack][index].idx_0_shares=insert0Shares(index);

		return index;
	}
	return -1;
}

void CVirtualMachine::sharePointer(int index){

	if(index < 0){
		print_error_cr("index -1");
		return;
	}

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("bad reference shared pointers");
		return;
	}

	if(shared_pointer[idxCurrentStack][index].shared_ptr != NULL){
		shared_pointer[idxCurrentStack][index].n_shares++; // remove from 0 shares

		if(shared_pointer[idxCurrentStack][index].idx_0_shares != -1){
			remove0Shares(shared_pointer[idxCurrentStack][index].idx_0_shares);
			shared_pointer[idxCurrentStack][index].idx_0_shares= -1;
		}
	}
}

void CVirtualMachine::unrefSharedPointer( int index){

	if(index < 0){
		print_error_cr("index -1");
		return;
	}

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("bad reference shared pointers");
		return;
	}

	if(shared_pointer[idxCurrentStack][index].shared_ptr != NULL){

		if(shared_pointer[idxCurrentStack][index].n_shares > 0){
			shared_pointer[idxCurrentStack][index].n_shares--;
		}

		if(shared_pointer[idxCurrentStack][index].n_shares==0){
			delete shared_pointer[idxCurrentStack][index].shared_ptr;
			shared_pointer[idxCurrentStack][index].shared_ptr = NULL;
			setFreeCell(index);
		}
	}
}

bool CVirtualMachine::push(){ // save current stack
	if(idxCurrentStack < MAX_STACK){
		idxCurrentStack++;
		return true;
	}
	else{
		print_error_cr("Reached max stack");
	}
	return false;
}

bool CVirtualMachine::pop(){ // restore current stack.
	if(idxCurrentStack > 0){
		idxCurrentStack--;
		return true;
	}
	else{
		print_error_cr("Reached min stack");
	}
	return false;
}

#define LDR_OP1 ptrResultInstructionOp1=&ptrStartStkResultInstruction[instruction->index_op1];properties_result_instruction_1=ptrResultInstructionOp1->properties;
#define LDR_OP2 ptrResultInstructionOp2=&ptrStartStkResultInstruction[instruction->index_op2];properties_result_instruction_2=ptrResultInstructionOp2->properties;
#define LAST_INSTRUCTION   (ptrStkCurrentResultInstruction-1)

//============================================================================================================================================

CScriptVariable * CVirtualMachine::execute(CScriptFunctionObject *info_function, CScriptVariable *this_object, vector<CScriptVariable *> * argv, int stk){

	print_debug_cr("Executing function %s ...",info_function->object_info.symbol_info.symbol_name.c_str());

	if((info_function->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // C-Call

		void * fun_ptr = (void *)info_function->object_info.symbol_info.ref_ptr;

		if((info_function->object_info.symbol_info.properties &  SYMBOL_INFO_PROPERTIES::PROPERTY_STATIC_REF) != SYMBOL_INFO_PROPERTIES::PROPERTY_STATIC_REF){ // if not static then is function depends of object ...

			if(this_object!= NULL && this_object != CZetScript::getInstance()->getMainObject()){
				fun_ptr = this_object->getFunctionSymbolByIndex(info_function->object_info.symbol_info.idxSymbol)->proxy_ptr;
			}
		}

		return CZetScript::call_C_function(fun_ptr,info_function,argv);
	}

	PASTNode ast = AST_NODE(info_function->object_info.symbol_info.idxAstNode);

	int scope_index =  ast->idxScope;

	if(ast->keyword_info!=NULL){
		if(ast->keyword_info->id == KEYWORD_TYPE::FUNCTION_KEYWORD){
			scope_index=ast->children[1]->idxScope;
		}
	}

	CScriptVariable *ret=VM_UNDEFINED;
	PtrStatment m_listStatements = info_function->object_info.statment_op;
	int jmp_to_statment =ZS_UNDEFINED_IDX;
	int jmp_to_instruction = ZS_UNDEFINED_IDX;
	//bool end_by_ret=false;

	// reserve vars and assign argv vars ...
	pushStack(info_function, argv);

	bool	aux_boolean=false;
	unsigned char operator_type=NOP;
	CScriptVariable **obj=NULL;
	CScriptFunctionObject * aux_function_info=NULL;
	CScriptVariable *ret_obj=NULL, *svar=NULL;
	CScriptFunctionObject *constructor_function=NULL;
	CScriptVariable *calling_object=NULL;
	int n_local_vars =  info_function->object_info.local_symbols.m_registeredVariable.size();
	tAleObjectInfo *stkResultInstructionIteration = ptrStartStkResultInstruction;
	tAleObjectInfo *ptrResultInstructionOp1=NULL;//&stkResultInstruction[index_op1+startIdxStkResultInstruction];
	tAleObjectInfo *ptrResultInstructionOp2=NULL;//&stkResultInstruction[index_op2+startIdxStkResultInstruction];
	unsigned short properties_result_instruction_1,properties_result_instruction_2;
	//tAleObjectInfo *ptrResultLastInstruction=NULL;//&stkResultInstruction[idxStkCurrentResultInstruction-1];
	//unsigned int prop1, prop2;

	//int index_op1 = -1;//
	//int index_op2 = -1;
	tAleObjectInfo *ptrSavedInstruction=NULL;
	PtrStatment current_statment = NULL,
				ptr_statment_iteration=m_listStatements;

	tInfoAsmOp *instruction_it,*instruction;

	for(;;){ // foreach stament ...

		current_statment = ptr_statment_iteration++;
		instruction_it = *current_statment;

	 	if(!instruction_it){
	 		goto lbl_exit_function;
	 	}


	 	// reset vars ...
		//jmp_to_statment = ZS_UNDEFINED_IDX;
		idxStkCurrentNumber=startIdxStkNumber;
		idxStkCurrentString=startIdxStkString;
		stkResultInstructionIteration=ptrStartStkResultInstruction;


		//-----------------------------------------------------------------------------------------------------------------------
		//
		// START OPERATOR MANAGEMENT
		//

		 for(;;){ // foreach asm instruction ...

			instruction = instruction_it++;
			ptrStkCurrentResultInstruction=stkResultInstructionIteration++;


			switch(instruction->operator_type){
			default:
				print_error_cr("operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
				return NULL;
				break;
			case INVALID_OP:
				goto lbl_exit_statment;
			case NOP: // ignore ...
				continue;
			case LOAD: // load value in function of value/constant ...
				LOAD_OP();
				continue;
			case MOV: // mov value expression to var
				LDR_OP1;
				LDR_OP2;

				// ok load object pointer ...
				if((obj = ptrResultInstructionOp1->ptrObjectRef) != NULL) {// == CScriptVariable::VAR_TYPE::OBJECT){
					// get pointer object (can be assigned)
					ASSIGN_FROM_RESULT_INSTRUCTION(ptrResultInstructionOp1->ptrObjectRef,ptrResultInstructionOp2);

				}else{
					print_error_cr("Expected object l-value mov");
					return NULL;
				}

				continue;

			case EQU:  // == --> boolean && boolean or string && string or number && number
				LDR_OP1;
				LDR_OP2;
				if(OP1_AND_OP2_ARE_BOOLEANS) {
					PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) == LOAD_BOOL_OP(ptrResultInstructionOp2),NULL,0);
				}else if(OP1_AND_OP2_ARE_STRINGS){
					PUSH_BOOLEAN(LOAD_STRING_OP(ptrResultInstructionOp1) == LOAD_STRING_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) == LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) == LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) == LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) == LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if(OP1_AND_OP2_ARE_UNDEFINED){
					PUSH_BOOLEAN(true,NULL,0);
				}else if(OP1_AND_OP2_ARE_NULL){
					PUSH_BOOLEAN(true,NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("==");
					return NULL;
				}

				break;

			case NOT_EQU:  // == --> boolean && boolean or string && string or number && number
				LDR_OP1;
				LDR_OP2;
				if(OP1_AND_OP2_ARE_BOOLEANS) {
					PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) != LOAD_BOOL_OP(ptrResultInstructionOp2),NULL,0);
				}else if(OP1_AND_OP2_ARE_STRINGS){
					PUSH_BOOLEAN(LOAD_STRING_OP(ptrResultInstructionOp1) != LOAD_STRING_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) != LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) != LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) != LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) != LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("!=");
					return NULL;
				}

				break;
			case LT:  // <
				LDR_OP1;
				LDR_OP2;

				if (IS_INT(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) < LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) < LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) < LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) < LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("<");
					return NULL;
				}
				continue;
			case LTE:  // <=
				LDR_OP1;
				LDR_OP2;
				if (IS_INT(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) <= LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) <= LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) <= LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) <= LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("<=");
					return NULL;
				}

				break;
			case NOT: // !
				LDR_OP1;
				if (ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_BOOLEAN){
					PUSH_BOOLEAN(!LOAD_BOOL_OP(ptrResultInstructionOp1),NULL,0);
				}else{
					print_error_cr("Expected operands 1 as boolean!");
					return NULL;
				}
				break;
			case NEG: // !
				LDR_OP1;
				LDR_OP2;
				if (IS_GENERIC_NUMBER(properties_result_instruction_1)){
					if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_INTEGER){ // operation will result as integer.
						PUSH_INTEGER(-LOAD_INT_OP(ptrResultInstructionOp1),NULL,0);
					}
					else{
						if(!pushNumber(-LOAD_NUMBER_OP(ptrResultInstructionOp2))){
							return NULL;
						}
					}

				}else{
						print_error_cr("Line %i:Expected operands 1 as number or integer!",AST_LINE_VALUE(instruction->idxAstNode));
						return NULL;
				}
				break;

			case GT:  // >
				LDR_OP1;
				LDR_OP2;
				if (IS_INT(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) > LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) > LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) > LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) > LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP(">");
					return NULL;
				}
				break;
			case GTE: // >=
				LDR_OP1;
				LDR_OP2;
				if (IS_INT(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) >= LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) >= LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) >= LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) >= LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP(">=");
					return NULL;
				}
				break;

			case ADD: // +

				LDR_OP1;
				LDR_OP2;
				// get indexes and check whether is possible or not its calculation.
				// check indexes
				if (IS_STRING(properties_result_instruction_1) && (IS_UNDEFINED(properties_result_instruction_2) || IS_VAR(properties_result_instruction_2))){
					string result = "undefined";
					if(IS_VAR(properties_result_instruction_2)){
						result = ((CScriptVariable *)(ptrResultInstructionOp2->stkResultObject))->getClassName();
					}
					if(!pushString(LOAD_STRING_OP(ptrResultInstructionOp1)+result)) return NULL;
				}else if (IS_INT(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) + LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) + LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
					if(!pushNumber(LOAD_NUMBER_OP(ptrResultInstructionOp1) + LOAD_INT_OP(ptrResultInstructionOp2))) return NULL;
				}else if (IS_NUMBER(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
					if(!pushNumber(LOAD_NUMBER_OP(ptrResultInstructionOp1) + LOAD_NUMBER_OP(ptrResultInstructionOp2))) return NULL;
				}else if(OP1_IS_STRING_AND_OP2_IS_NUMBER){ // concatenate string + number

					aux_string =  LOAD_STRING_OP(ptrResultInstructionOp1);

					if(ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_INTEGER)
						aux_string = aux_string + CStringUtils::intToString(LOAD_INT_OP(ptrResultInstructionOp2));
					else
						aux_string = aux_string + CStringUtils::intToString(LOAD_NUMBER_OP(ptrResultInstructionOp2));

					if(!pushString(aux_string)) return NULL;
				}else if(OP1_IS_NUMBER_AND_OP2_IS_STRING){ // concatenate string + number

					aux_string =  LOAD_STRING_OP(ptrResultInstructionOp2);

					if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_INTEGER)
						aux_string = CStringUtils::intToString(LOAD_INT_OP(ptrResultInstructionOp1))+ aux_string;
					else
						aux_string = CStringUtils::intToString(LOAD_NUMBER_OP(ptrResultInstructionOp1))+aux_string;

					if(!pushString(aux_string)) return NULL;
				}else if(OP1_IS_STRING_AND_OP2_IS_BOOLEAN){ // concatenate string + boolean

					aux_string =  LOAD_STRING_OP(ptrResultInstructionOp1);
					aux_boolean =  LOAD_BOOL_OP(ptrResultInstructionOp2);

					if(aux_boolean)
						aux_string = aux_string + "true";
					else
						aux_string = aux_string + "false";

					if(!pushString(aux_string)) return NULL;

				}else if(OP1_AND_OP2_ARE_STRINGS){ // concatenate string + boolean

					if(!pushString(LOAD_STRING_OP(ptrResultInstructionOp1)+LOAD_STRING_OP(ptrResultInstructionOp2))) return NULL;

				}else{

					// full error description ...
					PRINT_DUAL_ERROR_OP("+");
					return NULL;
				}

				break;

			case LOGIC_AND: // &&
				LDR_OP1;
				LDR_OP2;
				if(OP1_AND_OP2_ARE_BOOLEANS) {
					PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) && LOAD_BOOL_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("&&");
					return NULL;
				}
				break;
			case LOGIC_OR:  // ||
				LDR_OP1;
				LDR_OP2;
				if(OP1_AND_OP2_ARE_BOOLEANS) {
					PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) || LOAD_BOOL_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("||");
					return NULL;
				}
				break;
			case DIV: // /
				LDR_OP1;
				LDR_OP2;
				if(OP1_AND_OP2_ARE_NUMBERS) {

					if(IS_INT(properties_result_instruction_2)){
						if(LOAD_INT_OP(ptrResultInstructionOp2) == 0) {
							print_error_cr("Divide by 0 at line %i.",AST_LINE_VALUE(instruction->idxAstNode));
							return NULL;
						}
					}else{
						if(LOAD_NUMBER_OP(ptrResultInstructionOp2) == 0) {
							print_error_cr("Divide by 0 at line %i.",AST_LINE_VALUE(instruction->idxAstNode));
							return NULL;
						}
					}

					PROCESS_NUM_OPERATION(/);
				}else{
					PRINT_DUAL_ERROR_OP("/");
					return NULL;
				}

				break;
			case MUL: // *
				LDR_OP1;
				LDR_OP2;
				if(OP1_AND_OP2_ARE_NUMBERS) {
						PROCESS_NUM_OPERATION(*);

				}else{
					PRINT_DUAL_ERROR_OP("*");
					return NULL;
				}
				continue;
			case MOD:  // %

				LDR_OP1;
				LDR_OP2;

				if(OP1_AND_OP2_ARE_NUMBERS) {

					if(IS_INT(properties_result_instruction_2)){
						if(LOAD_INT_OP(ptrResultInstructionOp2) == 0) {
							print_error_cr("Divide by 0 at line %i.",AST_LINE_VALUE(instruction->idxAstNode));
							return NULL;
						}
					}else{
						if(LOAD_NUMBER_OP(ptrResultInstructionOp2) == 0) {
							print_error_cr("Divide by 0 at line %i.",AST_LINE_VALUE(instruction->idxAstNode));
							return NULL;
						}
					}
					if (IS_INT(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
						PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) % LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
					}else if (IS_INT(properties_result_instruction_1) && IS_NUMBER(properties_result_instruction_2)){
						PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) % ((int) LOAD_NUMBER_OP(ptrResultInstructionOp2)),NULL,0);

					}else if (IS_NUMBER(properties_result_instruction_1) && IS_INT(properties_result_instruction_2)){
						if(!pushNumber(fmod(LOAD_NUMBER_OP(ptrResultInstructionOp1), LOAD_INT_OP(ptrResultInstructionOp2)))) {
							return NULL;
						}
					}else {
						if(!pushNumber(fmod(LOAD_NUMBER_OP(ptrResultInstructionOp1) , LOAD_NUMBER_OP(ptrResultInstructionOp2)))) {
							return NULL;
						}
					}

				}else{
					PRINT_DUAL_ERROR_OP("%");
					return NULL;
				}

				break;
			case AND: // bitwise logic and
				LDR_OP1;
				LDR_OP2;
				if((properties_result_instruction_1 & INS_PROPERTY_TYPE_INTEGER) && (properties_result_instruction_2 & INS_PROPERTY_TYPE_INTEGER)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) & LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("&");
					return NULL;
				}
				break;
			case OR: // bitwise logic or
				LDR_OP1;
				LDR_OP2;
				if((properties_result_instruction_1 & INS_PROPERTY_TYPE_INTEGER) && (properties_result_instruction_2 & INS_PROPERTY_TYPE_INTEGER)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) | LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("|");
					return NULL;
				}

				break;
			case XOR: // logic xor
				LDR_OP1;
				LDR_OP2;
				if((properties_result_instruction_1 & INS_PROPERTY_TYPE_INTEGER) && (properties_result_instruction_2 & INS_PROPERTY_TYPE_INTEGER)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) ^ LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("^");
					return NULL;
				}
				break;
			case SHL: // shift left
				LDR_OP1;
				LDR_OP2;
				if((properties_result_instruction_1 & INS_PROPERTY_TYPE_INTEGER) && (properties_result_instruction_2 & INS_PROPERTY_TYPE_INTEGER)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) << LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP(">>");
					return NULL;
				}
				break;
			case SHR: // shift right
				LDR_OP1;
				LDR_OP2;
				if((properties_result_instruction_1 & INS_PROPERTY_TYPE_INTEGER) && (properties_result_instruction_2 & INS_PROPERTY_TYPE_INTEGER)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) >> LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("<<");
					return NULL;
				}
				break;
			// special internal ops...
			case JMP:
				jmp_to_statment = instruction->index_op1;
				jmp_to_instruction = instruction->index_op2;
				break;
			case JNT: // goto if not true ... goes end to conditional.

				if(LAST_INSTRUCTION->properties & INS_PROPERTY_TYPE_BOOLEAN){

					if(!((bool)(LAST_INSTRUCTION->stkResultObject))){
						jmp_to_statment = instruction->index_op1;
						jmp_to_instruction = instruction->index_op2;
					}
				}else{
					print_error_cr("No boolean elements");
					return NULL;
				}
				break;
			case JT: // goto if true ... goes end to conditional.
				if(LAST_INSTRUCTION->properties & INS_PROPERTY_TYPE_BOOLEAN){

					if(((bool)(LAST_INSTRUCTION->stkResultObject))){
						jmp_to_statment = instruction->index_op1;
					}
				}else{
					print_error_cr("No boolean elements");
					return NULL;
				}
				break;
			case CALL: // calling function after all of args are processed...
				// check whether signatures matches or not ...
				// 1. get function object ...
				LDR_OP1;
				LDR_OP2;
				aux_function_info=(CScriptFunctionObject *)ptrResultInstructionOp1->stkResultObject;

				if(aux_function_info == NULL){ // we must find function ...

					tInfoAsmOp *iao = &(*current_statment)[instruction->index_op1];
					unsigned scope_type = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);
					CScriptVariable::tSymbolInfo * si=NULL;
					CScriptVariable **script_var=NULL;

					if(iao->index_op2 == ZS_UNDEFINED_IDX || scope_type == INS_PROPERTY_ACCESS_SCOPE){
						vector<int> *vec_global_functions=&(GET_MAIN_FUNCTION_OBJECT->object_info.local_symbols.vec_idx_registeredFunction);
						bool all_check=true;
						bool found = NULL;
						CScriptVariable * base_var=NULL;

						vector<CScriptVariable *> *argv = &m_functionArgs;


						switch(scope_type){
						default: // global default!

							// try first all function with no conversion and then try the same with conversions ...

							// for all symbols from calling object ...
							for(int h=0; h < 2 && !found; h++){

								for(int i = 0; i < (int)vec_global_functions->size() && !found; i++){

									CScriptFunctionObject *irfs = GET_SCRIPT_FUNCTION_OBJECT(vec_global_functions->at(i));

									if(irfs->object_info.symbol_info.symbol_name == AST_SYMBOL_VALUE(iao->idxAstNode) && (irfs->m_arg.size() == argv->size())){


										all_check=true;
										// convert parameters script to c...
										for( int k = 0; k < (int)argv->size() && all_check;k++){
											//converted_param[i]= (int)(argv->at(i));
											if(h==0){
												all_check = argv->at(k)->getPointer_C_ClassName()==irfs->m_arg[k];
											}
											else{
												if((argv->at(k))->getPointer_C_ClassName()!=irfs->m_arg[k]){
													all_check =CScriptClass::getConversionType((argv->at(k))->getPointer_C_ClassName(),irfs->m_arg[k], NULL)!=NULL;
												}
											}
										}

										if(all_check){ // we found the right function ...
											iao->index_op2 = i;
											found=true;
										}
									}
								}
							}

							// update structure ...
							if(found){
								aux_function_info = GET_SCRIPT_FUNCTION_OBJECT((*vec_global_functions)[iao->index_op2]);
							}else{
								print_error_cr("Cannot find right C symbol for \"%s\"",AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode));
							}
							break;

						case INS_PROPERTY_ACCESS_SCOPE:

							script_var = (CScriptVariable **)ptrResultInstructionOp1->ptrObjectRef;////((CScriptVariable **)stkResultInstruction[iao->index_op2+startIdxStkResultInstruction].ptrObjectRef);
							base_var = *script_var;
							for(int h=0; h < 2 && !found; h++){ // h=0 -> match signature, 1=doesn't match signature
								int idx_function;
								if(h==0){
									idx_function=base_var->getidxScriptFunctionObjectWithMatchArgs(AST_SYMBOL_VALUE(iao->idxAstNode),argv, true);
								}else{
									idx_function=base_var->getidxScriptFunctionObjectWithMatchArgs(AST_SYMBOL_VALUE(iao->idxAstNode),argv, NULL);
								}

								if(idx_function != ZS_UNDEFINED_IDX){
									si = base_var->getFunctionSymbolByIndex(idx_function);
									found = true;
								}

							}

							if(si != NULL){
								aux_function_info = (CScriptFunctionObject *)si->object;
							}

							if(aux_function_info == NULL){
								print_error_cr("cannot find function \"%s\" at line %i",AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode),AST_LINE_VALUE(iao->idxAstNode));
								return NULL;
							}

							break;

						}
					}
					else{
						print_error_cr("Unexpected C calling exception");
					}
				}

				if((ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_FUNCTION)==0){
					if((ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_SCRIPTVAR) && ((CScriptVariable *)ptrResultInstructionOp1->stkResultObject)->idxScriptClass == IDX_CLASS_FUNCTOR){
						aux_function_info = (CScriptFunctionObject *)(((CFunctor *)ptrResultInstructionOp1->stkResultObject)->m_value);
					}else {
						print_error_cr("object \"%s\" is not function at line %i",AST_SYMBOL_VALUE_CONST_CHAR(instruction->idxAstNode), AST_LINE_VALUE(instruction->idxAstNode));
						return NULL;
					}
				}

				calling_object = this_object;
				if((instruction->instruction_properties & INS_PROPERTY_CALLING_OBJECT) != 0){
					calling_object= ptrResultInstructionOp1->ptrObjectRef!=NULL?*ptrResultInstructionOp1->ptrObjectRef:NULL;
				}
				else if((aux_function_info->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){

				}

				push();

				// by default virtual machine gets main object class in order to run functions ...
				if((ret_obj=execute(aux_function_info,calling_object,&m_functionArgs))==NULL){
					return NULL;
				}

				pop();

				// if function is C must register pointer !

				//if((aux_function_info->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // C-Call
				if((instruction->instruction_properties & INS_PROPERTY_DIRECT_CALL_RETURN) == 0){ // share pointer
					if(ret_obj != VM_UNDEFINED && ret_obj != VM_NULL){

						if(!ret_obj->refSharedPtr()){
							return NULL;
						}
					}
				}

				// deallocates stack...
				m_functionArgs.clear();

				// finally set result value into stkResultObject...
				PUSH_VAR(ret_obj,NULL,0,false);
				break;
			case PUSH: // push arg instruction will creating object ensures not to have feature e/s...
				LDR_OP1;
				if((svar = createVarFromResultInstruction(ptrResultInstructionOp1)) == NULL){
					return NULL;
				}

				m_functionArgs.push_back(svar);
				break;
			case CLR: // clear args
				m_functionArgs.clear();
				break;
			case VGET: // vector access after each index is processed...
				LDR_OP1;
				LDR_OP2;
				// index_op1 is vector, index op2 is index...
				if(IS_VECTOR(ptrResultInstructionOp1)){
					if(IS_INT(properties_result_instruction_2)){
						// determine object ...
						CVector * vec = (CVector *)(ptrResultInstructionOp1->stkResultObject);
						int v_index = LOAD_INT_OP(ptrResultInstructionOp2);

						//print_info_cr("%i",v_index);

						// check indexes ...
						if(v_index < 0 || v_index >= (int)(vec->m_objVector.size())){
							print_error_cr("Line %i. Index vector out of bounds!",AST_LINE_VALUE(instruction->idxAstNode));
							return NULL;
						}

						PUSH_VAR(vec->m_objVector[v_index],&vec->m_objVector[v_index],0,false);

					}else{
						print_error_cr("Expected vector-index as integer");
						return NULL;
					}
				}
				else{
					print_error_cr("Line %i: Variable \"%s\" is not type vector",
							AST_LINE_VALUE(instruction->idxAstNode),
							AST_SYMBOL_VALUE_CONST_CHAR((*current_statment)[instruction->index_op1].idxAstNode),
							AST_SYMBOL_VALUE_CONST_CHAR(instruction->idxAstNode)
							//base_var->getClassName().c_str(), iao->ast_node->symbol_value.c_str()
							);
					return NULL;
				}

				break;
			case VPUSH: // Value push for vector
				LDR_OP1;
				LDR_OP2;
				if(IS_VECTOR(ptrResultInstructionOp1)){
					CVector * vec = (CVector *)(ptrResultInstructionOp1->stkResultObject);
					if((svar = createVarFromResultInstruction(ptrResultInstructionOp2)) == NULL){
						return NULL;
					}
					vec->m_objVector.push_back(svar);
					if(svar != VM_NULL && svar != VM_UNDEFINED){
						// add share + 1
						sharePointer(svar->idx_shared_ptr);
					}
				}else{
					print_error_cr("Expected operand 1 as vector");
					return NULL;
				}
				break;
			case VEC: // Create new vector object...
				svar=NEW_VECTOR_VAR;
				PUSH_VAR(svar,NULL,0,false);

				if(!svar->refSharedPtr()){
					return NULL;
				}
				break;

			case RET:

				break;
			case NEW:
				svar=NEW_CLASS_VAR_BY_IDX(instruction->index_op1);
				PUSH_VAR(svar, NULL,0, true);

				if(!svar->refSharedPtr()){
					return NULL;
				}

				// execute its constructor ...
				push();

				if((constructor_function = svar->getConstructorFunction()) != NULL){
					execute(constructor_function,svar);
				}

				pop();

				break;

			case SAVE_I: // get Last For ternary condition ...
				ptrSavedInstruction=ptrStkCurrentResultInstruction-1;
				break;
			case LOAD_I: // For ternary condition ...

				switch(GET_INS_PROPERTY_TYPE_VAR(ptrSavedInstruction->properties)){
				default:
					print_error_cr("unexpected type");
					break;
				case INS_PROPERTY_TYPE_INTEGER:
					PUSH_INTEGER(((int)ptrSavedInstruction->stkResultObject),NULL,0);
					break;
				case INS_PROPERTY_TYPE_BOOLEAN:
					PUSH_BOOLEAN(ptrSavedInstruction->stkResultObject,NULL,0);
					break;
				case INS_PROPERTY_TYPE_NUMBER:
					if(!pushNumber(*((float *)ptrSavedInstruction->stkResultObject))){
						return NULL;
					}
					break;


				case INS_PROPERTY_TYPE_STRING:
					if(!pushString(*((string *)ptrSavedInstruction->stkResultObject))){
						return NULL;
					}
					break;
				case INS_PROPERTY_TYPE_SCRIPTVAR:
					PUSH_VAR(((CScriptVariable *)ptrSavedInstruction->stkResultObject),NULL,0,false);
					break;
				}
				break;
			case POP_SCOPE:
				popScope(info_function,instruction->index_op1);
				break;

			case DECL_STRUCT: // symply creates a variable ...
				svar=NEW_STRUCT;
				PUSH_VAR(svar,NULL,0,false);
				if(!svar->refSharedPtr())
					return NULL;
				break;
			case PUSH_ATTR:
				LDR_OP1;
				LDR_OP2;
				// get symbol ... (current instruction -1)
				if(LAST_INSTRUCTION->properties & INS_PROPERTY_TYPE_STRING){

					CScriptVariable::tSymbolInfo *si;
					string *variable_name = (string *)LAST_INSTRUCTION->stkResultObject;
					if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_SCRIPTVAR){
						CScriptVariable *var1=(CScriptVariable *)ptrResultInstructionOp1->stkResultObject;
						if(var1->idxScriptClass == IDX_CLASS_STRUCT){//::getInstance()->getIdxClassStruct()){

								if(ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_FUNCTION){
									si=var1->addFunctionSymbol(*variable_name, instruction->idxAstNode,(CScriptFunctionObject *)ptrResultInstructionOp2->stkResultObject);
								}else{
									si=var1->addVariableSymbol(*variable_name, instruction->idxAstNode);

									ASSIGN_FROM_RESULT_INSTRUCTION((CScriptVariable **)&si->object ,ptrResultInstructionOp2);

								}

						}else{
							print_error_cr("Struct: Expected operant 1 as struct at line %i",AST_LINE_VALUE(instruction->idxAstNode));
							return NULL;
						}

					}
					else{
						print_error_cr("Struct: Op1 is not a var");
					}

				}
				else{
					print_error_cr("Struct: Expected symbol at line %i",AST_LINE_VALUE(instruction->idxAstNode));
					return NULL;
				}
				break;
			}

			//
			// END OPERATOR MANAGEMENT
			//
			//-----------------------------------------------------------------------------------------------------------------------

			if(operator_type == ASM_OPERATOR::RET){ // return...

				if((ret=createVarFromResultInstruction(&ptrStartStkResultInstruction[instruction->index_op1], false)) == NULL){ // share pointer but not add as shared!
					return NULL;
				}
				//end_by_ret=true;
				goto lbl_exit_function;
				//break;
			}

			if(jmp_to_statment != ZS_UNDEFINED_IDX){ // jmp detected ...

				if(current_statment!=&m_listStatements[jmp_to_statment]){ // assign current statment and break loop-for
					ptr_statment_iteration=&m_listStatements[jmp_to_statment];
					jmp_to_statment=ZS_UNDEFINED_IDX;
					break; // break for
				}

				if(jmp_to_instruction != ZS_UNDEFINED_IDX){
					instruction=&(*current_statment)[jmp_to_instruction];
					ptrStkCurrentResultInstruction=&stkResultInstruction[jmp_to_instruction+1];
					jmp_to_instruction=ZS_UNDEFINED_IDX;
					jmp_to_statment=ZS_UNDEFINED_IDX;
				}else{

				}
			}

		}

lbl_exit_statment:

		CALL_GC;

	}//while(ptr_statment_iteration->asm_op!=NULL && !end_by_ret);

lbl_exit_function:

	popScope(info_function,scope_index);
	popStack();
	return ret;
}


void CVirtualMachine::pushStack(CScriptFunctionObject *info_function, vector<CScriptVariable *> * argv){
	unsigned n_arg_size = info_function->m_arg.size();
	unsigned n_local_vars = info_function->object_info.local_symbols.m_registeredVariable.size();
	unsigned n_total_vars = n_arg_size+n_local_vars;

	if((idxStkCurrentLocalVar+n_total_vars) >=  VM_LOCAL_VAR_MAX_STACK){
		print_error_cr("Error MAXIMUM stack size reached");
		exit(EXIT_FAILURE);
	}

	basePtrLocalVar=&stack[CVirtualMachine::idxStkCurrentLocalVar];

	// init local vars ...
	for(unsigned i = 0; i < n_local_vars; i++){
		basePtrLocalVar[i].ptrObjectRef = NULL;
		basePtrLocalVar[i].properties = INS_PROPERTY_TYPE_UNDEFINED;
		basePtrLocalVar[i].stkResultObject=VM_UNDEFINED;
	}

	// init argv vars ...
	for(unsigned i = 0; i < n_arg_size; i++){

		basePtrLocalVar[n_local_vars+i].ptrObjectRef = &argv->at(i);
		//basePtrLocalVar[n_local_vars+i].type = INS_TYPE_UNDEFINED;
		CScriptVariable *var = argv->at(i);//CScriptVariable::UndefinedSymbol;
		basePtrLocalVar[n_local_vars+i].stkResultObject = var;

		switch(var->idxScriptClass){ // check bsic types...
		case IDX_CLASS_UNDEFINED:
			basePtrLocalVar[n_local_vars+i].properties =INS_PROPERTY_TYPE_UNDEFINED;
			break;
		case IDX_CLASS_NULL:
			basePtrLocalVar[n_local_vars+i].properties =INS_PROPERTY_TYPE_NULL;
			break;
		case IDX_CLASS_INTEGER:
			basePtrLocalVar[n_local_vars+i].properties =INS_PROPERTY_TYPE_INTEGER;
			break;
		case IDX_CLASS_NUMBER:
			basePtrLocalVar[n_local_vars+i].properties =INS_PROPERTY_TYPE_NUMBER;
			break;
		case IDX_CLASS_STRING:
			basePtrLocalVar[n_local_vars+i].properties =INS_PROPERTY_TYPE_STRING;
			break;
		case IDX_CLASS_FUNCTOR:
			basePtrLocalVar[n_local_vars+i].properties =INS_PROPERTY_TYPE_FUNCTION;
			break;
		case IDX_CLASS_BOOLEAN:
			basePtrLocalVar[n_local_vars+i].properties =INS_PROPERTY_TYPE_BOOLEAN;
			break;
		default: // any default VAR
			basePtrLocalVar[n_local_vars+i].properties =INS_PROPERTY_TYPE_SCRIPTVAR;
			break;
		}
	}

	CVirtualMachine::vecIdxLocalVar.push(n_total_vars);
	CVirtualMachine::idxStkCurrentLocalVar+=n_total_vars;

	// save current aux vars ...
	vecIdxStkNumber.push(startIdxStkNumber);
	vecIdxStkString.push(startIdxStkString);
	vecPtrCurrentStkResultInstruction.push(ptrStartStkResultInstruction);

	startIdxStkNumber=idxStkCurrentNumber;
	startIdxStkString=idxStkCurrentString;
	ptrStartStkResultInstruction=ptrStkCurrentResultInstruction+1;
}

void CVirtualMachine::popStack(){

	// unref 1st scope ...

	CVirtualMachine::idxStkCurrentLocalVar-=CVirtualMachine::vecIdxLocalVar.top();
	CVirtualMachine::vecIdxLocalVar.pop();

	if(!CVirtualMachine::vecIdxLocalVar.empty()){

		basePtrLocalVar=&stack[CVirtualMachine::idxStkCurrentLocalVar-CVirtualMachine::vecIdxLocalVar.top()];

		// restore last current instruction...
		ptrStkCurrentResultInstruction=ptrStartStkResultInstruction-1;

		// save current aux vars ...
		startIdxStkNumber = vecIdxStkNumber.top();
		startIdxStkString=vecIdxStkString.top();
		ptrStartStkResultInstruction=vecPtrCurrentStkResultInstruction.top();

		vecIdxStkNumber.pop();
		vecIdxStkString.pop();
		vecPtrCurrentStkResultInstruction.pop();
	}
}


// NUMBER result behaviour.
// this is the combination for number operations:
//
// op1 | op2 |  R
// ----+-----+----
//  i  |  i  |  i
//  i  |  f  |  i
//  f  |  i  |  f
//  f  |  f  |  f



bool CVirtualMachine::pushNumber(float init_value, CScriptVariable ** ptrAssignable, unsigned short properties){

	if(idxStkCurrentNumber ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max string operations");
		return false;
	}


	stkNumber[idxStkCurrentNumber]=init_value;
	*ptrStkCurrentResultInstruction={(unsigned short)(INS_PROPERTY_TYPE_STRING|properties),&stkNumber[idxStkCurrentNumber],ptrAssignable};
	idxStkCurrentNumber++;

	return true;
}

bool CVirtualMachine::pushString(const string & init_value, CScriptVariable ** ptrAssignable,unsigned short properties){
	if(idxStkCurrentString ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max string operations");
		return false;
	}


	stkString[idxStkCurrentString]=init_value;
	*ptrStkCurrentResultInstruction={(unsigned short)(INS_PROPERTY_TYPE_STRING|properties),&stkString[idxStkCurrentString],ptrAssignable};
	idxStkCurrentString++;

	return true;

}

bool CVirtualMachine::pushFunction(CScriptFunctionObject * init_value, CScriptVariable ** ptrAssignable,unsigned short properties){

	*ptrStkCurrentResultInstruction={(unsigned short)(INS_PROPERTY_TYPE_FUNCTION|properties),init_value,ptrAssignable};
	return true;
}


CScriptVariable * CVirtualMachine::createVarFromResultInstruction(tAleObjectInfo * ptr_instruction, bool share_ptr){
	CScriptVariable *obj = NULL;
bool created_pointer = false;



	// check second operand valid object..
	switch(GET_INS_PROPERTY_TYPE_VAR(ptr_instruction->properties)){
	default:
		print_error_cr("(internal) cannot determine var type");
		return NULL;
		break;
	case INS_PROPERTY_TYPE_UNDEFINED:
		obj=VM_UNDEFINED;
		break;
	case INS_PROPERTY_TYPE_INTEGER:
		obj= NEW_INTEGER_VAR;
		*((int *)(((CScriptVariable *)obj)->m_value)) = ((int)(ptr_instruction->stkResultObject));
		created_pointer = true;
		break;
	case INS_PROPERTY_TYPE_NUMBER:
		obj= NEW_NUMBER_VAR;
		*((float *)(((CScriptVariable *)obj)->m_value)) = *((float *)(ptr_instruction->stkResultObject));
		created_pointer = true;
		break;
	case INS_PROPERTY_TYPE_STRING:
		obj= NEW_STRING_VAR;
		*((string *)(((CScriptVariable *)obj)->m_value)) = *((string *)(ptr_instruction->stkResultObject));
		created_pointer = true;
		break;
	case INS_PROPERTY_TYPE_BOOLEAN:
		obj= NEW_BOOLEAN_VAR;
		*((bool *)(((CScriptVariable *)obj)->m_value)) = ((bool)(ptr_instruction->stkResultObject));
		created_pointer = true;
		break;
	case INS_PROPERTY_TYPE_FUNCTION:
		obj = NEW_FUNCTOR_VAR;//((CScriptFunctionObject *)ptr_instruction->stkResultObject));
		((CFunctor *)obj)->setFunctionSymbol((CScriptFunctionObject *)ptr_instruction->stkResultObject);
		created_pointer = true;
		break;
	case INS_PROPERTY_TYPE_SCRIPTVAR:
		obj = (CScriptVariable *)ptr_instruction->stkResultObject;
		break;

	}

	if(share_ptr){
		if(created_pointer && ((ptr_instruction->properties & INS_PROPERTY_IS_ARG)==0)){
			if(!obj->refSharedPtr()){
				return NULL;
			}
		}
	}else{
		if(!obj->unrefSharedPtr()){
			return NULL;
		}

	}
	return obj;
}





bool CVirtualMachine::loadFunctionValue(const tInfoAsmOp *iao,
		CScriptFunctionObject *local_function,
		CScriptVariable *this_object,
		tInfoAsmOp *asm_op){

	if(iao->index_op1 != LOAD_TYPE_FUNCTION){
		print_error_cr("expected load type function.");
		return false;
	}

	CScriptFunctionObject *info_function=NULL;
	vector<int> *vec_global_functions;

	CScriptVariable::tSymbolInfo *si;
	CScriptVariable ** calling_object = NULL;
	PASTNode iao_ast = AST_NODE(iao->idxAstNode);//>ast_node->symbol_value)
	unsigned int scope_type = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

	switch(scope_type){
	default: // global!

		vec_global_functions = &(GET_MAIN_FUNCTION_OBJECT->object_info.local_symbols.vec_idx_registeredFunction);

		if(iao->index_op2 == ZS_UNDEFINED_IDX){ // is will be processed after in CALL instruction ...
			info_function= NULL;
		}else{

			if((iao->index_op2<(int)vec_global_functions->size()))
			{
				info_function =GET_SCRIPT_FUNCTION_OBJECT((*vec_global_functions)[iao->index_op2]);
			}
			else{
				print_error_cr("cannot find symbol global \"%s\"",iao_ast->symbol_value.c_str());
				return false;

			}
		}

		break;
	case INS_PROPERTY_ACCESS_SCOPE:
	case INS_PROPERTY_THIS_SCOPE:
	case INS_PROPERTY_SUPER_SCOPE:

		// get var from object ...
		if(scope_type == INS_PROPERTY_ACCESS_SCOPE){
			calling_object = ((CScriptVariable **)ptrStartStkResultInstruction[iao->index_op2].ptrObjectRef);
			info_function = NULL; // TODO: always get symbol in CALL op. Make a way to do it optimized!

		}else if(scope_type == INS_PROPERTY_THIS_SCOPE){
			if((si = this_object->getFunctionSymbolByIndex(iao->index_op2))==NULL){
				print_error_cr("cannot find function \"this.%s\"",iao_ast->symbol_value.c_str());
				return false;
			}
		}else{ // super scope ?
			if((si = this_object->getFunctionSymbolByIndex(iao->index_op2))==NULL){
				print_error_cr("cannot find function \"super.%s\"",iao_ast->symbol_value.c_str());
				return false;
			}
		}

		if(scope_type != INS_PROPERTY_ACCESS_SCOPE){
			info_function =(CScriptFunctionObject *)si->object;
		}

		break;

	case INS_PROPERTY_LOCAL_SCOPE:
		info_function = GET_SCRIPT_FUNCTION_OBJECT(local_function->object_info.local_symbols.vec_idx_registeredFunction[iao->index_op2]);
		break;
	}

	// generic object pushed ...
	if(!pushFunction(info_function,calling_object != NULL? calling_object:NULL)) {
		return false;
	}
	//stkResultInstruction[idxStkCurrentResultInstruction]={CScriptVariable::FUNCTION,(CScriptVariable **)si, false};
	return true;
}




void CVirtualMachine::popScope(CScriptFunctionObject *info_function,int index)//, CScriptVariable *ret)
{
	if(index < 0){
		print_error_cr("index < 0");
		return;
	}

	if(index >= (int)info_function->object_info.n_info_var_scope){
		print_error_cr("index >= info_function->object_info.info_var_scope.size()");
		return;
	}

	for(unsigned i = 0; i < info_function->object_info.info_var_scope[index].n_var_index; i++){
		int idx_local_var = info_function->object_info.info_var_scope[index].var_index[i];

		CScriptVariable *var =((CScriptVariable *)(basePtrLocalVar[idx_local_var].stkResultObject));

		if(var != VM_NULL && var !=  VM_UNDEFINED){

			//if(ret != var){ // is not ret variable ...
			if(var->idx_shared_ptr != ZS_UNDEFINED_IDX){
				unrefSharedPointer(var->idx_shared_ptr);
			}
		}
		basePtrLocalVar[idx_local_var].stkResultObject = VM_UNDEFINED;
	}
}


CVirtualMachine::~CVirtualMachine(){

}
