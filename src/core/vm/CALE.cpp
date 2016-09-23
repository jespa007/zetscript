#include "core/zg_core.h"


CALE::tAleObjectInfo  	CALE::stack[VM_LOCAL_VAR_MAX_STACK];
int 					CALE::stkInteger[VM_ALE_OPERATIONS_MAX_STACK];
float 					CALE::stkNumber[VM_ALE_OPERATIONS_MAX_STACK];
bool					CALE::stkBoolean[VM_ALE_OPERATIONS_MAX_STACK];
string 					CALE::stkString[VM_ALE_OPERATIONS_MAX_STACK];
tInfoRegisteredFunctionSymbol 			CALE::stkFunction[VM_ALE_OPERATIONS_MAX_STACK];
CScriptVariable *			CALE::stkVar[VM_ALE_OPERATIONS_MAX_STACK];

//CScriptVariable  **stkObject[MAX_PER_TYPE_OPERATIONS]; // all variables references to this ...
//CVector	 * vector[MAX_OPERANDS];

int CALE::n_stkInteger=0;
int CALE::n_stkNumber=0;
int CALE::n_stkBoolean=0;
int CALE::n_stkString=0;
int CALE::n_stkFunction=0;
int CALE::n_stkVar=0;

CALE::tAleObjectInfo CALE::result_object_instruction[VM_ALE_OPERATIONS_MAX_STACK];


CALE::tAleObjectInfo *CALE::currentBaseStack=NULL;
int CALE::idxCurrentStack=0;


CALE::tAleObjectInfo *CALE::allocStack(unsigned n_vars){

	if((idxCurrentStack+n_vars) >=  VM_LOCAL_VAR_MAX_STACK){
		print_error_cr("Error MAXIMUM stack size reached");
		exit(EXIT_FAILURE);
	}


	currentBaseStack=&stack[CALE::idxCurrentStack];

	// init vars ...
	for(unsigned i = 0; i < n_vars; i++){
		currentBaseStack[i].stkObject=CScopeInfo::UndefinedSymbol;
		currentBaseStack[i].type = CScriptVariable::VAR_TYPE::UNDEFINED;
		currentBaseStack[i].ptrAssignableVar = NULL;
	}


	CALE::idxCurrentStack+=n_vars;

	return currentBaseStack;
}

CALE::tAleObjectInfo *CALE::freeStack(unsigned n_vars){

	if((idxCurrentStack-n_vars) <  0){
		print_error_cr("Error MINIMUM stack size reached");
		exit(EXIT_FAILURE);
	}

	CALE::idxCurrentStack-=n_vars;
	currentBaseStack=&stack[CALE::idxCurrentStack];


	return currentBaseStack;
}


// general
#define CHECK_VALID_INDEXES \
if(!(index_op1 >= 0 && index_op1 <=current_asm_instruction)) { print_error_cr("instruction 1 out of bounds"); return false;} \
if(!(index_op2 >= 0 && index_op2 <=current_asm_instruction)) { print_error_cr("instruction 2 out of bounds"); return false;} \
if(!(index_op2 >= index_op1 )) { print_error_cr("invalid indexes"); return false;}


#define LOAD_NUMBER_OP(idx) \
		*(((int *)(result_object_instruction[idx].stkObject)))

#define LOAD_INT_OP(idx) \
		*(((int *)(result_object_instruction[idx].stkObject)))




#define LOAD_BOOL_OP(idx) \
		*(((bool *)(result_object_instruction[(idx)].stkObject)))


#define LOAD_STRING_OP(idx) \
		*(((string *)(result_object_instruction[idx].stkObject)))



// Check types
#define IS_NUMBER(idx) \
(result_object_instruction[idx].type == INS_TYPE_NUMBER)


#define IS_INT(idx) \
(result_object_instruction[idx].type == INS_TYPE_INTEGER)


#define IS_STRING(idx) \
(result_object_instruction[idx].type == INS_TYPE_STRING)

#define IS_BOOLEAN(idx) \
(result_object_instruction[idx].type == INS_TYPE_BOOLEAN)

#define IS_UNDEFINED(idx) \
(result_object_instruction[idx].type == INS_TYPE_UNDEFINED)

#define IS_FUNCTION(idx) \
(result_object_instruction[idx].type == INS_TYPE_FUNCTION)

#define IS_VAR(idx) \
(result_object_instruction[idx].type == INS_TYPE_VAR)

#define IS_VECTOR(idx) \
(( result_object_instruction[idx].type == INS_TYPE_VAR) && ((((CScriptVariable *)(result_object_instruction[idx].stkObject)))->getIdxClass()==CScriptClassFactory::getInstance()->getIdxClassVector()))

#define IS_GENERIC_NUMBER(idx) \
((result_object_instruction[idx].type == INS_TYPE_INTEGER) ||\
(result_object_instruction[idx].type == INS_TYPE_NUMBER))


#define OP1_AND_OP2_ARE_NUMBERS \
(IS_GENERIC_NUMBER(index_op1) && IS_GENERIC_NUMBER(index_op2))

#define OP1_IS_STRING_AND_OP2_IS_NUMBER \
(result_object_instruction[index_op1].type == INS_TYPE_STRING) && \
IS_GENERIC_NUMBER(index_op2)

#define OP1_IS_STRING_AND_OP2_IS_BOOLEAN \
(result_object_instruction[index_op1].type == INS_TYPE_STRING) && \
(result_object_instruction[index_op2].type == INS_TYPE_BOOLEAN)


#define OP1_AND_OP2_ARE_BOOLEANS \
(result_object_instruction[index_op1].type == INS_TYPE_BOOLEAN) && \
(result_object_instruction[index_op2].type == INS_TYPE_BOOLEAN)

#define OP1_AND_OP2_ARE_STRINGS \
(result_object_instruction[index_op1].type == INS_TYPE_STRING) && \
(result_object_instruction[index_op2].type == INS_TYPE_STRING)

#define PROCESS_NUM_OPERATION(__OVERR_OP__)\
					if (IS_INT(index_op1) && IS_INT(index_op2)){\
						if(!pushInteger(LOAD_INT_OP(index_op1) __OVERR_OP__ LOAD_INT_OP(index_op2))){\
							return false;\
						}\
					}else if (IS_INT(index_op1) && IS_NUMBER(index_op2)){\
						if(!pushInteger(LOAD_INT_OP(index_op1) __OVERR_OP__ LOAD_NUMBER_OP(index_op2))) {\
							return false;\
						}\
					}else if (IS_NUMBER(index_op1) && IS_INT(index_op2)){\
						if(!pushNumber(LOAD_NUMBER_OP(index_op1) __OVERR_OP__ LOAD_INT_OP(index_op2))) {\
							return false;\
						}\
					}else {\
						if(!pushNumber(LOAD_NUMBER_OP(index_op1) __OVERR_OP__ LOAD_NUMBER_OP(index_op2))) {\
							return false;\
						}\
					}


string CALE::STR_GET_TYPE_VAR_INDEX_INSTRUCTION(int index){
	string result="udnefined";
	if(IS_INT(index))
		result= "int";
	else if(IS_NUMBER(index))
		result= "number";
	else if(IS_BOOLEAN(index))
		result= "bool";
	else if(IS_STRING(index))
		result= "string";
	else if(IS_FUNCTION(index))
		result= "function";
	else if(IS_VAR(index)){
		result=((CScriptVariable *)result_object_instruction[index].stkObject)->getClassName();
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



CALE::CALE(){

	reset();

}

bool CALE::pushInteger(int  init_value){
	if(n_stkInteger ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Max int operands");
		return false;
	}


	stkInteger[n_stkInteger]=init_value;
	result_object_instruction[current_asm_instruction]={INS_TYPE_INTEGER,&stkInteger[n_stkInteger],NULL};
	n_stkInteger++;

	return true;
}

bool CALE::pushBoolean(bool init_value, int n_stk){
	if(n_stkBoolean ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max bool operations");
		return false;
	}



	stkBoolean[n_stkBoolean]=init_value;
	result_object_instruction[current_asm_instruction]={INS_TYPE_BOOLEAN,&stkBoolean[n_stkBoolean],NULL};
	n_stkBoolean++;

	return true;
}

bool CALE::pushNumber(float init_value){
	if(n_stkNumber ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max number operations");
		return false;
	}

	stkNumber[n_stkNumber]=init_value;
	result_object_instruction[current_asm_instruction]={INS_TYPE_NUMBER,&stkNumber[n_stkNumber],NULL};
	n_stkNumber++;

	return true;
}

bool CALE::pushString(const string & init_value){
	if(n_stkString ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max string operations");
		return false;
	}


	stkString[n_stkString]=init_value;
	result_object_instruction[current_asm_instruction]={INS_TYPE_STRING,&stkString[n_stkString],NULL};
	n_stkString++;

	return true;

}

bool CALE::pushFunction(tInfoRegisteredFunctionSymbol * init_value){
	if(n_stkFunction ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max string operations");
		return false;
	}

	stkFunction[n_stkFunction]=init_value;
	result_object_instruction[current_asm_instruction]={INS_TYPE_FUNCTION,&stkFunction[n_stkFunction],NULL};
	n_stkFunction++;

	return true;

}

bool CALE::pushVar(CScriptVariable * init_value, CScriptVariable ** ptrAssignableVar=NULL){
	if(n_stkVar ==VM_ALE_OPERATIONS_MAX_STACK){
		print_error_cr("Reached max object operations");
		return false;
	}

	if(init_value == NULL){
		print_error_cr("NULL value");
		return false;
	}

	// try to deduce object type ...
	CScriptVariable::VAR_TYPE	var_type = (init_value)->getVariableType();


	result_object_instruction[current_asm_instruction]={var_type,init_value, ptrAssignableVar};

	return true;
}


CScriptVariable * CALE::createVarFromIndex(int index){
	CScriptVariable *obj = NULL;


	// check second operand valid object..
	switch(result_object_instruction[index].type){
	default:
		print_error_cr("(internal) cannot determine var type");
		return NULL;
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_UNDEFINED:
		obj=CScopeInfo::UndefinedSymbol;
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_INTEGER:
		obj= new CInteger();//CScriptClassFactory::getInstance()->newClassByIdx(CScriptClassFactory::getInstance()->getIdxClassInteger());
		((CInteger *)obj)->m_value = *((int *)(result_object_instruction[index].stkObject));
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_NUMBER:
		obj= new CNumber();//CScriptClassFactory::getInstance()->newClassByIdx(CScriptClassFactory::getInstance()->getIdxClassNumber());
		((CNumber *)obj)->m_value = *((float *)(result_object_instruction[index].stkObject));
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_STRING:
		obj= new CString();;//=CScriptClassFactory::getInstance()->newClassByIdx(CScriptClassFactory::getInstance()->getIdxClassString());
		((CString *)obj)->m_value = *((string *)(result_object_instruction[index].stkObject));
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_BOOLEAN:
		obj= new CBoolean();//=CScriptClassFactory::getInstance()->newClassByIdx(CScriptClassFactory::getInstance()->getIdxClassBoolean());
		((CBoolean *)obj)->m_value = *((bool *)(result_object_instruction[index].stkObject));
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_FUNCTION:
		obj = new CFunctor((tInfoRegisteredFunctionSymbol *)result_object_instruction[index].stkObject);
		break;
	case VALUE_INSTRUCTION_TYPE::INS_TYPE_VAR:
		obj = (CScriptVariable *)result_object_instruction[index].stkObject;
		break;

	}

	return obj;
}

bool CALE::performPreOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *var){
	// ok from here, let's check preoperator ...

	switch(var->getVariableType()){
	case CScriptVariable::INTEGER:
		if(pre_post_operator_type == ASM_PRE_POST_OPERATORS::PRE_INC)
			((CInteger *)var)->m_value++;
		else //dec
			((CInteger *)var)->m_value--;
		break;
	case CScriptVariable::NUMBER:
		if(pre_post_operator_type == ASM_PRE_POST_OPERATORS::PRE_INC)
			((CNumber *)var)->m_value++;
		else // dec
			((CNumber *)var)->m_value--;

		break;
	default:
		print_error_cr("Cannot perform preoperator ?? because is not number");
		return false;
		break;

	}

	return true;


}

bool CALE::performPostOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *var){
	// ok from here, let's check preoperator ...

	switch(var->getVariableType()){
	case CScriptVariable::INTEGER:
		if(pre_post_operator_type == ASM_PRE_POST_OPERATORS::POST_INC)
			((CInteger *)var)->m_value++;
		else //dec
			((CInteger *)var)->m_value--;
		break;
	case CScriptVariable::NUMBER:
		if(pre_post_operator_type == ASM_PRE_POST_OPERATORS::POST_INC)
			((CNumber *)var)->m_value++;
		else // dec
			((CNumber *)var)->m_value--;

		break;
	default:
		print_error_cr("Cannot perform postoperator ?? because is not number");
		return false;
		break;

	}

	return true;
}


bool CALE::loadVariableValue(tInfoAsmOp *iao, tInfoRegisteredFunctionSymbol *info_function,CScriptVariable *this_object, int n_stk){

	if(iao->index_op1 != LOAD_TYPE_VARIABLE){
		print_error_cr("expected load type variable.");
		return false;
	}

	//CScriptVariable *this_object = function_object->getThisObject();
	CScriptVariable::tSymbolInfo *si;
	CScriptVariable **ptr_var_object=NULL;
	CScriptVariable *var_object = NULL;



	switch(iao->scope_type){
	default:
		print_error_cr("unknow scope type");
		break;
	case SCOPE_TYPE::THIS_SCOPE:

		// get var from object ...
		if((si = this_object->getVariableSymbolByIndex(iao->index_op2))==NULL){
			print_error_cr("cannot find symbol \"%s\"",iao->ast_node->value_symbol.c_str());
			return false;
		}

		ptr_var_object = (CScriptVariable **)(&si->object);
		var_object = (CScriptVariable *)(si->object);

		break;

	case SCOPE_TYPE::LOCAL_SCOPE:

		if(CALE::currentBaseStack[iao->index_op2].type != VALUE_INSTRUCTION_TYPE::INS_TYPE_VAR){
			print_error_cr("instruction not var type");
			return false;
		}

		// get var from base stack ...
		ptr_var_object = (CScriptVariable **)(&CALE::currentBaseStack[iao->index_op2].stkObject);
		var_object = (CScriptVariable *)(CALE::currentBaseStack[iao->index_op2].stkObject);


		/*if((si = this_object->getVariableSymbolByIndex(iao->index_op2))==NULL){
			print_error_cr("cannot find symbol \"%s\"",iao->ast_node->value_symbol.c_str());

			return false;
		}*/

		break;


	}

	//CScriptVariable **ptr_var_object = (CScriptVariable **)(&si->object);
	//CScriptVariable *var_object = (CScriptVariable *)(si->object);
	if(iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::PRE_DEC || iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::PRE_INC){

		if(!performPreOperator(iao->pre_post_operator_type, var_object)){
			return false;
		}
	}

	if(iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::POST_DEC || iao->pre_post_operator_type == ASM_PRE_POST_OPERATORS::POST_INC){
		// 1. Load value as constant value
		if(!loadConstantValue(var_object,n_stk)){
			return false;
		}

		// 2. then perform post operation ...
		if(!performPostOperator(iao->pre_post_operator_type, var_object)){
			return false;
		}

	}
	else{
	// generic object pushed ...
		if(!pushVar(var_object,ptr_var_object)) {
			return false;
		}
	}

	return true;
}

bool CALE::loadFunctionValue(tInfoAsmOp *iao,tInfoRegisteredFunctionSymbol *local_function, CScriptVariable *this_object, int n_stk){

	if(iao->index_op1 != LOAD_TYPE_FUNCTION){
		print_error_cr("expected load type function.");
		return false;
	}


	tInfoRegisteredFunctionSymbol *info_function=NULL;

	CScriptVariable::tSymbolInfo *si;

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
		break;
	case SCOPE_TYPE::THIS_SCOPE:

		// get var from object ...
		if((si = this_object->getFunctionSymbolByIndex(iao->index_op2))==NULL){
			print_error_cr("cannot find symbol \"%s\"",iao->ast_node->value_symbol.c_str());
			return false;
		}

		info_function =(tInfoRegisteredFunctionSymbol *)si->object;

		break;

	case SCOPE_TYPE::LOCAL_SCOPE:
		info_function = &local_function->object_info.local_symbols.m_registeredFunction[iao->index_op2];
		break;


	}

	// generic object pushed ...
	if(!pushFunction(info_function)) {
		return false;
	}
	//result_object_instruction[current_asm_instruction]={CScriptVariable::FUNCTION,(CScriptVariable **)si, false};


	return true;
}

bool CALE::loadConstantValue(CScriptVariable *var, int n_stk){

	if(var != NULL){

		switch(var->getVariableType()){
			default:
				print_error_cr("Invalid load constant value as %i",var->getVariableType());
				return false;
				break;
			case CScriptVariable::VAR_TYPE::UNDEFINED:

				result_object_instruction[current_asm_instruction]={INS_TYPE_UNDEFINED,NULL,NULL};

				break;
			case CScriptVariable::VAR_TYPE::INTEGER:
				if(!pushInteger(((CInteger *)var)->m_value)) return false;
				break;
			case CScriptVariable::VAR_TYPE::BOOLEAN:
				if(!pushBoolean(((CBoolean *)var)->m_value,n_stk)) return false;
				break;
			case CScriptVariable::VAR_TYPE::STRING:
				if(!pushString(((CString *)var)->m_value)) return false;
				break;
			case CScriptVariable::VAR_TYPE::NUMBER:
				if(!pushNumber(((CNumber *)var)->m_value)) return false;
				break;
			}

		return true;

	}

	print_error_cr("Null value");
	return false;

}





bool CALE::assignVarFromIndex(CScriptVariable **var, int index){

	CScriptVariable *aux_var=NULL;


	// if undefined, create new by default ...
	if(*var == NULL){

		if((*var = createVarFromIndex(index)) == NULL){
			return false;
		}
	}

	tInfoRegisteredFunctionSymbol * init_value;

	// finally assign the value ...
	switch(result_object_instruction[index].type){

		case INS_TYPE_UNDEFINED:
			*var = NULL;
			break;

		case INS_TYPE_INTEGER:
			if((*var) == NULL) {
				print_error_cr("Expected variable type!");
				return false;
			}
			if((*var)->getIdxClass() == CScriptClassFactory::getInstance()->getIdxClassInteger()){
				((CInteger *)(*var))->m_value=((CInteger *)(result_object_instruction[index].stkObject))->m_value;
			}else if((*var)->getIdxClass() == CScriptClassFactory::getInstance()->getIdxClassNumber()){
				((CNumber *)(*var))->m_value=((CInteger *)(result_object_instruction[index].stkObject))->m_value;
			}else
			{
				print_error_cr("var is not type integer!");
				return false;
			}
			break;
		case INS_TYPE_NUMBER:
			if((*var) == NULL) {print_error_cr("Expected variable type!");return false;}
			if((*var)->getIdxClass() == CScriptClassFactory::getInstance()->getIdxClassInteger()){
				((CInteger *)aux_var)->m_value=((CNumber *)(result_object_instruction[index].stkObject))->m_value;
			}else if((*var)->getIdxClass() == CScriptClassFactory::getInstance()->getIdxClassNumber()){
				((CNumber *)(*var))->m_value=((CNumber *)(result_object_instruction[index].stkObject))->m_value;
			}else
			{
				print_error_cr("var is not type number!");
				return false;
			}
			break;
		case INS_TYPE_STRING:
			if((*var) == NULL) {print_error_cr("Expected variable type!");return false;}
			if((*var)->getIdxClass() == CScriptClassFactory::getInstance()->getIdxClassString()){
				((CString  *)(*var))->m_value= ((CString *)(result_object_instruction[index].stkObject))->m_value;
			}else
			{
				print_error_cr("var is not type string!");
				return false;
			}

			break;
		case INS_TYPE_BOOLEAN:
			if((*var) == NULL) {print_error_cr("Expected variable type!");return false;}
			if((*var)->getIdxClass() == CScriptClassFactory::getInstance()->getIdxClassBoolean()){
				((CBoolean  *)aux_var)->m_value= ((CBoolean *)(result_object_instruction[index].stkObject))->m_value;
			}else
			{
				print_error_cr("var is not type boolean!");
				return false;
			}
			break;

		// pointer assigment ...
		case CScriptVariable::VAR_TYPE::FUNCTION: // function object

			if((*var)->getIdxClass() == CScriptClassFactory::getInstance()->getIdxClassFunctor()){
				((CFunctor  *)aux_var)->m_value= ((tInfoRegisteredFunctionSymbol *)(result_object_instruction[index].stkObject));
			}else
			{
				print_error_cr("var is not type functor!");
				return false;
			}
			break;
		case INS_TYPE_VAR: // generic object
			*var = (result_object_instruction[index].stkObject);
			break;

		default:
				print_error_cr("Unknow assignment 2!");
				return false;
				break;
		}


	return true;
}

bool CALE::performInstruction(int idx_instruction, tInfoAsmOp * instruction, int & jmp_to_statment,tInfoRegisteredFunctionSymbol *info_function,CScriptVariable *this_object,vector<CScriptVariable *> * argv, int n_stk){


	string 	aux_string;
	bool	aux_boolean;
	string symbol;
	CScriptVariable **obj=NULL;


	jmp_to_statment=-1;

	current_asm_instruction = idx_instruction;

	//CScopeInfo *_lc = instruction->ast_node->scope_info_ptr;

	int index_op1 = instruction->index_op1;
	int index_op2 = instruction->index_op2;


	switch(instruction->operator_type){
	default:
		print_error_cr("operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
		break;
	case NOP: // ignore ...
		break;
	case LOAD: // load value in function of value/constant ...
		/*if(!loadValue(instruction, n_stk)){
			return false;
		}*/
		//sprintf(print_aux_load_value,"UNDEFINED");
		switch(instruction->index_op1){
		case LOAD_TYPE::LOAD_TYPE_CONSTANT:

			if(!loadConstantValue((CScriptVariable *)instruction->index_op2, n_stk)){
				return false;
			}

			//sprintf(print_aux_load_value,"CONST(%s)",value_symbol.c_str());
			break;
		case LOAD_TYPE::LOAD_TYPE_VARIABLE:

			if(!loadVariableValue(instruction, info_function,this_object, n_stk)){
				return false;
			}

			break;
		case LOAD_TYPE::LOAD_TYPE_FUNCTION:
			if(!loadFunctionValue(instruction,info_function, this_object, n_stk)){
				return false;
			}

			break;
		case LOAD_TYPE::LOAD_TYPE_ARGUMENT:

			if(argv!=NULL){
				if(index_op2<(int)argv->size()){
					CScriptVariable *var=(*argv)[index_op2];

					pushVar(var,NULL);
				}else{
					print_error_cr("index out of bounds");
					return false;
				}
			}
			else{
				print_error_cr("argv null");
				return false;
			}

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
			if((obj = result_object_instruction[index_op1].ptrAssignableVar) != NULL) {// == CScriptVariable::VAR_TYPE::OBJECT){

				// get pointer object (can be assigned)
				//obj = result_object_instruction[index_op1].stkObject;



				if(!assignVarFromIndex(obj,index_op2))
						return false;

			}else{
				print_error_cr("Expected object l-value mov");
				return false;
			}

			break;
		case EQU:  // == --> boolean && boolean or string && string or number && number

			if(OP1_AND_OP2_ARE_BOOLEANS) {
				if(!pushBoolean(LOAD_BOOL_OP(index_op1) == LOAD_BOOL_OP(index_op2))) return false;
			}else if(OP1_AND_OP2_ARE_STRINGS){
				if(!pushBoolean(LOAD_STRING_OP(index_op1) == LOAD_STRING_OP(index_op2))) return false;
			}else if (IS_INT(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) == LOAD_INT_OP(index_op2))) return false;
			}else if (IS_INT(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) == LOAD_NUMBER_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) == LOAD_INT_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) == LOAD_NUMBER_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as string, number or boolean!");
				return false;
			}

			break;

		case NOT_EQU:  // == --> boolean && boolean or string && string or number && number

			if(OP1_AND_OP2_ARE_BOOLEANS) {
				if(!pushBoolean(LOAD_BOOL_OP(index_op1) != LOAD_BOOL_OP(index_op2))) return false;
			}else if(OP1_AND_OP2_ARE_STRINGS){
				if(!pushBoolean(LOAD_STRING_OP(index_op1) != LOAD_STRING_OP(index_op2))) return false;
			}else if (IS_INT(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) != LOAD_INT_OP(index_op2))) return false;
			}else if (IS_INT(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) != LOAD_NUMBER_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) != LOAD_INT_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) != LOAD_NUMBER_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as string, number or boolean!");
				return false;
			}

			break;
		case LT:  // <
			if (IS_INT(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) < LOAD_INT_OP(index_op2))) return false;
			}else if (IS_INT(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) < LOAD_NUMBER_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) < LOAD_INT_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) < LOAD_NUMBER_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}
			break;
		case LTE:  // <=

			if (IS_INT(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) <= LOAD_INT_OP(index_op2))) return false;
			}else if (IS_INT(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) <= LOAD_NUMBER_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) <= LOAD_INT_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) <= LOAD_NUMBER_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}

			break;
		case NOT: // !
			if (result_object_instruction[index_op1].type == CScriptVariable::VAR_TYPE::BOOLEAN){
				if(!pushBoolean(!LOAD_BOOL_OP(index_op1))) return false;
			}else{
				print_error_cr("Expected operands 1 as boolean!");
				return false;
			}
			break;
		case NEG: // !
			if (IS_GENERIC_NUMBER(index_op1)){
				if(result_object_instruction[index_op1].type == CScriptVariable::VAR_TYPE::INTEGER){ // operation will result as integer.
					if(!pushInteger(-LOAD_INT_OP(index_op1))) {
						return false;
					}
				}
				else{
					if(!pushNumber(-LOAD_NUMBER_OP(index_op2))){
						return false;
					}
				}

			}else{
					print_error_cr("Expected operands 1 as number or integer!");
					return false;
			}
			break;

		case GT:  // >
			if (IS_INT(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) > LOAD_INT_OP(index_op2))) return false;
			}else if (IS_INT(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) > LOAD_NUMBER_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) > LOAD_INT_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) > LOAD_NUMBER_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}
			break;
		case GTE: // >=
			if (IS_INT(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) >= LOAD_INT_OP(index_op2))) return false;
			}else if (IS_INT(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_INT_OP(index_op1) >= LOAD_NUMBER_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_INT(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) >= LOAD_INT_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_NUMBER(index_op2)){
				if(!pushBoolean(LOAD_NUMBER_OP(index_op1) >= LOAD_NUMBER_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}
			break;

		case ADD: // +

			// get indexes and check whether is possible or not its calculation.
			// check indexes
			CHECK_VALID_INDEXES;

			// check types ...
			if (IS_STRING(index_op1) && (IS_UNDEFINED(index_op2) || IS_VAR(index_op2))){

				string result = "undefined";
				if(IS_VAR(index_op2)){
					result = ((CScriptVariable *)(result_object_instruction[index_op2].stkObject))->getClassName();
				}

				if(!pushString(LOAD_STRING_OP(index_op1)+result)) return false;
			}else if (IS_INT(index_op1) && IS_INT(index_op2)){
				if(!pushInteger(LOAD_INT_OP(index_op1) + LOAD_INT_OP(index_op2))) return false;
			}else if (IS_INT(index_op1) && IS_NUMBER(index_op2)){
				if(!pushInteger(LOAD_INT_OP(index_op1) + LOAD_NUMBER_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_INT(index_op2)){
				if(!pushNumber(LOAD_NUMBER_OP(index_op1) + LOAD_INT_OP(index_op2))) return false;
			}else if (IS_NUMBER(index_op1) && IS_NUMBER(index_op2)){
				if(!pushNumber(LOAD_NUMBER_OP(index_op1) + LOAD_NUMBER_OP(index_op2))) return false;
			}else if(OP1_IS_STRING_AND_OP2_IS_NUMBER){ // concatenate string + number

				aux_string =  LOAD_STRING_OP(index_op1);

				if(result_object_instruction[index_op2].type == CScriptVariable::VAR_TYPE::INTEGER)
					aux_string = aux_string + CStringUtils::intToString(LOAD_INT_OP(index_op2));
				else
					aux_string = aux_string + CStringUtils::intToString(LOAD_NUMBER_OP(index_op2));

				if(!pushString(aux_string)) return false;
			}else if(OP1_IS_STRING_AND_OP2_IS_BOOLEAN){ // concatenate string + boolean

				aux_string =  LOAD_STRING_OP(index_op1);
				aux_boolean =  LOAD_BOOL_OP(index_op2);

				if(aux_boolean)
					aux_string = aux_string + "true";
				else
					aux_string = aux_string + "false";

				if(!pushString(aux_string)) return false;

			}else if(OP1_AND_OP2_ARE_STRINGS){ // concatenate string + boolean

				if(!pushString(LOAD_STRING_OP(index_op1)+LOAD_STRING_OP(index_op2))) return false;

			}else{

				// full error description ...

				string var_type1=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(index_op1),
					   var_type2=STR_GET_TYPE_VAR_INDEX_INSTRUCTION(index_op2);


				//print_error_cr("Expected operands as number+number, string+string, string+number or string + boolean!");


				print_error_cr("Error at line %i cannot perform operator \"%s\" +  \"%s\"",
						instruction->ast_node->definedValueline,
						var_type1.c_str(),
						var_type2.c_str());
				return false;
			}

			break;

		case LOGIC_AND: // &&
			if(OP1_AND_OP2_ARE_BOOLEANS) {
				if(!pushBoolean(LOAD_BOOL_OP(index_op1) && LOAD_BOOL_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands boolean!");
				return false;
			}
			break;
		case LOGIC_OR:  // ||
			if(OP1_AND_OP2_ARE_BOOLEANS) {
				if(!pushBoolean(LOAD_BOOL_OP(index_op1) || LOAD_BOOL_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands boolean!");
				return false;
			}
			break;
		case DIV: // /
			if(OP1_AND_OP2_ARE_NUMBERS) {

				if(IS_INT(index_op2)){
					if(LOAD_INT_OP(index_op2) == 0) {
						print_error_cr("Divide by 0 at line %i.",instruction->ast_node->definedValueline);
						return false;
					}
				}else{
					if(LOAD_NUMBER_OP(index_op2) == 0) {
						print_error_cr("Divide by 0 at line %i.",instruction->ast_node->definedValueline);
						return false;
					}
				}

				PROCESS_NUM_OPERATION(/);
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}

			break;
		case MUL: // *
			if(OP1_AND_OP2_ARE_NUMBERS) {
					PROCESS_NUM_OPERATION(*);

			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}
			break;
		case MOD:  // %
			if(OP1_AND_OP2_ARE_NUMBERS) {

				if(IS_INT(index_op2)){
					if(LOAD_INT_OP(index_op2) == 0) {
						print_error_cr("Divide by 0 at line %i.",instruction->ast_node->definedValueline);
						return false;
					}
				}else{
					if(LOAD_NUMBER_OP(index_op2) == 0) {
						print_error_cr("Divide by 0 at line %i.",instruction->ast_node->definedValueline);
						return false;
					}
				}
				//PROCESS_NUM_OPERATION(%);

				if (IS_INT(index_op1) && IS_INT(index_op2)){
					if(!pushInteger(LOAD_INT_OP(index_op1) % LOAD_INT_OP(index_op2))){
						return false;\
					}
				}else if (IS_INT(index_op1) && IS_NUMBER(index_op2)){
					if(!pushInteger(LOAD_INT_OP(index_op1) % ((int) LOAD_NUMBER_OP(index_op2)))) {
						return false;\
					}
				}else if (IS_NUMBER(index_op1) && IS_INT(index_op2)){
					if(!pushNumber(fmod(LOAD_NUMBER_OP(index_op1), LOAD_INT_OP(index_op2)))) {
						return false;
					}
				}else {
					if(!pushNumber(fmod(LOAD_NUMBER_OP(index_op1) , LOAD_NUMBER_OP(index_op2)))) {
						return false;
					}
				}

			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}

			break;
		case AND: // bitwise logic and
			if((result_object_instruction[index_op1].type == CScriptVariable::INTEGER) && (result_object_instruction[index_op2].type == CScriptVariable::INTEGER)){
				if(!pushInteger(LOAD_INT_OP(index_op1) & LOAD_INT_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		case OR: // bitwise logic or
			if((result_object_instruction[index_op1].type == CScriptVariable::INTEGER) && (result_object_instruction[index_op2].type == CScriptVariable::INTEGER)){
				if(!pushInteger(LOAD_INT_OP(index_op1) | LOAD_INT_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}

			break;
		case XOR: // logic xor
			if((result_object_instruction[index_op1].type == CScriptVariable::INTEGER) && (result_object_instruction[index_op2].type == CScriptVariable::INTEGER)){
				if(!pushInteger(LOAD_INT_OP(index_op1) ^ LOAD_INT_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		case SHL: // shift left
			if((result_object_instruction[index_op1].type == CScriptVariable::INTEGER) && (result_object_instruction[index_op2].type == CScriptVariable::INTEGER)){
				if(!pushInteger(LOAD_INT_OP(index_op1) << LOAD_INT_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		case SHR: // shift right
			if((result_object_instruction[index_op1].type == CScriptVariable::INTEGER) && (result_object_instruction[index_op2].type == CScriptVariable::INTEGER)){
				if(!pushInteger(LOAD_INT_OP(index_op1) >> LOAD_INT_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		// special internal ops...
		case JMP:
			jmp_to_statment = index_op1;
			break;
		case JNT: // goto if not true ... goes end to conditional.

			// load boolean var and jmp if true...
			if(current_asm_instruction > 0){

				if(result_object_instruction[current_asm_instruction-1].type == CScriptVariable::VAR_TYPE::BOOLEAN){

					if(!(*((CBoolean **)result_object_instruction[current_asm_instruction-1].stkObject))->m_value){
						jmp_to_statment = index_op1;
					}
				}
			}else{
				print_error_cr("No boolean elements");
				return false;
			}
			break;
		case JT: // goto if true ... goes end to conditional.
			if(current_asm_instruction > 0){

				if(result_object_instruction[current_asm_instruction-1].type == CScriptVariable::VAR_TYPE::BOOLEAN){

					if((*((CBoolean **)result_object_instruction[current_asm_instruction-1].stkObject))->m_value){
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
			if(result_object_instruction[index_op1].type == CScriptVariable::FUNCTION){
				CScriptVariable *ret_obj;
				tInfoRegisteredFunctionSymbol * function_info = (tInfoRegisteredFunctionSymbol *)result_object_instruction[index_op1].stkObject;

				// by default virtual machine gets main object class in order to run functions ...
				if((ret_obj=CVirtualMachine::execute(function_info,this_object,&m_functionArgs, n_stk+1))==NULL){
					return false;
				}

				// finally set result value into stkObject...
				if(!pushVar(ret_obj)){
					return false;
				}
			}
			else{
				print_error_cr("object \"%s\" is not function at line %i",instruction->ast_node->value_symbol.c_str(), instruction->ast_node->definedValueline);
				return false;
			}
			break;
		case PUSH: // push arg instruction will creating object ensures not to have e/s...
			m_functionArgs.push_back(createVarFromIndex(index_op1));
			break;
		case CLR: // clear args
			m_functionArgs.clear();
			break;
		case VGET: // vector access after each index is processed...
			// index_op1 is vector, index op2 is index...
			if(IS_VECTOR(index_op1)){
				if(IS_INT(index_op2)){
					// determine object ...
					CScriptVariable * vec = (CVector *)(result_object_instruction[index_op1].stkObject);//[stkInteger[result_object_instruction[index_op2].index]];
					int v_index = LOAD_INT_OP(index_op2);

					print_info_cr("%i",v_index);

					// check indexes ...
					if(v_index < 0 || v_index >= (int)vec->m_value.size()){
						print_error_cr("Line %i. Index vector out of bounds!",instruction->ast_node->definedValueline);
						return false;
					}

					if(!pushVar(vec->m_value[v_index],&vec->m_value[v_index])){
						return false;
					}

				}else{
					print_error_cr("Expected vector-index as integer");
					return false;
				}
			}
			else{
				print_error_cr("Expected operand 1 as vector");
				return false;
			}

			break;
		case VPUSH: // Value push for vector
			if(result_object_instruction[index_op1].type == CScriptVariable::VECTOR){
				CVector * vec = (CVector *)(result_object_instruction[index_op1].stkObject);
				vec->m_value.push_back(createVarFromIndex(index_op2));
			}else{
				print_error_cr("Expected operand 1 as vector");
				return false;
			}
			break;
		case VEC: // Create new vector object...
			pushVar(CScriptClassFactory::getInstance()->newClassByIdx(CScriptClassFactory::getInstance()->getIdxClassVector()));
			break;

		case RET:

			/*if(!assignObjectFromIndex(function_object->getReturnObjectPtr(),instruction->index_op1)){
				return false;
			}*/

			break;

	}

	return true;
}

void CALE::reset(){

	// deallocate allocated aux vectors...

	n_stkInteger=
	n_stkNumber=
	n_stkBoolean=
	n_stkString=
	n_stkFunction=
	n_stkVar=
	current_asm_instruction=0;

	//memset(result_object_instruction,0,sizeof(result_object_instruction));
	m_functionArgs.clear();

}

CALE::~CALE(){

	reset();


}




