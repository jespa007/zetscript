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
 (asm_op->result_type == VAR_TYPE::NUMBER)?(*((float *)asm_op->result_obj)):\
 (asm_op->result_type == VAR_TYPE::INTEGER)?(*((int *)asm_op->result_obj)):\
 default_value


#define PERFORM_NUMBER_TWO_OPS(v1,op,v2)\
(v1->result_type == VAR_TYPE::NUMBER && v2->result_type == VAR_TYPE::NUMBER)?(*((float *)v1->result_obj)op(*((float *)v2->result_obj))):\
(v1->result_type == VAR_TYPE::NUMBER && v2->result_type == VAR_TYPE::INTEGER)?(*((float *)v1->result_obj)op(*((int *)v2->result_obj))):\
(v1->result_type == VAR_TYPE::INTEGER && v2->result_type == VAR_TYPE::NUMBER)?(*((int *)v1->result_obj)op(*((float *)v2->result_obj))):\
(v1->result_type == VAR_TYPE::INTEGER && v2->result_type == VAR_TYPE::INTEGER)?(*((int *)v1->result_obj)op(*((int *)v2->result_obj))):\
0


#define ASSIGN_NUMBER(asm_op,v)\
(asm_op->result_type == VAR_TYPE::NUMBER)?(*((float *)asm_op->result_obj)=(v)):\
(asm_op->result_type == VAR_TYPE::INTEGER)?(*((int *)asm_op->result_obj)=(v)):\
 default_value = (v)

bool CVirtualMachine::execute(CScriptFunction *fs, vector<CObject *> * argv){

	vector<tInfoStatementOp> * m_listStatements = fs->getCompiledCode();

	int index_right=-1,index_left=-1;


	for(unsigned s = 0; s < (*m_listStatements).size(); s++){

		if((*m_listStatements)[s].asm_op.size()>0){

			print_info_cr("executing code...");

			tInfoAsmOp * instruction=NULL;
			tInfoStatementOp * current_statment = &(*m_listStatements)[s];
			for(unsigned i = 0; i  < (*m_listStatements)[s].asm_op.size(); i++){
				print_info_cr("executing instruction %i...",i);
				index_right = current_statment->asm_op[i]->index_right;
				index_left = current_statment->asm_op[i]->index_left;
				tInfoAsmOp * instruction=current_statment->asm_op[i];
				tInfoAsmOp * right_instruction, *left_instruction;//=current_statment->asm_op[i];
				instruction=current_statment->asm_op[i];
				switch(instruction->operator_type){
					default:
						print_error_cr("operator type(%i) not implemented",instruction->operator_type);
						break;
					case LOAD:
						print_info_cr("load value...");
					break;
					case MOV:{ // only can mov vars...

						print_info_cr("mov var");
						right_instruction = current_statment->asm_op[index_right];

						switch(instruction->result_type){
						default:
						case UNKNOW:
							print_error_cr("result type must be var (result type:%i)");
							break;
						case VAR_TYPE::INTEGER:
						case NUMBER:
							if(right_instruction->result_type == VAR_TYPE::INTEGER || right_instruction->result_type == VAR_TYPE::NUMBER){

								cout << "assign:" << (CAST_RESULT_AS_NUMBER(right_instruction))<< "" << endl;
								ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(right_instruction));
							}else{
								print_error_cr("right operant is not number");
							}
							break;

						case STRING:
							if(right_instruction->result_type == VAR_TYPE::STRING){
								*((string *)instruction->result_obj) = *((string *)right_instruction->result_obj);
							}else{
								print_error_cr("right operant is not string");
							}
							break;
						case BOOL:
							if(right_instruction->result_type == VAR_TYPE::BOOL){
								*((bool *)instruction->result_obj) = *((bool *)right_instruction->result_obj);
							}else{
								print_error_cr("right operant is not bool");
							}
							break;
						case OBJ:
							if(right_instruction->result_type == VAR_TYPE::OBJ){ // assign pointer ?

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
					case PRE_INC: // for numbers...
						//left_instruction = current_statment->asm_op[index_left];
						print_info_cr("pre inc");
						left_instruction = current_statment->asm_op[index_left];
						// increment variable...
						ASSIGN_NUMBER(left_instruction,CAST_RESULT_AS_NUMBER(left_instruction)+1);
						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));

						break;
					case POST_INC: // for numbers...
						//left_instruction = current_statment->asm_op[index_left];
						print_info_cr("post inc");
						left_instruction = current_statment->asm_op[index_left];

						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));
						// then increment variable...
						ASSIGN_NUMBER(left_instruction,CAST_RESULT_AS_NUMBER(left_instruction)+1);

						break;

					case ADD: // for numbers...
						print_info_cr("add");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
						    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
							//print_info_cr("%f + %f",*((float *)left_instruction->result_obj),*((float *)right_instruction->result_obj));
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " + " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
							ASSIGN_NUMBER(instruction, PERFORM_NUMBER_TWO_OPS(left_instruction, +,right_instruction));

							{
								int jjj;
								jjj=PERFORM_NUMBER_TWO_OPS(left_instruction, +,right_instruction);
								cout << "assign2:" << jjj << "" << endl;
							}
							cout << "assign:" << (CAST_RESULT_AS_NUMBER(instruction))<< "" << endl;

						}else if(left_instruction->result_type == VAR_TYPE::STRING && right_instruction->result_type == VAR_TYPE::STRING){
							*((string *)instruction->result_obj) = *((string *)left_instruction->result_obj) + *((string *)right_instruction->result_obj);
						}else if(left_instruction->result_type == VAR_TYPE::STRING &&  right_instruction->result_type == VAR_TYPE::NUMBER){
							*((string *)instruction->result_obj) = *((string *)left_instruction->result_obj) + CStringUtils::doubleToString(CAST_RESULT_AS_NUMBER(right_instruction));
						}else{
							print_error_cr("cannot operate + with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case DIV: // for numbers...
						print_info_cr("div");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " / " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
							ASSIGN_NUMBER(instruction, PERFORM_NUMBER_TWO_OPS(left_instruction, /,right_instruction));
						}
						else{
							print_error_cr("cannot operate + with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case MUL: // for numbers...
						print_info_cr("mul");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " * " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
							ASSIGN_NUMBER(instruction, PERFORM_NUMBER_TWO_OPS(left_instruction, *,right_instruction));
						}
						else{
							print_error_cr("cannot operate * with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case LOGIC_AND: // for booleans...
						print_info_cr("and");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::BOOL && right_instruction->result_type == VAR_TYPE::BOOL){
							print_info_cr("%i and %i",*((bool *)left_instruction->result_obj),*((bool *)right_instruction->result_obj));
							*((bool *)instruction->result_obj) = *((bool *)left_instruction->result_obj) && *((bool *)right_instruction->result_obj);
						}
						else{
							print_error_cr("cannot operate && with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case LOGIC_OR: // for booleans...
						print_info_cr("or");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::BOOL && right_instruction->result_type == VAR_TYPE::BOOL){
							print_info_cr("%i or %i",*((bool *)left_instruction->result_obj),*((bool *)right_instruction->result_obj));
							*((bool *)instruction->result_obj) = *((bool *)left_instruction->result_obj) || *((bool *)right_instruction->result_obj);
						}
						else{
							print_error_cr("cannot operate || with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case LT: // for numbers...
						print_info_cr("<");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " < " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, <,right_instruction);
						}
						else{
							print_error_cr("cannot operate < with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case GT: // for numbers...
						print_info_cr(">");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " > " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, >,right_instruction);
						}
						else{
							print_error_cr("cannot operate > with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case LTE: // for numbers...
						print_info_cr("<=");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " <= " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, <=,right_instruction);
						}
						else{
							print_error_cr("cannot operate <= with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case GTE: // for numbers...
						print_info_cr(">=");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " >= " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, >=,right_instruction);
						}
						else{
							print_error_cr("cannot operate >= with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case EQU: // for numbers...
						print_info_cr("==");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " == " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, ==,right_instruction);
						}
						else if(left_instruction->result_type == VAR_TYPE::STRING && right_instruction->result_type == VAR_TYPE::STRING){
							print_info_cr("%s == %s",((string *)left_instruction->result_obj)->c_str(),((string *)right_instruction->result_obj)->c_str());
							*((bool *)instruction->result_obj) = *((string *)left_instruction->result_obj) == *((string *)right_instruction->result_obj);
						}
						else{
							print_error_cr("cannot operate == with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case MOD: // for integers...
						print_info_cr("mod");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_info_cr("%i % %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) % *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate % with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case OR: // for integers...
						print_info_cr("or");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_info_cr("%i | %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) | *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate | with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case AND: // for integers...
						print_info_cr("and");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_info_cr("%i & %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) & *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate & with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case XOR: // for integers...
						print_info_cr("xor");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_info_cr("%i ^ %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) ^ *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate ^ with (type(%i)  type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case SHL: // for integers...
						print_info_cr("<<");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_info_cr("%i << %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) << *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate << with (type(%i)  type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case SHR: // for integers...
						print_info_cr(">>");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_info_cr("%i >> %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction,*((int *)left_instruction->result_obj) >> *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate >> with (type(%i)  type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case NOT:
						print_info_cr("===============================================");
						print_info_cr("not");
						left_instruction = current_statment->asm_op[index_left];
						(*((bool *)instruction->result_obj))=!(*((bool *)left_instruction->result_obj));

						break;
					case NEG:
						print_info_cr("===============================================");
						print_info_cr("negative");
						left_instruction = current_statment->asm_op[index_left];
						ASSIGN_NUMBER(instruction,-CAST_RESULT_AS_NUMBER(left_instruction));
						break;
					case POP_SCOPE: // trivial
					//	print_info_cr("pop scope");
					//	return true;
						break;
					case PUSH_SCOPE: // because the variables are already defined, the instruction push/pop are useless here.
					//	print_info_cr("push scope");
					//	if(!execute((CScope *)(instruction->result_obj))){
					//		return false;
					//	}
						break;

				}
			}

			// try to get the result...
			instruction=current_statment->asm_op[current_statment->asm_op.size()-1];
			if(instruction!=NULL){
				switch(instruction->result_type){
				case VAR_TYPE::NOT_DEFINED:
					print_info_cr("type not defined!");
					break;
				case VAR_TYPE::INTEGER:
					print_info_cr("Integer with value=%i",*((int*)instruction->result_obj));
					break;
				case VAR_TYPE::NUMBER:
					print_info_cr("Number with value=%f",*((float*)instruction->result_obj));
					break;
				case VAR_TYPE::BOOL:
					print_info_cr("Boolean with value=%i",*((bool*)instruction->result_obj));
					break;
				case VAR_TYPE::STRING:
					print_info_cr("String with value=%s",((string*)instruction->result_obj)->c_str());
					break;
				case VAR_TYPE::OBJ: // print type var?
					print_info_cr("print type var not implemented!");
					break;

				}
			}
		}
	}

	return true;

}

CVirtualMachine::~CVirtualMachine(){

}
