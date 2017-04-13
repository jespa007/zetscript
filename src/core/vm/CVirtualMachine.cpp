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
(IS_GENERIC_NUMBER(ptrResultInstructionOp1->properties) && IS_GENERIC_NUMBER(ptrResultInstructionOp2->properties))

#define OP1_IS_STRING_AND_OP2_IS_NUMBER \
(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_STRING) && \
IS_GENERIC_NUMBER(ptrResultInstructionOp2->properties)

#define OP1_IS_NUMBER_AND_OP2_IS_STRING \
(ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_STRING) && \
IS_GENERIC_NUMBER(ptrResultInstructionOp1->properties)

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




#define PROCESS_ARITHMETIC_OPERATION(__OVERR_OP__)\
{\
	unsigned short properties = GET_INS_PROPERTY_TYPE_VAR(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
	if(properties==INS_PROPERTY_TYPE_INTEGER){\
			PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);\
	}\
	else if(properties==(INS_PROPERTY_TYPE_INTEGER|INS_PROPERTY_TYPE_NUMBER)){\
			if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){\
				pushNumber(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);\
			}else{\
				pushNumber(LOAD_NUMBER_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
			}\
	}\
	else if(properties== INS_PROPERTY_TYPE_NUMBER){\
			pushNumber(LOAD_NUMBER_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_NUMBER_OP(ptrResultInstructionOp2));\
	}\
	else{\
		PRINT_DUAL_ERROR_OP(#__OVERR_OP__);\
	}\
}


#define PUSH_INTEGER(init_value,ptrAssignable,properties)\
*ptrStkCurrentResultInstruction++={INS_PROPERTY_TYPE_INTEGER|properties,(void*)((int)(init_value)),ptrAssignable};

#define PUSH_BOOLEAN(init_value, ptrAssignable, properties)\
*ptrStkCurrentResultInstruction++={INS_PROPERTY_TYPE_BOOLEAN|properties,(void *)((bool)(init_value)),ptrAssignable};

#define PUSH_VAR(init_value, ptrObjectRefParam,propertiesParam,is_new_var)\
{\
	bool push_var = is_new_var;\
	if(!push_var){\
		int idx_class=(init_value)->idxScriptClass;\
		if(idx_class==IDX_CLASS_INTEGER){\
				PUSH_INTEGER(*((int *)((CInteger *)init_value)->m_value),ptrObjectRefParam,propertiesParam);\
		}\
		else if(idx_class==IDX_CLASS_NUMBER){\
			pushNumber(*((float *)((CNumber *)(init_value))->m_value),ptrObjectRefParam,propertiesParam);\
		}\
		else if(idx_class==IDX_CLASS_STRING){\
			pushString(*((string *)((CString *)(init_value))->m_value),ptrObjectRefParam,propertiesParam);\
		}\
		else if(idx_class==IDX_CLASS_BOOLEAN){\
			PUSH_BOOLEAN(*((bool *)((CBoolean *)(init_value))->m_value),ptrObjectRefParam,propertiesParam);\
		}\
		else if(idx_class==IDX_CLASS_FUNCTOR){\
			pushFunction((CScriptFunctionObject *)(((CFunctor *)(init_value))->m_value),ptrObjectRefParam,propertiesParam);\
		}\
		else{\
			push_var=true;\
		}\
	}\
	if(push_var){\
		*ptrStkCurrentResultInstruction++={INS_PROPERTY_TYPE_SCRIPTVAR|propertiesParam,init_value,ptrObjectRefParam};\
	}\
}

#define PERFORM_PRE_POST_OPERATOR(__OPERATOR__) \
		switch((var_object)->idxScriptClass){\
			case IDX_CLASS_INTEGER:\
				(*((int *)((CInteger *)(var_object))->m_value))__OPERATOR__;\
				break;\
			case IDX_CLASS_NUMBER:\
				(*((float *)((CNumber *)(var_object))->m_value))__OPERATOR__;\
				break;\
			default:\
				print_error_cr("internal error:Cannot perform pre/post operator ?? because is not number");\
				return NULL;\
				break;\
		}




//CVirtualMachine::createVarFromResultInstruction(tAleObjectInfo * ptr_instruction, bool share_ptr, CScriptVariable * ret)

#define CREATE_VAR_FROM_RESULT_INSTRUCTION(ptr_instruction,share_ptr,ret) \
{\
	bool created_pointer = false;\
	unsigned short type_var=GET_INS_PROPERTY_TYPE_VAR(ptr_instruction->properties);\
	if(type_var==INS_PROPERTY_TYPE_UNDEFINED){\
		(ret)=VM_UNDEFINED;\
	}\
	else if(type_var==INS_PROPERTY_TYPE_INTEGER){\
		(ret)= NEW_INTEGER_VAR;\
		*((int *)(((CScriptVariable *)(ret))->m_value)) = ((int)(ptr_instruction->stkResultObject));\
		created_pointer = true;\
	}\
	else if(type_var==INS_PROPERTY_TYPE_NUMBER){\
		(ret)= NEW_NUMBER_VAR;\
		*((float *)(((CScriptVariable *)(ret))->m_value)) = *((float *)(ptr_instruction->stkResultObject));\
		created_pointer = true;\
	}\
	else if(type_var==INS_PROPERTY_TYPE_STRING){\
		(ret)= NEW_STRING_VAR;\
		*((string *)(((CScriptVariable *)(ret))->m_value)) = *((string *)(ptr_instruction->stkResultObject));\
		created_pointer = true;\
	}\
	else if(type_var==INS_PROPERTY_TYPE_BOOLEAN){\
		(ret)= NEW_BOOLEAN_VAR;\
		*((bool *)(((CScriptVariable *)(ret))->m_value)) = ((bool)(ptr_instruction->stkResultObject));\
		created_pointer = true;\
	}\
	else if(type_var==INS_PROPERTY_TYPE_FUNCTION){\
		(ret) = NEW_FUNCTOR_VAR;\
		((CFunctor *)(ret))->setFunctionSymbol((CScriptFunctionObject *)ptr_instruction->stkResultObject);\
		created_pointer = true;\
	}\
	else if(type_var==INS_PROPERTY_TYPE_SCRIPTVAR){\
		(ret) = (CScriptVariable *)ptr_instruction->stkResultObject;\
	}\
	else{\
		print_error_cr("(internal) cannot determine var type");\
		return NULL;\
	}\
	if(share_ptr){\
		if(created_pointer && ((ptr_instruction->properties & INS_PROPERTY_IS_ARG)==0)){\
			if(!(ret)->refSharedPtr()){\
				return NULL;\
			}\
		}\
	}else{\
		if(!(ret)->unrefSharedPtr()){\
			return NULL;\
		}\
	}\
}


/*

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


bool CVirtualMachine::LOAD_VARIABLE(tInfoAsmOp *iao,
		CScriptFunctionObject *local_function,
		CScriptVariable *this_object,
		tInfoAsmOp *asm_op)*/






#define CALL_GC\
	if(n_pointers_with_0_shares[idxCurrentStack]>0){\
		short *index = pointers_with_0_shares[idxCurrentStack];\
		do{\
			unrefSharedPointer(*(index++));\
		}while(--n_pointers_with_0_shares[idxCurrentStack]);\
	}




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

	idxStkCurrentNumber=startIdxStkNumber;
	idxStkCurrentString=startIdxStkString;
	ptrStkCurrentResultInstruction=ptrStartStkResultInstruction;
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


#define POP_TWO \
ptrResultInstructionOp2=--ptrStkCurrentResultInstruction;\
ptrResultInstructionOp1=--ptrStkCurrentResultInstruction;


#define POP_ONE \
ptrResultInstructionOp1=--ptrStkCurrentResultInstruction;

#define CHK_JMP \
if(instruction->index_op2 != ZS_UNDEFINED_IDX){\
\
	if(current_statment!=&m_listStatements[instruction->index_op2]){\
		ptr_statment_iteration=&m_listStatements[instruction->index_op2];\
		goto lbl_exit_statment;\
	}\
\
	if(instruction->index_op1 != ZS_UNDEFINED_IDX){\
		instruction=&(*current_statment)[instruction->index_op1];\
	}\
\
}


//#define LDR_OP2 ptrResultInstructionOp2=&ptrStartStkResultInstruction[instruction->index_op2];ptrResultInstructionOp2->properties=ptrResultInstructionOp2->properties;


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


	if(ast->keyword_info == KEYWORD_TYPE::FUNCTION_KEYWORD){
		scope_index=AST_NODE(ast->children[1])->idxScope;
	}


	CScriptVariable *ret=VM_UNDEFINED;
	PtrStatment m_listStatements = info_function->object_info.statment_op;
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
	//tAleObjectInfo *stkResultInstructionIteration = ptrStartStkResultInstruction;
	tAleObjectInfo *ptrResultInstructionOp1=NULL;//&stkResultInstruction[index_op1+startIdxStkResultInstruction];
	tAleObjectInfo *ptrResultInstructionOp2=NULL;//&stkResultInstruction[index_op2+startIdxStkResultInstruction];
	//unsigned short ptrResultInstructionOp1->properties,ptrResultInstructionOp2->properties;
	//tAleObjectInfo *ptrResultLastInstruction=NULL;//&stkResultInstruction[idxStkCurrentResultInstruction-1];
	//unsigned int prop1, prop2;
	unsigned short pre_post_properties=0;
	unsigned short scope_properties=0;
	unsigned short instruction_properties=0;
	CScriptVariable::tSymbolInfo *si;
	CScriptVariable **ptr_var_object=NULL;
	CScriptVariable *var_object = NULL;
	//int start_index_local_var = 0;

	short int scope_type=0;//GET_INS_PROPERTY_SCOPE_TYPE(instruction->instruction_properties);
	short int pre_post_operator_type=0;//GET_INS_PROPERTY_PRE_POST_OP(instruction->instruction_properties);
	CCompiler::tInfoConstantValue * info_constant=NULL;
	//int index_op1 = -1;//
	//int index_op2 = -1;
	//tAleObjectInfo *ptrSavedInstruction=NULL;
	PtrStatment current_statment = NULL,
				ptr_statment_iteration=m_listStatements;

	tInfoAsmOp *instruction_it;

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
		ptrStkCurrentResultInstruction=ptrStartStkResultInstruction; // reset stack to start...


		//-----------------------------------------------------------------------------------------------------------------------
		//
		// START OPERATOR MANAGEMENT
		//

		 for(;;){ // foreach asm instruction ...

			const tInfoAsmOp * instruction = instruction_it++;
			const unsigned char operator_type=instruction->operator_type;
			const unsigned char index_op1=instruction->index_op1;


			if(operator_type==END_STATMENT){
				goto lbl_exit_statment;
			}
			else if(operator_type==NOP){ // ignore ...
				continue;
			}
			else if(operator_type==LOAD){// load value in function of value/constant ...
				if(index_op1== LOAD_TYPE::LOAD_TYPE_NULL){
					PUSH_VAR(VM_NULL,NULL,0,false);
					continue;
				}
				else if(index_op1==  LOAD_TYPE::LOAD_TYPE_UNDEFINED){
					PUSH_VAR(VM_UNDEFINED,NULL,0,false);
					continue;
				}
				else if(index_op1== LOAD_TYPE::LOAD_TYPE_CONSTANT){
					(*ptrStkCurrentResultInstruction++)=*(((CCompiler::tInfoConstantValue *)instruction->index_op2));
					continue;
				}
				else if(index_op1== LOAD_TYPE::LOAD_TYPE_VARIABLE){

					instruction_properties=instruction->instruction_properties;
					scope_properties=GET_INS_PROPERTY_SCOPE_TYPE(instruction_properties);

					if((scope_properties==INS_PROPERTY_ACCESS_SCOPE) || (scope_properties==INS_PROPERTY_THIS_SCOPE)){
						if(instruction->idxAstNode != -1)
							ast = AST_NODE(instruction->idxAstNode);
						if(scope_type == INS_PROPERTY_ACCESS_SCOPE){
							CScriptVariable * base_var = *((CScriptVariable **)ptrStartStkResultInstruction[instruction->index_op2].ptrObjectRef);
							if((si = base_var->getVariableSymbol(ast->symbol_value))==NULL){
								print_error_cr("Line %i: Variable %s as type %s has not symbol %s",ast->line_value,AST_SYMBOL_VALUE_CONST_CHAR((*current_statment)[instruction->index_op2].idxAstNode),base_var->getClassName().c_str(), ast->symbol_value.c_str());
								return NULL;
							}
						}
						else{
							if((si = this_object->getVariableSymbolByIndex(instruction->index_op2))==NULL){
								print_error_cr("cannot find symbol this.%s",ast->symbol_value.c_str());
								return NULL;
							}
						}
						ptr_var_object = (CScriptVariable **)(&si->object);
						var_object = (CScriptVariable *)(si->object);
					}
					else if(scope_properties==INS_PROPERTY_LOCAL_SCOPE){
						if(instruction->index_op2 >= idxStkCurrentLocalVar){
							print_error_cr("internal error: index out of stack");
							return NULL;
						}
						ptr_var_object = (CScriptVariable **)(&CVirtualMachine::basePtrLocalVar[instruction->index_op2].stkResultObject);
						var_object = *ptr_var_object;//(CScriptVariable *)(CVirtualMachine::basePtrLocalVar[start_index_local_var+instruction->index_op2].stkResultObject);

					}
					else{
						ptr_var_object = (CScriptVariable **)(&CVirtualMachine::stack[instruction->index_op2].stkResultObject);
						var_object = (CScriptVariable *)(CVirtualMachine::stack[instruction->index_op2].stkResultObject);
					}




					pre_post_properties = GET_INS_PROPERTY_PRE_POST_OP(instruction_properties);


					if(pre_post_properties == INS_PROPERTY_PRE_DEC){
							PERFORM_PRE_POST_OPERATOR(--);
							PUSH_VAR(var_object,ptr_var_object,0,false);
					}else if(pre_post_properties ==INS_PROPERTY_PRE_INC){
							PERFORM_PRE_POST_OPERATOR(++);
							PUSH_VAR(var_object,ptr_var_object,0,false);
					}else if (pre_post_properties ==INS_PROPERTY_POST_DEC){
							PUSH_VAR(var_object,ptr_var_object,0,false);
							PERFORM_PRE_POST_OPERATOR(--);
					}else if(pre_post_properties ==INS_PROPERTY_POST_INC){
							PUSH_VAR(var_object,ptr_var_object,0,false);
							PERFORM_PRE_POST_OPERATOR(++);
					}else if(pre_post_properties ==INS_PROPERTY_PRE_NEG){
							switch((var_object)->idxScriptClass){
							case IDX_CLASS_INTEGER:
								PUSH_INTEGER(-(*((int *)((CInteger *)var_object)->m_value)),NULL,0);
								break;
							case IDX_CLASS_NUMBER:
								pushNumber(-(*((float *)((CNumber *)var_object)->m_value)));
								break;
							default:
								print_error_cr("internal error:cannot perform pre operator - because is not number");
								return NULL;
							}
					}else{
							PUSH_VAR(var_object,ptr_var_object,0,false);
					}


					continue;
				}
				else if(index_op1== LOAD_TYPE::LOAD_TYPE_FUNCTION){
					if(!loadFunctionValue(instruction,info_function, this_object,*current_statment)){
						return NULL;
					}
					continue;
				}
				else if(index_op1== LOAD_TYPE::LOAD_TYPE_ARGUMENT){
					PUSH_VAR(
							(CScriptVariable *)basePtrLocalVar[instruction->index_op2+n_local_vars].stkResultObject,
							basePtrLocalVar[instruction->index_op2+n_local_vars].ptrObjectRef,
							INS_PROPERTY_IS_ARG,
							false);
					continue;
				}
				else{
					print_error_cr("no load defined type");
					return NULL;
					break;
				}
			}
			else if(operator_type==MOV){ // mov value expression to var
				POP_TWO;

				// ok load object pointer ...
				if((obj = ptrResultInstructionOp1->ptrObjectRef) != NULL) {// == CScriptVariable::VAR_TYPE::OBJECT){
					// get pointer object (can be assigned)
					//ASSIGN_FROM_RESULT_INSTRUCTION(ptrResultInstructionOp1->ptrObjectRef,ptrResultInstructionOp2);
					//#define ASSIGN_FROM_RESULT_INSTRUCTION(var, ptr_instruction)

					CScriptVariable **var=ptrResultInstructionOp1->ptrObjectRef;
					//tAleObjectInfo *ptr_instruction = ptrResultInstructionOp2;
					CScriptVariable *aux_var=NULL;
					if(*var == NULL){

						CREATE_VAR_FROM_RESULT_INSTRUCTION(ptrResultInstructionOp2,true, *var);

						/*if((*var = createVarFromResultInstruction(ptrResultInstructionOp2)) == NULL){
							return NULL;
						}*/
					}
					int idxScriptClass = (*var)->idxScriptClass;
					bool create_from_index=false;
					unsigned int type_var = GET_INS_PROPERTY_TYPE_VAR(ptrResultInstructionOp2->properties);

					if(type_var==INS_PROPERTY_TYPE_NULL){
							*var = VM_NULL;
					}
					else if(type_var==INS_PROPERTY_TYPE_UNDEFINED){
							*var = VM_UNDEFINED;
					}
					else if(type_var==INS_PROPERTY_TYPE_INTEGER){
							if(idxScriptClass == IDX_CLASS_INTEGER){
								*((int *)((CInteger *)(*var))->m_value)=((int)(ptrResultInstructionOp2->stkResultObject));
							}else
							{
								create_from_index=true;
							}
					}
					else if(type_var==INS_PROPERTY_TYPE_NUMBER){
							if(idxScriptClass == IDX_CLASS_NUMBER){
								*((float *)((CNumber *)(*var))->m_value) = *((float *)(ptrResultInstructionOp2->stkResultObject));
							}else
							{
								create_from_index=true;
							}
					}
					else if(type_var==INS_PROPERTY_TYPE_STRING){
							if(idxScriptClass == IDX_CLASS_STRING){
								*((string *)((CString  *)(*var))->m_value)= *((string *)(ptrResultInstructionOp2->stkResultObject));
							}else
							{
								create_from_index=true;
							}
					}
					else if(type_var==INS_PROPERTY_TYPE_BOOLEAN){
							if(idxScriptClass == IDX_CLASS_BOOLEAN){
								*((bool *)((CBoolean  *)aux_var)->m_value)= ((bool)(ptrResultInstructionOp2->stkResultObject));
							}else
							{
								create_from_index=true;
							}
					}
					else if(type_var==INS_PROPERTY_TYPE_FUNCTION){
							if(idxScriptClass == IDX_CLASS_FUNCTOR){
								((CFunctor  *)aux_var)->m_value= ((CScriptFunctionObject *)(ptrResultInstructionOp2->stkResultObject));
							}else{
								create_from_index=true;
							}
					}
					else if(type_var==INS_PROPERTY_TYPE_SCRIPTVAR){
							if((*var)->idx_shared_ptr != ZS_UNDEFINED_IDX){
								unrefSharedPointer((*var)->idx_shared_ptr);
							}
							*var = (CScriptVariable *)(ptrResultInstructionOp2->stkResultObject);
							if(*var != VM_NULL && *var != VM_UNDEFINED){
								sharePointer((*var)->idx_shared_ptr);
							}
					}
					else{
						print_error_cr("internal error: unknow assignment %i!",type_var);
						return NULL;
					}

					if(create_from_index){

						if((*var)->idx_shared_ptr != ZS_UNDEFINED_IDX){
							unrefSharedPointer((*var)->idx_shared_ptr);
						}


						CREATE_VAR_FROM_RESULT_INSTRUCTION(ptrResultInstructionOp2,true,*var);
						/*if((*var = createVarFromResultInstruction(ptrResultInstructionOp2)) == NULL){
							return NULL;
						}*/
						sharePointer((*var)->idx_shared_ptr);
					}



				}else{
					print_error_cr("Expected object l-value mov");
					return NULL;
				}

				continue;
			}

		/*	case EQU:  // == --> boolean && boolean or string && string or number && number

				POP_TWO;

				if(OP1_AND_OP2_ARE_BOOLEANS) {
					PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) == LOAD_BOOL_OP(ptrResultInstructionOp2),NULL,0);
				}else if(OP1_AND_OP2_ARE_STRINGS){
					PUSH_BOOLEAN(LOAD_STRING_OP(ptrResultInstructionOp1) == LOAD_STRING_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) == LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) == LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) == LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
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

				POP_TWO;

				if(OP1_AND_OP2_ARE_BOOLEANS) {
					PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) != LOAD_BOOL_OP(ptrResultInstructionOp2),NULL,0);
				}else if(OP1_AND_OP2_ARE_STRINGS){
					PUSH_BOOLEAN(LOAD_STRING_OP(ptrResultInstructionOp1) != LOAD_STRING_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) != LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) != LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) != LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) != LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("!=");
					return NULL;
				}

				break;*/
			else if(operator_type==LT){  // <

				POP_TWO;


				if (IS_INT(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) < LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) < LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) < LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) < LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("<");
					return NULL;
				}




				continue;
			}
			/*case LTE:  // <=

				POP_TWO;

				if (IS_INT(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) <= LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) <= LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) <= LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) <= LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("<=");
					return NULL;
				}

				break;
			case NOT: // !
				POP_ONE;
				if (ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_BOOLEAN){
					PUSH_BOOLEAN(!LOAD_BOOL_OP(ptrResultInstructionOp1),NULL,0);
				}else{
					print_error_cr("Expected operands 1 as boolean!");
					return NULL;
				}
				break;
			case NEG: // !
				POP_ONE;
				if (IS_GENERIC_NUMBER(ptrResultInstructionOp1->properties)){
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
				POP_TWO;
				if (IS_INT(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) > LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) > LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) > LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) > LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP(">");
					return NULL;
				}
				break;
			case GTE: // >=
				POP_TWO;
				if (IS_INT(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) >= LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) >= LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) >= LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) >= LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP(">=");
					return NULL;
				}
				break;

			case ADD: // +

				POP_TWO;
				// get indexes and check whether is possible or not its calculation.
				// check indexes
				if (IS_STRING(ptrResultInstructionOp1->properties) && (IS_UNDEFINED(ptrResultInstructionOp2->properties) || IS_VAR(ptrResultInstructionOp2->properties))){
					string result = "undefined";
					if(IS_VAR(ptrResultInstructionOp2->properties)){
						result = ((CScriptVariable *)(ptrResultInstructionOp2->stkResultObject))->getClassName();
					}
					if(!pushString(LOAD_STRING_OP(ptrResultInstructionOp1)+result)) return NULL;
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) + LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) + LOAD_NUMBER_OP(ptrResultInstructionOp2),NULL,0);
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					if(!pushNumber(LOAD_NUMBER_OP(ptrResultInstructionOp1) + LOAD_INT_OP(ptrResultInstructionOp2))) return NULL;
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
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
				POP_TWO;
				if(OP1_AND_OP2_ARE_BOOLEANS) {
					PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) && LOAD_BOOL_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("&&");
					return NULL;
				}
				break;
			case LOGIC_OR:  // ||
				POP_TWO;
				if(OP1_AND_OP2_ARE_BOOLEANS) {
					PUSH_BOOLEAN(LOAD_BOOL_OP(ptrResultInstructionOp1) || LOAD_BOOL_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("||");
					return NULL;
				}
				break;
			case DIV: // /
				POP_TWO;
				if(OP1_AND_OP2_ARE_NUMBERS) {

					if(IS_INT(ptrResultInstructionOp2->properties)){
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

					PROCESS_ARITHMETIC_OPERATION(/);
				}else{
					PRINT_DUAL_ERROR_OP("/");
					return NULL;
				}

				break;*/
			else if(operator_type==MUL){ // *
				POP_TWO;


				//if(OP1_AND_OP2_ARE_NUMBERS) {
				PROCESS_ARITHMETIC_OPERATION(*);

				/*}else{
					PRINT_DUAL_ERROR_OP("*");
					return NULL;
				}*/
				continue;
			}
			/*case MOD:  // %

				POP_TWO;

				if(OP1_AND_OP2_ARE_NUMBERS) {

					if(IS_INT(ptrResultInstructionOp2->properties)){
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
					if (IS_INT(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
						PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) % LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
					}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
						PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) % ((int) LOAD_NUMBER_OP(ptrResultInstructionOp2)),NULL,0);

					}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
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
				POP_TWO;
				if((ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_INTEGER) && (ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_INTEGER)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) & LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("&");
					return NULL;
				}
				break;
			case OR: // bitwise logic or
				POP_TWO;
				if((ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_INTEGER) && (ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_INTEGER)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) | LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("|");
					return NULL;
				}

				break;
			case XOR: // logic xor
				POP_TWO;
				if((ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_INTEGER) && (ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_INTEGER)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) ^ LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("^");
					return NULL;
				}
				break;
			case SHL: // shift left
				POP_TWO;
				if((ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_INTEGER) && (ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_INTEGER)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) << LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP(">>");
					return NULL;
				}
				break;
			case SHR: // shift right
				POP_TWO;
				if((ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_INTEGER) && (ptrResultInstructionOp2->properties & INS_PROPERTY_TYPE_INTEGER)){
					PUSH_INTEGER(LOAD_INT_OP(ptrResultInstructionOp1) >> LOAD_INT_OP(ptrResultInstructionOp2),NULL,0);
				}else{
					PRINT_DUAL_ERROR_OP("<<");
					return NULL;
				}
				break;*/
			// special internal ops...
			else if(operator_type==JMP){
				CHK_JMP;
				continue;
			}
			else if(operator_type==JNT){ // goto if not true ... goes end to conditional.
				POP_ONE;
				if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_BOOLEAN){

					if(!((bool)(ptrResultInstructionOp1->stkResultObject))){
						CHK_JMP;
					}
				}else{
					print_error_cr("Expected boolean element");
					return NULL;
				}

				continue;
		 	 }
			else if(operator_type== JT){ // goto if true ... goes end to conditional.
				POP_ONE;
				if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_BOOLEAN){

					if(((bool)(ptrResultInstructionOp1->stkResultObject))){
						CHK_JMP;
					}
				}else{
					print_error_cr("Expected boolean element");
					return NULL;
				}

				continue;
			}
			/*case CALL: // calling function after all of args are processed...
				// check whether signatures matches or not ...
				// 1. get function object ...
				POP_TWO;
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
				if((instruction_properties & INS_PROPERTY_CALLING_OBJECT) != 0){
					calling_object= ptrResultInstructionOp1->ptrObjectRef!=NULL?*ptrResultInstructionOp1->ptrObjectRef:NULL;
				}
				else if((aux_function_info->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){

				}

				//push();

				// by default virtual machine gets main object class in order to run functions ...
				if((ret_obj=execute(aux_function_info,calling_object,&m_functionArgs))==NULL){
					return NULL;
				}

				//pop();

				// if function is C must register pointer !

				//if((aux_function_info->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // C-Call
				if((instruction_properties & INS_PROPERTY_DIRECT_CALL_RETURN) == 0){ // share pointer
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
				POP_ONE;
				if((svar = createVarFromResultInstruction(ptrResultInstructionOp1)) == NULL){
					return NULL;
				}

				m_functionArgs.push_back(svar);
				break;
			case CLR: // clear args
				m_functionArgs.clear();
				break;
			case VGET: // vector access after each index is processed...
				POP_TWO;
				// index_op1 is vector, index op2 is index...
				if(IS_VECTOR(ptrResultInstructionOp1)){
					if(IS_INT(ptrResultInstructionOp2->properties)){
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
				POP_TWO;
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
				if((ret=createVarFromResultInstruction(&ptrStartStkResultInstruction[instruction->index_op1], false)) == NULL){ // share pointer but not add as shared!
					return NULL;
				}
				//end_by_ret=true;
				goto lbl_exit_function;

			case NEW:
				svar=NEW_CLASS_VAR_BY_IDX(instruction->index_op1);
				PUSH_VAR(svar, NULL,0, true);

				if(!svar->refSharedPtr()){
					return NULL;
				}

				// execute its constructor ...
				//push();

				if((constructor_function = svar->getConstructorFunction()) != NULL){
					execute(constructor_function,svar);
				}

				//pop();

				break;*/

			//case SAVE_I: // get Last For ternary condition ...
			//	ptrSavedInstruction=ptrStkCurrentResultInstruction-1;
			//	break;
			//case LOAD_I: // For ternary condition ...
/*
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
				break;*/
			else if(operator_type==POP_SCOPE){
				popScope(info_function,instruction->index_op1);
			}

			/*case DECL_STRUCT: // symply creates a variable ...
				svar=NEW_STRUCT;
				PUSH_VAR(svar,NULL,0,false);
				if(!svar->refSharedPtr())
					return NULL;
				break;
			case PUSH_ATTR:
				POP_TWO;
				// get symbol ... (current instruction -1)
				if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_STRING){

					CScriptVariable::tSymbolInfo *si;
					string *variable_name = (string *)ptrResultInstructionOp1->stkResultObject;

					print_error_cr("PUSH ATTRIB NOT IMPLEMENTED +++");
					return NULL;

				}
				else{
					print_error_cr("Struct: Expected symbol at line %i",AST_LINE_VALUE(instruction->idxAstNode));
					return NULL;
				}
				break;*/

		 	 else{
				print_error_cr("operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
				return NULL;
		 	 }



			//
			// END OPERATOR MANAGEMENT
			//
			//-----------------------------------------------------------------------------------------------------------------------




		}

// exit statment (don't remove ;, it gives a compile error)
lbl_exit_statment:;

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

	if(idxCurrentStack < MAX_STACK){
		idxCurrentStack++;
	}
	else{
		print_error_cr("Reached max stack");
		exit(EXIT_FAILURE);
	}

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

	if(idxCurrentStack > 0){
		idxCurrentStack--;
	}
	else{
		print_error_cr("Reached min stack");
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
	*ptrStkCurrentResultInstruction++={(unsigned short)(INS_PROPERTY_TYPE_STRING|properties),&stkNumber[idxStkCurrentNumber],ptrAssignable};
	idxStkCurrentNumber++;

	return true;
}

bool CVirtualMachine::pushString(const string & init_value, CScriptVariable ** ptrAssignable,unsigned short properties){
	if(idxStkCurrentString ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max string operations");
		return false;
	}


	stkString[idxStkCurrentString]=init_value;
	*ptrStkCurrentResultInstruction++={(unsigned short)(INS_PROPERTY_TYPE_STRING|properties),&stkString[idxStkCurrentString],ptrAssignable};
	idxStkCurrentString++;

	return true;

}

bool CVirtualMachine::pushFunction(CScriptFunctionObject * init_value, CScriptVariable ** ptrAssignable,unsigned short properties){

	*ptrStkCurrentResultInstruction++={(unsigned short)(INS_PROPERTY_TYPE_FUNCTION|properties),init_value,ptrAssignable};
	return true;
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

	CALL_GC;
}


CVirtualMachine::~CVirtualMachine(){

}
