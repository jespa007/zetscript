#include "../../CZetScript.h"

//CVirtualMachine * CVirtualMachine::m_virtualMachine = NULL;
//vector<CVirtualMachine::CVirtualMachine> CVirtualMachine::ALE;
// static: only defined in this module...

#define COPY_NUMBER(d,s)  memcpy((d),(s),sizeof(float))


// general
#define PRINT_DUAL_ERROR_OP(c)\
string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp1),\
	   var_type2=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp2);\
\
print_error_cr("Error at line %i cannot perform operator \"%s\" %s  \"%s\"",\
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
*ptrCurrentOp++={INS_PROPERTY_TYPE_BOOLEAN,(void *)((bool)(init_value)),NULL};

#define PUSH_INTEGER(init_value) \
*ptrCurrentOp++={INS_PROPERTY_TYPE_INTEGER,(void *)((int)(init_value)),NULL};

#define PUSH_NUMBER(init_value) \
{\
	float aux=init_value;\
	COPY_NUMBER(&ptrCurrentOp->stkValue,&aux);\
	ptrCurrentOp->properties=INS_PROPERTY_TYPE_NUMBER;\
	ptrCurrentOp++;\
}

#define PUSH_STRING(init_value)\
	if(ptrCurrentStr==ptrLastStr){print_error_cr("Error stkString out-stack");return NULL;}\
	*ptrCurrentStr=init_value;\
	*ptrCurrentOp++={INS_PROPERTY_TYPE_STRING,ptrCurrentStr++,NULL};\



#define PUSH_FUNCTION(function_prop,fun_obj,class_obj) \
*ptrCurrentOp++={(unsigned short)(INS_PROPERTY_TYPE_FUNCTION|function_prop),(void *)(fun_obj),class_obj};

#define PUSH_SCRIPTVAR(var_ref) \
*ptrCurrentOp++={INS_PROPERTY_SCRIPTVAR,NULL,var_ref};



// Push stack var value (as varValue and put ptr as ref second value)...
#define PUSH_STACK_VAR(stack_ref_var) \
	*ptrCurrentOp++={(unsigned short)(((stack_ref_var)->properties)|INS_PROPERTY_IS_STACKVAR),(stack_ref_var)->stkValue,stack_ref_var};




#define PROCESS_ARITHMETIC_OPERATION(__OVERR_OP__)\
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
		PRINT_DUAL_ERROR_OP(#__OVERR_OP__);\
	}\
}

#define PROCESS_LOGIC_OPERATION(__OVERR_OP__)\
{\
	unsigned short properties = GET_INS_PROPERTY_VAR_TYPE(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
	if(properties==INS_PROPERTY_TYPE_INTEGER){\
			PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2));\
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
		print_error_cr("internal error:Cannot perform pre/post operator ?? because is not number");\
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
			print_info_cr("Primitive type not equal! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals");\
			return NULL;\
		}else{\
			if(\
				(GET_INS_PROPERTY_VAR_TYPE(src_ins->properties) == INS_PROPERTY_TYPE_SCRIPTVAR)\
			){\
					print_info_cr("Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator in order to make the proper operation.");\
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
		*((bool *)dst_ref)=*((bool *)src_ref);\
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
		print_error_cr("(internal) cannot determine var type %i",GET_INS_PROPERTY_VAR_TYPE(src_ins->properties));\
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
		print_error_cr("index < 0");\
		return NULL;\
	}\
\
	if(index >= (int)info_function->object_info.n_info_var_scope){\
		print_error_cr("index >= info_function->object_info.info_var_scope.size()");\
		return NULL;\
	}\
\
	for(unsigned i = 0; i < info_function->object_info.info_var_scope[index].n_var_index; i++){\
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






#define CALL_GC SHARED_LIST_DESTROY(zero_shares[idxCurrentStack])



/*
if(n_pointers_with_0_shares[idxCurrentStack]>0){\
		short *index = pointers_with_0_shares[idxCurrentStack];\
		do{\
			unrefSharedScriptVar(*(index++));\
		}while(--n_pointers_with_0_shares[idxCurrentStack]);\
	}
*/



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
		result=((CScriptVariable *)ptr_info_ale->stkValue)->getClassName();
	}

	return result;
}

CVirtualMachine::CVirtualMachine(){

	//-----------------------------------------------------------
	// set memory manager
/*	for(int s = 0; s < MAX_FUNCTION_CALL; s++){
		n_pointers_with_0_shares[s]=0;
		memset(shared_pointer[s],0,sizeof(shared_pointer[s]));
		n_freeCell[s]=MAX_UNIQUE_OBJECTS_POINTERS-1;
		for(int i = 0; i < MAX_UNIQUE_OBJECTS_POINTERS; i++){
			indexFreeCell[s][i]=i;
		}
	}
*/

	memset(zero_shares,0,sizeof(zero_shares));
	memset(shared_var,0,sizeof(shared_var));


	idxCurrentStack=0;

	//------------------

	//ptrBaseOp=NULL;
	//ptrArg=NULL;

	//idxCurrentOp=0;
	//idxSavedInstruction = 0;

	//idxBaseNumber=
	//idxBaseString=0;

	//idxCurrentNumber=
	//idxCurrentString=0;

	//idxBaseStk=0;

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


	// push indexes ...

	//idxStkCurrentNumber=startIdxStkNumber;
	//idxCurrentString=idxBaseString;
	//ptrCurrentOp=ptrBaseOp;
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
/*
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

void CVirtualMachine::unrefSharedScriptVar( int index){

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
}*/
/*
PInfoSharedPointerNode CVirtualMachine::InsertZeroNode(CScriptVariable *shared_ptr){


	zero_shares[idxCurrentStack].InsertNode(_node);
	_node->data.n_shared=0;
	_node->data.shared_ptr=shared_ptr;

	return _node;


}
*/
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
		print_error_cr("MAX SHARED VARIABLES (Max. %i)",MAX_SHARES_VARIABLE);
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
/*
void CVirtualMachine::detachSharedScriptVarForRet(PInfoSharedPointerNode _node){

	//unsigned char *n_shares = &_node->data.n_shares;
	//if(*n_shares > 0){
	//	if(--(*n_shares)==0){ // mov back to 0s shares (candidate to be deleted on GC check)
	SHARE_LIST_DEATTACH(shared_var[idxCurrentStack],_node);
			//SHARE_LIST_INSERT(zero_shares[idxCurrentStack],_node);
	//		return true;
	//	}
	//}

	//print_error_cr("Internal error");
	//return false;
}*/

#define POP_TWO \
ptrResultInstructionOp2=--ptrCurrentOp;\
ptrResultInstructionOp1=--ptrCurrentOp;


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
		instruction=&(*current_statment)[instruction->index_op1];\
	}\
\
}


//#define LDR_OP2 ptrResultInstructionOp2=&ptrBaseOp[instruction->index_op2];ptrResultInstructionOp2->properties=ptrResultInstructionOp2->properties;


//============================================================================================================================================
tStackElement * CVirtualMachine::call_C_function(
		void *fun_ptr,
		const CScriptFunctionObject *irfs,
		tStackElement *ptrArg,
		unsigned char n_args){


	//auto v = argv->at(0)->getPointer_C_ClassName();
	CScriptVariable *script_variable=NULL;
	int converted_param[MAX_N_ARGS];
	intptr_t result;
	tStackElement *currentArg;




	/*if(idxBaseStk<n_args){
		print_error_cr("Internal error (idxBaseStk<n_args)");
		return &callc_result;
	}*/




	if((irfs->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) {
		print_error_cr("Function is not registered as C");
		return &callc_result;//CScriptVariable::UndefinedSymbol;
	}


	if(fun_ptr==0){
		print_error_cr("Null function");
		return &callc_result;//CScriptVariable::UndefinedSymbol;
	}

	if((char)irfs->m_arg.size() != n_args){
		print_error_cr("C argument VS scrip argument doestn't match sizes");
		return &callc_result;//CScriptVariable::UndefinedSymbol;
	}

	if(irfs->m_arg.size() >= MAX_N_ARGS){
		print_error_cr("Reached max param for C function (Current: %i Max Allowed: %i)",irfs->m_arg.size(),MAX_N_ARGS);
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

		switch(GET_INS_PROPERTY_VAR_TYPE(currentArg->properties)){
		case INS_PROPERTY_TYPE_BOOLEAN:
		case INS_PROPERTY_TYPE_NUMBER:
		case INS_PROPERTY_TYPE_INTEGER:
			converted_param[i]=(intptr_t)(&currentArg->stkValue);
			break;

		case INS_PROPERTY_TYPE_STRING:
			converted_param[i]=(intptr_t)(currentArg->stkValue);
			break;
		default: // script variable by default ...
			script_variable=(CScriptVariable *)currentArg->varRef;

			if(!(script_variable->getPointer_C_ClassName()==TYPE_SCRIPT_VARIABLE && irfs->m_arg[i]==typeid(CScriptVariable *).name())){ //not script, then it can pass through ...

				if((script_variable)->getPointer_C_ClassName()!=irfs->m_arg[i]){
					fntConversionType paramConv=CScriptClass::getConversionType((script_variable)->getPointer_C_ClassName(),irfs->m_arg[i]);

					if(paramConv == NULL){
						return &callc_result;
					}
					converted_param[i] = paramConv(script_variable);
				}
			}
			break;
		}
	}

	print_debug_cr("pre_call %i",n_args);

	if(irfs->idx_return_type == IDX_CLASS_VOID_C){ // getInstance()->getIdxClassVoid()){

		switch(n_args){
		default:
			print_error_cr("Max run-time args! (Max:%i Provided:%i)",MAX_N_ARGS,n_args);
			return &callc_result;//CScriptVariable::UndefinedSymbol;
		case 0:
			(*((std::function<void ()> *)fun_ptr))();
			break;
		case 1:
			(*((std::function<void (int)> *)fun_ptr))(converted_param[0]);
			break;
		case 2:
			(*((std::function<void (int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1]
									);
			break;
		case 3:
			(*((std::function<void (int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2]
									);
			break;
		case 4:
			(*((std::function<void (int,int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2],
					converted_param[3]
									);
			break;
		case 5:
			(*((std::function<void (int,int,int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2],
					converted_param[3],
					converted_param[4]
   				);
			break;
		case 6:
			(*((std::function<void (int,int,int,int,int,int)> *)fun_ptr))(
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
			print_error_cr("Max run-time args! (Max:%i Provided:%i)",MAX_N_ARGS,n_args);
			return NULL;//CScriptVariable::UndefinedSymbol;
		case 0:
			result=(*((std::function<int ()> *)fun_ptr))();
			break;
		case 1:
			result=(*((std::function<int (int)> *)fun_ptr))(converted_param[0]);
			break;
		case 2:
			result=(*((std::function<int (int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1]
									);
			break;
		case 3:
			result=(*((std::function<int (int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2]
									);
			break;
		case 4:
			result=(*((std::function<int (int,int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2],
					converted_param[3]
									);
			break;
		case 5:
			result=(*((std::function<int (int,int,int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2],
					converted_param[3],
					converted_param[4]
   				);
			break;
		case 6:
			result=(*((std::function<int (int,int,int,int,int,int)> *)fun_ptr))(
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

		/*if(arg->size() > 0){
			for(unsigned j=0; j < arg->size(); j++){
				switch(arg->at(j)->idxScriptClass){
				case IDX_PRIMITIVE_INTEGER:
					(*_ptrIniCurrentOp++)={INS_PROPERTY_TYPE_INTEGER,&((CInteger *)arg->at(j))->m_value};
					break;
				case IDX_PRIMITIVE_FLOAT:
					(*_ptrIniCurrentOp++)={INS_PROPERTY_TYPE_NUMBER,&((CNumber *)arg->at(j))->m_value};
					break;
				case IDX_CLASS_BOOLEAN:
					(*_ptrIniCurrentOp++)={INS_PROPERTY_TYPE_BOOLEAN,&((CBoolean *)arg->at(j))->m_value};
					break;
				case IDX_CLASS_STRING:
					(*_ptrIniCurrentOp++)={INS_PROPERTY_TYPE_STRING,&((CString *)arg->at(j))->m_value};
					break;
				default:
				case IDX_CLASS_SCRIPT_VAR:
					(*_ptrIniCurrentOp++)={INS_PROPERTY_TYPE_SCRIPTVAR,arg->at(j)};
					break;


				}
			}

		}*/

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

	print_debug_cr("Executing function %s ...",info_function->object_info.symbol_info.symbol_name.c_str());
	int idxBaseStk=(ptrCurrentOp-stack);//>>sizeof(tStackElement *);

	if(idxBaseStk<n_args){
		print_error_cr("Internal error (idxBaseStk<n_args)");
		exit(EXIT_FAILURE);
	}

	if(n_args>0){
		ptrArg=(ptrStartOp-n_args);
	}

	ptrLocalVar=ptrStartOp;


	if((info_function->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // C-Call

		void * fun_ptr = (void *)info_function->object_info.symbol_info.ref_ptr;

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
		print_error_cr("Reached max stack");
		exit(EXIT_FAILURE);
	}

	if((idxBaseStk+n_total_vars) >=  VM_LOCAL_VAR_MAX_STACK){
		print_error_cr("Error MAXIMUM stack size reached");
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



	bool	aux_boolean=false;
	float aux_float=0.0;
	CScriptVariable * ret_scriptvariable_node =NULL;
	unsigned char operator_type=NOP;
	CScriptVariable *var_aux=NULL;
	CScriptVariable **obj=NULL;
	CScriptFunctionObject * aux_function_info=NULL;
	tStackElement *ret_obj=NULL;

	CScriptVariable *svar=NULL;
	CScriptFunctionObject *constructor_function=NULL;
	CScriptVariable *calling_object=NULL;

	//tStackElement *stkResultInstructionIteration = ptrBaseOp;
	tStackElement *ptrResultInstructionOp1=NULL;//&stkResultInstruction[index_op1+startIdxStkResultInstruction];
	tStackElement *ptrResultInstructionOp2=NULL;//&stkResultInstruction[index_op2+startIdxStkResultInstruction];
	tStackElement *ldrVar;
	//tStackElement *ldrVar;
	//unsigned short ptrResultInstructionOp1->properties,ptrResultInstructionOp2->properties;
	//tStackElement *ptrResultLastInstruction=NULL;//&stkResultInstruction[idxStkCurrentResultInstruction-1];
	//unsigned int prop1, prop2;
	unsigned short pre_post_properties=0;
	//unsigned short scope_properties=0;
	unsigned short instruction_properties=0;
	tSymbolInfo *si;
	CScriptVariable **ptr_var_object=NULL;
	CScriptVariable *var_object = NULL;
	//int start_index_local_var = 0;

	unsigned short scope_type=0;//GET_INS_PROPERTY_SCOPE_TYPE(instruction->instruction_properties);
	short int pre_post_operator_type=0;//GET_INS_PROPERTY_PRE_POST_OP(instruction->instruction_properties);
	CCompiler::tInfoConstantValue * info_constant=NULL;
	//int index_op1 = -1;//
	//int index_op2 = -1;
	//tStackElement *ptrSavedInstruction=NULL;
	PtrStatment current_statment = NULL,
				ptr_statment_iteration=m_listStatements;

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

			const tInfoAsmOp * instruction = instruction_it++;
			const unsigned char operator_type=instruction->operator_type;
			const unsigned char index_op1=instruction->index_op1;


			if(operator_type==END_STATMENT){
				goto lbl_exit_statment;
			}
			else if(operator_type==NOP){ // ignore ...
				continue;
			}
			else if(
					   operator_type==LOAD
					|| operator_type==VGET

					){// load value in function of value/constant ...

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
										print_error_cr("Line %i. Index vector out of bounds!",AST_LINE_VALUE(instruction->idxAstNode));
										return NULL;
									}
																		ldrVar = &vec->m_objVector[v_index];;

									ok = true;

								}else{
									print_error_cr("Expected vector-index as integer");
									return NULL;
								}

							}
						}

						if(!ok){
							print_error_cr("Line %i: Variable \"%s\" is not type vector",
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

									print_error_cr("Line %i:var is not scriptvariable",AST_NODE(instruction->idxAstNode)->line_value);
									return NULL;
								}

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
							ldrVar=&si->object;
							//ptr_var_object = (CScriptVariable **)(&si->object);
							//var_object = (CScriptVariable *)(si->object);
							break;
						case INS_PROPERTY_LOCAL_SCOPE:
							/*if(instruction->index_op2 >= idxCurrentOp){
								print_error_cr("internal error: index out of stack");
								return NULL;
							}*/

							ldrVar = &ptrLocalVar[instruction->index_op2];
							//ptr_var_object = (CScriptVariable **)(&CVirtualMachine::ptrLocalVar[instruction->index_op2].stkValue);
							//var_object = *ptr_var_object;//(CScriptVariable *)(CVirtualMachine::ptrLocalVar[start_index_local_var+instruction->index_op2].stkValue);
							break;



							//ptr_var_object = (CScriptVariable **)(&CVirtualMachine::stack[instruction->index_op2].stkValue);
							//var_object = (CScriptVariable *)(CVirtualMachine::stack[instruction->index_op2].stkValue);
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
							case INS_PROPERTY_TYPE_NUMBER:
								if(ldrVar->properties& INS_PROPERTY_IS_C_VAR){
									COPY_NUMBER(&aux_float,ldrVar->stkValue);
								}else{
									COPY_NUMBER(&aux_float,&ldrVar->stkValue);
								}
								aux_float=-aux_float;
								COPY_NUMBER(ptrCurrentOp->stkValue,&aux_float);
								*ptrCurrentOp++={INS_PROPERTY_TYPE_NUMBER,ptrCurrentOp->stkValue,0};
								break;
							default:
								print_error_cr("internal error:cannot perform pre operator - because is not number");
								return NULL;
							}
							break;
					default:
							//PUSH_VAR(var_object,ptr_var_object,0,false);
							//(*ptrCurrentOp++)={0,ldrVar}; //
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
					(*ptrCurrentOp++)=*(((CCompiler::tInfoConstantValue *)instruction->index_op2));
					continue;
				}else if(index_op1== LOAD_TYPE::LOAD_TYPE_FUNCTION){
					/*if(!loadFunctionValue(instruction,info_function, this_object,*current_statment)){
						return NULL;
					}*/
					void *function_obj=NULL;
					vector<int> *vec_functions;
					unsigned short function_properties=0;
					CScriptVariable * class_obj=NULL;
					int index_op2 = (int)instruction->index_op2;
					instruction_properties=instruction->instruction_properties;
					scope_type=GET_INS_PROPERTY_SCOPE_TYPE(instruction_properties);



					//else
					{

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
								print_error_cr("Expected scriptvar");
								return NULL;
							}


							/*class_obj = ((CScriptVariable **)ptrBaseOp[index_op2].varRef);
							function_obj = NULL; // TODO: always get symbol in CALL op. Make a way to do it optimized!*/
						}else if(scope_type == INS_PROPERTY_THIS_SCOPE){
							if((si = this_object->getFunctionSymbolByIndex(index_op2))==NULL){
								print_error_cr("cannot find function \"this.%s\"",AST_NODE(instruction->idxAstNode)->symbol_value.c_str());
								return NULL;
							}

							function_obj =(CScriptFunctionObject *)si->object.stkValue;

						}else if(scope_type == INS_PROPERTY_SUPER_SCOPE){ // super scope ?
							if((si = this_object->getFunctionSymbolByIndex(index_op2))==NULL){
								print_error_cr("cannot find function \"super.%s\"",AST_NODE(instruction->idxAstNode)->symbol_value.c_str());
								return NULL;
							}
							function_obj =(CScriptFunctionObject *)si->object.stkValue;
						}else{ // global
							vec_functions = &(GET_MAIN_FUNCTION_OBJECT->object_info.local_symbols.vec_idx_registeredFunction);
							//function_obj = GET_SCRIPT_FUNCTION_OBJECT(info_function->object_info.local_symbols.vec_idx_registeredFunction[index_op2]);
						}


						if(index_op2 == ZS_UNDEFINED_IDX){ // is will be processed after in CALL instruction ...
							function_properties=INS_PROPERTY_UNRESOLVED_FUNCTION;
							function_obj= (void *)instruction; // saves current instruction in order to resolve its idx later (in call instruction)
						}else if((index_op2<(int)vec_functions->size())) // get the function ...
						{
							function_obj =GET_SCRIPT_FUNCTION_OBJECT((*vec_functions)[index_op2]);
						}
						else{
							print_error_cr("cannot find symbol global \"%s\"",AST_NODE(instruction->idxAstNode)->symbol_value.c_str());
							return NULL;

						}
					}

					PUSH_FUNCTION(function_properties,function_obj,class_obj);
					continue;

				}else if(index_op1== LOAD_TYPE::LOAD_TYPE_ARGUMENT){
		 	 		ldrVar=&ptrArg[instruction->index_op2];
					PUSH_STACK_VAR(ldrVar);
					continue;
				}
				else{

					print_error_cr("no load defined type");
					return NULL;
					break;
				}
				continue;
			}
			else if(
					   operator_type==STORE
					|| operator_type==VPUSH
					|| operator_type==PUSH_ATTR
					){ // mov value expression to var


				tStackElement *dst_ins=NULL;
				tStackElement *src_ins=NULL;
				bool ok=false;

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
						print_error_cr("Expected vector object");
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
								print_error_cr("internal error (operator2 is not string)");
								return NULL;
							}
						}
					}


					if(!ok){
						print_error_cr("Expected struct object");
						return NULL;
					}



				}
				else{ // pop two parameters nothing ...
					POP_TWO;

					if(ptrResultInstructionOp1->properties & INS_PROPERTY_IS_STACKVAR) {// == CScriptVariable::VAR_TYPE::OBJECT){
						// get pointer object (can be assigned)
						//ASSIGN_FROM_RESULT_INSTRUCTION(ptrResultInstructionOp1->ptrObjectRef,ptrResultInstructionOp2);
						//#define ASSIGN_FROM_RESULT_INSTRUCTION(var, ptr_instruction)

						dst_ins=(tStackElement *)ptrResultInstructionOp1->varRef; // stkValue is expect to contents a stack variable
						src_ins=ptrResultInstructionOp2; // store ptr instruction2 op as src_var_value
						//tStackElement *ptr_instruction = ptrResultInstructionOp2;
					}else{
						print_error_cr("Expected object l-value mov");
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


				continue;
			}
			else if(operator_type==EQU){  // <

				POP_TWO;
				PROCESS_LOGIC_OPERATION(==);

				/*if (IS_INT(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) < LOAD_INT_OP(ptrResultInstructionOp2));
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) < LOAD_NUMBER_OP(ptrResultInstructionOp2));
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) < LOAD_INT_OP(ptrResultInstructionOp2));
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) < LOAD_NUMBER_OP(ptrResultInstructionOp2));
				}else{
					PRINT_DUAL_ERROR_OP("<");
					return NULL;
				}*/




				continue;
			}
		/*

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
				PROCESS_LOGIC_OPERATION(<);

				/*if (IS_INT(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) < LOAD_INT_OP(ptrResultInstructionOp2));
				}else if (IS_INT(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_INT_OP(ptrResultInstructionOp1) < LOAD_NUMBER_OP(ptrResultInstructionOp2));
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_INT(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) < LOAD_INT_OP(ptrResultInstructionOp2));
				}else if (IS_NUMBER(ptrResultInstructionOp1->properties) && IS_NUMBER(ptrResultInstructionOp2->properties)){
					PUSH_BOOLEAN(LOAD_NUMBER_OP(ptrResultInstructionOp1) < LOAD_NUMBER_OP(ptrResultInstructionOp2));
				}else{
					PRINT_DUAL_ERROR_OP("<");
					return NULL;
				}*/




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
				break;*/

			else if(operator_type==NEG){ // !
				POP_ONE;

				if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_INTEGER){ // operation will result as integer.
					PUSH_INTEGER((-((int)(ptrResultInstructionOp1->stkValue))));
				}else if(ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_NUMBER){
					COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);
					PUSH_NUMBER(-f_aux_value1);
				}else{
						print_error_cr("Line %i:Expected operands 1 as number or integer!",AST_LINE_VALUE(instruction->idxAstNode));
						return NULL;
				}
				continue;

			}else if(operator_type== ADD || operator_type== ADD_ASSIGN){ // +


				if(operator_type== ADD_ASSIGN){
					POP_ONE;
					ptrResultInstructionOp2=(ptrCurrentOp-1);
				}else{
					POP_TWO;
				}


				unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(ptrResultInstructionOp1->properties|ptrResultInstructionOp2->properties);\
				if(properties==INS_PROPERTY_TYPE_INTEGER){
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

				}else if (properties== INS_PROPERTY_TYPE_STRING){
						sprintf(str_aux,"%s%s",((string *)ptrResultInstructionOp1->stkValue)->c_str(),((string *)(ptrResultInstructionOp2->stkValue))->c_str());
						PUSH_STRING(str_aux);


				}else if(properties== INS_PROPERTY_TYPE_NUMBER){
					COPY_NUMBER(&f_aux_value1,&ptrResultInstructionOp1->stkValue);\
					COPY_NUMBER(&f_aux_value2,&ptrResultInstructionOp2->stkValue);\
					PUSH_NUMBER(f_aux_value1 + f_aux_value2);\

				}
				else{
					PRINT_DUAL_ERROR_OP(+);
				}

				// get indexes and check whether is possible or not its calculation.
				// check indexes
				/*if (IS_STRING(ptrResultInstructionOp1->properties) && (IS_UNDEFINED(ptrResultInstructionOp2->properties) || IS_VAR(ptrResultInstructionOp2->properties))){
					string result = "undefined";
					if(IS_VAR(ptrResultInstructionOp2->properties)){
						result = ((CScriptVariable *)(ptrResultInstructionOp2->stkValue))->getClassName();
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
				}*/

				continue;
			}

			/*case LOGIC_AND: // &&
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

					if(!((bool)(ptrResultInstructionOp1->stkValue))){
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

					if(((bool)(ptrResultInstructionOp1->stkValue))){
						CHK_JMP;
					}
				}else{
					print_error_cr("Expected boolean element");
					return NULL;
				}

				continue;
			}
			/*else if(operator_type==START_ARG){ // set start arg val...
				*ptrCurrentOp++={INS_PROPERTY_START_FUNCTION_ARGS,0,0};
			}*/
			else if(operator_type== CALL){ // calling function after all of args are processed...
				// check whether signatures matches or not ...
				// 1. get function object ...
				//POP_ONE;

				/*if((ptrResultInstructionOp1->properties & INS_PROPERTY_TYPE_FUNCTION)==0){
					print_error_cr("object \"%s\" is not function at line %i",AST_SYMBOL_VALUE_CONST_CHAR(instruction->idxAstNode), AST_LINE_VALUE(instruction->idxAstNode));
					return NULL;
				}*/

				aux_function_info = NULL;
				unsigned char n_args=0;//iao->instruction_properties;
				tStackElement *startArg=ptrCurrentOp;
				tStackElement *callAle=NULL;

				while(n_args <= MAX_N_ARGS && (((startArg-1)->properties&INS_PROPERTY_TYPE_FUNCTION)==0)){
					startArg--;
					n_args++;
				}

				callAle = ((startArg-1));
				calling_object = this_object;

				// load function ...
				if(callAle->properties & INS_PROPERTY_UNRESOLVED_FUNCTION){
					tInfoAsmOp *iao = (tInfoAsmOp *)callAle->stkValue;
					//tInfoAsmOp *iao = &(*current_statment)[instruction->index_op1];
					unsigned short scope_type = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);
					tSymbolInfo * si=NULL;


					if(iao->index_op2 == ZS_UNDEFINED_IDX
					//|| scope_type == INS_PROPERTY_ACCESS_SCOPE
					){
						// local vars as functions ...
						vector<int> *vec_global_functions=&(GET_MAIN_FUNCTION_OBJECT->object_info.local_symbols.vec_idx_registeredFunction);

						int size_fun_vec = (int)vec_global_functions->size()-1;

						// registered symbols in case is INS_PROPERTY_ACCESS_SCOPE...
						vector<tSymbolInfo> *m_functionSymbol=NULL;
						if(scope_type==INS_PROPERTY_ACCESS_SCOPE){
							calling_object = (CScriptVariable *)callAle->varRef;
							m_functionSymbol=calling_object->getVectorFunctionSymbol();
							size_fun_vec = (int)m_functionSymbol->size()-1;
						}


						bool all_check=true;

						// startArgs point to first stack value ...
						bool found = NULL;
						//CScriptVariable * base_var=NULL;




						//switch(scope_type){
						//default: // check function parameters!

							// try first all function with no conversion and then try the same with conversions ...

							// for all symbols from calling object ...
							//for(int h=0; h < 2 && !found; h++){
								// h=0: match all args.
								// h=1: match some args...


								for(int i = size_fun_vec; i>=0 && aux_function_info==NULL; i--){ // search all function that match symbol ...
									CScriptFunctionObject *irfs = NULL;
									if(scope_type==INS_PROPERTY_ACCESS_SCOPE){
										irfs = (CScriptFunctionObject *)m_functionSymbol->at(i).object.stkValue;
										aux_string=m_functionSymbol->at(i).symbol_value;
									}else{
										irfs=GET_SCRIPT_FUNCTION_OBJECT(vec_global_functions->at(i));
										aux_string=irfs->object_info.symbol_info.symbol_name;
									}

									// we found a function is match ...
									if(aux_string == AST_NODE(iao->idxAstNode)->symbol_value && irfs->m_arg.size() == n_args){

										if(irfs->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF){ // C! Must match args...

											//if(){ // let's check parameters ...

												all_check=true; // check arguments types ...



												// convert parameters script to c...
												//for( int h = 0 ; h < 2; h++){
													for( int k = 0; k < n_args && all_check;k++){


														tStackElement *currentArg=&startArg[k];

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


														//converted_param[i]= (int)(argv->at(i));
														//if(h==0){ // match the all parameters
														all_check = aux_string==irfs->m_arg[k];
														//}

														if(!all_check){
															// let's see whether it can be coneverted to target signature ...
															all_check =CScriptClass::getConversionType(aux_string,irfs->m_arg[k], NULL)!=NULL;
														}
													}

													if(all_check){ // we found the right function (set it up!) ...
														iao->index_op2 = i;
														aux_function_info = irfs;//(CScriptFunctionObject *)m_functionSymbol->at(i)->object.stkValue;
													}
												//}

											//}

										}else{ // type script function  ...

											iao->index_op2=i;
											//if(irfs->m_arg.size() == n_args){
											aux_function_info = irfs;//GET_SCRIPT_FUNCTION_OBJECT((*vec_global_functions)[iao->index_op2]);
											//found = true;
											//}else{
											//	print_error_cr("Error at line %i calling function \"%s\" expected to have %i parameters");
											//}

										}
									}
								}

								if(aux_function_info == NULL){
									int n_candidates=0;
									string str_candidates="";
									for(int i = size_fun_vec; i>=0 && aux_function_info==NULL; i--){ // search all function that match symbol ...
										CScriptFunctionObject *irfs = NULL;
										if(scope_type==INS_PROPERTY_ACCESS_SCOPE){
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
														str_candidates+=demangle(irfs->m_arg[a]);
													}

													str_candidates+=");\n";


													n_candidates++;

												}
									}

									print_error_cr("Cannot find right C symbol for \"%s\" at line %i.\n\n%s",
											AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode),
											AST_LINE_VALUE(iao->idxAstNode),
											str_candidates.c_str());

									return NULL;
								}
					}


									/*if(

									   (irfs->object_info.symbol_info.symbol_name == AST_NODE(iao->idxAstNode)->symbol_value)
									&& (irfs->m_arg.size() == n_args) // matching identical number args!

									){


										all_check=true; // check arguments types ...
										// convert parameters script to c...
										for( int k = 0; k < n_args && all_check;k++){

											unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(startArg[k].properties);
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
												aux_string = ((CScriptVariable *)startArg[k].varRef)->getPointer_C_ClassName();
												break;
											}


											//converted_param[i]= (int)(argv->at(i));
											if(h==0){
												all_check = aux_string==irfs->m_arg[k];
											}
											else{
												if(aux_string!=irfs->m_arg[k]){
													all_check =CScriptClass::getConversionType(aux_string,irfs->m_arg[k], NULL)!=NULL;
												}
											}
										}

										if(all_check){ // we found the right function (set it up!) ...
											iao->index_op2 = i;
											found=true;
										}
									}
								}
							//}

							// update structure ...
							if(found){
								aux_function_info = GET_SCRIPT_FUNCTION_OBJECT((*vec_global_functions)[iao->index_op2]);
							}else{

								int n_candidates=0;
								string str_candidates="";
								for(int i = 0; i < (int)vec_global_functions->size(); i++){ // search all function that match symbol ...

											CScriptFunctionObject *irfs = GET_SCRIPT_FUNCTION_OBJECT(vec_global_functions->at(i));

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
													str_candidates+=demangle(irfs->m_arg[a]);
												}

												str_candidates+=");\n";


												n_candidates++;

											}
								}


								print_error_cr("Cannot find right C symbol for \"%s\" at line %i.\n\n%s",
										AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode),
										AST_LINE_VALUE(iao->idxAstNode),
										str_candidates.c_str());

								return NULL;



							}
						//	break;

						case INS_PROPERTY_ACCESS_SCOPE: // check function access

							calling_object = (CScriptVariable *)callAle->varRef;////((CScriptVariable **)stkResultInstruction[iao->index_op2+startIdxStkResultInstruction].varRef);

							for(int h=0; h < 2 && !found; h++){ // h=0 -> match signature, 1=doesn't match signature
								int idx_function;
								if(h==0){
									idx_function=calling_object->getidxScriptFunctionObjectWithMatchArgs(AST_SYMBOL_VALUE(iao->idxAstNode),startArg,n_args, true);
								}else{
									idx_function=calling_object->getidxScriptFunctionObjectWithMatchArgs(AST_SYMBOL_VALUE(iao->idxAstNode),startArg,n_args);
								}

								if(idx_function != ZS_UNDEFINED_IDX){
									iao->index_op2=idx_function; // we found the right function (set it up!) ...
									si = calling_object->getFunctionSymbolByIndex(idx_function);
									found = true;
								}

							}

							if(si != NULL){
								aux_function_info = (CScriptFunctionObject *)si->object.stkValue;
							}

							if(aux_function_info == NULL){
								print_error_cr("cannot find function \"%s\" at line %i",
										AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode),
										AST_LINE_VALUE(iao->idxAstNode),
										calling_object->getMessageMatchingFunctions(AST_SYMBOL_VALUE(iao->idxAstNode)).c_str()
										);
								return NULL;
							}

							break;

						//}
					}
					else{
						print_error_cr("Unexpected C calling exception");
					}*/
				}
				else{
					aux_function_info=(CScriptFunctionObject *) (callAle->stkValue);
				}

				if(aux_function_info == NULL){ // undefined function due different signatures ...


				}

				if(aux_function_info == NULL){
					print_error_cr("Internal error");
					return NULL;
				}

				if(n_args > MAX_N_ARGS){
					print_error_cr("Max arguments reached function at line XXX");
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



				/*if((instruction_properties & INS_PROPERTY_CALLING_OBJECT) != 0){ // function depends on scriptvariable obj...
					calling_object= (CScriptVariable *)ptrResultInstructionOp1->varRef;
				}
				else if((aux_function_info->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){

				}





				calling_object = this_object;
				if((instruction_properties & INS_PROPERTY_CALLING_OBJECT) != 0){
					calling_object= (CScriptVariable *)ptrResultInstructionOp1->varRef;
				}
				else if((aux_function_info->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){

				}*/

				//push();

				// by default virtual machine gets main object class in order to run functions ...
				if((ret_obj=execute_internal(aux_function_info,calling_object,ptrCurrentOp,ptrCurrentStr,n_args))==NULL){
					return NULL;
				}

				//pop();

				// if function is C must register pointer !

				//if((aux_function_info->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // C-Call
				//if((instruction_properties & INS_PROPERTY_DIRECT_CALL_RETURN) == 0){ // no direct call, so make share pointer
					if(ret_obj->properties & INS_PROPERTY_TYPE_SCRIPTVAR){

						if(!((CScriptVariable *)(ret_obj->varRef))->initSharedPtr()){
							return NULL;
						}
					}
				//}

				// deallocates stack...
				//m_functionArgs.clear();


				// reset stack ...
				ptrCurrentOp=startArg-1;

				// ... and push result if not function constructor...
				if(!(instruction_properties & INS_PROPERTY_CONSTRUCT_CALL)){
					*ptrCurrentOp++ = *ret_obj;
				}
				continue;

			/*case PUSH: // push arg instruction will creating object ensures not to have feature e/s...
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
						CVector * vec = (CVector *)(ptrResultInstructionOp1->stkValue);
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

				break;*/
			}else if(operator_type== NEW){
					svar=NEW_CLASS_VAR_BY_IDX(instruction->index_op1);

					if(!svar->initSharedPtr()){
						return NULL;
					}


					/*if((constructor_function = svar->getConstructorFunction()) != NULL){
						execute(constructor_function,svar);
					}*/

					(*ptrCurrentOp++)={INS_PROPERTY_TYPE_SCRIPTVAR,NULL,svar};
					continue;

			}else if(operator_type== DECL_VEC){ // Create new vector object...
					svar=NEW_VECTOR_VAR;
					//PUSH_VAR(svar,NULL,0,false);

					if(!svar->initSharedPtr()){
						return NULL;
					}

					(*ptrCurrentOp++)={INS_PROPERTY_TYPE_SCRIPTVAR,NULL,svar};

					continue;
			/*}else if(operator_type== VPUSH){ // Push values into vector
				POP_ONE;
				CScriptVariable *vec_obj = NULL;
				if((ptrCurrentOp-1)->properties & INS_PROPERTY_TYPE_SCRIPTVAR){
					vec_obj = (CScriptVariable *)(ptrCurrentOp-1)->varRef;
					if(vec_obj->idxScriptClass == IDX_CLASS_VECTOR){ // push value ...

						((CVector *)vec_obj)->add(*ptrResultInstructionOp1);
						continue;
					}
				}

				print_error_cr("Expected vector object");
				return NULL;
*/
				/*aux_function_info = NULL;
				unsigned char n_args=0;//iao->instruction_properties;
				tStackElement *startArg=ptrCurrentOp;
				tStackElement *callAle=NULL;

				while((((startArg-1)->properties&INS_PROPERTY_TYPE_SCRIPTVAR))){
					startArg--;
					n_args++;
				}

				callAle = ((startArg-1));*/
				//POP_TWO;

				/*if(IS_VECTOR(ptrResultInstructionOp1)){
					CVector * vec = (CVector *)(ptrResultInstructionOp1->stkValue);
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
				}*/

			}else if(operator_type== DECL_STRUCT){ // Create new vector object...
				svar=NEW_STRUCT_VAR;
				//PUSH_VAR(svar,NULL,0,false);

				if(!svar->initSharedPtr()){
					return NULL;
				}

				(*ptrCurrentOp++)={INS_PROPERTY_TYPE_SCRIPTVAR,NULL,svar};

				continue;

			/*}else if(operator_type==PUSH_ATTR){

				POP_ONE;
				CScriptVariable *struct_obj = NULL;
				if((ptrCurrentOp-1)->properties & INS_PROPERTY_TYPE_SCRIPTVAR){
					struct_obj = (CScriptVariable *)(ptrCurrentOp-1)->varRef;
					if(struct_obj->idxScriptClass == IDX_CLASS_STRUCT){ // push value ...

						((CStruct *)struct_obj)->(*ptrResultInstructionOp1);
						continue;
					}
				}

				print_error_cr("Expected vector object");
				return NULL;
*/
			}else if(operator_type== RET){

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


				// remove sharepointer in order to avoid deallocate value ... function parent is in charge to share pointer of resulting value ...

				/*if(callc_result.properties & INS_PROPERTY_TYPE_SCRIPTVAR){

					if(!((CScriptVariable *)(ret_obj->varRef))->removeSharedPtr()){
						return NULL;
					}
				}*/



				/*if((ret=createVarFromResultInstruction(&ptrBaseOp[instruction->index_op1], false)) == NULL){ // share pointer but not add as shared!
					return NULL;
				}*/
				//end_by_ret=true;


				goto lbl_exit_function;
			}
			/*case NEW:
				svar=NEW_CLASS_VAR_BY_IDX(instruction->index_op1);
				PUSH_VAR(svar, NULL,0, true);

				if(!svar->initSharedPtr()){
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
			//	ptrSavedInstruction=ptrCurrentOp-1;
			//	break;
			//case LOAD_I: // For ternary condition ...
/*
				switch(GET_INS_PROPERTY_VAR_TYPE(ptrSavedInstruction->properties)){
				default:
					print_error_cr("unexpected type");
					break;
				case INS_PROPERTY_TYPE_INTEGER:
					PUSH_INTEGER(((int)ptrSavedInstruction->stkValue),NULL,0);
					break;
				case INS_PROPERTY_TYPE_BOOLEAN:
					PUSH_BOOLEAN(ptrSavedInstruction->stkValue,NULL,0);
					break;
				case INS_PROPERTY_TYPE_NUMBER:
					if(!pushNumber(*((float *)ptrSavedInstruction->stkValue))){
						return NULL;
					}
					break;


				case INS_PROPERTY_TYPE_STRING:
					if(!pushString(*((string *)ptrSavedInstruction->stkValue))){
						return NULL;
					}
					break;
				case INS_PROPERTY_TYPE_SCRIPTVAR:
					PUSH_VAR(((CScriptVariable *)ptrSavedInstruction->stkValue),NULL,0,false);
					break;
				}
				break;*/
			else if(operator_type==POP_SCOPE){
				POP_SCOPE(instruction->index_op1);
			}

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

	POP_SCOPE(scope_index);






	//=========================
	// POP STACK
	// unref 1st scope ...

	//CVirtualMachine::idxCurrentOp-=CVirtualMachine::vecIdxLocalVar.top();
	//CVirtualMachine::vecIdxLocalVar.pop();

	/*if(!CVirtualMachine::vecIdxLocalVar.empty()){

		ptrLocalVar=&stack[CVirtualMachine::idxCurrentOp-CVirtualMachine::vecIdxLocalVar.top()];

		// restore last current instruction...
		//ptrBaseOp=ptrLocalVar-1;

		// save current aux vars ...
		//idxBaseNumber = vecIdxStkNumber.top();
		idxBaseString=vecIdxStkString.top();
		//ptrBaseOp=vecPtrCurrentStkResultInstruction.top();

		//vecIdxStkNumber.pop();
		vecIdxStkString.pop();
		//vecPtrCurrentStkResultInstruction.pop();
	}*/

	if(idxCurrentStack > 0){
		idxCurrentStack--;
	}
	else{
		print_error_cr("Reached min stack");
		return NULL;
	}

	// POP STACK
	//=========================


	//popStack();
	return &callc_result;
}

/*
void CVirtualMachine::pushStack(const CScriptFunctionObject *info_function, int n_args){//, vector<CScriptVariable *> * argv){
	unsigned n_arg_size = n_args;//info_function->m_arg.size();
	unsigned n_local_vars = info_function->object_info.local_symbols.m_registeredVariable.size();
	unsigned n_total_vars=n_arg_size+n_local_vars;


	if(idxCurrentStack < MAX_FUNCTION_CALL){
		idxCurrentStack++;
	}
	else{
		print_error_cr("Reached max stack");
		exit(EXIT_FAILURE);
	}

	if((idxBaseStk+n_total_vars) >=  VM_LOCAL_VAR_MAX_STACK){
		print_error_cr("Error MAXIMUM stack size reached");
		exit(EXIT_FAILURE);
	}

	ptrLocalVar=&stack[n_arg_size];//CVirtualMachine::idxBaseStk];


	// init local vars ...
	tStackElement *ptr_aux = ptrLocalVar;
	for(unsigned i = 0; i < n_local_vars; i++){
		*ptr_aux++={
				INS_PROPERTY_TYPE_UNDEFINED, // starts undefined.
				0,							 // no value assigned.
				0 						     // no varref related.
			};


	}

	CVirtualMachine::vecIdxLocalVar.push(n_total_vars);
	CVirtualMachine::idxCurrentOp+=n_local_vars;

	// save current aux vars ...
	vecIdxStkString.push(idxBaseString);
	vecPtrCurrentStkResultInstruction.push(ptrBaseOp);

	//idxBaseNumber=idxCurrentNumber;
	idxBaseString=idxCurrentString;
	ptrBaseOp=&ptrLocalVar[idxCurrentOp];

}*/
/*
void CVirtualMachine::popStack(){

	// unref 1st scope ...

	//CVirtualMachine::idxCurrentOp-=CVirtualMachine::vecIdxLocalVar.top();
	CVirtualMachine::vecIdxLocalVar.pop();

	if(!CVirtualMachine::vecIdxLocalVar.empty()){

		ptrLocalVar=&stack[CVirtualMachine::idxCurrentOp-CVirtualMachine::vecIdxLocalVar.top()];

		// restore last current instruction...
		//ptrBaseOp=ptrLocalVar-1;

		// save current aux vars ...
		//idxBaseNumber = vecIdxStkNumber.top();
		idxBaseString=vecIdxStkString.top();
		//ptrBaseOp=vecPtrCurrentStkResultInstruction.top();

		//vecIdxStkNumber.pop();
		vecIdxStkString.pop();
		//vecPtrCurrentStkResultInstruction.pop();
	}

	if(idxCurrentStack > 0){
		idxCurrentStack--;
	}
	else{
		print_error_cr("Reached min stack");
	}

}*/


// NUMBER result behaviour.
// this is the combination for number operations:
//
// op1 | op2 |  R
// ----+-----+----
//  i  |  i  |  i
//  i  |  f  |  i
//  f  |  i  |  f
//  f  |  f  |  f


/*
bool CVirtualMachine::pushNumber(float init_value, CScriptVariable ** ptrAssignable, unsigned short properties){

	if(idxStkCurrentNumber ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max string operations");
		return false;
	}


	stkNumber[idxStkCurrentNumber]=init_value;
	*ptrCurrentOp++={(unsigned short)(INS_PROPERTY_TYPE_STRING|properties),&stkNumber[idxStkCurrentNumber],ptrAssignable};
	idxStkCurrentNumber++;

	return true;
}
*/
/*
bool CVirtualMachine::pushString(const string & init_value, CScriptVariable ** ptrAssignable,unsigned short properties){
	if(idxCurrentString ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max string operations");
		return false;
	}


	stkString[idxCurrentString]=init_value;
	*ptrCurrentOp++={(unsigned short)(INS_PROPERTY_TYPE_STRING|properties),&stkString[idxCurrentString],ptrAssignable};
	idxCurrentString++;

	return true;

}

bool CVirtualMachine::PUSH_FUNCTION(CScriptFunctionObject * init_value, CScriptVariable ** ptrAssignable,unsigned short properties){

	*ptrCurrentOp++={(unsigned short)(INS_PROPERTY_TYPE_FUNCTION|properties),init_value,ptrAssignable};
	return true;
}






*/

/*

void CVirtualMachine::popScope( CScriptFunctionObject *info_function,int index)//, CScriptVariable *ret)
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
		tStackElement *ptr_ale =&ptrLocalVar[idx_local_var];

		//unsigned short type_var = GET_INS_PROPERTY_VAR_TYPE(ptrLocalVar[idx_local_var].var_type);
		CScriptVariable *var = NULL;
		switch(GET_INS_PROPERTY_VAR_TYPE(ptr_ale->properties)){
		case INS_PROPERTY_TYPE_STRING:
		case INS_PROPERTY_TYPE_SCRIPTVAR:
			var =((CScriptVariable *)(ptr_ale->varRef));

			if(var != VM_NULL && var !=  VM_UNDEFINED){

				//if(ret != var){ // is not ret variable ...
				if(var->ptr_shared_pointer_node != NULL){
					unrefSharedScriptVar(var->ptr_shared_pointer_node);
				}
			}
		}

		// reinit local var...
		*ptr_ale={
				INS_PROPERTY_TYPE_UNDEFINED, // undefined
				0,							 // no value
				0							 // no var ref. related
		};
	}

	CALL_GC;
}
*/

CVirtualMachine::~CVirtualMachine(){

}
