#include "core/zg_core.h"

//CVirtualMachine * CVirtualMachine::m_virtualMachine = NULL;
//vector<CVirtualMachine::CVirtualMachine> CVirtualMachine::ALE;


// static: only defined in this module...

CVirtualMachine::CVirtualMachine(){

	basePtrLocalVar=NULL;
	idxStkCurrentLocalVar=0;
	idxSavedInstruction = 0;

	startIdxStkNumber=

	startIdxStkString=
	startIdxStkResultInstruction=0;

	// push indexes ...

	reset();
}

void CVirtualMachine::reset(){

	// deallocate allocated aux vectors...

	idxStkCurrentNumber=startIdxStkNumber;

	idxStkCurrentString=startIdxStkString;
	idxStkCurrentResultInstruction=startIdxStkResultInstruction;
	//memset(stkResultInstruction,0,sizeof(stkResultInstruction));
	//m_functionArgs.clear();
}



#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __VERBOSE_MESSAGE__
#endif



#ifdef  __VERBOSE_MESSAGE__

#define print_vm_cr print_info_cr
#else
#define print_vm_cr(s,...)
#endif



CScriptVariable * CVirtualMachine::execute(tInfoRegisteredFunctionSymbol *info_function, CScriptVariable *this_object, vector<CScriptVariable *> * argv, int stk){

	print_info_cr("Executing function %s ...",info_function->object_info.symbol_info.symbol_name.c_str());


	//tInfoRegisteredFunctionSymbol *irsf=sf->getFunctionInfo();

	//tInfoRegisteredFunctionSymbol *function_info =function_object->getFunctionInfo();

	if((info_function->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // C-Call

		void * fun_ptr = (void *)info_function->object_info.symbol_info.ref_ptr;

		if((info_function->object_info.symbol_info.properties &  SYMBOL_INFO_PROPERTIES::PROPERTY_STATIC_REF) != SYMBOL_INFO_PROPERTIES::PROPERTY_STATIC_REF){ // if not static then is function depends of object ...

			if(this_object!= NULL && this_object != CZG_ScriptCore::getInstance()->getMainObject()){
				fun_ptr = this_object->getFunctionSymbolByIndex(info_function->object_info.symbol_info.index)->proxy_ptr;
			}
		}

		return CZG_ScriptCore::call_C_function(fun_ptr,info_function,argv);

	}


	int scope_index =  info_function->object_info.symbol_info.ast->scope_info_ptr->getIndex();

	if(info_function->object_info.symbol_info.ast->keyword_info!=NULL){
		if(info_function->object_info.symbol_info.ast->keyword_info->id == KEYWORD_TYPE::FUNCTION_KEYWORD){
			scope_index=info_function->object_info.symbol_info.ast->children[1]->scope_info_ptr->getIndex();
		}
	}


	CScriptVariable *ret=CScriptVariable::UndefinedSymbol;
	vector<tInfoStatementOp> * m_listStatements = &info_function->object_info.statment_op;
	//bool conditional_jmp=false;
	int jmp_to_statment = -1;
	int instruction_to_statment = -1;
	bool end_by_ret=false;
	bool break_jmp = false;

	/*if(argv != NULL){

		if((*argv).size() != function_info->m_arg.size()){
			print_error_cr("calling function s from line . Expected %i but passed %i",function_info->m_arg.size(),(*argv).size());
			return false;
		}

		//print_info_cr("assign local symbol the passing args...");
		for(unsigned i = 0; i < (*argv).size(); i++){
			//print_info_cr("%s=%s <cr>",sf->getArgVector()->at(i).c_str(),(*argv).at(i)->getPointerClassStr().c_str());
			(*this_object->getArgSymbol(i)).object=(*argv).at(i);
		}

	}*/



	//CVirtualMachine ALE; // new ale ?

	// reserve vars and assign argv vars ...
	pushStack(info_function, argv);


	int n_stats=(*m_listStatements).size();


	for(int s = 0; s < n_stats && !end_by_ret;){

		//conditional_jmp = false;
		jmp_to_statment = -1;
		break_jmp=false;
		instruction_to_statment = -1;
		tInfoStatementOp * current_statment = &(*m_listStatements)[s];
		vector<tInfoAsmOp *> * asm_op_statment = &current_statment->asm_op;
		int n_asm_op= asm_op_statment->size();

		//if(stk==2){
		//	s++;
		//	continue;
		//}

		if(n_asm_op>0){

			// clear previous ALE information stack..
			//if(stk!=2){
			reset();
			//}

			//vector<CCompiler::tInfoAsmOp *> * asm_op_statment = &(*m_listStatements)[s].asm_op;



			//CCompiler::tInfoAsmOp * instruction=NULL;
			int ins = 0;

			while(ins  <  n_asm_op && !break_jmp){ // for each code-instruction execute it.
				print_vm_cr("executing instruction  [%02i:%02i]...", s,ins);


				//print_vm_cr("executing code...%i/%i",s,i);

				//if(stk!=2){

					//return true;


					if(!performInstruction(asm_op_statment->at(ins),jmp_to_statment,instruction_to_statment,info_function,this_object,argv,asm_op_statment,stk)){
						return NULL;
					}

					if(asm_op_statment->at(ins)->operator_type == ASM_OPERATOR::RET){ // return...



						if((ret=createVarFromResultInstruction(&stkResultInstruction[asm_op_statment->at(ins)->index_op1+startIdxStkResultInstruction])) == NULL){ // return last ALE value
							return NULL;
						}/*else{
							if(ret != CScriptVariable::UndefinedSymbol || ret != CScriptVariable::NullSymbol){
								CSharedPointerManager::getInstance()->newSharedPointer(ret);
							}
  						}*/

						end_by_ret=true;
					}

				//}


					if(jmp_to_statment != -1){

						if(s!=jmp_to_statment){
							break_jmp = true;
						}

						s=jmp_to_statment;

						if(instruction_to_statment != -1){
							ins=instruction_to_statment;
							idxStkCurrentResultInstruction=ins+1;
						}else{

						}



					}else{
						ins++;
					}

				//previous_instruction = instruction;
			}


			if(!break_jmp){// next statment ...
				s++;
			}

			// to deallocate not used poiners...
			if(!end_by_ret){
			 CSharedPointerManager::getInstance()->gc();
			}


		}else{ //no instructions ... pass next statment ...
			s++;
		}
	}

	popScope(info_function,scope_index);
	popStack();
	return ret;

}


#include "core/zg_core.h"

/*
CVirtualMachine::tAleObjectInfo  	CVirtualMachine::stack[VM_LOCAL_VAR_MAX_STACK];
int 					CVirtualMachine::stkInteger[VM_ALE_OPERATIONS_MAX_STACK];
float 					CVirtualMachine::stkNumber[VM_ALE_OPERATIONS_MAX_STACK];
bool					CVirtualMachine::stkBoolean[VM_ALE_OPERATIONS_MAX_STACK];
string 					CVirtualMachine::stkString[VM_ALE_OPERATIONS_MAX_STACK];
CScriptVariable *			CVirtualMachine::stkLocalVar[VM_ALE_OPERATIONS_MAX_STACK];

//CScriptVariable  **stkResultObject[MAX_PER_TYPE_OPERATIONS]; // all variables references to this ...
//CVector	 * vector[MAX_OPERANDS];

int CVirtualMachine::idxStkCurrentInteger=0;
int CVirtualMachine::idxStkCurrentNumber=0;
int CVirtualMachine::idxStkCurrentBoolean=0;
int CVirtualMachine::idxStkCurrentString=0;
int CVirtualMachine::idxStkCurrentLocalVar=0;

CVirtualMachine::tAleObjectInfo CVirtualMachine::stkResultInstruction[VM_ALE_OPERATIONS_MAX_STACK];


CVirtualMachine::tAleObjectInfo *CVirtualMachine::basePtrLocalVar=NULL;
int CVirtualMachine::idxStkCurrentLocalVar=0;
*/

void CVirtualMachine::pushStack(tInfoRegisteredFunctionSymbol *info_function, vector<CScriptVariable *> * argv){



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
		basePtrLocalVar[i].type = INS_TYPE_UNDEFINED;
		basePtrLocalVar[i].stkResultObject=CScriptVariable::UndefinedSymbol;

	}


	// init argv vars ...
	for(unsigned i = 0; i < n_arg_size; i++){

		basePtrLocalVar[n_local_vars+i].ptrObjectRef = NULL;
		basePtrLocalVar[n_local_vars+i].type = INS_TYPE_UNDEFINED;
		basePtrLocalVar[n_local_vars+i].stkResultObject=CScriptVariable::UndefinedSymbol;

		/*if(i < argv->size()){
			basePtrLocalVar[n_local_vars+i].stkResultObject=argv->at(i);
			basePtrLocalVar[n_local_vars+i].type = INS_TYPE_UNDEFINED;
			basePtrLocalVar[n_local_vars+i].ptrObjectRef = NULL;
		}*/
	}

	CVirtualMachine::vecIdxLocalVar.push(n_total_vars);
	CVirtualMachine::idxStkCurrentLocalVar+=n_total_vars;

	// save current aux vars ...
	vecIdxStkNumber.push(startIdxStkNumber);
	vecIdxStkString.push(startIdxStkString);
	vecIdxStkResultInstruction.push(startIdxStkResultInstruction);



	startIdxStkNumber=idxStkCurrentNumber;
	startIdxStkString=idxStkCurrentString;
	startIdxStkResultInstruction=idxStkCurrentResultInstruction+1;



}

void CVirtualMachine::popStack(){

	// unref 1st scope ...

	CVirtualMachine::idxStkCurrentLocalVar-=CVirtualMachine::vecIdxLocalVar.top();
	CVirtualMachine::vecIdxLocalVar.pop();


	if(!CVirtualMachine::vecIdxLocalVar.empty()){


		basePtrLocalVar=&stack[CVirtualMachine::idxStkCurrentLocalVar-CVirtualMachine::vecIdxLocalVar.top()];

		// restore last current instruction...
		idxStkCurrentResultInstruction=startIdxStkResultInstruction-1;

		// save current aux vars ...
		startIdxStkNumber = vecIdxStkNumber.top();
		startIdxStkString=vecIdxStkString.top();
		startIdxStkResultInstruction=vecIdxStkResultInstruction.top();



		vecIdxStkNumber.pop();
		vecIdxStkString.pop();
		vecIdxStkResultInstruction.pop();


	}/*else{
		print_error_cr("stack is already empty");
	}*/


}


// general
/*
#define CHECK_VALID_INDEXES \
if(!(index_op1 >= 0 && index_op1 <=idxStkCurrentResultInstruction)) { print_error_cr("instruction 1 out of bounds"); return false;} \
if(!(index_op2 >= 0 && index_op2 <=idxStkCurrentResultInstruction)) { print_error_cr("instruction 2 out of bounds"); return false;} \
if(!(index_op2 >= index_op1 )) { print_error_cr("invalid indexes"); return false;}
*/

#define PRINT_DUAL_ERROR_OP(c)\
string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp1),\
	   var_type2=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(ptrResultInstructionOp2);\
\
print_error_cr("Error at line %i cannot perform operator \"%s\" %c  \"%s\"",\
		instruction->ast_node->definedValueline,\
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
#define IS_NUMBER(ptr_result_instruction) \
(ptr_result_instruction->type == INS_TYPE_NUMBER)


#define IS_INT(ptr_result_instruction) \
(ptr_result_instruction->type == INS_TYPE_INTEGER)


#define IS_STRING(ptr_result_instruction) \
(ptr_result_instruction->type == INS_TYPE_STRING)

#define IS_BOOLEAN(ptr_result_instruction) \
(ptr_result_instruction->type == INS_TYPE_BOOLEAN)

#define IS_UNDEFINED(ptr_result_instruction) \
(ptr_result_instruction->type == INS_TYPE_UNDEFINED)

#define IS_FUNCTION(ptr_result_instruction) \
(ptr_result_instruction->type == INS_TYPE_FUNCTION)

#define IS_VAR(ptr_result_instruction) \
(ptr_result_instruction->type == INS_TYPE_VAR)

#define IS_VECTOR(ptr_result_instruction) \
(( ptr_result_instruction->type == INS_TYPE_VAR) &&\
 (((CScriptVariable *)(ptr_result_instruction->stkResultObject))->getIdxClass()==CScriptClassFactory::getInstance()->getIdxClassVector()))

#define IS_GENERIC_NUMBER(ptr_result_instruction) \
((ptr_result_instruction->type == INS_TYPE_INTEGER) ||\
(ptr_result_instruction->type == INS_TYPE_NUMBER))


#define OP1_AND_OP2_ARE_NUMBERS \
(IS_GENERIC_NUMBER(ptrResultInstructionOp1) && IS_GENERIC_NUMBER(ptrResultInstructionOp2))

#define OP1_IS_STRING_AND_OP2_IS_NUMBER \
(ptrResultInstructionOp1->type == INS_TYPE_STRING) && \
IS_GENERIC_NUMBER(ptrResultInstructionOp2)

#define OP1_IS_NUMBER_AND_OP2_IS_STRING \
(ptrResultInstructionOp2->type == INS_TYPE_STRING) && \
IS_GENERIC_NUMBER(ptrResultInstructionOp1)

#define OP1_IS_STRING_AND_OP2_IS_BOOLEAN \
(ptrResultInstructionOp1->type == INS_TYPE_STRING) && \
(ptrResultInstructionOp2->type == INS_TYPE_BOOLEAN)


#define OP1_AND_OP2_ARE_BOOLEANS \
(ptrResultInstructionOp1->type == INS_TYPE_BOOLEAN) && \
(ptrResultInstructionOp2->type == INS_TYPE_BOOLEAN)

#define OP1_AND_OP2_ARE_STRINGS \
(ptrResultInstructionOp1->type == INS_TYPE_STRING) && \
(ptrResultInstructionOp2->type == INS_TYPE_STRING)

#define OP1_AND_OP2_ARE_UNDEFINED \
		(ptrResultInstructionOp1->stkResultObject == CScriptVariable::UndefinedSymbol) && \
		(ptrResultInstructionOp2->stkResultObject == CScriptVariable::UndefinedSymbol)

#define OP1_AND_OP2_ARE_NULL \
		(ptrResultInstructionOp1->stkResultObject == CScriptVariable::NullSymbol) && \
		(ptrResultInstructionOp2->stkResultObject == CScriptVariable::NullSymbol)



#define OP1_OR_OP2_IS_UNDEFINED \
		(ptrResultInstructionOp1->stkResultObject == CScriptVariable::UndefinedSymbol) || \
		(ptrResultInstructionOp2->stkResultObject == CScriptVariable::UndefinedSymbol)

#define OP1_OR_OP2_ARE_NULL \
		(ptrResultInstructionOp1->stkResultObject == CScriptVariable::NullSymbol) || \
		(ptrResultInstructionOp2->stkResultObject == CScriptVariable::NullSymbol)




#define PROCESS_NUM_OPERATION(__OVERR_OP__)\
					if (IS_INT(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){\
						if(!pushInteger(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2))){\
							return false;\
						}\
					}else if (IS_INT(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){\
						if(!pushInteger(LOAD_INT_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_NUMBER_OP(ptrResultInstructionOp2))) {\
							return false;\
						}\
					}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){\
						if(!pushNumber(LOAD_NUMBER_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_INT_OP(ptrResultInstructionOp2))) {\
							return false;\
						}\
					}else {\
						if(!pushNumber(LOAD_NUMBER_OP(ptrResultInstructionOp1) __OVERR_OP__ LOAD_NUMBER_OP(ptrResultInstructionOp2))) {\
							return false;\
						}\
					}


string CVirtualMachine::STR_GET_TYPE_VAR_INDEX_INSTRUCTION(tAleObjectInfo *ptr_info_ale){
	string result="undefined";
	if(IS_INT(ptr_info_ale))
		result= "int";
	else if(IS_NUMBER(ptr_info_ale))
		result= "number";
	else if(IS_BOOLEAN(ptr_info_ale))
		result= "bool";
	else if(IS_STRING(ptr_info_ale))
		result= "string";
	else if(IS_FUNCTION(ptr_info_ale))
		result= "function";
	else if(IS_VAR(ptr_info_ale)){
		result=((CScriptVariable *)ptr_info_ale->stkResultObject)->getClassName();
	}


	return result;

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




bool CVirtualMachine::pushInteger(int  init_value, CScriptVariable ** ptrAssignable){

	stkResultInstruction[idxStkCurrentResultInstruction]={INS_TYPE_INTEGER,(void*)init_value,ptrAssignable};

	return true;
}

bool CVirtualMachine::pushBoolean(bool init_value, CScriptVariable ** ptrAssignable, int n_stk){


	stkResultInstruction[idxStkCurrentResultInstruction]={INS_TYPE_BOOLEAN,(void *)init_value,ptrAssignable};

	return true;
}

bool CVirtualMachine::pushNumber(float init_value, CScriptVariable ** ptrAssignable){
	if(idxStkCurrentNumber ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max number operations");
		return false;
	}

	stkNumber[idxStkCurrentNumber]=init_value;
	stkResultInstruction[idxStkCurrentResultInstruction]={INS_TYPE_NUMBER,&stkNumber[idxStkCurrentNumber],ptrAssignable};
	idxStkCurrentNumber++;

	return true;
}

bool CVirtualMachine::pushString(const string & init_value, CScriptVariable ** ptrAssignable){
	if(idxStkCurrentString ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max string operations");
		return false;
	}


	stkString[idxStkCurrentString]=init_value;
	stkResultInstruction[idxStkCurrentResultInstruction]={INS_TYPE_STRING,&stkString[idxStkCurrentString],ptrAssignable};
	idxStkCurrentString++;

	return true;

}

bool CVirtualMachine::pushFunction(tInfoRegisteredFunctionSymbol * init_value, CScriptVariable ** ptrAssignable){

	stkResultInstruction[idxStkCurrentResultInstruction]={INS_TYPE_FUNCTION,init_value,ptrAssignable};
	return true;
}



bool CVirtualMachine::pushVar(CScriptVariable * init_value, CScriptVariable ** ptrObjectRef, bool is_new_var){

	if(init_value == NULL){
		return false;
	}

	int idxClass = init_value->getIdxClass();


	// finally assign the value ...
	if(!is_new_var){
		if(idxClass == CScriptClassFactory::getInstance()->getIdxClassInteger()){
			return pushInteger(*((int *)((CInteger *)init_value)->m_value),ptrObjectRef);
		}else if(idxClass == CScriptClassFactory::getInstance()->getIdxClassNumber()){
			return pushNumber(*((float *)((CNumber *)init_value)->m_value),ptrObjectRef);
		}else if(idxClass == CScriptClassFactory::getInstance()->getIdxClassString()){
			return pushString(*((string *)((CString *)init_value)->m_value),ptrObjectRef);
		}else if(idxClass == CScriptClassFactory::getInstance()->getIdxClassBoolean()){
			return pushBoolean(*((bool *)((CBoolean *)init_value)->m_value),ptrObjectRef);
		}else if(idxClass == CScriptClassFactory::getInstance()->getIdxClassFunctor()){
			return pushFunction((tInfoRegisteredFunctionSymbol *)(((CFunctor *)init_value)->m_value),ptrObjectRef);
		}else{
			is_new_var=true;
		}
	}

	if(is_new_var){
		if(idxStkCurrentLocalVar ==VM_ALE_OPERATIONS_MAX_STACK){
			print_error_cr("Reached max object operations");
			return false;
		}

		stkResultInstruction[idxStkCurrentResultInstruction]={INS_TYPE_VAR,init_value, ptrObjectRef};
	}




	return true;

}


CScriptVariable * CVirtualMachine::createVarFromResultInstruction(tAleObjectInfo * ptr_instruction){
	CScriptVariable *obj = NULL;
bool created_pointer = false;

	// check second operand valid object..
	switch(ptr_instruction->type){
	default:
		print_error_cr("(internal) cannot determine var type");
		return NULL;
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_UNDEFINED:
		obj=CScriptVariable::UndefinedSymbol;
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_INTEGER:
		obj= NEW_INTEGER_VAR;
		*((int *)(((CScriptVariable *)obj)->m_value)) = ((int)(ptr_instruction->stkResultObject));
		created_pointer = true;
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_NUMBER:
		obj= NEW_NUMBER_VAR;
		*((float *)(((CScriptVariable *)obj)->m_value)) = *((float *)(ptr_instruction->stkResultObject));
		created_pointer = true;
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_STRING:
		obj= NEW_STRING_VAR;
		*((string *)(((CScriptVariable *)obj)->m_value)) = *((string *)(ptr_instruction->stkResultObject));
		created_pointer = true;
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_BOOLEAN:
		obj= NEW_BOOLEAN_VAR;
		*((bool *)(((CScriptVariable *)obj)->m_value)) = ((bool)(ptr_instruction->stkResultObject));
		created_pointer = true;
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_FUNCTION:
		obj = NEW_FUNCTOR_VAR(((tInfoRegisteredFunctionSymbol *)ptr_instruction->stkResultObject));
		created_pointer = true;
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_VAR:
		obj = (CScriptVariable *)ptr_instruction->stkResultObject;
		break;

	}

	if(created_pointer){
		if((obj->idx_shared_ptr = CSharedPointerManager::getInstance()->newSharedPointer(obj)) == -1){
			return NULL;
		}
	}

	return obj;
}


bool CVirtualMachine::loadVariableValue(tInfoAsmOp *iao,
		tInfoRegisteredFunctionSymbol *info_function,
		CScriptVariable *this_object,
		vector<tInfoAsmOp *> *asm_op,
		int n_stk){

	if(iao->index_op1 != LOAD_TYPE_VARIABLE){
		print_error_cr("expected load type variable.");
		return false;
	}

	//CScriptVariable *this_object = function_object->getThisObject();
	CScriptVariable::tSymbolInfo *si;
	CScriptVariable **ptr_var_object=NULL;
	CScriptVariable *var_object = NULL;

	int start_index_local_var = 0;//info_function->m_arg.size();

	bool push_object=false;
	bool is_valid_variable = true;
	//bool function_struct_type = false;



	switch(iao->scope_type){
	default:
		print_error_cr("unknow scope type %i",iao->scope_type);
		return false;
		break;


		break;
	case SCOPE_TYPE::ACCESS_SCOPE: // needs load previous object...
	case SCOPE_TYPE::THIS_SCOPE: // fast


		if(iao->scope_type == SCOPE_TYPE::ACCESS_SCOPE){


			CScriptVariable * base_var = *((CScriptVariable **)stkResultInstruction[iao->index_op2+startIdxStkResultInstruction].ptrObjectRef);

			if((si = base_var->getVariableSymbol(iao->ast_node->value_symbol))==NULL){
				print_error_cr("Line %i: Variable \"%s\" as type \"%s\" has not symbol \"%s\"",iao->ast_node->definedValueline,asm_op->at(iao->index_op2)->ast_node->value_symbol.c_str(),base_var->getClassName().c_str(), iao->ast_node->value_symbol.c_str());
				return false;
			}
		}
		else{
			// get var from object ...
			if((si = this_object->getVariableSymbolByIndex(iao->index_op2))==NULL){
				print_error_cr("cannot find symbol \"this.%s\"",iao->ast_node->value_symbol.c_str());
				return false;
			}
		}

		ptr_var_object = (CScriptVariable **)(&si->object);
		var_object = (CScriptVariable *)(si->object);

		break;

	case SCOPE_TYPE::GLOBAL_SCOPE:

		ptr_var_object = (CScriptVariable **)(&CVirtualMachine::stack[iao->index_op2].stkResultObject);
		var_object = (CScriptVariable *)(CVirtualMachine::stack[iao->index_op2].stkResultObject);
		break;

	case SCOPE_TYPE::LOCAL_SCOPE:

		if(iao->index_op2 >=  CVirtualMachine::idxStkCurrentLocalVar){
			print_error_cr("internal error: index out of stack");
			return false;
		}

		// get var from base stack ...
		ptr_var_object = (CScriptVariable **)(&CVirtualMachine::basePtrLocalVar[start_index_local_var+iao->index_op2].stkResultObject);
		var_object = (CScriptVariable *)(CVirtualMachine::basePtrLocalVar[start_index_local_var+iao->index_op2].stkResultObject);


		/*if((si = this_object->getVariableSymbolByIndex(iao->index_op2))==NULL){
			print_error_cr("cannot find symbol \"%s\"",iao->ast_node->value_symbol.c_str());

			return false;
		}*/

		break;


	}


	is_valid_variable = (var_object != CScriptVariable::UndefinedSymbol);// && (!function_struct_type);

	push_object = true;


	if(is_valid_variable){

		int idx_class =  var_object->getIdxClass();

		//CScriptVariable **ptr_var_object = (CScriptVariable **)(&si->object);
		//CScriptVariable *var_object = (CScriptVariable *)(si->object);
		if(iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::PRE_DEC || iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::PRE_INC){

			/*if(!performPreOperator(iao->pre_post_operator_type, var_object)){
				return false;
			}*/
			if(idx_class==CScriptClassFactory::getInstance()->getIdxClassInteger()){
				if(iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::PRE_INC)
					(*((int *)((CInteger *)var_object)->m_value))++;
				else //dec
					(*((int *)((CInteger *)var_object)->m_value))--;

			}else if(idx_class == CScriptClassFactory::getInstance()->getIdxClassNumber()){
				if(iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::PRE_INC)
					(*((float *)((CNumber *)var_object)->m_value))++;
				else // dec
					(*((float *)((CNumber *)var_object)->m_value))--;

			}else{

				print_error_cr("internal error:Cannot perform preoperator ?? because is not number");
				return false;
			}
		}

		if(iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::POST_DEC || iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::POST_INC){

			push_object = false; // first will push the value and after will increment ...
			// 1. Load value as constant value
			//if(!loadConstantValue(var_object,n_stk)){
			if(CScriptClassFactory::getInstance()->getIdxClassInteger() == idx_class){
				// 1. first load ...
				pushInteger(*((int *)((CInteger *)var_object)->m_value));
				// 2. increment ...
				if(iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::POST_INC)
					(*((int *)((CInteger *)var_object)->m_value))++;
				else
					(*((int *)((CInteger *)var_object)->m_value))--;
			}else if(CScriptClassFactory::getInstance()->getIdxClassNumber() == var_object->getIdxClass()){
				// 1. first load ...
				pushNumber(*((float *)((CNumber *)var_object)->m_value));
				// 2. increment ...
				if(iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::POST_INC)
					(*((float *)((CNumber *)var_object)->m_value))++;
				else
					(*((float *)((CNumber *)var_object)->m_value))--;
			}else{
				print_error_cr("internal error:cannot postoperator ?? because is not number");
				return false;
			}

			// 2. then perform post operation ...
			/*if(!performPostOperator(iao->pre_post_operator_type, var_object)){
				return false;
			}*/

		}
	}


	// generic object pushed ...
	if(push_object){

		if(iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::PRE_NEG){

			int idx_class = var_object->getIdxClass();
			if(idx_class==CScriptClassFactory::getInstance()->getIdxClassInteger()){
				return pushInteger(-(*((int *)((CInteger *)var_object)->m_value)));
			}else if(idx_class==CScriptClassFactory::getInstance()->getIdxClassNumber()){
				return pushNumber(-(*((float *)((CNumber *)var_object)->m_value)));
			}else{
				print_error_cr("internal error:cannot perform pre operator - because is not number");
				return false;
			}
		}else{
			return pushVar(var_object,ptr_var_object);
		}
	}

	return true;
}

bool CVirtualMachine::loadFunctionValue(tInfoAsmOp *iao,
		tInfoRegisteredFunctionSymbol *local_function,
		CScriptVariable *this_object,
		vector<tInfoAsmOp *> *asm_op,
		int n_stk){

	if(iao->index_op1 != LOAD_TYPE_FUNCTION){
		print_error_cr("expected load type function.");
		return false;
	}


	tInfoRegisteredFunctionSymbol *info_function=NULL;
	vector<tInfoRegisteredFunctionSymbol> *vec_global_functions;

	CScriptVariable::tSymbolInfo *si;
	CScriptVariable ** calling_object = NULL;

	//CScriptVariable *var_object = NULL;
	//tInfoRegisteredFunctionSymbol *info_function = (tInfoRegisteredFunctionSymbol *)(si->object);
	//CScriptVariable *this_object = function_object->getThisObject();
	//tInfoRegisteredFunctionSymbol *si;

	/*if((si = this_object->getFunctionSymbol(iao->index_op2))==NULL){
		print_error_cr("cannot find function info \"%s\"",iao->ast_node->value_symbol.c_str());
		return false;
	}*/

	switch(iao->scope_type){
	default:
		print_error_cr("unknow scope type");
		return false;
		break;
	case SCOPE_TYPE::ACCESS_SCOPE:
	case SCOPE_TYPE::THIS_SCOPE:
	case SCOPE_TYPE::SUPER_SCOPE:

		// get var from object ...
		if(iao->scope_type == SCOPE_TYPE::ACCESS_SCOPE){
			calling_object = ((CScriptVariable **)stkResultInstruction[iao->index_op2+startIdxStkResultInstruction].ptrObjectRef);
			info_function = NULL; // TODO: always get symbol in CALL op. Make a way to do it optimized!

		}else if(iao->scope_type == SCOPE_TYPE::THIS_SCOPE){
			if((si = this_object->getFunctionSymbolByIndex(iao->index_op2))==NULL){
				print_error_cr("cannot find function \"this.%s\"",iao->ast_node->value_symbol.c_str());
				return false;
			}
		}else{ // super scope ?
			if((si = this_object->getFunctionSymbolByIndex(iao->index_op2))==NULL){
				print_error_cr("cannot find function \"super.%s\"",iao->ast_node->value_symbol.c_str());
				return false;
			}
		}

		if(iao->scope_type != SCOPE_TYPE::ACCESS_SCOPE){
			info_function =(tInfoRegisteredFunctionSymbol *)si->object;
		}

		break;
	case SCOPE_TYPE::GLOBAL_SCOPE:
		vec_global_functions = &CZG_ScriptCore::getInstance()->getMainStructInfo()->object_info.local_symbols.m_registeredFunction;

		if(iao->index_op2 == -1){ // is will be processed after in CALL instruction ...
			info_function= NULL;
		}else{

			if((iao->index_op2<(int)vec_global_functions->size()))
			{
				info_function =&(*vec_global_functions)[iao->index_op2];
			}
			else{
				print_error_cr("cannot find symbol global \"%s\"",iao->ast_node->value_symbol.c_str());
				return false;

			}
		}


		break;
	case SCOPE_TYPE::LOCAL_SCOPE:
		info_function = &local_function->object_info.local_symbols.m_registeredFunction[iao->index_op2];
		break;


	}

	// generic object pushed ...
	if(!pushFunction(info_function,calling_object != NULL? calling_object:NULL)) {
		return false;
	}
	//stkResultInstruction[idxStkCurrentResultInstruction]={CScriptVariable::FUNCTION,(CScriptVariable **)si, false};


	return true;
}

bool CVirtualMachine::loadConstantValue(CCompiler::tInfoConstantValue *info_constant, int n_stk){



	switch(info_constant->type){
		default:
			print_error_cr("Invalid load constant value as %i",info_constant->type);
			return false;
			break;
		case INS_TYPE_INTEGER:
			if(!pushInteger(*((int *)info_constant->ptr))) return false;
			break;
		case INS_TYPE_BOOLEAN:
			if(!pushBoolean(*((bool *)info_constant->ptr))) return false;
			break;
		case INS_TYPE_STRING:
			if(!pushString((*((string *)info_constant->ptr)))) return false;
			break;
		case INS_TYPE_NUMBER:
			if(!pushNumber(*((float *)info_constant->ptr))) return false;
			break;
		}

		return true;


}





bool CVirtualMachine::assignVarFromResultInstruction(CScriptVariable **var, tAleObjectInfo  * ptr_instruction){

	CScriptVariable *aux_var=NULL;


	// if undefined, create new by default ...
	if(*var == NULL){

		if((*var = createVarFromResultInstruction(ptr_instruction)) == NULL){
			return false;
		}
	}

	//int idxClass=-1;

	//if(*var!=CScriptVariable::UndefinedSymbol){
	int idxClass = (*var)->getIdxClass();
	//}

	//tInfoRegisteredFunctionSymbol * init_value;
	bool create_from_index=false;

	// finally assign the value ...

	/*if(idxClass == CScriptClassFactory::getInstance()->getIdxClassInteger()){

	}*/


	switch(ptr_instruction->type){


		case INS_TYPE_NULL:
			*var = CScriptVariable::NullSymbol;
			break;
		case INS_TYPE_UNDEFINED:
			*var = CScriptVariable::UndefinedSymbol;
			break;

		case INS_TYPE_INTEGER:
			if(idxClass == CScriptClassFactory::getInstance()->getIdxClassInteger()){
				*((int *)((CInteger *)(*var))->m_value)=((int)(ptr_instruction->stkResultObject));

			}else
			{
				create_from_index=true;
			}
			break;
		case INS_TYPE_NUMBER:


			if(idxClass == CScriptClassFactory::getInstance()->getIdxClassNumber()){
				*((float *)((CNumber *)(*var))->m_value) = *((float *)(ptr_instruction->stkResultObject));
			}else
			{
				create_from_index=true;
			}
			break;
		case INS_TYPE_STRING:

			if(idxClass == CScriptClassFactory::getInstance()->getIdxClassString()){
				*((string *)((CString  *)(*var))->m_value)= *((string *)(ptr_instruction->stkResultObject));
			}else
			{
				create_from_index=true;
			}

			break;
		case INS_TYPE_BOOLEAN:
			if(idxClass == CScriptClassFactory::getInstance()->getIdxClassBoolean()){
				*((bool *)((CBoolean  *)aux_var)->m_value)= ((bool)(ptr_instruction->stkResultObject));
			}else
			{
				create_from_index=true;
			}
			break;
		case INS_TYPE_FUNCTION: // function object

			if(idxClass == CScriptClassFactory::getInstance()->getIdxClassFunctor()){
				((CFunctor  *)aux_var)->m_value= ((tInfoRegisteredFunctionSymbol *)(ptr_instruction->stkResultObject));
			}else{
				create_from_index=true;
			}
			break;
		case INS_TYPE_VAR: // generic object, assign pointer ...


			// unref pointer ...
			if((*var)->idx_shared_ptr != -1){
				CSharedPointerManager::getInstance()->unrefSharedPointer((*var)->idx_shared_ptr);
			}

			*var = (CScriptVariable *)(ptr_instruction->stkResultObject);

			if(*var != CScriptVariable::NullSymbol && *var != CScriptVariable::UndefinedSymbol){
				// ref the new pointer ...
				CSharedPointerManager::getInstance()->sharePointer((*var)->idx_shared_ptr);
			}

			break;

		default:
				print_error_cr("internal error: unknow assignment %i!",ptr_instruction->type);
				return false;
				break;
		}

	if(create_from_index){
		// unref pointer ...
		if((*var)->idx_shared_ptr != -1){
			CSharedPointerManager::getInstance()->unrefSharedPointer((*var)->idx_shared_ptr);
		}


		if((*var = createVarFromResultInstruction(ptr_instruction)) == NULL){
			return false;
		}

		CSharedPointerManager::getInstance()->sharePointer((*var)->idx_shared_ptr);


	}


	return true;
}


void CVirtualMachine::popScope(tInfoRegisteredFunctionSymbol *info_function,int index)
{
	if(index < 0){
		print_error_cr("index < 0");
		return;
	}

	if(index >= (int)info_function->object_info.info_var_scope.size()){
		print_error_cr("index >= info_function->object_info.info_var_scope.size()");
		return;
	}

	for(unsigned i = 0; i < info_function->object_info.info_var_scope[index].var_index.size(); i++){
		int idx_local_var = info_function->object_info.info_var_scope[index].var_index[i];

		CScriptVariable *var =((CScriptVariable *)(basePtrLocalVar[idx_local_var].stkResultObject));

		if(var != CScriptVariable::UndefinedSymbol && var !=  CScriptVariable::NullSymbol){

			CSharedPointerManager::getInstance()->unrefSharedPointer(var->idx_shared_ptr);
		}

		basePtrLocalVar[idx_local_var].stkResultObject = CScriptVariable::UndefinedSymbol;
	}
}

bool CVirtualMachine::performInstruction(
		tInfoAsmOp * instruction,
		int & jmp_to_statment,
		int & jmp_to_instruction,
		tInfoRegisteredFunctionSymbol *info_function,
		CScriptVariable *this_object,vector<CScriptVariable *> * argv,
		vector<tInfoAsmOp *> *asm_op,
		int n_stk){


	string 	aux_string;
	bool	aux_boolean;
	//string symbol;
	CScriptVariable **obj=NULL;
	tInfoRegisteredFunctionSymbol * aux_function_info=NULL;
	CScriptVariable *ret_obj, *svar;
	tInfoRegisteredFunctionSymbol *constructor_function;
	CScriptVariable *calling_object = this_object;
	int n_local_vars =  info_function->object_info.local_symbols.m_registeredVariable.size();


	jmp_to_statment=-1;

	//idxStkCurrentResultInstruction = idx_instruction;

	//CScopeInfo *_lc = instruction->ast_node->scope_info_ptr;

	int index_op1 = instruction->index_op1;
	int index_op2 = instruction->index_op2;
	tAleObjectInfo *ptrResultInstructionOp1=&stkResultInstruction[index_op1+startIdxStkResultInstruction];
	tAleObjectInfo *ptrResultInstructionOp2=&stkResultInstruction[index_op2+startIdxStkResultInstruction];
	tAleObjectInfo *ptrResultLastInstruction=&stkResultInstruction[idxStkCurrentResultInstruction-1];






	switch(instruction->operator_type){
	default:
		print_error_cr("operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
		return false;
		break;
	case NOP: // ignore ...
		break;
	case LOAD: // load value in function of value/constant ...
		/*if(!loadValue(instruction, n_stk)){
			return false;
		}*/
		//sprintf(print_aux_load_value,"UNDEFINED");
		switch(instruction->index_op1){
		case LOAD_TYPE::LOAD_TYPE_NULL:
			if(!pushVar(CScriptVariable::NullSymbol)) return false;

			break;

		case LOAD_TYPE::LOAD_TYPE_UNDEFINED:
			if(!pushVar(CScriptVariable::UndefinedSymbol)) return false;

			break;

		case LOAD_TYPE::LOAD_TYPE_CONSTANT:

			if(!loadConstantValue((CCompiler::tInfoConstantValue *)instruction->index_op2, n_stk)){
				return false;
			}

			//sprintf(print_aux_load_value,"CONST(%s)",value_symbol.c_str());
			break;
		case LOAD_TYPE::LOAD_TYPE_VARIABLE:

			if(!loadVariableValue(instruction, info_function,this_object,asm_op, n_stk)){
				return false;
			}

			break;
		case LOAD_TYPE::LOAD_TYPE_FUNCTION:
			if(!loadFunctionValue(instruction,info_function, this_object,asm_op, n_stk)){
				return false;
			}

			break;
		case LOAD_TYPE::LOAD_TYPE_ARGUMENT:

			//if(argv!=NULL){
				//if(index_op2<(int)argv->size())
				//{
					//CScriptVariable *var=basePtrLocalVar[index_op2].stkResultObject;//(*argv)[index_op2];
					if(!pushVar((CScriptVariable *)basePtrLocalVar[index_op2+n_local_vars].stkResultObject,NULL)){
						return false;
					}
				//}else{
				//	print_error_cr("index out of bounds");
				//	return false;
				//}
			//}
			//else{
			//	print_error_cr("argv null");
			//	return false;
			//}

			//sprintf(print_aux_load_value,"ARG(%s)",value_symbol.c_str());
			break;
		default:
			print_error_cr("no load defined type");
			return false;
			break;

		}

		break;
		case MOV: // mov value expression to var

			// ok load object pointer ...
			if((obj = ptrResultInstructionOp1->ptrObjectRef) != NULL) {// == CScriptVariable::VAR_TYPE::OBJECT){

				// get pointer object (can be assigned)
				//obj = stkResultInstruction[index_op1].stkResultObject;



				if(!assignVarFromResultInstruction(ptrResultInstructionOp1->ptrObjectRef,ptrResultInstructionOp2))
						return false;

			}else{
				print_error_cr("Expected object l-value mov");
				return false;
			}

			break;
		case EQU:  // == --> boolean && boolean or string && string or number && number

			if(OP1_AND_OP2_ARE_BOOLEANS) {
				if(!pushBoolean(LOAD_BOOL_OP(ptrResultInstructionOp1) == LOAD_BOOL_OP(ptrResultInstructionOp2))) return false;
			}else if(OP1_AND_OP2_ARE_STRINGS){
				if(!pushBoolean(LOAD_STRING_OP(ptrResultInstructionOp1) == LOAD_STRING_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_INT(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) == LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_INT(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) == LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) == LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) == LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else if(OP1_AND_OP2_ARE_UNDEFINED){
				if(!pushBoolean(true)) return false;
			}else if(OP1_AND_OP2_ARE_NULL){
				if(!pushBoolean(true)) return false;
			}else if(OP1_OR_OP2_IS_UNDEFINED){
				if(!pushBoolean(false)) return false;
			}else if(OP1_OR_OP2_IS_UNDEFINED){
				if(!pushBoolean(false)) return false;

			}else{
				print_error_cr("Expected both operands as string, number or boolean!");
				return false;
			}

			break;

		case NOT_EQU:  // == --> boolean && boolean or string && string or number && number

			if(OP1_AND_OP2_ARE_BOOLEANS) {
				if(!pushBoolean(LOAD_BOOL_OP(ptrResultInstructionOp1) != LOAD_BOOL_OP(ptrResultInstructionOp2))) return false;
			}else if(OP1_AND_OP2_ARE_STRINGS){
				if(!pushBoolean(LOAD_STRING_OP(ptrResultInstructionOp1) != LOAD_STRING_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_INT(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) != LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_INT(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) != LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) != LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) != LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Expected both operands as string, number or boolean!");
				return false;
			}

			break;
		case LT:  // <
			if (IS_INT(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) < LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_INT(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) < LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) < LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) < LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}
			break;
		case LTE:  // <=

			if (IS_INT(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) <= LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_INT(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) <= LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) <= LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) <= LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}

			break;
		case NOT: // !
			if (ptrResultInstructionOp1->type == INS_TYPE_BOOLEAN){
				if(!pushBoolean(!LOAD_BOOL_OP(ptrResultInstructionOp1))) return false;
			}else{
				print_error_cr("Expected operands 1 as boolean!");
				return false;
			}
			break;
		case NEG: // !
			if (IS_GENERIC_NUMBER(ptrResultInstructionOp1)){
				if(ptrResultInstructionOp1->type == INS_TYPE_INTEGER){ // operation will result as integer.
					if(!pushInteger(-LOAD_INT_OP(ptrResultInstructionOp1))) {
						return false;
					}
				}
				else{
					if(!pushNumber(-LOAD_NUMBER_OP(ptrResultInstructionOp2))){
						return false;
					}
				}

			}else{
					print_error_cr("Line %i:Expected operands 1 as number or integer!",instruction->ast_node->definedValueline);
					return false;
			}
			break;

		case GT:  // >
			if (IS_INT(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) > LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_INT(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) > LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) > LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) > LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Line %i:Expected both operands as number!",instruction->ast_node->definedValueline);
				return false;
			}
			break;
		case GTE: // >=
			if (IS_INT(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) >= LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_INT(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_INT_OP(ptrResultInstructionOp1) >= LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) >= LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushBoolean(LOAD_NUMBER_OP(ptrResultInstructionOp1) >= LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Line %i:Expected both operands as number!",instruction->ast_node->definedValueline);
				return false;
			}
			break;

		case ADD: // +

			// get indexes and check whether is possible or not its calculation.
			// check indexes
			//CHECK_VALID_INDEXES;

			// check types ...
			if (IS_STRING(ptrResultInstructionOp1) && (IS_UNDEFINED(ptrResultInstructionOp2) || IS_VAR(ptrResultInstructionOp2))){

				string result = "undefined";
				if(IS_VAR(ptrResultInstructionOp2)){
					result = ((CScriptVariable *)(ptrResultInstructionOp2->stkResultObject))->getClassName();
				}

				if(!pushString(LOAD_STRING_OP(ptrResultInstructionOp1)+result)) return false;
			}else if (IS_INT(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushInteger(LOAD_INT_OP(ptrResultInstructionOp1) + LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_INT(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushInteger(LOAD_INT_OP(ptrResultInstructionOp1) + LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
				if(!pushNumber(LOAD_NUMBER_OP(ptrResultInstructionOp1) + LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
				if(!pushNumber(LOAD_NUMBER_OP(ptrResultInstructionOp1) + LOAD_NUMBER_OP(ptrResultInstructionOp2))) return false;
			}else if(OP1_IS_STRING_AND_OP2_IS_NUMBER){ // concatenate string + number

				aux_string =  LOAD_STRING_OP(ptrResultInstructionOp1);

				if(ptrResultInstructionOp2->type == INS_TYPE_INTEGER)
					aux_string = aux_string + CStringUtils::intToString(LOAD_INT_OP(ptrResultInstructionOp2));
				else
					aux_string = aux_string + CStringUtils::intToString(LOAD_NUMBER_OP(ptrResultInstructionOp2));

				if(!pushString(aux_string)) return false;
			}else if(OP1_IS_NUMBER_AND_OP2_IS_STRING){ // concatenate string + number

				aux_string =  LOAD_STRING_OP(ptrResultInstructionOp2);

				if(ptrResultInstructionOp1->type == INS_TYPE_INTEGER)
					aux_string = CStringUtils::intToString(LOAD_INT_OP(ptrResultInstructionOp1))+ aux_string;
				else
					aux_string = CStringUtils::intToString(LOAD_NUMBER_OP(ptrResultInstructionOp1))+aux_string;

				if(!pushString(aux_string)) return false;
			}else if(OP1_IS_STRING_AND_OP2_IS_BOOLEAN){ // concatenate string + boolean

				aux_string =  LOAD_STRING_OP(ptrResultInstructionOp1);
				aux_boolean =  LOAD_BOOL_OP(ptrResultInstructionOp2);

				if(aux_boolean)
					aux_string = aux_string + "true";
				else
					aux_string = aux_string + "false";

				if(!pushString(aux_string)) return false;

			}else if(OP1_AND_OP2_ARE_STRINGS){ // concatenate string + boolean

				if(!pushString(LOAD_STRING_OP(ptrResultInstructionOp1)+LOAD_STRING_OP(ptrResultInstructionOp2))) return false;

			}else{

				// full error description ...
				PRINT_DUAL_ERROR_OP('+');
				return false;
			}

			break;

		case LOGIC_AND: // &&
			if(OP1_AND_OP2_ARE_BOOLEANS) {
				if(!pushBoolean(LOAD_BOOL_OP(ptrResultInstructionOp1) && LOAD_BOOL_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Expected both operands boolean!");
				return false;
			}
			break;
		case LOGIC_OR:  // ||
			if(OP1_AND_OP2_ARE_BOOLEANS) {
				if(!pushBoolean(LOAD_BOOL_OP(ptrResultInstructionOp1) || LOAD_BOOL_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Expected both operands boolean!");
				return false;
			}
			break;
		case DIV: // /
			if(OP1_AND_OP2_ARE_NUMBERS) {

				if(IS_INT(ptrResultInstructionOp2)){
					if(LOAD_INT_OP(ptrResultInstructionOp2) == 0) {
						print_error_cr("Divide by 0 at line %i.",instruction->ast_node->definedValueline);
						return false;
					}
				}else{
					if(LOAD_NUMBER_OP(ptrResultInstructionOp2) == 0) {
						print_error_cr("Divide by 0 at line %i.",instruction->ast_node->definedValueline);
						return false;
					}
				}

				PROCESS_NUM_OPERATION(/);
			}else{
				PRINT_DUAL_ERROR_OP('/');
				return false;
			}

			break;
		case MUL: // *
			if(OP1_AND_OP2_ARE_NUMBERS) {
					PROCESS_NUM_OPERATION(*);

			}else{
				PRINT_DUAL_ERROR_OP('*');
				return false;
			}
			break;
		case MOD:  // %
			if(OP1_AND_OP2_ARE_NUMBERS) {

				if(IS_INT(ptrResultInstructionOp2)){
					if(LOAD_INT_OP(ptrResultInstructionOp2) == 0) {
						print_error_cr("Divide by 0 at line %i.",instruction->ast_node->definedValueline);
						return false;
					}
				}else{
					if(LOAD_NUMBER_OP(ptrResultInstructionOp2) == 0) {
						print_error_cr("Divide by 0 at line %i.",instruction->ast_node->definedValueline);
						return false;
					}
				}
				//PROCESS_NUM_OPERATION(%);

				if (IS_INT(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
					if(!pushInteger(LOAD_INT_OP(ptrResultInstructionOp1) % LOAD_INT_OP(ptrResultInstructionOp2))){
						return false;\
					}
				}else if (IS_INT(ptrResultInstructionOp1) && IS_NUMBER(ptrResultInstructionOp2)){
					if(!pushInteger(LOAD_INT_OP(ptrResultInstructionOp1) % ((int) LOAD_NUMBER_OP(ptrResultInstructionOp2)))) {
						return false;\
					}
				}else if (IS_NUMBER(ptrResultInstructionOp1) && IS_INT(ptrResultInstructionOp2)){
					if(!pushNumber(fmod(LOAD_NUMBER_OP(ptrResultInstructionOp1), LOAD_INT_OP(ptrResultInstructionOp2)))) {
						return false;
					}
				}else {
					if(!pushNumber(fmod(LOAD_NUMBER_OP(ptrResultInstructionOp1) , LOAD_NUMBER_OP(ptrResultInstructionOp2)))) {
						return false;
					}
				}

			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}

			break;
		case AND: // bitwise logic and
			if((ptrResultInstructionOp1->type == INS_TYPE_INTEGER) && (ptrResultInstructionOp2->type == INS_TYPE_INTEGER)){
				if(!pushInteger(LOAD_INT_OP(ptrResultInstructionOp1) & LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		case OR: // bitwise logic or
			if((ptrResultInstructionOp1->type == INS_TYPE_INTEGER) && (ptrResultInstructionOp2->type == INS_TYPE_INTEGER)){
				if(!pushInteger(LOAD_INT_OP(ptrResultInstructionOp1) | LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}

			break;
		case XOR: // logic xor
			if((ptrResultInstructionOp1->type == INS_TYPE_INTEGER) && (ptrResultInstructionOp2->type == INS_TYPE_INTEGER)){
				if(!pushInteger(LOAD_INT_OP(ptrResultInstructionOp1) ^ LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		case SHL: // shift left
			if((ptrResultInstructionOp1->type == INS_TYPE_INTEGER) && (ptrResultInstructionOp2->type == INS_TYPE_INTEGER)){
				if(!pushInteger(LOAD_INT_OP(ptrResultInstructionOp1) << LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		case SHR: // shift right
			if((ptrResultInstructionOp1->type == INS_TYPE_INTEGER) && (ptrResultInstructionOp2->type == INS_TYPE_INTEGER)){
				if(!pushInteger(LOAD_INT_OP(ptrResultInstructionOp1) >> LOAD_INT_OP(ptrResultInstructionOp2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		// special internal ops...
		case JMP:
			jmp_to_statment = index_op1;
			jmp_to_instruction = index_op2;
			break;
		case JNT: // goto if not true ... goes end to conditional.

			// load boolean var and jmp if true...
			if(idxStkCurrentResultInstruction > 0){

				if(ptrResultLastInstruction->type == INS_TYPE_BOOLEAN){

					if(!((bool)(ptrResultLastInstruction->stkResultObject))){
						jmp_to_statment = index_op1;
						jmp_to_instruction = index_op2;
					}
				}
			}else{
				print_error_cr("No boolean elements");
				return false;
			}
			break;
		case JT: // goto if true ... goes end to conditional.
			if(idxStkCurrentResultInstruction > 0){

				if(ptrResultLastInstruction->type == INS_TYPE_BOOLEAN){

					if(((bool)(ptrResultLastInstruction->stkResultObject))){
						jmp_to_statment = index_op1;
					}
				}
			}else{
				print_error_cr("No boolean elements");
				return false;
			}
			break;
		case CALL: // calling function after all of args are processed...

			// check whether signatures matches or not ...
			// 1. get function object ...
			aux_function_info=(tInfoRegisteredFunctionSymbol *)ptrResultInstructionOp1->stkResultObject;

			if(aux_function_info == NULL){ // we must find function ...
				tInfoAsmOp *iao = asm_op->at(instruction->index_op1);
				CScriptVariable::tSymbolInfo * si=NULL;
				CScriptVariable **script_var=NULL;

				if(iao->index_op2 == -1 || iao->scope_type == SCOPE_TYPE::ACCESS_SCOPE){


					vector<tInfoRegisteredFunctionSymbol> *vec_global_functions=&CZG_ScriptCore::getInstance()->getMainStructInfo()->object_info.local_symbols.m_registeredFunction;
					bool all_check=true;
					bool found = false;
					CScriptVariable * base_var=NULL;

					vector<CScriptVariable *> *argv = &m_functionArgs;


					switch(iao->scope_type){
					default:
						print_error_cr("unknow scope type");
						return false;

					case SCOPE_TYPE::ACCESS_SCOPE:

						script_var = (CScriptVariable **)ptrResultInstructionOp1->ptrObjectRef;////((CScriptVariable **)stkResultInstruction[iao->index_op2+startIdxStkResultInstruction].ptrObjectRef);
						base_var = *script_var;
						for(int h=0; h < 2 && !found; h++){
							int idx_function;
							if(h==0){
								idx_function=base_var->getIdxFunctionSymbolWithMatchArgs(iao->ast_node->value_symbol,argv, true);
							}else{
								idx_function=base_var->getIdxFunctionSymbolWithMatchArgs(iao->ast_node->value_symbol,argv, false);
							}

							if(idx_function != -1){
								si = base_var->getFunctionSymbolByIndex(idx_function);
								found = true;
							}

						}
						// get appropiate function  ...
						/*if((si = base_var->getFunctionSymbol(iao->ast_node->value_symbol))==NULL){
							print_error_cr("Line %i: Variable \"%s\" as type \"%s\" has not function \"%s\"",iao->ast_node->definedValueline,asm_op->at(iao->index_op2)->ast_node->value_symbol.c_str(),base_var->getClassName().c_str(), iao->ast_node->value_symbol.c_str());
							//print_error_cr("cannot find function \"%s\"",iao->ast_node->value_symbol.c_str());
							return false;
						}*/

						if(si != NULL){
							aux_function_info = (tInfoRegisteredFunctionSymbol *)si->object;
						}

						break;
					case SCOPE_TYPE::GLOBAL_SCOPE: // only for function (first time)

						// try first all function with no conversion and then try the same with conversions ...

						// for all symbols from calling object ...
						for(int h=0; h < 2 && !found; h++){

							for(int i = 0; i < (int)vec_global_functions->size() && !found; i++){

								tInfoRegisteredFunctionSymbol *irfs = &vec_global_functions->at(i);

								if(irfs->object_info.symbol_info.symbol_name == iao->ast_node->value_symbol && (irfs->m_arg.size() == argv->size())){


									all_check=true;
									// convert parameters script to c...
									for( int k = 0; k < (int)argv->size() && all_check;k++){
										//converted_param[i]= (int)(argv->at(i));
										if(h==0){
											all_check = argv->at(k)->getPointer_C_ClassName()==irfs->m_arg[k];
										}
										else{
											if((argv->at(k))->getPointer_C_ClassName()!=irfs->m_arg[k]){
												all_check =CScriptClassFactory::getInstance()->getConversionType((argv->at(k))->getPointer_C_ClassName(),irfs->m_arg[k], false)!=NULL;
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
							aux_function_info = &(*vec_global_functions)[iao->index_op2];
						}else{
							print_error_cr("Cannot find right C symbol for \"%s\"",iao->ast_node->value_symbol.c_str());
						}
						break;

					}


				}
				else{
					print_error_cr("Unexpected C calling exception");
				}


			}



			if(ptrResultInstructionOp1->type != INS_TYPE_FUNCTION){
				if(ptrResultInstructionOp1->type == INS_TYPE_VAR && ((CScriptVariable *)ptrResultInstructionOp1->stkResultObject)->getIdxClass() == CScriptClassFactory::getInstance()->getIdxClassFunctor()){
					aux_function_info = (tInfoRegisteredFunctionSymbol *)(((CFunctor *)ptrResultInstructionOp1->stkResultObject)->m_value);



				}else {
					print_error_cr("object \"%s\" is not function at line %i",instruction->ast_node->value_symbol.c_str(), instruction->ast_node->definedValueline);
					return false;
				}
			}

			calling_object = this_object;
			if((instruction->asm_properties & ASM_PROPERTY_CALLING_OBJECT) != 0){
				calling_object= ptrResultInstructionOp1->ptrObjectRef!=NULL?*ptrResultInstructionOp1->ptrObjectRef:NULL;//(CScriptVariable *)stkResultInstruction[index_op1+startIdxStkResultInstruction-1].stkResultObject;
						//((CFunctor *)ptrResultInstructionOp1->stkResultObject)->getThisObject();
			}
			else if((aux_function_info->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){

			}


			CSharedPointerManager::getInstance()->push();

			// by default virtual machine gets main object class in order to run functions ...
			if((ret_obj=CVirtualMachine::execute(aux_function_info,calling_object,&m_functionArgs, n_stk+1))==NULL){
				return false;
			}

			CSharedPointerManager::getInstance()->pop();

			// deallocates stack...
			m_functionArgs.clear();

			// finally set result value into stkResultObject...
			if(!pushVar(ret_obj)){
				return false;
			}
			break;
		case PUSH: // push arg instruction will creating object ensures not to have e/s...

			if((svar = createVarFromResultInstruction(ptrResultInstructionOp1)) == NULL){
				return false;
			}

			m_functionArgs.push_back(svar);
			break;
		case CLR: // clear args
			m_functionArgs.clear();
			break;
		case VGET: // vector access after each index is processed...
			// index_op1 is vector, index op2 is index...
			if(IS_VECTOR(ptrResultInstructionOp1)){
				if(IS_INT(ptrResultInstructionOp2)){
					// determine object ...
					CVector * vec = (CVector *)(ptrResultInstructionOp1->stkResultObject);//[stkInteger[stkResultInstruction[index_op2].index]];
					int v_index = LOAD_INT_OP(ptrResultInstructionOp2);

					print_info_cr("%i",v_index);

					// check indexes ...
					if(v_index < 0 || v_index >= (int)vec->m_objVector.size()){
						print_error_cr("Line %i. Index vector out of bounds!",instruction->ast_node->definedValueline);
						return false;
					}

					if(!pushVar(vec->m_objVector[v_index],&vec->m_objVector[v_index])){
						return false;
					}

				}else{
					print_error_cr("Expected vector-index as integer");
					return false;
				}
			}
			else{
				print_error_cr("Line %i: Variable \"%s\" is not type vector",
						instruction->ast_node->definedValueline,
						asm_op->at(instruction->index_op1)->ast_node->value_symbol.c_str(),
						instruction->ast_node->value_symbol.c_str()
						//base_var->getClassName().c_str(), iao->ast_node->value_symbol.c_str()
						);
				return false;
			}

			break;
		case VPUSH: // Value push for vector
			if(IS_VECTOR(ptrResultInstructionOp1)){
				CVector * vec = (CVector *)(ptrResultInstructionOp1->stkResultObject);
				if((svar = createVarFromResultInstruction(ptrResultInstructionOp2)) == NULL){
					return false;
				}
				vec->m_objVector.push_back(svar);


				if(svar != CScriptVariable::NullSymbol && svar != CScriptVariable::UndefinedSymbol){
					// add share + 1
					CSharedPointerManager::getInstance()->sharePointer(svar->idx_shared_ptr);
				}

			}else{
				print_error_cr("Expected operand 1 as vector");
				return false;
			}
			break;
		case VEC: // Create new vector object...
			if(!pushVar(svar=NEW_VECTOR_VAR)){
				return false;
			}
			svar->idx_shared_ptr = CSharedPointerManager::getInstance()->newSharedPointer(svar);
			break;

		case RET:

			/*if(!assignObjectFromIndex(function_object->getReturnObjectPtr(),instruction->index_op1)){
				return false;
			}*/
			break;
		case NEW:
			if(!pushVar(svar=NEW_CLASS_VAR_BY_IDX(instruction->index_op1), NULL, true)){
				return false;
			}

			svar->idx_shared_ptr = CSharedPointerManager::getInstance()->newSharedPointer(svar);

			// execute its constructor ...
			CSharedPointerManager::getInstance()->push();


			if((constructor_function = svar->getConstructorFunction()) != NULL){
				execute(constructor_function,svar);
			}

			CSharedPointerManager::getInstance()->pop();

			break;

		case SAVE_I: // For ternary condition ...
			idxSavedInstruction=idxStkCurrentResultInstruction-1;
			break;
		case LOAD_I: // For ternary condition ...

			switch(stkResultInstruction[idxSavedInstruction].type){
			default:
				print_error_cr("unexpected type");
				break;
			case INS_TYPE_INTEGER:
				if(!pushInteger((int)stkResultInstruction[idxSavedInstruction].stkResultObject)){
					return false;
				}
				break;
			case INS_TYPE_BOOLEAN:
				if(!pushBoolean((bool)stkResultInstruction[idxSavedInstruction].stkResultObject)){
					return false;
				}
				break;
			case INS_TYPE_NUMBER:
				if(!pushNumber(*((float *)stkResultInstruction[idxSavedInstruction].stkResultObject))){
					return false;
				}
				break;


			case INS_TYPE_STRING:
				if(!pushString(*((string *)stkResultInstruction[idxSavedInstruction].stkResultObject))){
					return false;
				}
				break;
			case INS_TYPE_VAR:
				if(!pushVar(((CScriptVariable *)stkResultInstruction[idxSavedInstruction].stkResultObject))){
					return false;
				}
				break;

			}

			break;

		case POP_SCOPE:


			popScope(info_function,instruction->index_op1);


			break;

	}

	idxStkCurrentResultInstruction++;

	return true;
}





CVirtualMachine::~CVirtualMachine(){
	reset();
}
