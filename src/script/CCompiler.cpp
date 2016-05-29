#include "zg_script.h"

#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __VERBOSE_MESSAGE__
#endif



#ifdef  __VERBOSE_MESSAGE__

#define print_com_cr print_ast_cr
#else
#define print_com_cr(s,...)
#endif


CCompiler::tDefOperator CCompiler::def_operator[MAX_OPERATORS];

CCompiler *CCompiler::m_compiler = NULL;



CCompiler *CCompiler::getInstance(){
	if(m_compiler == NULL){
		m_compiler = new CCompiler();
	}

	return m_compiler;
}

void CCompiler::destroySingletons(){
	if(m_compiler != NULL){
		delete m_compiler;
		m_compiler=NULL;
	}

}



void CCompiler::printGeneratedCode(CScriptFunction *fs){

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

//------------------------------------------------------------------------------------------------------------------
//
// CONSTRUCTOR
//

CCompiler::CCompiler(){
	m_currentListStatements = NULL;
	m_currentScriptFunction = NULL;
	m_treescope = NULL;

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
// VARIABLE
//
/*
CCompiler::tInfoRegisteredVar * CCompiler::existRegisteredSymbolRecursive(const string & var_name){
	if(m_registeredSymbol.count(var_name)==0){ // not exit but we will deepth through parents ...
		CScope * parent = lc->getParent();
		if(parent != NULL){
			return existRegisteredSymbolRecursive(var_name, parent);
		}
		return NULL;
	}else{
		return m_registeredSymbol[var_name];
		//print_error_cr("variable %s already registered at line %i",var_name.c_str(),lc->m_registeredSymbol[var_name]->m_line);
	}

	return NULL;
}

CCompiler::tInfoRegisteredVar * CCompiler::existRegisteredSymbol(const string & var_name){
	if(m_registeredSymbol.count(var_name)==1){
		return m_registeredSymbol[var_name];
	}

	return NULL;

}


CObject *CCompiler::getRegisteredVariable(const string & var_name, bool print_msg){
	tInfoRegisteredVar * irv;
	if((irv = existRegisteredSymbol(var_name))!=NULL){ // check whether is local var registered scope ...

		return irv->m_obj;
	}else{
		if(print_msg){
			print_error_cr("var \"%s\" doesn't exist!", var_name.c_str());
		}
	}

	return NULL;

}


bool CCompiler::registerSymbol(const string & var_name, int m_line){
	tInfoRegisteredVar * irv;
	if((irv = existRegisteredSymbol(var_name))==NULL){ // check whether is local var registered scope ...
		irv = new tInfoRegisteredVar;
		irv->m_line = m_line;
		irv->m_obj = m_defaultVar;
		m_registeredSymbol[var_name]=irv;
		return true;
	}else{
		print_error_cr("error var \"%s\" already registered at line %i!", var_name.c_str(), irv->m_line);
	}

	return false;
}

bool CCompiler::defineSymbol(const string & var_name, CObject *obj){

	tInfoRegisteredVar * irv;
	if((irv = existRegisteredSymbol(var_name))!=NULL){ // check whether is local var registered scope ...
		irv->m_obj=obj;
		irv->m_obj->setName(var_name);
		return true;
	}else{
		print_error_cr("error var \"%s\" is not registered!", var_name.c_str());
	}
	return false;
}
*/



//------------------------------------------------------------------------------------------------------------------
//
// COMPILE COMPILER MANAGEMENT
//
string  CCompiler::getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	string result="unknow";

	if(instruction >=0 && instruction < ptr_current_statement_op->asm_op.size()){
		switch(ptr_current_statement_op->asm_op[instruction]->result_type){
		default:
		case CCompiler::NOT_DEFINED:
			break;
		case CCompiler::OBJ:
			result= (((CObject *)ptr_current_statement_op->asm_op[instruction]->result_obj)->getPointerClassStr()); // type result..
			break;
		case CCompiler::NUMBER:
			result= CNumberFactory::getPointerTypeStr();
			break;
		case CCompiler::INTEGER:
			result= CIntegerFactory::getPointerTypeStr();
			break;
		case CCompiler::STRING:
			result= CStringFactory::getPointerTypeStr();
			break;
		case CCompiler::BOOL:
			result= CBooleanFactory::getPointerTypeStr();
			break;
		}
	}
	else{
		print_error_cr("index out of bounds");
	}
	return result;
}

int CCompiler::getCurrentInstructionIndex(){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	return ptr_current_statement_op->asm_op.size()-1;
}

int CCompiler::getCurrentStatmentIndex(){
	return (int)(m_currentListStatements->size()-1);
}

CCompiler::tInfoStatementOp * CCompiler::newStatment(){
	CCompiler::tInfoStatementOp st;

	m_currentListStatements->push_back(st);

	return  &(*m_currentListStatements)[m_currentListStatements->size()-1];
}

bool *  CCompiler::getObjectResultCurrentStatmentAsBoolean(){

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	bool *result=NULL;
	unsigned instruction = ptr_current_statement_op->asm_op.size()-1;

	if(instruction >=0 && instruction < ptr_current_statement_op->asm_op.size()){
		if(ptr_current_statement_op->asm_op[instruction]->result_type == CCompiler::BOOL){
			result= (((bool *)ptr_current_statement_op->asm_op[instruction]->result_obj)); // type result..
		}else{
			print_error_cr("Result type is not boolean");
		}
	}
	else{
		print_error_cr("index out of bounds");
	}
	return result;
}


bool CCompiler::insertLoadValueInstruction(const string & v, CScope * _lc, int m_var_at_line){

	CNumber *num_obj;
	CInteger *int_obj;
	CString *str_obj;
	CBoolean *bool_obj;
	string type_value="CONST";

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	void *obj;
	CCompiler::VAR_TYPE type=CCompiler::NOT_DEFINED;
	// try parse value...
	if((obj=CInteger::ParsePrimitive(v))!=NULL){
			type=CCompiler::INTEGER;
			print_com_cr("%s detected as int\n",v.c_str());
	}
	else if((obj=CNumber::ParsePrimitive(v))!=NULL){
		type=CCompiler::NUMBER;
		print_com_cr("%s detected as float\n",v.c_str());
	}
	else if(v[0]=='\"' && v[v.size()-1]=='\"'){
		type=CCompiler::STRING;
		string s=v.substr(1,v.size()-2);
		string *so=new string();
		(*so)=s;
		obj = so;
		print_com_cr("%s detected as string\n",v.c_str());
	}
	else if((obj=CBoolean::ParsePrimitive(v))!=NULL){
		type=CCompiler::BOOL;
		print_com_cr("%s detected as boolean\n",v.c_str());
	}else{
		CScope::tInfoRegisteredVar * info_var=_lc->getInfoRegisteredSymbol(v,false);
		type=CCompiler::OBJ;

		if(info_var==NULL){
			print_error_cr("symbol %s at line %i is not declared ", v.c_str(),m_var_at_line);
			return false;
		}

		CObject *var = info_var->m_obj;
		obj = var;
		type_value="OBJ";
		if((int_obj=dynamic_cast<CInteger *>(var))!=NULL){ // else it will store the value ...
			type_value="INT";
			type = CCompiler::INTEGER;
			obj=&int_obj->m_value;
		}else if((num_obj=dynamic_cast<CNumber *>(var))!=NULL){
			type_value="FLOAT";
			type = CCompiler::NUMBER;
			obj=&num_obj->m_value;
		}else if((bool_obj=dynamic_cast<CBoolean *>(var))!=NULL){
			type_value="BOOL";
			type = CCompiler::BOOL;
			obj=&bool_obj->m_value;
		}else if((str_obj=dynamic_cast<CString *>(var))!=NULL){
			type_value="STRING";
			type = CCompiler::STRING;
			obj=&str_obj->m_value;
		}
	}

	if(obj != NULL){

		CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();

		asm_op->result_obj=obj;
		asm_op->result_type = type;

		asm_op->operator_type=CCompiler::ASM_OPERATOR::LOAD;
		asm_op->result_str = v;


		//printf("[%02i:%02i] %s \t%s\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),type_value.c_str(),v.c_str());
		ptr_current_statement_op->asm_op.push_back(asm_op);

	}
	return true;
}

bool CCompiler::insertMovVarInstruction(CObject *var, int right){

	//string op="=";
	string left_type_ptr = var->getPointerClassStr();
	CNumber *num_obj;
	CInteger * int_obj;
	CString *str_obj;
	CBoolean *bool_obj;

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];

	if(right <0 || (unsigned)right >= ptr_current_statement_op->asm_op.size()){
		print_error_cr("ERROR: right operant is out of internal stack (%i,%i) ",right,ptr_current_statement_op->asm_op.size());
		return false;
	}

	if(right >= 0){ // insert both op...

		CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
		asm_op->result_type = CCompiler::OBJ; // this only stores pointer...
		asm_op->result_obj=var;

		if((int_obj=dynamic_cast<CInteger *>(var))!=NULL){ // else it will store the value ...
				asm_op->result_type = CCompiler::INTEGER;
				asm_op->result_obj=&int_obj->m_value;
		}else if((num_obj=dynamic_cast<CNumber *>(var))!=NULL){ // else it will store the value ...
			asm_op->result_type = CCompiler::NUMBER;
			asm_op->result_obj=&num_obj->m_value;
		}else if((bool_obj=dynamic_cast<CBoolean *>(var))!=NULL){
			asm_op->result_type = CCompiler::BOOL;
			asm_op->result_obj=&bool_obj->m_value;
		}else if((str_obj=dynamic_cast<CString *>(var))!=NULL){
			asm_op->result_type = CCompiler::STRING;
			asm_op->result_obj=&str_obj->m_value;
		}

		asm_op->index_left = -1;
		asm_op->index_right = right;
		asm_op->operator_type = CCompiler::ASM_OPERATOR::MOV;
		asm_op->result_str =var->getName();

		//printf("[%02i:%02i] MOV \tV(%s),[%02i:%02i]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),var->getName().c_str(),m_currentListStatements->size(),right);

		ptr_current_statement_op->asm_op.push_back(asm_op);


	}else{
		print_error_cr("ERROR: right operand is out of internal stack ");
		return false;
	}

	return true;
}

void CCompiler::insertPushScopeInstruction(CScope * _goto_scope){
	/*CCompiler::tInfoStatementOp op_push_scope;
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->result_obj = _goto_scope;
	asm_op->operator_type=CCompiler::ASM_OPERATOR::PUSH_SCOPE;
	op_push_scope.asm_op.push_back(asm_op);

	m_listStatements.push_back(op_push_scope);*/
}


void CCompiler::insertPopScopeInstruction(){
/*
	CCompiler::tInfoStatementOp op_pop_scope;
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->result_obj = this->m_parentScope;
	asm_op->operator_type=CCompiler::ASM_OPERATOR::POP_SCOPE;
	op_pop_scope.asm_op.push_back(asm_op);

	m_listStatements.push_back(op_pop_scope);*/

}

CCompiler::tInfoAsmOp * CCompiler::insert_JMP_Instruction(int jmp_statement){

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->result_obj = (void *)jmp_statement;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CCompiler::ASM_OPERATOR::JMP;
	ptr_current_statement_op->asm_op.push_back(asm_op);
	//printf("[%02i:%02i]\tJMP\t[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());

	return asm_op;
}

CCompiler::tInfoAsmOp * CCompiler::insert_JNT_Instruction(int jmp_statement){

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->result_obj = (void *)jmp_statement;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CCompiler::ASM_OPERATOR::JNT;

	//printf("[%02i:%02i]\tJNT\t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);



	return asm_op;
}

CCompiler::tInfoAsmOp * CCompiler::insert_JT_Instruction(int jmp_statement){

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->result_obj = (void *)jmp_statement;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CCompiler::ASM_OPERATOR::JT;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);
	return asm_op;
}

void CCompiler::insert_NOP_Instruction(){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->result_obj = NULL;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CCompiler::ASM_OPERATOR::NOP;
	//printf("[%02i:%02i]\tNOP\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());
	ptr_current_statement_op->asm_op.push_back(asm_op);

}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

CCompiler::ASM_OPERATOR CCompiler::getIntegerOperatorId_TwoOps(PUNCTUATOR_TYPE op, CCompiler::VAR_TYPE & result_type){

	result_type = CCompiler::INTEGER;

	switch(op){
	default:
		return CCompiler::ASM_OPERATOR::INVALID_OP;
	case PUNCTUATOR_TYPE::ADD_PUNCTUATOR:
		return CCompiler::ADD;
	case PUNCTUATOR_TYPE::DIV_PUNCTUATOR:
		return CCompiler::DIV;
	case PUNCTUATOR_TYPE::MOD_PUNCTUATOR:
		return CCompiler::MOD;
	case PUNCTUATOR_TYPE::MUL_PUNCTUATOR:
		return CCompiler::MUL;
	case PUNCTUATOR_TYPE::BINARY_AND_PUNCTUATOR:
		return CCompiler::AND;
	case PUNCTUATOR_TYPE::BINARY_OR_PUNCTUATOR:
		return CCompiler::OR;
	case PUNCTUATOR_TYPE::BINARY_XOR_PUNCTUATOR:
		return CCompiler::XOR;
	case PUNCTUATOR_TYPE::SHIFT_LEFT_PUNCTUATOR:
		return CCompiler::SHL;
	case PUNCTUATOR_TYPE::SHIFT_RIGHT_PUNCTUATOR:
		return CCompiler::SHR;
	case PUNCTUATOR_TYPE::LOGIC_GTE_PUNCTUATOR:
		result_type = CCompiler::BOOL;
		return CCompiler::GTE;
	case PUNCTUATOR_TYPE::LOGIC_GT_PUNCTUATOR:
		result_type = CCompiler::BOOL;
		return CCompiler::GT;
	case PUNCTUATOR_TYPE::LOGIC_LT_PUNCTUATOR:
		result_type = CCompiler::BOOL;
		return CCompiler::LT;
	case PUNCTUATOR_TYPE::LOGIC_LTE_PUNCTUATOR:
		result_type = CCompiler::BOOL;
		return CCompiler::LTE;
	case PUNCTUATOR_TYPE::LOGIC_EQUAL_PUNCTUATOR:
		result_type = CCompiler::BOOL;
		return CCompiler::EQU;

	}
	return CCompiler::ASM_OPERATOR::INVALID_OP;
}

CCompiler::ASM_OPERATOR CCompiler::getNumberOperatorId_TwoOps(PUNCTUATOR_TYPE op, CCompiler::VAR_TYPE & result_type){

	result_type = CCompiler::NUMBER;

	switch(op){
	default:
		return CCompiler::ASM_OPERATOR::INVALID_OP;
	case PUNCTUATOR_TYPE::ADD_PUNCTUATOR:
		return CCompiler::ADD;
	case PUNCTUATOR_TYPE::DIV_PUNCTUATOR:
		return CCompiler::DIV;
	case PUNCTUATOR_TYPE::MOD_PUNCTUATOR:
		return CCompiler::MOD;
	case PUNCTUATOR_TYPE::MUL_PUNCTUATOR:
		return CCompiler::MUL;
	case PUNCTUATOR_TYPE::LOGIC_GTE_PUNCTUATOR:
		result_type = CCompiler::BOOL;
		return CCompiler::GTE;
	case PUNCTUATOR_TYPE::LOGIC_GT_PUNCTUATOR:
		result_type = CCompiler::BOOL;
		return CCompiler::GT;
	case PUNCTUATOR_TYPE::LOGIC_LT_PUNCTUATOR:
		result_type = CCompiler::BOOL;
		return CCompiler::LT;
	case PUNCTUATOR_TYPE::LOGIC_LTE_PUNCTUATOR:
		result_type = CCompiler::BOOL;
		return CCompiler::LTE;
	case PUNCTUATOR_TYPE::LOGIC_EQUAL_PUNCTUATOR:
		result_type = CCompiler::BOOL;
		return CCompiler::EQU;
	}
	return CCompiler::ASM_OPERATOR::INVALID_OP;
}

CCompiler::ASM_OPERATOR CCompiler::getNumberOperatorId_OneOp(PUNCTUATOR_TYPE op){

	switch(op){
	default:
		return CCompiler::ASM_OPERATOR::INVALID_OP;
	case PUNCTUATOR_TYPE::SUB_PUNCTUATOR:
		return CCompiler::NEG;
	case PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR:
		return CCompiler::PRE_INC;
	case PUNCTUATOR_TYPE::POST_INC_PUNCTUATOR:
		return CCompiler::POST_INC;
	case PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR:
		return CCompiler::PRE_DEC;
	case PUNCTUATOR_TYPE::POST_DEC_PUNCTUATOR:
		return CCompiler::POST_DEC;
	}

	return CCompiler::ASM_OPERATOR::INVALID_OP;
}

CCompiler::ASM_OPERATOR CCompiler::getBoleanOperatorId_TwoOps(PUNCTUATOR_TYPE op, CCompiler::VAR_TYPE & result_type){

	result_type = CCompiler::BOOL;

	switch(op){
	default:
		return CCompiler::ASM_OPERATOR::INVALID_OP;
	case PUNCTUATOR_TYPE::LOGIC_AND_PUNCTUATOR:
		return CCompiler::LOGIC_AND;
	case PUNCTUATOR_TYPE::LOGIC_OR_PUNCTUATOR:
		return CCompiler::LOGIC_OR;
	}

	return CCompiler::ASM_OPERATOR::INVALID_OP;
}

CCompiler::ASM_OPERATOR CCompiler::getBoleanOperatorId_OneOp(PUNCTUATOR_TYPE op){
	switch(op){
	default:
		return CCompiler::ASM_OPERATOR::INVALID_OP;
	case PUNCTUATOR_TYPE::LOGIC_NOT_PUNCTUATOR:
		return CCompiler::NOT;
	}
	return CCompiler::ASM_OPERATOR::INVALID_OP;
}

CCompiler::ASM_OPERATOR CCompiler::getStringOperatorId_TwoOps(PUNCTUATOR_TYPE op, CCompiler::VAR_TYPE & result_type){

	result_type = CCompiler::STRING;

	switch(op){
	default:
		return CCompiler::ASM_OPERATOR::INVALID_OP;
	case PUNCTUATOR_TYPE::ADD_PUNCTUATOR:
		return CCompiler::CAT;
	case PUNCTUATOR_TYPE::LOGIC_EQUAL_PUNCTUATOR:
		result_type = CCompiler::BOOL;
		return CCompiler::EQU;
	}
	return CCompiler::ASM_OPERATOR::INVALID_OP;
}

CCompiler::ASM_OPERATOR CCompiler::getStringOperatorId_OneOp(PUNCTUATOR_TYPE op){

	return CCompiler::ASM_OPERATOR::INVALID_OP;
}


bool CCompiler::insertOperatorInstruction(tInfoPunctuator * op, string & error_str, int left, int right){

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];

	if(left <0 || (unsigned)left >= ptr_current_statement_op->asm_op.size()){
		error_str = CStringUtils::formatString("left operant is out of internal stack (%i,%i) ",left,ptr_current_statement_op->asm_op.size());
		return false;
	}

	string t_left = getUserTypeResultCurrentStatmentAtInstruction(left);
	string t_right = "unknow";
	if(right>=0){
		t_right = getUserTypeResultCurrentStatmentAtInstruction(right);
	}

	CCompiler::ASM_OPERATOR id_op;
	CCompiler::VAR_TYPE result_type=CCompiler::NOT_DEFINED;
	void *res_obj=NULL;


	if(left >=0 && right >= 0){ // insert both op...

		if(((t_left == CIntegerFactory::getPointerTypeStr())) &&
						((t_right == CIntegerFactory::getPointerTypeStr()))){
					if((id_op=getIntegerOperatorId_TwoOps(op->id,result_type))==CCompiler::ASM_OPERATOR::INVALID_OP){
						error_str = CStringUtils::formatString("undefined operator %s",op->str);
						return false;
					}

		}else if(((t_left == CNumberFactory::getPointerTypeStr()) ) &&
				((t_right == CNumberFactory::getPointerTypeStr()) )){
			if((id_op=getNumberOperatorId_TwoOps(op->id,result_type))==CCompiler::ASM_OPERATOR::INVALID_OP){
				error_str = CStringUtils::formatString("undefined operator %s",op->str);
				return false;
			}

		}else if(t_left== CStringFactory::getPointerTypeStr()){

			if((id_op=getStringOperatorId_TwoOps(op->id,result_type))==CCompiler::ASM_OPERATOR::INVALID_OP){
				error_str = CStringUtils::formatString("undefined operator %s",op->str);
				return false;
			}

		}else if((t_left == CBooleanFactory::getPointerTypeStr()) && t_right == CBooleanFactory::getPointerTypeStr()){
			if((id_op=getBoleanOperatorId_TwoOps(op->id, result_type))==CCompiler::ASM_OPERATOR::INVALID_OP){
				error_str = CStringUtils::formatString("undefined operator %s",op->str);
				return false;
			}
		}else{
			error_str = CStringUtils::formatString("Operator %s: not compatible left value as %s with right value as %s",op->str,t_left.c_str(),t_right.c_str());
			return false;
		}

		switch(result_type){
		default:
		case CCompiler::NOT_DEFINED:
			error_str = CStringUtils::formatString("unknow result type");
			return false;
			break;
		case CCompiler::NUMBER:
			res_obj = new float;
			break;
		case CCompiler::INTEGER:
			res_obj = new int;
			break;
		case CCompiler::STRING:
			res_obj = new string;
			break;
		case CCompiler::BOOL:
			res_obj = new bool;
			break;

		}

		CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
		//asm_op->type_op=OPERATOR;
		asm_op->index_left = left;
		asm_op->index_right = right;


		//-----------------------
		asm_op->result_type = result_type;
		asm_op->operator_type = id_op;
		asm_op->result_obj = res_obj;

	//	printf("[%02i:%02i]\t%s\t[%02i:%02i],[%02i:%02i]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),CCompiler::def_operator[id_op].op_str,m_currentListStatements->size(),left,m_currentListStatements->size(),right);

		ptr_current_statement_op->asm_op.push_back(asm_op);


	}else if(left >=0){ // insert one operand

			if((t_left == CIntegerFactory::getPointerTypeStr()) ){
						if((id_op=getNumberOperatorId_OneOp(op->id))==CCompiler::ASM_OPERATOR::INVALID_OP){
							error_str = CStringUtils::formatString("undefined operator %s for number type",op->str);
							return false;
						}
						res_obj = new int;
						result_type = CCompiler::INTEGER;

			}else if((t_left == CNumberFactory::getPointerTypeStr()) ){
				if((id_op=getNumberOperatorId_OneOp(op->id))==CCompiler::ASM_OPERATOR::INVALID_OP){
					error_str = CStringUtils::formatString("undefined operator %s for number type",op->str);
					return false;
				}
				res_obj = new float;
				result_type = CCompiler::NUMBER;

			}else if(t_left== CStringFactory::getPointerTypeStr()){

				if((id_op=getStringOperatorId_OneOp(op->id))==CCompiler::ASM_OPERATOR::INVALID_OP){
					error_str = CStringUtils::formatString("undefined operator %s for string type",op->str);
					return false;
				}
				res_obj = new string;
				result_type = CCompiler::STRING;
			}else if((t_left == CBooleanFactory::getPointerTypeStr())){
				if((id_op=getBoleanOperatorId_OneOp(op->id))==CCompiler::ASM_OPERATOR::INVALID_OP){
					error_str = CStringUtils::formatString("undefined operator %s for boolean type",op->str);
					return false;
				}
				res_obj = new bool;
				result_type = CCompiler::BOOL;
			}else{
				error_str = CStringUtils::formatString("not compatible %s %s",t_left.c_str(),t_right.c_str());
				return false;
			}

			CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
			//asm_op->type_op=OPERATOR;
			asm_op->operator_type = id_op;
			asm_op->result_type = result_type;
			asm_op->result_obj = res_obj;
			asm_op->index_left = left;
			asm_op->index_right = -1;

		//	printf("[%02i:%02i]\t%s\t[%02i:%02i]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),CCompiler::def_operator[id_op].op_str,m_currentListStatements->size(),left);

			ptr_current_statement_op->asm_op.push_back(asm_op);

	}else{
		error_str = CStringUtils::formatString("ERROR: both operant is out of internal stack ");
		return false;
	}
	return true;
}
//------------------------------------------------------------------------------------------------------------------
//
// COMPILE EXPRESSIONS AND GENERATE ITS ASM
//
int CCompiler::gacExpression_Recursive(PASTNode op, int & numreg, bool & error, CScope *_lc){

	//CScope * _lc = m_currentScriptFunction->getScope();
	int r=0;
	bool inline_if_else=false;
	string error_str;
	if(op==NULL){
		return -1;
	}



	if(op->children.size()==0){//[LEFT_NODE]==NULL && op->children[RIGHT_NODE]==NULL){ // trivial case value itself...

		//printf("CONST \tE[%i],%s\n",numreg,op->value.c_str());
		if(!insertLoadValueInstruction(op->value_symbol, _lc, op->definedValueline)){
			error|=true;
			return -1;

		}

		r=numreg;
	}else{

		if(op->children.size()==3){

			if(op->operator_info->id == INLINE_IF_PUNCTUATOR && op->children[2]->operator_info->id == INLINE_ELSE_PUNCTUATOR){
				// node children[0]: conditional.
				// node children[1]: body-if
				// node children[2]: body-else
				inline_if_else = true;
			}

		}

		// check if there's inline-if-else
		int right=0, left=0;

		left=gacExpression_Recursive(op->children[LEFT_NODE],numreg,error, _lc);

		if(error) return -1;

		if(op->children.size()==2){
			right=gacExpression_Recursive(op->children[RIGHT_NODE],numreg,error,_lc);
		}
		else {
			right = -1;
		}

		if(error) return -1;

		r=numreg;

		if(left !=-1 && right!=-1){ // 2 ops

			// particular case if operator is =
			if(op->operator_info->id == PUNCTUATOR_TYPE::ASSIGN_PUNCTUATOR){
				// the variable can only assigned if the type is the same or if the type is undefined.
				// check if left operand is registered variable...
				CScope::tInfoRegisteredVar * info_var = _lc->getInfoRegisteredSymbol(op->children[LEFT_NODE]->value_symbol,false);
				if(info_var == NULL){
					print_error_cr("undeclared variable \"%s\"");
					error|=true;
					return -1;
				}else{ // ok is declared ... let's see if undefined variable or is the same type ...

					CObject *var_obj = info_var->m_obj;

					bool is_undefined = dynamic_cast<CUndefined *>(var_obj) != NULL;
					string ptr_class_type =  getUserTypeResultCurrentStatmentAtInstruction(right);

					if(ptr_class_type=="unknow"){
						error|=true;
						return -1;
					}

					if(is_undefined ||
							var_obj->getPointerClassStr() == ptr_class_type
						){

						if(is_undefined){ // create object ...

							var_obj = CFactoryContainer::getInstance()->newObjectByClassPtr(ptr_class_type);
							if(var_obj!=NULL){
								_lc->defineSymbol(op->children[LEFT_NODE]->value_symbol,var_obj);
								print_com_cr("%s defined as %s",op->children[LEFT_NODE]->value_symbol.c_str(),ptr_class_type.c_str());
							}else{
								print_error_cr("ERRRRRRRRRROR %s is not registered",ptr_class_type.c_str());
								error|=true;
								return -1;
							}
						}




					    // set value the operator = must be defined !
						if(!CCompiler::getInstance()->insertMovVarInstruction(var_obj,right)){
							print_error_cr("ERRRRRRRRRROR 2");
							error|=true;
							return -1;
						}

						//printf("MOV \tV(%s),E[%i]\n",op->left->value.c_str(),right);


					}else{
						print_error_cr("\"%s\" was instanced as \"%s\" and cannot be change type as \"%s\"",op->children[LEFT_NODE]->value_symbol.c_str(),var_obj->getPointerClassStr().c_str(),op->type_ptr.c_str());
						error|=true;
						return -1;
					}
				}
			}
			else{
				//printf("%s\tE[%i],E[%i],E[%i]\n",op->token.c_str(),numreg,left,right);
				if(!insertOperatorInstruction(op->operator_info, error_str,left,right)){
					print_error_cr("%s at line %i",error_str.c_str(),op->definedValueline);
					error|=true;
					return -1;
				}
			}

		}else if(right!=-1){ // one op..
			//printf("%s\tE[%i],E[%i]\n",op->token.c_str(),numreg,right);
			if(!insertOperatorInstruction(op->operator_info, error_str,right)){
				print_error_cr("%s at line %i",error_str.c_str(),op->definedValueline);
				error|=true;
				return -1;
			}

		}else if(left!=-1){ // one op..
		//	printf("%s\tE[%i],E[%i]\n",op->token.c_str(),numreg,left);
			if(!insertOperatorInstruction(op->operator_info, error_str,left)){
				print_error_cr("%s at line %i",error_str.c_str(),op->definedValueline);
				error|=true;
				return -1;
			}

		}else{ // ERROR
			print_error_cr("ERROR both ops ==0!");
			error|=true;
			return -1;
		}

	}
	numreg++;

	return r;
}



bool CCompiler::gacFor(PASTNode _node, CScope * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::FOR_KEYWORD){print_error_cr("node is not FOR keyword type");return false;}
	if(_node->children.size()!=4) {print_error_cr("node FOR has not valid number of nodes");return false;}
	if(!(_node->children[0]->node_type==PRE_FOR_NODE && _node->children[1]->node_type==CONDITIONAL_NODE &&
	_node->children[2]->node_type==POST_FOR_NODE && _node->children[3]->node_type==BODY_NODE)) {print_error_cr("node FOR has not valid TYPE nodes");return false;}
	CCompiler::tInfoAsmOp *asm_op;
	//CScope * _currentScope = _node ->scope_ptr;

	// 1. compile var init ...
	if(!ast2asm_Recursive(_node->children[0],_node->scope_ptr)){ return false;}

	// 2. compile conditional

	if(!ast2asm_Recursive(_node->children[1],_node->scope_ptr)){ return false;}
	// get current index statment in order to jmp from end body for.
	int index_statment_conditional_for_= getCurrentStatmentIndex();

	// insert conditional jmp (if not true go to the end)
	asm_op = insert_JNT_Instruction();


	// 3. compile body
	if(!gacBody(_node->children[3],_node->scope_ptr)){ return false;}


	// 4. compile post oper
	if(!ast2asm_Recursive(_node->children[2],_node->scope_ptr)){ return false;}

	// 5. jmp to the conditional index ...
	insert_JMP_Instruction(index_statment_conditional_for_);

	asm_op->result_obj=(void *)(getCurrentStatmentIndex()+1);

	return true;
}
bool CCompiler::gacWhile(PASTNode _node, CScope * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::WHILE_KEYWORD){print_error_cr("node is not WHILE keyword type");return false;}
	if(_node->children.size()!=2) {print_error_cr("node WHILE has not valid number of nodes");return false;}
	if(!(_node->children[0]->node_type==CONDITIONAL_NODE && _node->children[1]->node_type==BODY_NODE )) {print_error_cr("node WHILE has not valid TYPE nodes");return false;}
	CCompiler::tInfoAsmOp *asm_op_jmp_end;
	int index_ini_while;


	// compile conditional expression...
	if(!ast2asm_Recursive(_node->children[0],_lc)){ return false;}
	index_ini_while = getCurrentStatmentIndex();
	asm_op_jmp_end = insert_JNT_Instruction(); // goto end  ...

	// compile if-body ...
	if(!gacBody(_node->children[1],_lc)){ return false;}
	insert_JMP_Instruction(index_ini_while); // goto end  ...
	asm_op_jmp_end->result_obj = (void *)(getCurrentStatmentIndex()+1);
	return true;
}

bool CCompiler::gacIf(PASTNode _node, CScope * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::IF_KEYWORD){print_error_cr("node is not IF keyword type");return false;}
	if(_node->children.size()<2) {print_error_cr("node IF has not valid number of nodes");return false;}
	if(!(_node->children[0]->node_type==CONDITIONAL_NODE && _node->children[1]->node_type==BODY_NODE )) {print_error_cr("node IF has not valid TYPE nodes");return false;}
	CCompiler::tInfoAsmOp *asm_op_jmp_else_if,*asm_op_jmp_end;

	// compile conditional expression...
	if(!ast2asm_Recursive(_node->children[0],_lc)){ return false;}
	asm_op_jmp_else_if = insert_JNT_Instruction(); // goto else body ...

	// compile if-body ...
	if(!gacBody(_node->children[1],_lc)){ return false;}


	// if there's else body, compile-it
	if(_node->children.size()==3){
		asm_op_jmp_end = insert_JMP_Instruction(); // goto end
		asm_op_jmp_else_if->result_obj = (void *)(getCurrentStatmentIndex()+1);
		if(!gacBody(_node->children[2],_lc)){ return false;}

		asm_op_jmp_end->result_obj = (void *)(getCurrentStatmentIndex()+1);

	}
	else{
		asm_op_jmp_else_if->result_obj = (void *)(getCurrentStatmentIndex()+1);
	}


	return true;
}

bool CCompiler::gacSwitch(PASTNode _node, CScope * _lc){

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != SWITCH_KEYWORD){print_error_cr("node is not SWITCH keyword type");return false;}
	bool has_default = false;
	PASTNode switch_node;
	PASTNode group_cases;
	PASTNode case_value;
	PASTNode case_body;

	CCompiler::tInfoAsmOp * asm_op;

	string error_str;
	string detected_type_str;
	CScope *_scope = this->m_currentScriptFunction->getScope()->getCurrentScopePointer();

	// create new statment ...
	CCompiler::getInstance()->newStatment();


	if(insertLoadValueInstruction(_node->value_symbol,_scope,_node->definedValueline)){ // insert condition value ...

		// get current instruction value to take as ref for compare within value cases...
		int switch_value_index  = getCurrentInstructionIndex();

		// the stratege is first evaluate all cases and then their bodies...
		for(unsigned s=0; s < 3; s++){
			for(unsigned i = 0; i < _node->children.size(); i++){ // expect node type group cases ...

				switch_node = _node->children[i];

				if(switch_node != NULL){ // the rules are the following children[0]:group_cases and children[1]:body_case

					if(switch_node->children.size() == 2){
						group_cases = switch_node->children[0];
						case_body = switch_node->children[1];

						switch(s){

						case 0: // GENERATE ASM FOR CONDITIONAL CASES ...

							if(group_cases->node_type == GROUP_CASES_NODE){

								//int condition_index  = getCurrentInstructionIndex();

								for(unsigned j = 0; j < group_cases->children.size(); j++){ // generate condition case ...
									case_value = group_cases->children[j];

									if(case_value->node_type == KEYWORD_NODE && case_value->keyword_info != NULL){

										switch(case_value->keyword_info->id){
										default:
											print_error_cr("Unexpected %s keyword node in SWITCH node",case_value->keyword_info->str);
											break;
										case DEFAULT_KEYWORD:

											if(!has_default){
												has_default = true;
												// insert jmp instruction and save its information to store where to jmp when we know the total code size of cases + body...
												case_value->aux_value = insert_JMP_Instruction();
											}else{
												print_error_cr("case already defined!");
												return false;
											}
											break;
										case CASE_KEYWORD:

											// load case X:
											insertLoadValueInstruction(case_value->value_symbol,_scope,case_value->definedValueline);

											// is equal ? ==
											if(!insertOperatorInstruction(&CAst::defined_operator_punctuator[LOGIC_EQUAL_PUNCTUATOR],error_str, switch_value_index ,getCurrentInstructionIndex())){
													print_error_cr("%s",error_str.c_str());
													return false;
											}

											// insert jmp instruction and save its information to store where to jmp when we know the total code size of cases...
											case_value->aux_value = insert_JT_Instruction();

											break;

										}

									}else{
										print_error_cr("Not SWITCH case or NULL keyword info");
										return false;
									}
								}

							}else{
								print_error_cr("Expected group cases type node in SWITCH node");
								return false;
							}
							break;

						case 1: // GENERATE ASM FOR BODY AND WRITE initial JMP

							if(gacBody(case_body,_lc)){
								for(unsigned i = 0; i < group_cases->children.size(); i++){
									case_value = group_cases->children[i];
									asm_op = (CCompiler::tInfoAsmOp *)case_value->aux_value; // load jt instruction and set current instruction before write asm code.
									asm_op->result_obj = (void *)getCurrentStatmentIndex();
								}

								if(i < (_node->children.size()-1))
									case_body->aux_value = insert_JMP_Instruction();
							}else{
								return false;
							}
							break;

						case 2: // FINALLY, WRITE JMP's to end statment

							if((asm_op = (CCompiler::tInfoAsmOp *)case_body->aux_value) != NULL){
								asm_op->result_obj = (void *)(getCurrentStatmentIndex()+1);
							}
							break;
						}
					}
					else{
						print_error_cr("SWITCH node has not 2 nodes");
						return false;
					}
				}
				else{
					print_error_cr("SWITCH node NULL");
					return false;
				}
			}
		}
	}

	return true;
}

bool CCompiler::gacVar(PASTNode _node, CScope * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != VAR_KEYWORD){print_error_cr("node is not VAR keyword type");return false;}

	if(_node->children.size() == 1){ // an expression is expected ...
		return gacExpression(_node->children[0], _lc);
	}

	return false;

}

bool CCompiler::gacKeyword(PASTNode _node, CScope * _lc){

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}

	switch(_node->keyword_info->id){
	default:
		print_error_cr("Keyword [ %s ] not implemented yet!",_node->keyword_info->str);
		break;
	case KEYWORD_TYPE::SWITCH_KEYWORD:
		return gacSwitch(_node, _lc);
		break;
	case KEYWORD_TYPE::FOR_KEYWORD:
		return gacFor(_node, _lc);
		break;
	case KEYWORD_TYPE::WHILE_KEYWORD:
		return gacWhile(_node, _lc);
		break;
	case KEYWORD_TYPE::IF_KEYWORD:
		return gacIf(_node, _lc);
		break;
	case KEYWORD_TYPE::VAR_KEYWORD:
		return gacVar(_node, _lc);
		break;

	}

	return false;
}

bool CCompiler::gacBody(PASTNode _node, CScope * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != BODY_NODE ){print_error_cr("node is not BODY type or null");return false;}

	if(_node->children.size() == 1){ // body has code ...

		bool ok = ast2asm_Recursive(_node->children[0], _lc);

		return ok;
	}else{ // insert one statment at least ..
		newStatment();
	}

	return true;
}

bool CCompiler::gacExpression(PASTNode _node, CScope *_lc){

	int numreg=0;
	CCompiler::tInfoStatementOp i_stat;

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != EXPRESSION_NODE){print_error_cr("node is not Expression");return false;}

	if(_node->children.size() == 1){


		// new statment ...
		(*m_currentListStatements).push_back(i_stat);

		bool error_asm=false;
		gacExpression_Recursive(_node->children[0],numreg,error_asm, _lc);

		return !error_asm;
	}
	else{
		print_error_cr("Expression node has no expressions ");
	}

	return false;

}

bool CCompiler::ast2asm_Recursive(PASTNode _node, CScope *_lc){



	if(_node != NULL){
		switch(_node->node_type){
			default:
			case UNKNOWN_NODE:
				print_info_cr("UNKNOWN_NODE (%i)",_node->node_type);
				return false;
				break;
			break;
			break;
			case MAIN_NODE:print_info_cr("MAIN_NODE");
			break;
			case PUNCTUATOR_NODE:print_info_cr("PUNCTUATOR_NODE");break;
			case EXPRESSION_NODE:
				print_info_cr("EXPRESSION_NODE");
				return gacExpression(_node, _lc);
				break;
			case GROUP_CASES_NODE:
				print_info_cr("GROUP_CASES_NODE");
				break;
			case KEYWORD_NODE:
				print_info_cr("KEYWORD_NODE");
				return gacKeyword(_node, _lc);
				break;
			case FUNCTION_ARGS_DECL_NODE:print_info_cr("FUNCTION_ARGS_DECL_NODE");break;
			case FUNCTION_OR_CLASS_ARGS_CALL_NODE:print_info_cr("FUNCTION_OR_CLASS_ARGS_CALL_NODE");break;
			case ARRAY_INDEX_NODE:print_info_cr("ARRAY_INDEX_NODE");break;
			case ARRAY_OBJECT_NODE:print_info_cr("ARRAY_OBJECT_NODE");break;
			case FUNCTION_OBJECT_NODE:print_info_cr("FUNCTION_OBJECT_NODE");break;
			case SYMBOL_NODE:print_info_cr("SYMBOL_NODE");break;
			case BODY_NODE:
				print_info_cr("BODY_NODE");
				return gacBody(_node, _lc);
				break;
			case POST_FOR_NODE:
			case CONDITIONAL_NODE:
				print_info_cr("%s",_node->node_type == CONDITIONAL_NODE ? "CONDITIONAL_NODE":"POST_FOR_NODE");
				if(_node->children.size() == 1){
					return gacExpression(_node->children[0], _lc);
				}else{
					print_error_cr("Expected nodes for %i",_node->node_type);
				}

				break;
			case PRE_FOR_NODE:


				if(_node->children.size() == 1){
					return ast2asm_Recursive(_node->children[0], _lc);
				}else{
					print_error_cr("Expected nodes for %i",_node->node_type);
				}

				break;
			case CLASS_VAR_COLLECTION_NODE:print_info_cr("CLASS_VAR_COLLECTION_NODE");break;
			case CLASS_FUNCTION_COLLECTION_NODE:print_info_cr("CLASS_FUNCTION_COLLECTION_NODE");break;
			case BASE_CLASS_NODE:print_info_cr("BASE_CLASS_NODE");break;
			case CALLING_OBJECT_NODE:print_info_cr("CALLING_OBJECT_NODE");break;
		}
	}


	return false;
}

bool CCompiler::ast2asm(PASTNode _node, CScriptFunction *sf){

	if(_node == NULL){
		print_error_cr("NULL node!");
		return false;
	}

	m_currentScriptFunction = sf;

	// reset current pointer ...
	m_treescope->resetScopePointer();


	if(_node->node_type == NODE_TYPE::MAIN_NODE){

		for(unsigned i = 0; i < _node->children.size(); i++){
			if(!ast2asm_Recursive(_node->children[i], m_treescope->getCurrentScopePointer())){
				print_error_cr("Error 1!");
				return false;
			}
		}

		return true;
	}
	else{
		print_error_cr("Main node expected");
	}

	return false;
}


/*
bool CCompiler::generateAsmCode_Recursive(root){

}


bool CCompiler::generateAsmCode(root){

}*/

bool CCompiler::compile(const string & s, CScriptFunction * sf){

	PASTNode root=NULL;
	this->m_currentScriptFunction = sf;
	this->m_currentListStatements = sf->getCompiledCode();
	this->m_treescope = sf->getScope();

	// generate whole AST

	if(CAst::generateAST(s.c_str(),sf, &root)){


		if(ast2asm(root,sf)){

			// print generated asm ...
			CCompiler::printGeneratedCode(sf);
		}
		// then you have all information -> compile into asm!
		//generateAsmCode(root);

	}

	delete root;


	return false;
}


CCompiler::~CCompiler(){

}
