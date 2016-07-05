#include "zg_script.h"

CVirtualMachine * CVirtualMachine::m_virtualMachine = NULL;
//vector<CVirtualMachine::CALE> CVirtualMachine::ALE;

// general
#define CHECK_VALID_INDEXES \
if(!(index_op1 >= 0 && index_op1 <=current_asm_instruction)) { print_error_cr("instruction 1 out of bounds"); return false;} \
if(!(index_op2 >= 0 && index_op2 <=current_asm_instruction)) { print_error_cr("instruction 2 out of bounds"); return false;} \
if(!(index_op2 >= index_op1 )) { print_error_cr("invalid indexes"); return false;}

/*
#define CHECK_2ND_VALID_CREATE_OBJECT_OPERAND(idx) \
if(!(asm_instruction_expression[idx].type == CVariable::VAR_TYPE::INTEGER || \
     asm_instruction_expression[idx].type == CVariable::VAR_TYPE::NUMBER || \
     asm_instruction_expression[idx].type == CVariable::VAR_TYPE::STRING || \
	 asm_instruction_expression[idx].type == CVariable::VAR_TYPE::BOOLEAN)) \
{ print_error_cr("invalid create index type (%i)",asm_instruction_expression[idx].type); return false;}

#define CREATE_OBJECT_FROM_OPERAND(idx) \
(asm_instruction_expression[idx].type == CVariable::VAR_TYPE::INTEGER) ? NEW_INTEGER
     asm_instruction_expression[idx].type == CVariable::VAR_TYPE::NUMBER || \
     asm_instruction_expression[idx].type == CVariable::VAR_TYPE::STRING || \
	 asm_instruction_expression[idx].type == CVariable::VAR_TYPE::BOOLEAN)) \*/

#define LOAD_NUM_OP(idx) \
		(asm_instruction_expression[idx].type == CVariable::VAR_TYPE::INTEGER)? stkInteger[asm_instruction_expression[idx].index].m_value:\
		stkNumber[asm_instruction_expression[idx].index].m_value



// Check types
#define IS_NUMBER(idx) \
((asm_instruction_expression[idx].type == CVariable::VAR_TYPE::NUMBER) || \
(asm_instruction_expression[idx].type == CVariable::VAR_TYPE::INTEGER))



#define OP1_AND_OP2_ARE_NUMBERS \
(IS_NUMBER(index_op1) && IS_NUMBER(index_op2))

#define OP1_IS_STRING_AND_OP2_IS_NUMBER \
(asm_instruction_expression[index_op1].type == CVariable::VAR_TYPE::STRING) && \
IS_NUMBER(index_op2)

#define OP1_IS_STRING_AND_OP2_IS_BOOLEAN \
(asm_instruction_expression[index_op1].type == CVariable::VAR_TYPE::STRING) && \
(asm_instruction_expression[index_op2].type == CVariable::VAR_TYPE::BOOLEAN)


#define OP1_AND_OP2_ARE_BOOLEANS \
(asm_instruction_expression[index_op1].type == CVariable::VAR_TYPE::BOOLEAN) && \
(asm_instruction_expression[index_op2].type == CVariable::VAR_TYPE::BOOLEAN)

#define OP1_AND_OP2_ARE_STRINGS \
(asm_instruction_expression[index_op1].type == CVariable::VAR_TYPE::STRING) && \
(asm_instruction_expression[index_op2].type == CVariable::VAR_TYPE::STRING)


// NUMBER result behaviour.
// this is the combination for number operations:
//
// op1 | op2 |  R
// ----+-----+----
//  i  |  i  |  i
//  i  |  f  |  i
//  f  |  i  |  f
//  f  |  f  |  f


class CALE{

public:

	CALE(){
		reset();

	}

	CInteger *pushInteger(int  init_value){
		if(n_stkInteger ==MAX_PER_TYPE_OPERATIONS){
			print_error_cr("Max int operands");
			return NULL;
		}
		asm_instruction_expression[current_asm_instruction++]={CVariable::VAR_TYPE::INTEGER,n_stkInteger};
		stkInteger[n_stkInteger].m_value=init_value;


		return &stkInteger[n_stkInteger++];
	}

	CBoolean *pushBoolean(bool init_value){
		if(n_stkBoolean ==MAX_PER_TYPE_OPERATIONS){
			print_error_cr("Reached max bool operations");
			return NULL;
		}
		asm_instruction_expression[current_asm_instruction++]={CVariable::VAR_TYPE::BOOLEAN,n_stkBoolean};
		stkBoolean[n_stkBoolean].m_value=init_value;

		return &stkBoolean[n_stkBoolean++];
	}

	CNumber *pushNumber(float init_value){
		if(n_stkNumber ==MAX_PER_TYPE_OPERATIONS){
			print_error_cr("Reached max number operations");
			return NULL;
		}
		asm_instruction_expression[current_asm_instruction++]={CVariable::VAR_TYPE::NUMBER,n_stkNumber};
		stkNumber[n_stkNumber].m_value=init_value;

		return &stkNumber[n_stkNumber++];
	}

	CString *pushString(const string & init_value){
		if(n_stkString ==MAX_PER_TYPE_OPERATIONS){
			print_error_cr("Reached max string operations");
			return NULL;
		}
		asm_instruction_expression[current_asm_instruction++]={CVariable::VAR_TYPE::STRING,n_stkString};
		stkString[n_stkString].m_value=init_value;

		return &stkString[n_stkString++];

	}

	bool pushObject(CObject ** init_value){
		if(n_stkObject ==MAX_PER_TYPE_OPERATIONS){
			print_error_cr("Reached max object operations");
			return false;
		}
		asm_instruction_expression[current_asm_instruction++]={CVariable::VAR_TYPE::OBJECT,n_stkObject};
		stkObject[n_stkObject]=init_value;


		n_stkObject++;

		return true;

	}

	CVector **pushVector(CVector * init_value){
		if(n_stkVector ==MAX_PER_TYPE_OPERATIONS){
			print_error_cr("Reached max vector operations");
			return NULL;
		}
		asm_instruction_expression[current_asm_instruction++]={CVariable::VAR_TYPE::VECTOR,n_stkVector};
		stkVector[n_stkVector]=init_value;

		return &stkVector[n_stkVector++];
	}

	CScriptFunction **pushFunction(CScriptFunction * init_value){
		if(n_stkFunction ==MAX_PER_TYPE_OPERATIONS){
			print_error_cr("Reached max function operations");
			return NULL;
		}
		asm_instruction_expression[current_asm_instruction++]={CVariable::VAR_TYPE::FUNCTION,n_stkFunction};
		stkFunction[n_stkFunction]=init_value;

		return &stkFunction[n_stkFunction++];
	}


	bool loadObject(CObject **obj){

		if(*obj==NULL) {print_error_cr("Object null");return false;}

		if((*obj)->getObjectType() == CObject::OBJECT_TYPE::VARIABLE){

			CVariable *var = (CVariable *)obj;

			switch(var->getVariableType()){
				default:
				case CVariable::VAR_TYPE::OBJECT:
					pushObject(obj);
					break;

				case CVariable::VAR_TYPE::INTEGER:
					pushInteger(((CInteger *)var)->m_value);
					break;
				case CVariable::VAR_TYPE::BOOLEAN:
					pushBoolean(((CBoolean *)var)->m_value);
					break;
				case CVariable::VAR_TYPE::STRING:
					pushString(((CString *)var)->m_value);
					break;
				case CVariable::VAR_TYPE::NUMBER:
					pushNumber(((CNumber *)var)->m_value);
					break;
				case CVariable::VAR_TYPE::VECTOR:
					pushVector((CVector *)var);
					break;
				case CVariable::VAR_TYPE::FUNCTION:
					pushFunction((CScriptFunction *)var);
					break;


			}
		}else{ // push generic object
			pushObject(obj);
		}

		return true;
	}

	CObject * getObjectFromIndex(int index){
		CObject *obj=NULL;

		if(index >= 0 && index < this->current_asm_instruction){
			switch(asm_instruction_expression[index].type){
			case CVariable::VAR_TYPE::INTEGER:
				obj= &stkInteger[asm_instruction_expression[index].index];
				break;
			case CVariable::VAR_TYPE::NUMBER:
				obj = &stkNumber[asm_instruction_expression[index].index];
				break;
			case CVariable::VAR_TYPE::STRING:
				obj = &stkString[asm_instruction_expression[index].index];
				break;
			case CVariable::VAR_TYPE::BOOLEAN:
				obj = &stkBoolean[asm_instruction_expression[index].index];
				break;
			case CVariable::VAR_TYPE::FUNCTION: // function object
				obj = stkFunction[asm_instruction_expression[index].index];
				break;
			case CVariable::VAR_TYPE::VECTOR: // vector object ...
				obj = stkVector[asm_instruction_expression[index].index];
				break;
			case CVariable::VAR_TYPE::OBJECT: // generic object
				obj = *stkObject[asm_instruction_expression[index].index];
				break;
			default:
				print_error_cr("Unknown create object from!");
				break;
			}
		}else{
			print_error_cr("index out of bounds!");
		}

		return obj;
	}

	CObject * createObjectFromIndex(int index){
		CObject *obj = NULL;


		// check second operand valid object..
		switch(asm_instruction_expression[index].type){
		case CVariable::VAR_TYPE::INTEGER:
			obj=NEW_INTEGER();
			((CInteger *)obj)->m_value = stkInteger[asm_instruction_expression[index].index].m_value;
			break;
		case CVariable::VAR_TYPE::NUMBER:
			obj = NEW_NUMBER();
			((CNumber *)obj)->m_value = stkNumber[asm_instruction_expression[index].index].m_value;
			break;
		case CVariable::VAR_TYPE::STRING:
			obj = NEW_STRING();
			((CString *)obj)->m_value = stkString[asm_instruction_expression[index].index].m_value;
			break;
		case CVariable::VAR_TYPE::BOOLEAN:
			obj = NEW_BOOLEAN();
			((CBoolean *)obj)->m_value = stkBoolean[asm_instruction_expression[index].index].m_value;
			break;
		case CVariable::VAR_TYPE::FUNCTION: // function object
			obj = stkFunction[asm_instruction_expression[index].index];
			break;
		case CVariable::VAR_TYPE::VECTOR: // vector object ...
			obj = stkVector[asm_instruction_expression[index].index];
			break;
		case CVariable::VAR_TYPE::OBJECT: // generic object
			obj = *stkObject[asm_instruction_expression[index].index];
			break;
		default:
			print_error_cr("Unknow create object from!");
			break;

		}

		return obj;
	}


	bool loadValue(CCompiler::tInfoAsmOp *iao){

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
					pushInteger(((CInteger *)var)->m_value);
					break;
				case CVariable::VAR_TYPE::BOOLEAN:
					pushBoolean(((CBoolean *)var)->m_value);
					break;
				case CVariable::VAR_TYPE::STRING:
					pushString(((CString *)var)->m_value);
					break;
				case CVariable::VAR_TYPE::NUMBER:
					pushNumber(((CNumber *)var)->m_value);
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
			stkObject[n_stkObject++]=&irv->m_obj;
			//loadObject(obj);

			//sprintf(print_aux_load_value,"VAR(%s)",value_symbol.c_str());
			break;
		case CCompiler::LOAD_TYPE::LOAD_TYPE_ARGUMENT:
			//sprintf(print_aux_load_value,"ARG(%s)",value_symbol.c_str());
			break;
		default:
			break;

		}

		return false;
	}


	bool performInstruction(CCompiler::tInfoAsmOp * instruction,int & jmp_to_statment){

		string 	aux_string;
		bool	aux_boolean;
		string symbol;

		CVariable *aux_var;
		jmp_to_statment=-1;

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
				if(asm_instruction_expression[index_op1].type == CVariable::VAR_TYPE::OBJECT){

					// get pointer object (can be assigned)
					CObject **obj = stkObject[asm_instruction_expression[index_op1].index];
					aux_var = NULL;


					// if undefined, create new if required ...
					if(*obj == CScope::UndefinedSymbol){

						if((*obj = createObjectFromIndex(index_op2)) == NULL){
							return false;
						}
					}

					if((*obj)->getObjectType() == CObject::VARIABLE){
						aux_var = (CVariable *)(*obj);
					}


					// finally assign the value ...
					switch(asm_instruction_expression[index_op2].type){
						case CVariable::VAR_TYPE::INTEGER:
							if(aux_var == NULL) {print_error_cr("Expected variable type!");return false;}
							if(aux_var->getVariableType() == CVariable::VAR_TYPE::INTEGER){
								((CInteger *)aux_var)->m_value=stkInteger[asm_instruction_expression[index_op2].index].m_value;
							}else if(aux_var->getVariableType() == CVariable::VAR_TYPE::NUMBER){
								((CNumber *)aux_var)->m_value=stkInteger[asm_instruction_expression[index_op2].index].m_value;
							}else
							{
								print_error_cr("var is not type integer!");
								return false;
							}
							break;
						case CVariable::VAR_TYPE::NUMBER:
							if(aux_var == NULL) {print_error_cr("Expected variable type!");return false;}
							if(aux_var->getVariableType() == CVariable::VAR_TYPE::INTEGER){
								((CInteger *)aux_var)->m_value=stkNumber[asm_instruction_expression[index_op2].index].m_value;
							}else if(aux_var->getVariableType() == CVariable::VAR_TYPE::NUMBER){
								((CNumber *)aux_var)->m_value=stkNumber[asm_instruction_expression[index_op2].index].m_value;
							}else
							{
								print_error_cr("var is not type number!");
								return false;
							}
							break;
						case CVariable::VAR_TYPE::STRING:
							if(aux_var == NULL) {print_error_cr("Expected variable type!");return false;}
							if(aux_var->getVariableType() == CVariable::VAR_TYPE::STRING){
								((CString  *)aux_var)->m_value= stkString[asm_instruction_expression[index_op2].index].m_value;
							}else
							{
								print_error_cr("var is not type string!");
								return false;
							}

							break;
						case CVariable::VAR_TYPE::BOOLEAN:
							if(aux_var == NULL) {print_error_cr("Expected variable type!");return false;}
							if(aux_var->getVariableType() == CVariable::VAR_TYPE::BOOLEAN){
								((CBoolean  *)aux_var)->m_value= stkBoolean[asm_instruction_expression[index_op2].index].m_value;
							}else
							{
								print_error_cr("var is not type boolean!");
								return false;
							}
							break;

						// pointer assigment ...
						case CVariable::VAR_TYPE::VECTOR: // vector object ...
							*obj = stkVector[asm_instruction_expression[index_op2].index];
							break;
						case CVariable::VAR_TYPE::OBJECT: // generic object
							*obj = *stkObject[asm_instruction_expression[index_op2].index];
							break;
						case CVariable::VAR_TYPE::FUNCTION: // function object
							*obj = stkFunction[asm_instruction_expression[index_op2].index];
							break;
						default:
								print_error_cr("Unknow assignment 2!");
								return false;
								break;
						}

				}else{
					print_error_cr("Expected object l-value mov");
					return false;
				}


				/*if(_lc == NULL){
					print_error_cr("Local scope null");
					return false;
				}


				// get type l-value type (should be symbol or vector access)...
				//if(asm_instruction_expression[index_op1].type ==


				if((irv = _lc->getInfoRegisteredSymbol(instruction->ast_node->value_symbol))==NULL){
					print_error_cr("cannot find symbol \"%s\"",instruction->ast_node->value_symbol.c_str());
					return false;
				}

				// second operand must match with variable type...
				// check whether 1st operand is undefined ...
				if(irv->m_obj == CScope::UndefinedSymbol){ // create object type and assign it ...


					aux_var = NULL;
					if((irv->m_obj = createObjectFromIndex(index_op2)) == NULL){
						return false;
					}

					// check second operand valid object..
					/*switch(asm_instruction_expression[index_op2].type){
					case CVariable::VAR_TYPE::INTEGER:
						aux_var=NEW_INTEGER();
						 irv->m_obj =aux_var;
						break;
					case CVariable::VAR_TYPE::NUMBER:
						aux_var = NEW_NUMBER();
						irv->m_obj =aux_var;
						break;
					case CVariable::VAR_TYPE::STRING:
						irv->m_obj = NEW_STRING();
						irv->m_obj =aux_var;
						break;
					case CVariable::VAR_TYPE::BOOLEAN:
						irv->m_obj = NEW_BOOLEAN();
						irv->m_obj =aux_var;
						break;
					case CVariable::VAR_TYPE::OBJECT: // generic object
						irv->m_obj = stkObject[asm_instruction_expression[index_op2].index];
						break;
					case CVariable::VAR_TYPE::VECTOR: // vector object ...
						aux_var = stkVector[asm_instruction_expression[index_op2].index];
						irv->m_obj =aux_var;
						break;
					case CVariable::VAR_TYPE::FUNCTION: // function object
						aux_var = stkFunction[asm_instruction_expression[index_op2].index];
						irv->m_obj =aux_var;
						break;
					default:
						print_error_cr("Unknow assignment 1!");
						return false;
						break;

					}

				}



				if(irv->m_obj->getObjectType() == CObject::OBJECT_TYPE::VARIABLE){
					aux_var = (CVariable *)irv->m_obj;
				}



				// ok variable is already loaded...

				// check second operand and assign to variable regarding its type value ...
*/
				break;
			case CCompiler::EQU:  // == --> boolean && boolean or string && string or number && number

				if(OP1_AND_OP2_ARE_BOOLEANS) {
					pushBoolean(stkBoolean[asm_instruction_expression[index_op1].index].m_value == stkBoolean[asm_instruction_expression[index_op2].index].m_value);
				}else if(OP1_AND_OP2_ARE_STRINGS){
					pushBoolean(stkString[asm_instruction_expression[index_op1].index].m_value == stkString[asm_instruction_expression[index_op2].index].m_value);
				}else if (OP1_AND_OP2_ARE_NUMBERS){
					pushBoolean(LOAD_NUM_OP(index_op1) == LOAD_NUM_OP(index_op2));
				}else{
					print_error_cr("Expected both operands as string, number or boolean!");
					return false;
				}

				break;
			case CCompiler::LT:  // <
				if (OP1_AND_OP2_ARE_NUMBERS){
					pushBoolean(LOAD_NUM_OP(index_op1) < LOAD_NUM_OP(index_op2));
				}else{
					print_error_cr("Expected both operands as number!");
					return false;
				}
				break;
			case CCompiler::LTE:  // <=

				if (OP1_AND_OP2_ARE_NUMBERS){
					pushBoolean(LOAD_NUM_OP(index_op1) <= LOAD_NUM_OP(index_op2));
				}else{
					print_error_cr("Expected both operands as number!");
					return false;
				}
				break;
			case CCompiler::NOT: // !
				if (asm_instruction_expression[index_op1].type == CVariable::VAR_TYPE::BOOLEAN){
					pushBoolean(!stkBoolean[asm_instruction_expression[index_op1].index].m_value);
				}else{
					print_error_cr("Expected operands 1 as boolean!");
					return false;
				}
				break;
			case CCompiler::GT:  // >
				if (OP1_AND_OP2_ARE_NUMBERS){
					pushBoolean(LOAD_NUM_OP(index_op1) > LOAD_NUM_OP(index_op2));
				}else{
					print_error_cr("Expected both operands as number!");
					return false;
				}
				break;
			case CCompiler::GTE: // >=
				if (OP1_AND_OP2_ARE_NUMBERS){
					pushBoolean(LOAD_NUM_OP(index_op1) >= LOAD_NUM_OP(index_op2));
				}else{
					print_error_cr("Expected both operands as number!");
					return false;
				}
				break;
			/*case CCompiler::NEG: // !
				if (asm_instruction_expression[index_op1].type == CVariable::VAR_TYPE::NUMBER || asm_instruction_expression[index_op1].type == CVariable::VAR_TYPE::INTEGER){
					pushBoolean(-LOAD_NUM_OP(index_op1));
				}else{
					print_error_cr("Expected operand as numbers!");
					return false;
				}
				break;*/
			case CCompiler::ADD: // +

				// get indexes and check whether is possible or not its calculation.
				// check indexes
				CHECK_VALID_INDEXES;

				// check types ...
				if(OP1_AND_OP2_ARE_NUMBERS) {

					if(asm_instruction_expression[index_op1].type == CVariable::VAR_TYPE::INTEGER){ // operation will result as integer.
						pushInteger(LOAD_NUM_OP(index_op1) + LOAD_NUM_OP(index_op2));
					}else{ // as float ...
						pushNumber(LOAD_NUM_OP(index_op1) + LOAD_NUM_OP(index_op2));
					}
				}else if(OP1_IS_STRING_AND_OP2_IS_NUMBER){ // concatenate string + number

					aux_string =  stkString[asm_instruction_expression[index_op1].index].m_value;

					if(asm_instruction_expression[index_op2].type == CVariable::VAR_TYPE::INTEGER)
						aux_string = aux_string + CStringUtils::intToString(stkInteger[this->asm_instruction_expression[index_op2].index].m_value);
					else
						aux_string = aux_string + CStringUtils::intToString(stkNumber[this->asm_instruction_expression[index_op2].index].m_value);

					pushString(aux_string);
				}else if(OP1_IS_STRING_AND_OP2_IS_BOOLEAN){ // concatenate string + boolean

					aux_string =  stkString[asm_instruction_expression[index_op1].index].m_value;
					aux_boolean =  stkBoolean[asm_instruction_expression[index_op2].index].m_value;

					if(aux_boolean)
						aux_string = aux_string + "true";
					else
						aux_string = aux_string + "false";

					pushString(aux_string);

				}else if(OP1_AND_OP2_ARE_STRINGS){ // concatenate string + boolean

					pushString(stkString[asm_instruction_expression[index_op1].index].m_value+stkString[asm_instruction_expression[index_op2].index].m_value);

				}else{
					print_error_cr("Expected operands as number+number, string+string, string+number or string + boolean!");
					return false;
				}

				break;
			/*case CCompiler::PRE_INC: // ++a
				if (asm_instruction_expression[index_op1].type == CVariable::VAR_TYPE::NUMBER || asm_instruction_expression[index_op1].type == CVariable::VAR_TYPE::INTEGER){


					pushBoolean(-LOAD_NUM_OP(index_op1));
				}else{
					print_error_cr("Expected operand as number!");
					return false;
				}

				break;
			case CCompiler::POST_INC: // a++
				break;
			case CCompiler::PRE_DEC: // --a
				break;
			case CCompiler::POST_DEC: // a--
				break;*/
			case CCompiler::LOGIC_AND: // &&
				if(OP1_AND_OP2_ARE_BOOLEANS) {
					pushBoolean(stkBoolean[asm_instruction_expression[index_op1].index].m_value && stkBoolean[asm_instruction_expression[index_op2].index].m_value);
				}else{
					print_error_cr("Expected both operands boolean!");
					return false;
				}
				break;
			case CCompiler::LOGIC_OR:  // ||
				if(OP1_AND_OP2_ARE_BOOLEANS) {
					pushBoolean(stkBoolean[asm_instruction_expression[index_op1].index].m_value || stkBoolean[asm_instruction_expression[index_op2].index].m_value);
				}else{
					print_error_cr("Expected both operands boolean!");
					return false;
				}
				break;
			case CCompiler::DIV: // /
				if(OP1_AND_OP2_ARE_NUMBERS) {
					if(asm_instruction_expression[index_op1].index == CVariable::NUMBER)
						pushNumber(LOAD_NUM_OP(index_op1) / LOAD_NUM_OP(index_op2));
					else
						pushInteger(LOAD_NUM_OP(index_op1) / LOAD_NUM_OP(index_op2));
				}else{
					print_error_cr("Expected both operands as number!");
					return false;
				}

				break;
			case CCompiler::MUL: // *
				if(OP1_AND_OP2_ARE_NUMBERS) {
					if(asm_instruction_expression[index_op1].index == CVariable::NUMBER)
						pushNumber(LOAD_NUM_OP(index_op1) * LOAD_NUM_OP(index_op2));
					else
						pushInteger(LOAD_NUM_OP(index_op1) * LOAD_NUM_OP(index_op2));
				}else{
					print_error_cr("Expected both operands as number!");
					return false;
				}
				break;
			case CCompiler::MOD:  // %
				if(OP1_AND_OP2_ARE_NUMBERS) {
					if(asm_instruction_expression[index_op1].index == CVariable::NUMBER){
						pushNumber(fmod(LOAD_NUM_OP(index_op1),LOAD_NUM_OP(index_op2)));
					}else{
						pushInteger(fmod(LOAD_NUM_OP(index_op1), LOAD_NUM_OP(index_op2)));
					}
				}else{
					print_error_cr("Expected both operands as number!");
					return false;
				}

				break;
			case CCompiler::AND: // bitwise logic and
				if((asm_instruction_expression[index_op1].index == CVariable::INTEGER) && (asm_instruction_expression[index_op2].index == CVariable::INTEGER)){
					pushInteger(stkInteger[asm_instruction_expression[index_op1].index].m_value & stkInteger[asm_instruction_expression[index_op2].index].m_value);
				}else{
					print_error_cr("Expected both operands as integer types!");
					return false;
				}
				break;
			case CCompiler::OR: // bitwise logic or
				if((asm_instruction_expression[index_op1].index == CVariable::INTEGER) && (asm_instruction_expression[index_op2].index == CVariable::INTEGER)){
					pushInteger(stkInteger[asm_instruction_expression[index_op1].index].m_value | stkInteger[asm_instruction_expression[index_op2].index].m_value);
				}else{
					print_error_cr("Expected both operands as integer types!");
					return false;
				}

				break;
			case CCompiler::XOR: // logic xor
				if((asm_instruction_expression[index_op1].index == CVariable::INTEGER) && (asm_instruction_expression[index_op2].index == CVariable::INTEGER)){
					pushInteger(stkInteger[asm_instruction_expression[index_op1].index].m_value ^ stkInteger[asm_instruction_expression[index_op2].index].m_value);
				}else{
					print_error_cr("Expected both operands as integer types!");
					return false;
				}
				break;
			case CCompiler::SHL: // shift left
				if((asm_instruction_expression[index_op1].index == CVariable::INTEGER) && (asm_instruction_expression[index_op2].index == CVariable::INTEGER)){
					pushInteger(stkInteger[asm_instruction_expression[index_op1].index].m_value << stkInteger[asm_instruction_expression[index_op2].index].m_value);
				}else{
					print_error_cr("Expected both operands as integer types!");
					return false;
				}
				break;
			case CCompiler::SHR: // shift right
				if((asm_instruction_expression[index_op1].index == CVariable::INTEGER) && (asm_instruction_expression[index_op2].index == CVariable::INTEGER)){
					pushInteger(stkInteger[asm_instruction_expression[index_op1].index].m_value >> stkInteger[asm_instruction_expression[index_op2].index].m_value);
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
					if(stkBoolean[n_stkBoolean-1].m_value){
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
				if(asm_instruction_expression[index_op1].index == CVariable::FUNCTION){
					CScriptFunction * fun = stkFunction[asm_instruction_expression[index_op1].index];//[stkInteger[asm_instruction_expression[index_op2].index]];

					CVirtualMachine::getInstance()->execute(fun, &m_functionArgs);
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
				if(asm_instruction_expression[index_op1].index == CVariable::VECTOR){
					if(asm_instruction_expression[index_op2].index == CVariable::INTEGER){
						// determine object ...
						CVector * vec = stkVector[asm_instruction_expression[index_op1].index];//[stkInteger[asm_instruction_expression[index_op2].index]];
						int index = stkInteger[asm_instruction_expression[index_op2].index].m_value;

						// check indexes ...
						if(index < 0 || index >= (int)vec->m_value.size()){
							print_error_cr("Index vector out of bounds!");
							return false;
						}

						if(!loadObject(&vec->m_value[index])){
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
				if(asm_instruction_expression[index_op1].index == CVariable::VECTOR){
					CVector * vec = stkVector[asm_instruction_expression[index_op1].index];
					vec->m_value.push_back(createObjectFromIndex(index_op2));
				}else{
					print_error_cr("Expected operand 1 as vector");
					return false;
				}
				break;
			case CCompiler::VEC: // Create new vector object...

				stkVector[n_stkVector++] = NEW_VECTOR();

				break;
		}

		return true;
	}

	void reset(){
		n_stkInteger=
		n_stkNumber=
		n_stkBoolean=
		n_stkString=
		n_stkObject=
		n_stkVector=
		n_stkFunction=
		current_asm_instruction=0;

		memset(asm_instruction_expression,0,sizeof(asm_instruction_expression));
		m_functionArgs.clear();
	}


private:

	enum{
		MAX_OPERATIONS_PER_EXPRESSION=(MAX_PER_TYPE_OPERATIONS+1)*CVariable::MAX_VAR_TYPES // +1 because
	};

	CInteger 	stkInteger[MAX_PER_TYPE_OPERATIONS];
	CNumber		stkNumber[MAX_PER_TYPE_OPERATIONS];
	CBoolean 	stkBoolean[MAX_PER_TYPE_OPERATIONS];
	CString  stkString[MAX_PER_TYPE_OPERATIONS];
	CVector  *stkVector[MAX_PER_TYPE_OPERATIONS];
	CScriptFunction *stkFunction[MAX_PER_TYPE_OPERATIONS];
	CObject  **stkObject[MAX_PER_TYPE_OPERATIONS];
	//CVector	 * vector[MAX_OPERANDS];

	int n_stkInteger;
	int n_stkNumber;
	int n_stkBoolean;
	int n_stkString;
	int n_stkObject;
	int n_stkVector;
	int n_stkFunction;

	typedef struct{
		CVariable::VAR_TYPE type;
		int index;
	}tAleInstructionInfo;

	tAleInstructionInfo asm_instruction_expression[MAX_OPERATIONS_PER_EXPRESSION];
	int current_asm_instruction;

	vector<CObject *> m_functionArgs;


};

// static: only defined in this module...
static vector<CALE> ALE; // 1 push per function




CVirtualMachine * CVirtualMachine::getInstance(){
	if(m_virtualMachine == NULL){
		m_virtualMachine = new CVirtualMachine();
	}

	return m_virtualMachine;
}

void CVirtualMachine::destroySingletons(){
	if(m_virtualMachine != NULL){
		delete m_virtualMachine;
		m_virtualMachine = NULL;
	}


}

CVirtualMachine::CVirtualMachine(){

}

//------------------------------------------------------------------------------------------------------------------
//
// EXECUTE ASM CODE
//
float default_value=0;


#define CAST_RESULT_AS_NUMBER(asm_op)\
 (asm_op->result_type == CCompiler::VAR_TYPE::NUMBER)?(*((float *)asm_op->result_obj)):\
 (asm_op->result_type == CCompiler::VAR_TYPE::INTEGER)?(*((int *)asm_op->result_obj)):\
 default_value


#define PERFORM_NUMBER_TWO_OPS(v1,op,v2)\
(v1->result_type == CCompiler::CCompiler::VAR_TYPE::NUMBER && v2->result_type == CCompiler::VAR_TYPE::NUMBER)?(*((float *)v1->result_obj)op(*((float *)v2->result_obj))):\
(v1->result_type == CCompiler::VAR_TYPE::NUMBER && v2->result_type == CCompiler::VAR_TYPE::INTEGER)?(*((float *)v1->result_obj)op(*((int *)v2->result_obj))):\
(v1->result_type == CCompiler::VAR_TYPE::INTEGER && v2->result_type == CCompiler::VAR_TYPE::NUMBER)?(*((int *)v1->result_obj)op(*((float *)v2->result_obj))):\
(v1->result_type == CCompiler::VAR_TYPE::INTEGER && v2->result_type == CCompiler::VAR_TYPE::INTEGER)?(*((int *)v1->result_obj)op(*((int *)v2->result_obj))):\
0


#define ASSIGN_NUMBER(asm_op,v)\
(asm_op->result_type == CCompiler::VAR_TYPE::NUMBER)?(*((float *)asm_op->result_obj)=(v)):\
(asm_op->result_type == CCompiler::VAR_TYPE::INTEGER)?(*((int *)asm_op->result_obj)=(v)):\
 default_value = (v)

#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __VERBOSE_MESSAGE__
#endif



#ifdef  __VERBOSE_MESSAGE__

#define print_vm_cr print_info_cr
#else
#define print_vm_cr(s,...)
#endif



bool CVirtualMachine::execute(CScriptFunction *fs, vector<CObject *> * argv){

	vector<CCompiler::tInfoStatementOp> * m_listStatements = fs->getCompiledCode();
	CCompiler::tInfoAsmOp * previous_instruction=NULL;
	int index_op2=-1,index_op1=-1;
	bool conditional_jmp=false;

	CALE ALE; // new ale ?

	print_info_cr("size ALE: %i", sizeof(CALE));
	return true;


	for(unsigned s = 0; s < (*m_listStatements).size();){

		conditional_jmp = false;
		if((*m_listStatements)[s].asm_op.size()>0){

			vector<CCompiler::tInfoAsmOp *> * asm_op_statment = &(*m_listStatements)[s].asm_op;
			print_vm_cr("executing code...");

			CCompiler::tInfoAsmOp * instruction=NULL;
			CCompiler::tInfoStatementOp * current_statment = &(*m_listStatements)[s];
			for(unsigned i = 0; i  <  asm_op_statment->size() && !conditional_jmp; i++){
				print_vm_cr("executing instruction %i...",i);
				index_op2 = current_statment->asm_op[i]->index_op2;
				index_op1 = current_statment->asm_op[i]->index_op1;
				CCompiler::tInfoAsmOp * instruction=current_statment->asm_op[i];
				CCompiler::tInfoAsmOp * right_instruction, *left_instruction;//=current_statment->asm_op[i];
				instruction=current_statment->asm_op[i];
				int jmp_to_statment = -1;

				ALE.performInstruction(instruction,jmp_to_statment);

				/*switch(instruction->operator_type){
				default:
					print_error_cr("operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
					break;
				case CCompiler::NOP: // ignore ...
					break;
				case CCompiler::LOAD: // load value in function of value/constant ...

					m_stkAle.loadValue(instruction);

					break;

				case CCompiler::ADD: // check both operants type number / integer.

					//index_op2

					//checkOperation(CCompiler::ADD, index_op1, index_op2);



					break;


				}*/

				/*switch(instruction->operator_type){
					default:
						print_error_cr("operator type(%i) not implemented",instruction->operator_type);
						break;
					case CCompiler::NOP: // the most beautiful instructions that it doesn't do nothing :) --> less headache
						break;
					case CCompiler::LOAD:
						print_vm_cr("load value...");
						break;
					case CCompiler::MOV:{ // only can mov vars...

						print_vm_cr("mov var");
						right_instruction = current_statment->asm_op[index_op2];

						switch(instruction->result_type){
						default:
						case CCompiler::NOT_DEFINED:
							print_error_cr("result type must be var (result type:%i)");
							break;
						case CCompiler::VAR_TYPE::INTEGER:
						case CCompiler::NUMBER:
							if(right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER || right_instruction->result_type == CCompiler::VAR_TYPE::NUMBER){

#ifdef __VERBOSE_MESSAGE__
								cout << "assign:" << (CAST_RESULT_AS_NUMBER(right_instruction))<< "" << endl;
#endif
								ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(right_instruction));
							}else{
								print_error_cr("right operant is not number");
							}
							break;

						case CCompiler::STRING:
							if(right_instruction->result_type == CCompiler::VAR_TYPE::STRING){
								*((string *)instruction->result_obj) = *((string *)right_instruction->result_obj);
							}else{
								print_error_cr("right operant is not string");
							}
							break;
						case CCompiler::BOOL:
							if(right_instruction->result_type == CCompiler::VAR_TYPE::BOOL){
								*((bool *)instruction->result_obj) = *((bool *)right_instruction->result_obj);
							}else{
								print_error_cr("right operant is not bool");
							}
							break;
						case CVariable::OBJECT:
							if(right_instruction->result_type == CCompiler::VAR_TYPE::OBJ){ // assign pointer ?

								print_error_cr("mov var not implemented (think about!)");
								// get registered variable...
								//left_obj = reinterpret_cast<CObject *>(left_instruction);
							}else{
								print_error_cr("right operant is not var");
							}
							break;

						}
					}
					break;
					case CCompiler::PRE_INC: // for numbers...
						//left_instruction = current_statment->asm_op[index_op1];
						print_vm_cr("pre inc");
						left_instruction = current_statment->asm_op[index_op1];


						// increment
						if(left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER){
							*((int *)left_instruction->result_obj) = *((int *)left_instruction->result_obj)+1;
						}else if(left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER){
							*((float *)left_instruction->result_obj) = *((float *)left_instruction->result_obj)+1;
						}else{
							print_error_cr("Unexpected type");
							return false;
						}


						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));

						break;
					case CCompiler::POST_INC: // for numbers...
						//left_instruction = current_statment->asm_op[index_op1];
						print_vm_cr("post inc");
						left_instruction = current_statment->asm_op[index_op1];


						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));
						// then increment variable (is on the right instruction)...
						// increment
						if(left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER){
							*((int *)left_instruction->result_obj) = *((int *)left_instruction->result_obj)+1;
						}else if(left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER){
							*((float *)left_instruction->result_obj) = *((float *)left_instruction->result_obj)+1;
						}else{
							print_error_cr("Unexpected type");
							return false;
						}

						break;

					case CCompiler::PRE_DEC: // for numbers...
						//left_instruction = current_statment->asm_op[index_op1];
						print_vm_cr("pre inc");
						left_instruction = current_statment->asm_op[index_op1];


						// increment
						if(left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER){
							*((int *)left_instruction->result_obj) = *((int *)left_instruction->result_obj)-1;
						}else if(left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER){
							*((float *)left_instruction->result_obj) = *((float *)left_instruction->result_obj)-1;
						}else{
							print_error_cr("Unexpected type");
							return false;
						}


						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));

						break;
					case CCompiler::POST_DEC: // for numbers...
						//left_instruction = current_statment->asm_op[index_op1];
						print_vm_cr("post inc");
						left_instruction = current_statment->asm_op[index_op1];


						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));
						// then increment variable (is on the right instruction)...
						// increment
						if(left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER){
							*((int *)left_instruction->result_obj) = *((int *)left_instruction->result_obj)-1;
						}else if(left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER){
							*((float *)left_instruction->result_obj) = *((float *)left_instruction->result_obj)-1;
						}else{
							print_error_cr("Unexpected type");
							return false;
						}

						break;

					case CCompiler::ADD: // for numbers...
						print_vm_cr("add");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if((left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)&&
						    (right_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)){
							//print_vm_cr("%f + %f",*((float *)left_instruction->result_obj),*((float *)right_instruction->result_obj));
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " + " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							ASSIGN_NUMBER(instruction, PERFORM_NUMBER_TWO_OPS(left_instruction, +,right_instruction));

#ifdef __VERBOSE_MESSAGE__
							cout << "assign:" << (CAST_RESULT_AS_NUMBER(instruction))<< "" << endl;
#endif

						}else if(left_instruction->result_type == CCompiler::VAR_TYPE::STRING && right_instruction->result_type == CCompiler::VAR_TYPE::STRING){
							*((string *)instruction->result_obj) = *((string *)left_instruction->result_obj) + *((string *)right_instruction->result_obj);
						}else if(left_instruction->result_type == CCompiler::VAR_TYPE::STRING &&  right_instruction->result_type == CCompiler::VAR_TYPE::NUMBER){
							*((string *)instruction->result_obj) = *((string *)left_instruction->result_obj) + CStringUtils::doubleToString(CAST_RESULT_AS_NUMBER(right_instruction));
						}else{
							print_error_cr("cannot operate + with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::DIV: // for numbers...
						print_vm_cr("div");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if((left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " / " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							ASSIGN_NUMBER(instruction, PERFORM_NUMBER_TWO_OPS(left_instruction, /,right_instruction));
						}
						else{
							print_error_cr("cannot operate + with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::MUL: // for numbers...
						print_vm_cr("mul");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if((left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " * " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							ASSIGN_NUMBER(instruction, PERFORM_NUMBER_TWO_OPS(left_instruction, *,right_instruction));
						}
						else{
							print_error_cr("cannot operate * with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::LOGIC_AND: // for booleans...
						print_vm_cr("and");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if(left_instruction->result_type == CCompiler::VAR_TYPE::BOOL && right_instruction->result_type == CCompiler::VAR_TYPE::BOOL){
							print_vm_cr("%i and %i",*((bool *)left_instruction->result_obj),*((bool *)right_instruction->result_obj));
							*((bool *)instruction->result_obj) = *((bool *)left_instruction->result_obj) && *((bool *)right_instruction->result_obj);
						}
						else{
							print_error_cr("cannot operate && with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::LOGIC_OR: // for booleans...
						print_vm_cr("or");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if(left_instruction->result_type == CCompiler::VAR_TYPE::BOOL && right_instruction->result_type == CCompiler::VAR_TYPE::BOOL){
							print_vm_cr("%i or %i",*((bool *)left_instruction->result_obj),*((bool *)right_instruction->result_obj));
							*((bool *)instruction->result_obj) = *((bool *)left_instruction->result_obj) || *((bool *)right_instruction->result_obj);
						}
						else{
							print_error_cr("cannot operate || with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::LT: // for numbers...
						print_vm_cr("<");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if((left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " < " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, <,right_instruction);
						}
						else{
							print_error_cr("cannot operate < with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::GT: // for numbers...
						print_vm_cr(">");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if((left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " > " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, >,right_instruction);
						}
						else{
							print_error_cr("cannot operate > with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::LTE: // for numbers...
						print_vm_cr("<=");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if((left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " <= " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, <=,right_instruction);
						}
						else{
							print_error_cr("cannot operate <= with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::GTE: // for numbers...
						print_vm_cr(">=");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if((left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " >= " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, >=,right_instruction);
						}
						else{
							print_error_cr("cannot operate >= with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::EQU: // for numbers...
						print_vm_cr("==");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if((left_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == CCompiler::VAR_TYPE::NUMBER || right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " == " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, ==,right_instruction);
						}
						else if(left_instruction->result_type == CCompiler::VAR_TYPE::STRING && right_instruction->result_type == CCompiler::VAR_TYPE::STRING){
							print_vm_cr("%s == %s",((string *)left_instruction->result_obj)->c_str(),((string *)right_instruction->result_obj)->c_str());
							*((bool *)instruction->result_obj) = *((string *)left_instruction->result_obj) == *((string *)right_instruction->result_obj);
						}
						else{
							print_error_cr("cannot operate == with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::MOD: // for integers...
						print_vm_cr("mod");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if(left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER && right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER){
							print_vm_cr("%i % %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) % *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate % with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::OR: // for integers...
						print_vm_cr("or");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if(left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER && right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER){
							print_vm_cr("%i | %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) | *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate | with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::AND: // for integers...
						print_vm_cr("and");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if(left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER && right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER){
							print_vm_cr("%i & %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) & *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate & with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::XOR: // for integers...
						print_vm_cr("xor");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if(left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER && right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER){
							print_vm_cr("%i ^ %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) ^ *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate ^ with (type(%i)  type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::SHL: // for integers...
						print_vm_cr("<<");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if(left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER && right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER){
							print_vm_cr("%i << %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) << *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate << with (type(%i)  type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::SHR: // for integers...
						print_vm_cr(">>");
						right_instruction = current_statment->asm_op[index_op2];
						left_instruction = current_statment->asm_op[index_op1];

						if(left_instruction->result_type == CCompiler::VAR_TYPE::INTEGER && right_instruction->result_type == CCompiler::VAR_TYPE::INTEGER){
							print_vm_cr("%i >> %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction,*((int *)left_instruction->result_obj) >> *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate >> with (type(%i)  type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case CCompiler::NOT:
						print_vm_cr("===============================================");
						print_vm_cr("not");
						left_instruction = current_statment->asm_op[index_op1];
						(*((bool *)instruction->result_obj))=!(*((bool *)left_instruction->result_obj));

						break;
					case CCompiler::NEG:
						print_vm_cr("===============================================");
						print_vm_cr("negative");
						left_instruction = current_statment->asm_op[index_op1];
						ASSIGN_NUMBER(instruction,-CAST_RESULT_AS_NUMBER(left_instruction));
						break;
					case CCompiler::POP_SCOPE: // trivial
					//	print_vm_cr("pop scope");
					//	return true;
						break;
					case CCompiler::PUSH_SCOPE: // because the variables are already defined, the instruction push/pop are useless here.
					//	print_vm_cr("push scope");
					//	if(!execute((CScope *)(instruction->result_obj))){
					//		return false;
					//	}
					case CCompiler::JMP:
						s = (unsigned)instruction->result_obj;
						conditional_jmp = true;
						break;
					case CCompiler::JT:
					case CCompiler::JNT:
						if(previous_instruction != NULL){
							if(previous_instruction->result_type == CCompiler::BOOL){
								bool *b = (bool *)previous_instruction->result_obj;
								if(
								   ((*b == false) && (instruction->operator_type==CCompiler::JNT))
								|| ((*b == true) && (instruction->operator_type==CCompiler::JT))
									){ // then jmp to statment if not true...
									s = (unsigned)instruction->result_obj;
									conditional_jmp = true;
								}
							}else{
								print_error_cr("Previous statment is not type boolean");
								return false;
							}
						}
						else{
							print_error_cr("There's not boolean expression (s=0)");
						}
						break;

				}*/

				previous_instruction = instruction;
			}

			// try to get the result...
			/*instruction=current_statment->asm_op[current_statment->asm_op.size()-1];
			if(instruction!=NULL){
				switch(instruction->result_type){
				default:
					print_error_cr("var not implemented yet");
					break;
				case CCompiler::VAR_TYPE::NOT_DEFINED:
					print_vm_cr("type not defined!");
					break;
				case CCompiler::VAR_TYPE::INTEGER:
					print_vm_cr("Integer with value=%i",*((int*)instruction->result_obj));
					break;
				case CCompiler::VAR_TYPE::NUMBER:
					print_vm_cr("Number with value=%f",*((float*)instruction->result_obj));
					break;
				case CCompiler::VAR_TYPE::BOOL:
					print_vm_cr("Boolean with value=%i",*((bool*)instruction->result_obj));
					break;
				case CCompiler::VAR_TYPE::STRING:
					print_vm_cr("String with value=%s",((string*)instruction->result_obj)->c_str());
					break;
				case CCompiler::VAR_TYPE::OBJ: // print type var?
					print_vm_cr("print type var not implemented!");
					break;

				}
			}*/



			// if not conditional jmp increase next statment...
			if(!conditional_jmp){
				s++;
			}

		}
	}

	return true;

}

CVirtualMachine::~CVirtualMachine(){

}
