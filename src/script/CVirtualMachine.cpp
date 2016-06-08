#include "zg_script.h"

CVirtualMachine * CVirtualMachine::m_virtualMachine = NULL;



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
				switch(instruction->operator_type){
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
						case CCompiler::OBJ:
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

				}

				previous_instruction = instruction;
			}

			// try to get the result...
			instruction=current_statment->asm_op[current_statment->asm_op.size()-1];
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
			}



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
