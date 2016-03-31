#include "zg_script.h"

CVirtualMachine * CVirtualMachine::m_virtualMachine = NULL;

CVirtualMachine::tDefOperator CVirtualMachine::def_operator[MAX_OPERATORS];

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

	def_operator[NOP]         ={"NOP",NOP,0};
	def_operator[MOV]         ={"MOV",MOV,2}; // mov expression to var
	def_operator[LOAD]        ={"LOAD",LOAD,1}; // primitive value like number/string or boolean...
	def_operator[EQU]         ={"EQU",EQU,2};  // ==
	def_operator[LT]          ={"LT",LT,2};  // <
	def_operator[LTE]         ={"LTE",LTE,2};  // <=
	def_operator[NOT]         ={"NOT",NOT,1}; // !
	def_operator[GT]          ={"GT",GT,2};  // >
	def_operator[GTE]         ={"GTE",GTE,2}; // >=
	def_operator[NEG]         ={"NEG",NEG,1}; // !
	def_operator[ADD]         ={"ADD",ADD,2}; // +
	def_operator[PRE_INC]     ={"PRE_INC",PRE_INC,1};// ++
	def_operator[POST_INC]    ={"POST_INC",POST_INC,1}; // ++
	def_operator[PRE_DEC]     ={"PRE_DEC",PRE_DEC,1}; // ++
	def_operator[POST_DEC]    ={"POST_DEC",POST_DEC,1}; // ++
	def_operator[LOGIC_AND]   ={"LOGIC_AND",LOGIC_AND,2}; // &&
	def_operator[LOGIC_OR]    ={"LOGIC_OR",LOGIC_OR,2};  // ||
	def_operator[DIV]         ={"DIV",DIV,2}; // /
	def_operator[MUL]         ={"MUL",MUL,2}; // *
	def_operator[MOD]         ={"MOD",MOD,2};  // %
	def_operator[CAT]         ={"CAT",CAT,2};  // str+str
	def_operator[AND]         ={"AND",AND,2}; // bitwise logic and
	def_operator[OR]          ={"OR",OR,2}; // bitwise logic or
	def_operator[XOR]         ={"XOR",XOR,2}; // logic xor
	def_operator[SHL]         ={"SHL",SHL,2}; // shift left
	def_operator[SHR]         ={"SHR",SHR,2}; // shift right
	def_operator[PUSH_SCOPE]  ={"PUSH_SCOPE",PUSH_SCOPE,0};
	def_operator[POP_SCOPE]   ={"POP_SCOPE",POP_SCOPE,0};
	def_operator[JMP]         ={"JMP",JMP,1};
	def_operator[JNT]         ={"JNT",JNT,1}; // goto if not true ... goes end to conditional.
	def_operator[JT]          ={"JT",JT,1}; // goto if true ... goes end to conditional.

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

#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __VERBOSE_MESSAGE__
#endif



#ifdef  __VERBOSE_MESSAGE__

#define print_vm_cr print_info_cr
#else
#define print_vm_cr(s,...)
#endif

void CVirtualMachine::printGeneratedCode(CScriptFunction *fs){

	vector<tInfoStatementOp> * m_listStatements = fs->getCompiledCode();

	for(unsigned s = 0; s < (*m_listStatements).size();s++){
		vector<tInfoAsmOp *> * asm_op_statment = &(*m_listStatements)[s].asm_op;

		printf("\n[%s:%i]\t%s\n\n","file.zs",(*m_listStatements)[s].m_line,(*m_listStatements)[s].expression_str.c_str());

		for(unsigned i = 0; i  <  asm_op_statment->size(); i++){

			int n_ops=1;
			int left = (*asm_op_statment)[i]->index_left;
			int right = (*asm_op_statment)[i]->index_right;

			if(left != -1 && right != -1)
				n_ops=2;



			switch((*asm_op_statment)[i]->operator_type){
			case  LOAD:
				printf("[%02i:%02i]\t%s\t%s\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,(*asm_op_statment)[i]->result_str.c_str());
				break;
			case  MOV:
				printf("[%02i:%02i]\t%s\t%s,[%02i:%02i]\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,(*asm_op_statment)[i]->result_str.c_str(),s,right);
				break;
			case JNT:
			case JT:
			case JMP:
				printf("[%02i:%02i]\t%s\t[%04i]\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,(int)(*asm_op_statment)[i]->result_obj);
				break;
			default:

				if(n_ops==1){
					printf("[%02i:%02i]\t%s\t[%02i:%02i]\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,s,left);
				}else{
					printf("[%02i:%02i]\t%s\t[%02i:%02i],[%02i:%02i]\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,s,left,s,right);
				}
				break;
			}
		}
	}
}


bool CVirtualMachine::execute(CScriptFunction *fs, vector<CObject *> * argv){

	vector<tInfoStatementOp> * m_listStatements = fs->getCompiledCode();
	tInfoAsmOp * previous_instruction=NULL;
	int index_right=-1,index_left=-1;
	bool conditional_jmp=false;


	for(unsigned s = 0; s < (*m_listStatements).size();){

		conditional_jmp = false;
		if((*m_listStatements)[s].asm_op.size()>0){

			vector<tInfoAsmOp *> * asm_op_statment = &(*m_listStatements)[s].asm_op;
			print_vm_cr("executing code...");

			tInfoAsmOp * instruction=NULL;
			tInfoStatementOp * current_statment = &(*m_listStatements)[s];
			for(unsigned i = 0; i  <  asm_op_statment->size() && !conditional_jmp; i++){
				print_vm_cr("executing instruction %i...",i);
				index_right = current_statment->asm_op[i]->index_right;
				index_left = current_statment->asm_op[i]->index_left;
				tInfoAsmOp * instruction=current_statment->asm_op[i];
				tInfoAsmOp * right_instruction, *left_instruction;//=current_statment->asm_op[i];
				instruction=current_statment->asm_op[i];
				switch(instruction->operator_type){
					default:
						print_error_cr("operator type(%i) not implemented",instruction->operator_type);
						break;
					case NOP: // the most beautiful instructions that it doesn't do nothing :) --> less headache
						break;
					case LOAD:
						print_vm_cr("load value...");
						break;
					case MOV:{ // only can mov vars...

						print_vm_cr("mov var");
						right_instruction = current_statment->asm_op[index_right];

						switch(instruction->result_type){
						default:
						case NOT_DEFINED:
							print_error_cr("result type must be var (result type:%i)");
							break;
						case VAR_TYPE::INTEGER:
						case NUMBER:
							if(right_instruction->result_type == VAR_TYPE::INTEGER || right_instruction->result_type == VAR_TYPE::NUMBER){

#ifdef __VERBOSE_MESSAGE__
								cout << "assign:" << (CAST_RESULT_AS_NUMBER(right_instruction))<< "" << endl;
#endif
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
						print_vm_cr("pre inc");
						left_instruction = current_statment->asm_op[index_left];


						// increment
						if(left_instruction->result_type == VAR_TYPE::INTEGER){
							*((int *)left_instruction->result_obj) = *((int *)left_instruction->result_obj)+1;
						}else if(left_instruction->result_type == VAR_TYPE::NUMBER){
							*((float *)left_instruction->result_obj) = *((float *)left_instruction->result_obj)+1;
						}else{
							print_error_cr("Unexpected type");
							return false;
						}


						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));

						break;
					case POST_INC: // for numbers...
						//left_instruction = current_statment->asm_op[index_left];
						print_vm_cr("post inc");
						left_instruction = current_statment->asm_op[index_left];


						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));
						// then increment variable (is on the right instruction)...
						// increment
						if(left_instruction->result_type == VAR_TYPE::INTEGER){
							*((int *)left_instruction->result_obj) = *((int *)left_instruction->result_obj)+1;
						}else if(left_instruction->result_type == VAR_TYPE::NUMBER){
							*((float *)left_instruction->result_obj) = *((float *)left_instruction->result_obj)+1;
						}else{
							print_error_cr("Unexpected type");
							return false;
						}

						break;

					case PRE_DEC: // for numbers...
						//left_instruction = current_statment->asm_op[index_left];
						print_vm_cr("pre inc");
						left_instruction = current_statment->asm_op[index_left];


						// increment
						if(left_instruction->result_type == VAR_TYPE::INTEGER){
							*((int *)left_instruction->result_obj) = *((int *)left_instruction->result_obj)-1;
						}else if(left_instruction->result_type == VAR_TYPE::NUMBER){
							*((float *)left_instruction->result_obj) = *((float *)left_instruction->result_obj)-1;
						}else{
							print_error_cr("Unexpected type");
							return false;
						}


						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));

						break;
					case POST_DEC: // for numbers...
						//left_instruction = current_statment->asm_op[index_left];
						print_vm_cr("post inc");
						left_instruction = current_statment->asm_op[index_left];


						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));
						// then increment variable (is on the right instruction)...
						// increment
						if(left_instruction->result_type == VAR_TYPE::INTEGER){
							*((int *)left_instruction->result_obj) = *((int *)left_instruction->result_obj)-1;
						}else if(left_instruction->result_type == VAR_TYPE::NUMBER){
							*((float *)left_instruction->result_obj) = *((float *)left_instruction->result_obj)-1;
						}else{
							print_error_cr("Unexpected type");
							return false;
						}

						break;

					case ADD: // for numbers...
						print_vm_cr("add");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
						    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
							//print_vm_cr("%f + %f",*((float *)left_instruction->result_obj),*((float *)right_instruction->result_obj));
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " + " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							ASSIGN_NUMBER(instruction, PERFORM_NUMBER_TWO_OPS(left_instruction, +,right_instruction));

#ifdef __VERBOSE_MESSAGE__
							cout << "assign:" << (CAST_RESULT_AS_NUMBER(instruction))<< "" << endl;
#endif

						}else if(left_instruction->result_type == VAR_TYPE::STRING && right_instruction->result_type == VAR_TYPE::STRING){
							*((string *)instruction->result_obj) = *((string *)left_instruction->result_obj) + *((string *)right_instruction->result_obj);
						}else if(left_instruction->result_type == VAR_TYPE::STRING &&  right_instruction->result_type == VAR_TYPE::NUMBER){
							*((string *)instruction->result_obj) = *((string *)left_instruction->result_obj) + CStringUtils::doubleToString(CAST_RESULT_AS_NUMBER(right_instruction));
						}else{
							print_error_cr("cannot operate + with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case DIV: // for numbers...
						print_vm_cr("div");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " / " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							ASSIGN_NUMBER(instruction, PERFORM_NUMBER_TWO_OPS(left_instruction, /,right_instruction));
						}
						else{
							print_error_cr("cannot operate + with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case MUL: // for numbers...
						print_vm_cr("mul");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " * " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							ASSIGN_NUMBER(instruction, PERFORM_NUMBER_TWO_OPS(left_instruction, *,right_instruction));
						}
						else{
							print_error_cr("cannot operate * with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case LOGIC_AND: // for booleans...
						print_vm_cr("and");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::BOOL && right_instruction->result_type == VAR_TYPE::BOOL){
							print_vm_cr("%i and %i",*((bool *)left_instruction->result_obj),*((bool *)right_instruction->result_obj));
							*((bool *)instruction->result_obj) = *((bool *)left_instruction->result_obj) && *((bool *)right_instruction->result_obj);
						}
						else{
							print_error_cr("cannot operate && with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case LOGIC_OR: // for booleans...
						print_vm_cr("or");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::BOOL && right_instruction->result_type == VAR_TYPE::BOOL){
							print_vm_cr("%i or %i",*((bool *)left_instruction->result_obj),*((bool *)right_instruction->result_obj));
							*((bool *)instruction->result_obj) = *((bool *)left_instruction->result_obj) || *((bool *)right_instruction->result_obj);
						}
						else{
							print_error_cr("cannot operate || with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case LT: // for numbers...
						print_vm_cr("<");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " < " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, <,right_instruction);
						}
						else{
							print_error_cr("cannot operate < with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case GT: // for numbers...
						print_vm_cr(">");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " > " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, >,right_instruction);
						}
						else{
							print_error_cr("cannot operate > with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case LTE: // for numbers...
						print_vm_cr("<=");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " <= " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, <=,right_instruction);
						}
						else{
							print_error_cr("cannot operate <= with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case GTE: // for numbers...
						print_vm_cr(">=");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " >= " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, >=,right_instruction);
						}
						else{
							print_error_cr("cannot operate >= with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case EQU: // for numbers...
						print_vm_cr("==");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == VAR_TYPE::NUMBER || left_instruction->result_type == VAR_TYPE::INTEGER)&&
							    (right_instruction->result_type == VAR_TYPE::NUMBER || right_instruction->result_type == VAR_TYPE::INTEGER)){
#ifdef __VERBOSE_MESSAGE__
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " == " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
#endif
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, ==,right_instruction);
						}
						else if(left_instruction->result_type == VAR_TYPE::STRING && right_instruction->result_type == VAR_TYPE::STRING){
							print_vm_cr("%s == %s",((string *)left_instruction->result_obj)->c_str(),((string *)right_instruction->result_obj)->c_str());
							*((bool *)instruction->result_obj) = *((string *)left_instruction->result_obj) == *((string *)right_instruction->result_obj);
						}
						else{
							print_error_cr("cannot operate == with (type(%i) , type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case MOD: // for integers...
						print_vm_cr("mod");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_vm_cr("%i % %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) % *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate % with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case OR: // for integers...
						print_vm_cr("or");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_vm_cr("%i | %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) | *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate | with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case AND: // for integers...
						print_vm_cr("and");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_vm_cr("%i & %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) & *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate & with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case XOR: // for integers...
						print_vm_cr("xor");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_vm_cr("%i ^ %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) ^ *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate ^ with (type(%i)  type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case SHL: // for integers...
						print_vm_cr("<<");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_vm_cr("%i << %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction, *((int *)left_instruction->result_obj) << *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate << with (type(%i)  type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case SHR: // for integers...
						print_vm_cr(">>");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if(left_instruction->result_type == VAR_TYPE::INTEGER && right_instruction->result_type == VAR_TYPE::INTEGER){
							print_vm_cr("%i >> %i",*((int *)left_instruction->result_obj),*((int *)right_instruction->result_obj));
							ASSIGN_NUMBER(instruction,*((int *)left_instruction->result_obj) >> *((int *)right_instruction->result_obj));
						}
						else{
							print_error_cr("cannot operate >> with (type(%i)  type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case NOT:
						print_vm_cr("===============================================");
						print_vm_cr("not");
						left_instruction = current_statment->asm_op[index_left];
						(*((bool *)instruction->result_obj))=!(*((bool *)left_instruction->result_obj));

						break;
					case NEG:
						print_vm_cr("===============================================");
						print_vm_cr("negative");
						left_instruction = current_statment->asm_op[index_left];
						ASSIGN_NUMBER(instruction,-CAST_RESULT_AS_NUMBER(left_instruction));
						break;
					case POP_SCOPE: // trivial
					//	print_vm_cr("pop scope");
					//	return true;
						break;
					case PUSH_SCOPE: // because the variables are already defined, the instruction push/pop are useless here.
					//	print_vm_cr("push scope");
					//	if(!execute((CScope *)(instruction->result_obj))){
					//		return false;
					//	}
					case JMP:
						s = (unsigned)instruction->result_obj;
						conditional_jmp = true;
						break;
					case JT:
					case JNT:
						if(previous_instruction != NULL){
							if(previous_instruction->result_type == BOOL){
								bool *b = (bool *)previous_instruction->result_obj;
								if(
								   ((*b == false) && (instruction->operator_type==JNT))
								|| ((*b == true) && (instruction->operator_type==JT))
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
				case VAR_TYPE::NOT_DEFINED:
					print_vm_cr("type not defined!");
					break;
				case VAR_TYPE::INTEGER:
					print_vm_cr("Integer with value=%i",*((int*)instruction->result_obj));
					break;
				case VAR_TYPE::NUMBER:
					print_vm_cr("Number with value=%f",*((float*)instruction->result_obj));
					break;
				case VAR_TYPE::BOOL:
					print_vm_cr("Boolean with value=%i",*((bool*)instruction->result_obj));
					break;
				case VAR_TYPE::STRING:
					print_vm_cr("String with value=%s",((string*)instruction->result_obj)->c_str());
					break;
				case VAR_TYPE::OBJ: // print type var?
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
