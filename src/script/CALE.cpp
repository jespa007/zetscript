#include "zg_script.h"



// general
#define CHECK_VALID_INDEXES \
if(!(index_op1 >= 0 && index_op1 <=current_asm_instruction)) { print_error_cr("instruction 1 out of bounds"); return false;} \
if(!(index_op2 >= 0 && index_op2 <=current_asm_instruction)) { print_error_cr("instruction 2 out of bounds"); return false;} \
if(!(index_op2 >= index_op1 )) { print_error_cr("invalid indexes"); return false;}


#define LOAD_NUM_OP(idx) \
		((result_object_instruction[idx].type == CVariable::VAR_TYPE::INTEGER)? ((CInteger *)(*result_object_instruction[idx].stkObject))->m_value:\
		(result_object_instruction[idx].type == CVariable::VAR_TYPE::NUMBER)?	((CNumber *)(*result_object_instruction[idx].stkObject))->m_value:\
				(-1))

#define LOAD_INT_OP(idx) \
		(((CInteger *)(*result_object_instruction[idx].stkObject))->m_value)




#define LOAD_BOOL_OP(idx) \
		((result_object_instruction[idx].type == CVariable::VAR_TYPE::BOOLEAN)? ((CBoolean *)(*result_object_instruction[idx].stkObject))->m_value:\
		(false))

#define LOAD_STRING_OP(idx) \
		(((CString *)(*result_object_instruction[idx].stkObject))->m_value)



// Check types
#define IS_NUMBER(idx) \
((result_object_instruction[idx].type == CVariable::VAR_TYPE::NUMBER) || \
(result_object_instruction[idx].type == CVariable::VAR_TYPE::INTEGER))

#define IS_STRING(idx) \
(result_object_instruction[idx].type == CVariable::VAR_TYPE::STRING)

#define IS_BOOLEAN(idx) \
(result_object_instruction[idx].type == CVariable::VAR_TYPE::BOOLEAN)


#define OP1_AND_OP2_ARE_NUMBERS \
(IS_NUMBER(index_op1) && IS_NUMBER(index_op2))

#define OP1_IS_STRING_AND_OP2_IS_NUMBER \
(result_object_instruction[index_op1].type == CVariable::VAR_TYPE::STRING) && \
IS_NUMBER(index_op2)

#define OP1_IS_STRING_AND_OP2_IS_BOOLEAN \
(result_object_instruction[index_op1].type == CVariable::VAR_TYPE::STRING) && \
(result_object_instruction[index_op2].type == CVariable::VAR_TYPE::BOOLEAN)


#define OP1_AND_OP2_ARE_BOOLEANS \
(result_object_instruction[index_op1].type == CVariable::VAR_TYPE::BOOLEAN) && \
(result_object_instruction[index_op2].type == CVariable::VAR_TYPE::BOOLEAN)

#define OP1_AND_OP2_ARE_STRINGS \
(result_object_instruction[index_op1].type == CVariable::VAR_TYPE::STRING) && \
(result_object_instruction[index_op2].type == CVariable::VAR_TYPE::STRING)


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

	n_totalIntegerPointers=
	n_totalNumberPointers=
	n_totalBooleanPointers=
	n_totalStringPointers=0;


	reset();

}

bool CALE::pushInteger(int  init_value){
	if(n_stkInteger ==MAX_PER_TYPE_OPERATIONS){
		print_error_cr("Max int operands");
		return false;
	}

	if(n_stkInteger >= n_totalIntegerPointers){
		stkInteger[n_stkInteger] = new CInteger();
		n_totalIntegerPointers++;
	}

	stkInteger[n_stkInteger]->m_value=init_value;
	result_object_instruction[current_asm_instruction]={CVariable::VAR_TYPE::INTEGER,(CObject **)&stkInteger[n_stkInteger],false};
	n_stkInteger++;

	return true;
}

bool CALE::pushBoolean(bool init_value){
	if(n_stkBoolean ==MAX_PER_TYPE_OPERATIONS){
		print_error_cr("Reached max bool operations");
		return false;
	}


	if(n_stkBoolean >= n_totalBooleanPointers){
		stkBoolean[n_stkBoolean] = new CBoolean();
		n_totalBooleanPointers++;
	}

	stkBoolean[n_stkBoolean]->m_value=init_value;
	result_object_instruction[current_asm_instruction]={CVariable::VAR_TYPE::BOOLEAN,(CObject **)&stkBoolean[n_stkBoolean],false};
	n_stkBoolean++;

	return true;
}

bool CALE::pushNumber(float init_value){
	if(n_stkNumber ==MAX_PER_TYPE_OPERATIONS){
		print_error_cr("Reached max number operations");
		return false;
	}


	if(n_stkNumber >= n_totalNumberPointers){
		stkNumber[n_stkNumber] = new CNumber();
		n_totalNumberPointers++;
	}

	stkNumber[n_stkNumber]->m_value=init_value;
	result_object_instruction[current_asm_instruction]={CVariable::VAR_TYPE::NUMBER,(CObject **)&stkNumber[n_stkNumber],false};
	n_stkNumber++;

	return true;
}

bool CALE::pushString(const string & init_value){
	if(n_stkString ==MAX_PER_TYPE_OPERATIONS){
		print_error_cr("Reached max string operations");
		return false;
	}

	if(n_stkString >= n_totalStringPointers){
		stkString[n_stkString] = new CString();
		n_totalStringPointers++;
	}

	stkString[n_stkString]->m_value=init_value;
	result_object_instruction[current_asm_instruction]={CVariable::VAR_TYPE::STRING,(CObject **)&stkString[n_stkString],false};
	n_stkString++;

	return true;

}

bool CALE::pushObject(CObject ** init_value, bool isAssignable){
	if(n_stkObject ==MAX_PER_TYPE_OPERATIONS){
		print_error_cr("Reached max object operations");
		return false;
	}

	// try to deduce object type ...
	CVariable::VAR_TYPE var_type = CVariable::VAR_TYPE::OBJECT;

	if((*init_value)->getObjectType() == CObject::VARIABLE){
		var_type = ((CVariable *)(*init_value))->getVariableType();
	}

	result_object_instruction[current_asm_instruction]={var_type,init_value, isAssignable};

	return true;
}

bool CALE::pushVector(CObject * init_value){
	if(n_stkVector ==MAX_PER_TYPE_OPERATIONS){
		print_error_cr("Reached max vector operations");
		return false;
	}

	// this vector will be revised to deallocate vectors which wasn't moved to variables
	stkVector[n_stkVector]=((CVector *)init_value);

	result_object_instruction[current_asm_instruction]={CVariable::VAR_TYPE::VECTOR,(CObject **)&stkVector[n_stkVector],false};
	n_stkVector++;
	return true;
}

bool CALE::pushFunction(CObject ** init_value){
	if(n_stkFunction ==MAX_PER_TYPE_OPERATIONS){
		print_error_cr("Reached max function operations");
		return false;
	}

	result_object_instruction[current_asm_instruction]={CVariable::VAR_TYPE::FUNCTION,init_value,false};

	return true;
}

CObject * CALE::getObjectFromIndex(int index){
	CObject *obj=NULL;

	if(index >= 0 && index < this->current_asm_instruction){
		return (*result_object_instruction[index].stkObject);
	}else{
		print_error_cr("index out of bounds!");
	}

	return obj;
}

CObject * CALE::createObjectFromIndex(int index){
	CObject *obj = NULL;


	// check second operand valid object..
	switch(result_object_instruction[index].type){
	case CVariable::VAR_TYPE::INTEGER:
		obj=NEW_INTEGER();
		((CInteger *)obj)->m_value = ((CInteger *)(*result_object_instruction[index].stkObject))->m_value;
		break;
	case CVariable::VAR_TYPE::NUMBER:
		obj = NEW_NUMBER();
		((CNumber *)obj)->m_value = ((CNumber *)(*result_object_instruction[index].stkObject))->m_value;
		break;
	case CVariable::VAR_TYPE::STRING:
		obj = NEW_STRING();
		((CString *)obj)->m_value = ((CString *)(*result_object_instruction[index].stkObject))->m_value;
		break;
	case CVariable::VAR_TYPE::BOOLEAN:
		obj = NEW_BOOLEAN();
		((CBoolean *)obj)->m_value = ((CBoolean *)(*result_object_instruction[index].stkObject))->m_value;
		break;
	default:
		obj = *result_object_instruction[index].stkObject;
		break;
	}

	return obj;
}

bool CALE::loadValue(CCompiler::tInfoAsmOp *iao){

	CObject *obj = ((CObject *)iao->index_op2);
	CVariable *var=NULL;
	CScope *_lc = iao->ast_node->scope_ptr;
	CScope::tInfoRegisteredVar *irv=NULL;

	//sprintf(print_aux_load_value,"UNDEFINED");
	switch(iao->index_op1){
	case CCompiler::LOAD_TYPE::LOAD_TYPE_CONSTANT:

		if(obj->getObjectType() != CObject::OBJECT_TYPE::VARIABLE){
			print_error_cr("Load type %i is not variable type",obj->getObjectType() );
			return false;
		}

		var = (CVariable *)obj;

		switch(iao->variable_type){
			default:
				print_error_cr("Unexpected constant as %i",iao->variable_type);
				return false;
				break;
			case CVariable::VAR_TYPE::INTEGER:
				if(!pushInteger(((CInteger *)var)->m_value)) return false;
				break;
			case CVariable::VAR_TYPE::BOOLEAN:
				if(!pushBoolean(((CBoolean *)var)->m_value)) return false;
				break;
			case CVariable::VAR_TYPE::STRING:
				if(!pushString(((CString *)var)->m_value)) return false;
				break;
			case CVariable::VAR_TYPE::NUMBER:
				if(!pushNumber(((CNumber *)var)->m_value)) return false;
				break;
			}

		//sprintf(print_aux_load_value,"CONST(%s)",value_symbol.c_str());
		break;
	case CCompiler::LOAD_TYPE::LOAD_TYPE_VARIABLE:

		if(_lc == NULL){
			print_error_cr("Local scope null");
			return false;
		}

		if((irv = _lc->getInfoRegisteredSymbol(iao->ast_node->value_symbol))==NULL){
			print_error_cr("cannot find symbol \"%s\"",iao->ast_node->value_symbol.c_str());
			return false;
		}

		// generic object pushed ...
		if(!pushObject(&irv->m_obj,true)) return false;
		//stkObject[n_stkObject++]=
		//loadObject(obj);

		//sprintf(print_aux_load_value,"VAR(%s)",value_symbol.c_str());
		break;
	case CCompiler::LOAD_TYPE::LOAD_TYPE_ARGUMENT:
		//sprintf(print_aux_load_value,"ARG(%s)",value_symbol.c_str());
		break;
	default:
		break;

	}

	return true;
}

bool CALE::assignObjectFromIndex(CObject **obj, int index){

	CVariable *aux_var=NULL;


	// if undefined, create new by default ...
	if(*obj == CScope::UndefinedSymbol){

		if((*obj = createObjectFromIndex(index)) == NULL){
			return false;
		}
	}


	if((*obj)->getObjectType() == CObject::VARIABLE){
		aux_var = (CVariable *)(*obj);
	}

	// finally assign the value ...
	switch(result_object_instruction[index].type){
		case CVariable::VAR_TYPE::INTEGER:
			if(aux_var == NULL) {
				print_error_cr("Expected variable type!");
				return false;
			}
			if(aux_var->getVariableType() == CVariable::VAR_TYPE::INTEGER){
				((CInteger *)aux_var)->m_value=((CInteger *)(*result_object_instruction[index].stkObject))->m_value;
			}else if(aux_var->getVariableType() == CVariable::VAR_TYPE::NUMBER){
				((CNumber *)aux_var)->m_value=((CInteger *)(*result_object_instruction[index].stkObject))->m_value;
			}else
			{
				print_error_cr("var is not type integer!");
				return false;
			}
			break;
		case CVariable::VAR_TYPE::NUMBER:
			if(aux_var == NULL) {print_error_cr("Expected variable type!");return false;}
			if(aux_var->getVariableType() == CVariable::VAR_TYPE::INTEGER){
				((CInteger *)aux_var)->m_value=((CNumber *)(*result_object_instruction[index].stkObject))->m_value;
			}else if(aux_var->getVariableType() == CVariable::VAR_TYPE::NUMBER){
				((CNumber *)aux_var)->m_value=((CNumber *)(*result_object_instruction[index].stkObject))->m_value;
			}else
			{
				print_error_cr("var is not type number!");
				return false;
			}
			break;
		case CVariable::VAR_TYPE::STRING:
			if(aux_var == NULL) {print_error_cr("Expected variable type!");return false;}
			if(aux_var->getVariableType() == CVariable::VAR_TYPE::STRING){
				((CString  *)aux_var)->m_value= ((CString *)(*result_object_instruction[index].stkObject))->m_value;
			}else
			{
				print_error_cr("var is not type string!");
				return false;
			}

			break;
		case CVariable::VAR_TYPE::BOOLEAN:
			if(aux_var == NULL) {print_error_cr("Expected variable type!");return false;}
			if(aux_var->getVariableType() == CVariable::VAR_TYPE::BOOLEAN){
				((CBoolean  *)aux_var)->m_value= ((CBoolean *)(*result_object_instruction[index].stkObject))->m_value;
			}else
			{
				print_error_cr("var is not type boolean!");
				return false;
			}
			break;

		// pointer assigment ...
		case CVariable::VAR_TYPE::VECTOR: // vector object ...
			*obj = ((CVector *)(*result_object_instruction[index].stkObject));
			break;
		case CVariable::VAR_TYPE::FUNCTION: // function object
			*obj = ((CScriptFunction *)(*result_object_instruction[index].stkObject));
			break;
		case CVariable::VAR_TYPE::OBJECT: // generic object
			*obj = (*result_object_instruction[index].stkObject);
			break;

		default:
				print_error_cr("Unknow assignment 2!");
				return false;
				break;
		}


	return true;
}

bool CALE::performInstruction(int idx_instruction,CCompiler::tInfoAsmOp * instruction,CScriptFunction *sf,int & jmp_to_statment){

	string 	aux_string;
	bool	aux_boolean;
	string symbol;
	CObject **obj;


	jmp_to_statment=-1;

	current_asm_instruction = idx_instruction;

	//CScope *_lc = instruction->ast_node->scope_ptr;

	int index_op1 = instruction->index_op1;
	int index_op2 = instruction->index_op2;




	switch(instruction->operator_type){
	default:
		print_error_cr("operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
		break;
	case CCompiler::NOP: // ignore ...
		break;
	case CCompiler::LOAD: // load value in function of value/constant ...

		if(!loadValue(instruction)){
			return false;
		}

		break;
		case CCompiler::MOV: // mov value expression to var

			// ok load object pointer ...
			if(result_object_instruction[index_op1].isAssignable) {// == CVariable::VAR_TYPE::OBJECT){

				// get pointer object (can be assigned)
				obj = result_object_instruction[index_op1].stkObject;



				if(!assignObjectFromIndex(obj,index_op2))
						return false;

			}else{
				print_error_cr("Expected object l-value mov");
				return false;
			}

			break;
		case CCompiler::EQU:  // == --> boolean && boolean or string && string or number && number

			if(OP1_AND_OP2_ARE_BOOLEANS) {
				if(!pushBoolean(LOAD_BOOL_OP(index_op1) == LOAD_BOOL_OP(index_op2))) return false;
			}else if(OP1_AND_OP2_ARE_STRINGS){
				if(!pushBoolean(LOAD_STRING_OP(index_op1) == LOAD_STRING_OP(index_op2))) return false;
			}else if (OP1_AND_OP2_ARE_NUMBERS){
				if(!pushBoolean(LOAD_NUM_OP(index_op1) == LOAD_NUM_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as string, number or boolean!");
				return false;
			}

			break;
		case CCompiler::LT:  // <
			if (OP1_AND_OP2_ARE_NUMBERS){
				if(!pushBoolean(LOAD_NUM_OP(index_op1) < LOAD_NUM_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}
			break;
		case CCompiler::LTE:  // <=

			if (OP1_AND_OP2_ARE_NUMBERS){
				if(!pushBoolean(LOAD_NUM_OP(index_op1) <= LOAD_NUM_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}
			break;
		case CCompiler::NOT: // !
			if (result_object_instruction[index_op1].type == CVariable::VAR_TYPE::BOOLEAN){
				if(!pushBoolean(!LOAD_BOOL_OP(index_op1))) return false;
			}else{
				print_error_cr("Expected operands 1 as boolean!");
				return false;
			}
			break;
		case CCompiler::GT:  // >
			if (OP1_AND_OP2_ARE_NUMBERS){
				if(!pushBoolean(LOAD_NUM_OP(index_op1) > LOAD_NUM_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}
			break;
		case CCompiler::GTE: // >=
			if (OP1_AND_OP2_ARE_NUMBERS){
				if(!pushBoolean(LOAD_NUM_OP(index_op1) >= LOAD_NUM_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}
			break;

		case CCompiler::ADD: // +

			// get indexes and check whether is possible or not its calculation.
			// check indexes
			CHECK_VALID_INDEXES;

			// check types ...
			if(OP1_AND_OP2_ARE_NUMBERS) {

				if(result_object_instruction[index_op1].type == CVariable::VAR_TYPE::INTEGER){ // operation will result as integer.
					if(!pushInteger(LOAD_NUM_OP(index_op1) + LOAD_NUM_OP(index_op2))) return false;
				}else{ // as float ...
					if(!pushNumber(LOAD_NUM_OP(index_op1) + LOAD_NUM_OP(index_op2))) return false;
				}
			}else if(OP1_IS_STRING_AND_OP2_IS_NUMBER){ // concatenate string + number

				aux_string =  LOAD_STRING_OP(index_op1);

				if(result_object_instruction[index_op2].type == CVariable::VAR_TYPE::INTEGER)
					aux_string = aux_string + CStringUtils::intToString(LOAD_NUM_OP(index_op2));
				else
					aux_string = aux_string + CStringUtils::intToString(LOAD_NUM_OP(index_op2));

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

				string var_type1=(*result_object_instruction[index_op1].stkObject)->getClassStr(),
					   var_type2=(*result_object_instruction[index_op2].stkObject)->getClassStr();


				//print_error_cr("Expected operands as number+number, string+string, string+number or string + boolean!");


				print_error_cr("Error at line %i cannot perform operator \"%s\" +  \"%s\"",
						instruction->ast_node->definedValueline,
						var_type1.c_str(),
						var_type2.c_str());
				return false;
			}

			break;

		case CCompiler::LOGIC_AND: // &&
			if(OP1_AND_OP2_ARE_BOOLEANS) {
				if(!pushBoolean(LOAD_BOOL_OP(index_op1) && LOAD_BOOL_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands boolean!");
				return false;
			}
			break;
		case CCompiler::LOGIC_OR:  // ||
			if(OP1_AND_OP2_ARE_BOOLEANS) {
				if(!pushBoolean(LOAD_BOOL_OP(index_op1) || LOAD_BOOL_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands boolean!");
				return false;
			}
			break;
		case CCompiler::DIV: // /
			if(OP1_AND_OP2_ARE_NUMBERS) {
				if((result_object_instruction[index_op1].type == CVariable::NUMBER)){
					if(!pushNumber(LOAD_NUM_OP(index_op1) / LOAD_NUM_OP(index_op2))) return false;
				}else{
					if(!pushInteger(LOAD_NUM_OP(index_op1) / LOAD_NUM_OP(index_op2))) return false;
				}
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}

			break;
		case CCompiler::MUL: // *
			if(OP1_AND_OP2_ARE_NUMBERS) {
				if(result_object_instruction[index_op1].type == CVariable::NUMBER){
					if(!pushNumber(LOAD_NUM_OP(index_op1) * LOAD_NUM_OP(index_op2))) return false;
				}else {
					pushInteger(LOAD_NUM_OP(index_op1) * LOAD_NUM_OP(index_op2));
				}
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}
			break;
		case CCompiler::MOD:  // %
			if(OP1_AND_OP2_ARE_NUMBERS) {
				if(result_object_instruction[index_op1].type == CVariable::NUMBER){
					if(!pushNumber(fmod(LOAD_NUM_OP(index_op1),LOAD_NUM_OP(index_op2)))) return false;
				}else{
					if(!pushInteger(fmod(LOAD_NUM_OP(index_op1), LOAD_NUM_OP(index_op2)))) return false;
				}
			}else{
				print_error_cr("Expected both operands as number!");
				return false;
			}

			break;
		case CCompiler::AND: // bitwise logic and
			if((result_object_instruction[index_op1].type == CVariable::INTEGER) && (result_object_instruction[index_op2].type == CVariable::INTEGER)){
				if(!pushInteger(LOAD_INT_OP(index_op1) & LOAD_INT_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		case CCompiler::OR: // bitwise logic or
			if((result_object_instruction[index_op1].type == CVariable::INTEGER) && (result_object_instruction[index_op2].type == CVariable::INTEGER)){
				if(!pushInteger(LOAD_INT_OP(index_op1) | LOAD_INT_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}

			break;
		case CCompiler::XOR: // logic xor
			if((result_object_instruction[index_op1].type == CVariable::INTEGER) && (result_object_instruction[index_op2].type == CVariable::INTEGER)){
				if(!pushInteger(LOAD_INT_OP(index_op1) ^ LOAD_INT_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		case CCompiler::SHL: // shift left
			if((result_object_instruction[index_op1].type == CVariable::INTEGER) && (result_object_instruction[index_op2].type == CVariable::INTEGER)){
				if(!pushInteger(LOAD_INT_OP(index_op1) << LOAD_INT_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		case CCompiler::SHR: // shift right
			if((result_object_instruction[index_op1].type == CVariable::INTEGER) && (result_object_instruction[index_op2].type == CVariable::INTEGER)){
				if(!pushInteger(LOAD_INT_OP(index_op1) >> LOAD_INT_OP(index_op2))) return false;
			}else{
				print_error_cr("Expected both operands as integer types!");
				return false;
			}
			break;
		// special internal ops...
		case CCompiler::JMP:
			jmp_to_statment = index_op1;
			break;
		case CCompiler::JNT: // goto if not true ... goes end to conditional.

			// load boolean var and jmp if true...
			jmp_to_statment = index_op1;
			break;
		case CCompiler::JT: // goto if true ... goes end to conditional.
			if(n_stkBoolean > 0){
				if(LOAD_BOOL_OP(n_stkBoolean-1)){
					jmp_to_statment = index_op1;
				}
			}else{
				print_error_cr("No boolean elements");
				return false;
			}
			break;
		case CCompiler::CALL: // calling function after all of args are processed...

			// check whether signatures matches or not ...
			// 1. get function object ...
			if(result_object_instruction[index_op1].type == CVariable::FUNCTION){
				CScriptFunction * fun = (CScriptFunction *)(*result_object_instruction[index_op1].stkObject);//stkFunction[result_object_instruction[index_op1].index];//[stkInteger[result_object_instruction[index_op2].index]];

				if(!CVirtualMachine::execute(fun, &m_functionArgs)){
					return false;
				}

				// finally set result value into stkObject...
				if(!pushObject(fun->getReturnObjectPtr())){
					return false;
				}
			}
			break;
		case CCompiler::PUSH: // push arg instruction...
			m_functionArgs.push_back(getObjectFromIndex(index_op1));
			break;
		case CCompiler::CLR: // clear args
			m_functionArgs.clear();
			break;
		case CCompiler::VGET: // vector access after each index is processed...
			// index_op1 is vector, index op2 is index...
			if(result_object_instruction[index_op1].type == CVariable::VECTOR){
				if(result_object_instruction[index_op2].type == CVariable::INTEGER){
					// determine object ...
					CVector * vec = (CVector *)(*result_object_instruction[index_op1].stkObject);//[stkInteger[result_object_instruction[index_op2].index]];
					int v_index = LOAD_NUM_OP(index_op2);

					print_info_cr("%i",v_index);

					// check indexes ...
					if(v_index < 0 || v_index >= (int)vec->m_value.size()){
						print_error_cr("Line %i. Index vector out of bounds!",instruction->ast_node->definedValueline);
						return false;
					}

					if(!pushObject(&vec->m_value[v_index],true)){
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
		case CCompiler::VPUSH: // Value push for vector
			if(result_object_instruction[index_op1].type == CVariable::VECTOR){
				CVector * vec = (CVector *)(*result_object_instruction[index_op1].stkObject);
				vec->m_value.push_back(createObjectFromIndex(index_op2));
			}else{
				print_error_cr("Expected operand 1 as vector");
				return false;
			}
			break;
		case CCompiler::VEC: // Create new vector object...

			pushVector(NEW_VECTOR());

			break;

		case CCompiler::RET:

			if(!assignObjectFromIndex(sf->getReturnObjectPtr(),instruction->index_op1)){
				return false;
			}

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
	n_stkObject=
	n_stkVector=
	n_stkFunction=
	current_asm_instruction=0;

	memset(result_object_instruction,0,sizeof(result_object_instruction));
	m_functionArgs.clear();

}

CALE::~CALE(){

	reset();

	for(int i = 0 ; i < n_totalIntegerPointers; i++){
		delete 	stkInteger[i];
	}

	for(int i = 0 ; i < n_totalNumberPointers; i++){
		delete stkNumber[i];
	}

	for(int i = 0 ; i < n_totalBooleanPointers; i++){
		delete stkBoolean[i];
	}

	for(int i = 0 ; i < n_totalStringPointers; i++){
		delete stkString[i];
	}


	n_totalIntegerPointers=
	n_totalNumberPointers=
	n_totalBooleanPointers=
	n_totalStringPointers=0;



}




