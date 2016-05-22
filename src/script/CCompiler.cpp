#include "zg_script.h"

#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __VERBOSE_MESSAGE__
#endif



#ifdef  __VERBOSE_MESSAGE__

#define print_com_cr print_ast_cr
#else
#define print_com_cr(s,...)
#endif




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
//------------------------------------------------------------------------------------------------------------------
//
// CONSTRUCTOR
//

CCompiler::CCompiler(){
	m_currentListStatements = NULL;
	m_currentScriptFunction = NULL;
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
	CVirtualMachine::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	string result="unknow";

	if(instruction >=0 && instruction < ptr_current_statement_op->asm_op.size()){
		switch(ptr_current_statement_op->asm_op[instruction]->result_type){
		default:
		case CVirtualMachine::NOT_DEFINED:
			break;
		case CVirtualMachine::OBJ:
			result= (((CObject *)ptr_current_statement_op->asm_op[instruction]->result_obj)->getPointerClassStr()); // type result..
			break;
		case CVirtualMachine::NUMBER:
			result= CNumberFactory::getPointerTypeStr();
			break;
		case CVirtualMachine::INTEGER:
			result= CIntegerFactory::getPointerTypeStr();
			break;
		case CVirtualMachine::STRING:
			result= CStringFactory::getPointerTypeStr();
			break;
		case CVirtualMachine::BOOL:
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
	CVirtualMachine::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	return ptr_current_statement_op->asm_op.size()-1;
}

int CCompiler::getCurrentStatmentIndex(){
	return (int)(m_currentListStatements->size()-1);
}

CVirtualMachine::tInfoStatementOp * CCompiler::newStatment(){
	CVirtualMachine::tInfoStatementOp st;

	m_currentListStatements->push_back(st);

	return  &(*m_currentListStatements)[m_currentListStatements->size()-1];
}

bool *  CCompiler::getObjectResultCurrentStatmentAsBoolean(){

	CVirtualMachine::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	bool *result=NULL;
	unsigned instruction = ptr_current_statement_op->asm_op.size()-1;

	if(instruction >=0 && instruction < ptr_current_statement_op->asm_op.size()){
		if(ptr_current_statement_op->asm_op[instruction]->result_type == CVirtualMachine::BOOL){
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


bool CCompiler::insertLoadValueInstruction(const string & v, string & type_ptr, CScope * _lc, int m_var_at_line){

	CNumber *num_obj;
	CInteger *int_obj;
	CString *str_obj;
	CBoolean *bool_obj;
	string type_value="CONST";

	CVirtualMachine::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	void *obj;
	CVirtualMachine::VAR_TYPE type=CVirtualMachine::NOT_DEFINED;
	// try parse value...
	if((obj=CInteger::ParsePrimitive(v))!=NULL){
			type=CVirtualMachine::INTEGER;
			print_com_cr("%s detected as int\n",v.c_str());
	}
	else if((obj=CNumber::ParsePrimitive(v))!=NULL){
		type=CVirtualMachine::NUMBER;
		print_com_cr("%s detected as float\n",v.c_str());
	}
	else if(v[0]=='\"' && v[v.size()-1]=='\"'){
		type=CVirtualMachine::STRING;
		string s=v.substr(1,v.size()-2);
		string *so=new string();
		(*so)=s;
		obj = so;
		print_com_cr("%s detected as string\n",v.c_str());
	}
	else if((obj=CBoolean::ParsePrimitive(v))!=NULL){
		type=CVirtualMachine::BOOL;
		print_com_cr("%s detected as boolean\n",v.c_str());
	}else{
		CScope::tInfoRegisteredVar * info_var=_lc->getInfoRegisteredSymbol(v,false);
		type=CVirtualMachine::OBJ;

		if(info_var==NULL){
			print_error_cr("symbol %s at line %i is not declared ", v.c_str(),m_var_at_line);
			return false;
		}

		CObject *var = info_var->m_obj;
		obj = var;
		type_value="OBJ";
		if((int_obj=dynamic_cast<CInteger *>(var))!=NULL){ // else it will store the value ...
			type_value="INT";
			type = CVirtualMachine::INTEGER;
			obj=&int_obj->m_value;
		}else if((num_obj=dynamic_cast<CNumber *>(var))!=NULL){
			type_value="FLOAT";
			type = CVirtualMachine::NUMBER;
			obj=&num_obj->m_value;
		}else if((bool_obj=dynamic_cast<CBoolean *>(var))!=NULL){
			type_value="BOOL";
			type = CVirtualMachine::BOOL;
			obj=&bool_obj->m_value;
		}else if((str_obj=dynamic_cast<CString *>(var))!=NULL){
			type_value="STRING";
			type = CVirtualMachine::STRING;
			obj=&str_obj->m_value;
		}
	}

	if(obj != NULL){

		CVirtualMachine::tInfoAsmOp *asm_op = new CVirtualMachine::tInfoAsmOp();

		asm_op->result_obj=obj;
		asm_op->result_type = type;

		asm_op->operator_type=CVirtualMachine::ASM_OPERATOR::LOAD;
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

	CVirtualMachine::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];

	if(right <0 || (unsigned)right >= ptr_current_statement_op->asm_op.size()){
		print_error_cr("ERROR: right operant is out of internal stack (%i,%i) ",right,ptr_current_statement_op->asm_op.size());
		return false;
	}

	if(right >= 0){ // insert both op...

		CVirtualMachine::tInfoAsmOp *asm_op = new CVirtualMachine::tInfoAsmOp();
		asm_op->result_type = CVirtualMachine::OBJ; // this only stores pointer...
		asm_op->result_obj=var;

		if((int_obj=dynamic_cast<CInteger *>(var))!=NULL){ // else it will store the value ...
				asm_op->result_type = CVirtualMachine::INTEGER;
				asm_op->result_obj=&int_obj->m_value;
		}else if((num_obj=dynamic_cast<CNumber *>(var))!=NULL){ // else it will store the value ...
			asm_op->result_type = CVirtualMachine::NUMBER;
			asm_op->result_obj=&num_obj->m_value;
		}else if((bool_obj=dynamic_cast<CBoolean *>(var))!=NULL){
			asm_op->result_type = CVirtualMachine::BOOL;
			asm_op->result_obj=&bool_obj->m_value;
		}else if((str_obj=dynamic_cast<CString *>(var))!=NULL){
			asm_op->result_type = CVirtualMachine::STRING;
			asm_op->result_obj=&str_obj->m_value;
		}

		asm_op->index_left = -1;
		asm_op->index_right = right;
		asm_op->operator_type = CVirtualMachine::ASM_OPERATOR::MOV;
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
	/*CVirtualMachine::tInfoStatementOp op_push_scope;
	CVirtualMachine::tInfoAsmOp *asm_op = new CVirtualMachine::tInfoAsmOp();
	asm_op->result_obj = _goto_scope;
	asm_op->operator_type=CVirtualMachine::ASM_OPERATOR::PUSH_SCOPE;
	op_push_scope.asm_op.push_back(asm_op);

	m_listStatements.push_back(op_push_scope);*/
}


void CCompiler::insertPopScopeInstruction(){
/*
	CVirtualMachine::tInfoStatementOp op_pop_scope;
	CVirtualMachine::tInfoAsmOp *asm_op = new CVirtualMachine::tInfoAsmOp();
	asm_op->result_obj = this->m_parentScope;
	asm_op->operator_type=CVirtualMachine::ASM_OPERATOR::POP_SCOPE;
	op_pop_scope.asm_op.push_back(asm_op);

	m_listStatements.push_back(op_pop_scope);*/

}

CVirtualMachine::tInfoAsmOp * CCompiler::insert_JMP_Instruction(){

	CVirtualMachine::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CVirtualMachine::tInfoAsmOp *asm_op = new CVirtualMachine::tInfoAsmOp();
	asm_op->result_obj = NULL;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CVirtualMachine::ASM_OPERATOR::JMP;
	ptr_current_statement_op->asm_op.push_back(asm_op);
	//printf("[%02i:%02i]\tJMP\t[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());

	return asm_op;
}

CVirtualMachine::tInfoAsmOp * CCompiler::insert_JNT_Instruction(){

	CVirtualMachine::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CVirtualMachine::tInfoAsmOp *asm_op = new CVirtualMachine::tInfoAsmOp();
	asm_op->result_obj = NULL;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CVirtualMachine::ASM_OPERATOR::JNT;

	//printf("[%02i:%02i]\tJNT\t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);



	return asm_op;
}

CVirtualMachine::tInfoAsmOp * CCompiler::insert_JT_Instruction(){

	CVirtualMachine::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CVirtualMachine::tInfoAsmOp *asm_op = new CVirtualMachine::tInfoAsmOp();
	asm_op->result_obj = NULL;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CVirtualMachine::ASM_OPERATOR::JT;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);
	return asm_op;
}

void CCompiler::insert_NOP_Instruction(){
	CVirtualMachine::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CVirtualMachine::tInfoAsmOp *asm_op = new CVirtualMachine::tInfoAsmOp();
	asm_op->result_obj = NULL;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CVirtualMachine::ASM_OPERATOR::NOP;
	//printf("[%02i:%02i]\tNOP\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());
	ptr_current_statement_op->asm_op.push_back(asm_op);

}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

CVirtualMachine::ASM_OPERATOR CCompiler::getIntegerOperatorId_TwoOps(const string & op, CVirtualMachine::VAR_TYPE & result_type){

	result_type = CVirtualMachine::INTEGER;

	if(op=="+"){
		return CVirtualMachine::ADD;
	}else if(op=="/"){
		return CVirtualMachine::DIV;
	}else if(op=="%"){
		return CVirtualMachine::MOD;
	}else if(op=="*"){
		return CVirtualMachine::MUL;
	}else if(op=="&"){
		return CVirtualMachine::AND;
	}else if(op=="|"){
		return CVirtualMachine::OR;
	}else if(op=="^"){
		return CVirtualMachine::XOR;
	}else if(op=="<<"){
		return CVirtualMachine::SHL;
	}else if(op==">>"){
		return CVirtualMachine::SHR;
	}else if(op==">="){
		result_type = CVirtualMachine::BOOL;
		return CVirtualMachine::GTE;
	}else if(op==">"){
		result_type = CVirtualMachine::BOOL;
		return CVirtualMachine::GT;
	}else if(op=="<"){
		result_type = CVirtualMachine::BOOL;
		return CVirtualMachine::LT;
	}else if(op=="<="){
		result_type = CVirtualMachine::BOOL;
		return CVirtualMachine::LTE;
	}else if(op=="=="){
		result_type = CVirtualMachine::BOOL;
		return CVirtualMachine::EQU;
	}
	return CVirtualMachine::ASM_OPERATOR::INVALID_OP;
}

CVirtualMachine::ASM_OPERATOR CCompiler::getNumberOperatorId_TwoOps(const string & op, CVirtualMachine::VAR_TYPE & result_type){

	result_type = CVirtualMachine::NUMBER;

	if(op=="+"){
		return CVirtualMachine::ADD;
	}else if(op=="/"){
		return CVirtualMachine::DIV;
	}else if(op=="%"){
		return CVirtualMachine::MOD;
	}else if(op=="*"){
		return CVirtualMachine::MUL;
	}else if(op==">="){
		result_type = CVirtualMachine::BOOL;
		return CVirtualMachine::GTE;
	}else if(op==">"){
		result_type = CVirtualMachine::BOOL;
		return CVirtualMachine::GT;
	}else if(op=="<"){
		result_type = CVirtualMachine::BOOL;
		return CVirtualMachine::LT;
	}else if(op=="<="){
		result_type = CVirtualMachine::BOOL;
		return CVirtualMachine::LTE;
	}else if(op=="=="){
		result_type = CVirtualMachine::BOOL;
		return CVirtualMachine::EQU;
	}
	return CVirtualMachine::ASM_OPERATOR::INVALID_OP;
}

CVirtualMachine::ASM_OPERATOR CCompiler::getNumberOperatorId_OneOp(const string & op){

	if(op=="-"){
		return CVirtualMachine::NEG;
	}else if(op=="++"){
		return CVirtualMachine::PRE_INC;
	}else if(op=="post_++"){
		return CVirtualMachine::POST_INC;
	}else if(op=="--"){
		return CVirtualMachine::PRE_DEC;
	}else if(op=="post_--"){
		return CVirtualMachine::POST_DEC;
	}

	return CVirtualMachine::ASM_OPERATOR::INVALID_OP;
}

CVirtualMachine::ASM_OPERATOR CCompiler::getBoleanOperatorId_TwoOps(const string & op, CVirtualMachine::VAR_TYPE & result_type){

	result_type = CVirtualMachine::BOOL;

	if(op=="&&"){
		return CVirtualMachine::LOGIC_AND;
	}else if(op=="||"){
		return CVirtualMachine::LOGIC_OR;
	}

	return CVirtualMachine::ASM_OPERATOR::INVALID_OP;
}

CVirtualMachine::ASM_OPERATOR CCompiler::getBoleanOperatorId_OneOp(const string & op){
	if(op=="!"){
		return CVirtualMachine::NOT;
	}
	return CVirtualMachine::ASM_OPERATOR::INVALID_OP;
}

CVirtualMachine::ASM_OPERATOR CCompiler::getStringOperatorId_TwoOps(const string & op, CVirtualMachine::VAR_TYPE & result_type){

	result_type = CVirtualMachine::STRING;

	if(op=="+"){
		return CVirtualMachine::CAT;
	}else if(op=="=="){
		result_type = CVirtualMachine::BOOL;
		return CVirtualMachine::EQU;
	}
	return CVirtualMachine::ASM_OPERATOR::INVALID_OP;
}

CVirtualMachine::ASM_OPERATOR CCompiler::getStringOperatorId_OneOp(const string & op){

	return CVirtualMachine::ASM_OPERATOR::INVALID_OP;
}


bool CCompiler::insertOperatorInstruction(const string & op, string & error_str, int left, int right){

	CVirtualMachine::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];

	if(left <0 || (unsigned)left >= ptr_current_statement_op->asm_op.size()){
		error_str = CStringUtils::formatString("left operant is out of internal stack (%i,%i) ",left,ptr_current_statement_op->asm_op.size());
		return false;
	}

	string t_left = getUserTypeResultCurrentStatmentAtInstruction(left);
	string t_right = "unknow";
	if(right>=0){
		t_right = getUserTypeResultCurrentStatmentAtInstruction(right);
	}

	CVirtualMachine::ASM_OPERATOR id_op;
	CVirtualMachine::VAR_TYPE result_type=CVirtualMachine::NOT_DEFINED;
	void *res_obj=NULL;


	if(left >=0 && right >= 0){ // insert both op...

		if(((t_left == CIntegerFactory::getPointerTypeStr())) &&
						((t_right == CIntegerFactory::getPointerTypeStr()))){
					if((id_op=getIntegerOperatorId_TwoOps(op,result_type))==CVirtualMachine::ASM_OPERATOR::INVALID_OP){
						error_str = CStringUtils::formatString("undefined operator %s",op.c_str());
						return false;
					}

		}else if(((t_left == CNumberFactory::getPointerTypeStr()) ) &&
				((t_right == CNumberFactory::getPointerTypeStr()) )){
			if((id_op=getNumberOperatorId_TwoOps(op,result_type))==CVirtualMachine::ASM_OPERATOR::INVALID_OP){
				error_str = CStringUtils::formatString("undefined operator %s",op.c_str());
				return false;
			}

		}else if(t_left== CStringFactory::getPointerTypeStr()){

			if((id_op=getStringOperatorId_TwoOps(op,result_type))==CVirtualMachine::ASM_OPERATOR::INVALID_OP){
				error_str = CStringUtils::formatString("undefined operator %s",op.c_str());
				return false;
			}

		}else if((t_left == CBooleanFactory::getPointerTypeStr()) && t_right == CBooleanFactory::getPointerTypeStr()){
			if((id_op=getBoleanOperatorId_TwoOps(op, result_type))==CVirtualMachine::ASM_OPERATOR::INVALID_OP){
				error_str = CStringUtils::formatString("undefined operator %s",op.c_str());
				return false;
			}
		}else{
			error_str = CStringUtils::formatString("not compatible %s %s",t_left.c_str(),t_right.c_str());
			return false;
		}

		switch(result_type){
		default:
		case CVirtualMachine::NOT_DEFINED:
			error_str = CStringUtils::formatString("unknow result type");
			return false;
			break;
		case CVirtualMachine::NUMBER:
			res_obj = new float;
			break;
		case CVirtualMachine::INTEGER:
			res_obj = new int;
			break;
		case CVirtualMachine::STRING:
			res_obj = new string;
			break;
		case CVirtualMachine::BOOL:
			res_obj = new bool;
			break;

		}

		CVirtualMachine::tInfoAsmOp *asm_op = new CVirtualMachine::tInfoAsmOp();
		//asm_op->type_op=OPERATOR;
		asm_op->index_left = left;
		asm_op->index_right = right;


		//-----------------------
		asm_op->result_type = result_type;
		asm_op->operator_type = id_op;
		asm_op->result_obj = res_obj;

	//	printf("[%02i:%02i]\t%s\t[%02i:%02i],[%02i:%02i]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),CVirtualMachine::def_operator[id_op].op_str,m_currentListStatements->size(),left,m_currentListStatements->size(),right);

		ptr_current_statement_op->asm_op.push_back(asm_op);


	}else if(left >=0){ // insert one operand

			if((t_left == CIntegerFactory::getPointerTypeStr()) ){
						if((id_op=getNumberOperatorId_OneOp(op))==CVirtualMachine::ASM_OPERATOR::INVALID_OP){
							error_str = CStringUtils::formatString("undefined operator %s for number type",op.c_str());
							return false;
						}
						res_obj = new int;
						result_type = CVirtualMachine::INTEGER;

			}else if((t_left == CNumberFactory::getPointerTypeStr()) ){
				if((id_op=getNumberOperatorId_OneOp(op))==CVirtualMachine::ASM_OPERATOR::INVALID_OP){
					error_str = CStringUtils::formatString("undefined operator %s for number type",op.c_str());
					return false;
				}
				res_obj = new float;
				result_type = CVirtualMachine::NUMBER;

			}else if(t_left== CStringFactory::getPointerTypeStr()){

				if((id_op=getStringOperatorId_OneOp(op))==CVirtualMachine::ASM_OPERATOR::INVALID_OP){
					error_str = CStringUtils::formatString("undefined operator %s for string type",op.c_str());
					return false;
				}
				res_obj = new string;
				result_type = CVirtualMachine::STRING;
			}else if((t_left == CBooleanFactory::getPointerTypeStr())){
				if((id_op=getBoleanOperatorId_OneOp(op))==CVirtualMachine::ASM_OPERATOR::INVALID_OP){
					error_str = CStringUtils::formatString("undefined operator %s for boolean type",op.c_str());
					return false;
				}
				res_obj = new bool;
				result_type = CVirtualMachine::BOOL;
			}else{
				error_str = CStringUtils::formatString("not compatible %s %s",t_left.c_str(),t_right.c_str());
				return false;
			}

			CVirtualMachine::tInfoAsmOp *asm_op = new CVirtualMachine::tInfoAsmOp();
			//asm_op->type_op=OPERATOR;
			asm_op->operator_type = id_op;
			asm_op->result_type = result_type;
			asm_op->result_obj = res_obj;
			asm_op->index_left = left;
			asm_op->index_right = -1;

		//	printf("[%02i:%02i]\t%s\t[%02i:%02i]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),CVirtualMachine::def_operator[id_op].op_str,m_currentListStatements->size(),left);

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
int CCompiler::generateAsmCode(PASTNode op, int & numreg, bool & error, CScope * _lc){

	int r=0;
	/*string error_str;
	if(op==NULL){
		return -1;
	}

	if(op->children[LEFT_NODE]==NULL && op->children[RIGHT_NODE]==NULL){ // trivial case value itself...

		//printf("CONST \tE[%i],%s\n",numreg,op->value.c_str());
		if(!insertLoadValueInstruction(op->value, op->type_ptr, _lc, op->definedValueline)){
			error|=true;
			return -1;

		}

		r=numreg;
	}else{

		int right=0, left=0;

		left=generateAsmCode(op->children[LEFT_NODE],numreg,error,_lc);

		if(error) return -1;

		right=generateAsmCode(op->children[RIGHT_NODE],numreg,error,_lc);

		if(error) return -1;

		r=numreg;

		if(left !=-1 && right!=-1){ // 2 ops

			// particular case if operator is =
			if(op->token == "="){
				// the variable can only assigned if the type is the same or if the type is undefined.
				// check if left operand is registered variable...
				CScope::tInfoRegisteredVar * info_var = _lc->getInfoRegisteredSymbol(op->children[LEFT_NODE]->value,false);
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
								_lc->defineSymbol(op->children[LEFT_NODE]->value,var_obj);
								print_com_cr("%s defined as %s",op->children[LEFT_NODE]->value.c_str(),ptr_class_type.c_str());
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
						print_error_cr("\"%s\" was instanced as \"%s\" and cannot be change type as \"%s\"",op->children[LEFT_NODE]->value.c_str(),var_obj->getPointerClassStr().c_str(),op->type_ptr.c_str());
						error|=true;
						return -1;
					}
				}
			}
			else{
				//printf("%s\tE[%i],E[%i],E[%i]\n",op->token.c_str(),numreg,left,right);
				if(!insertOperatorInstruction(op->token, error_str,left,right)){
					print_error_cr("%s at line %i",error_str.c_str(),op->definedValueline);
					error|=true;
					return -1;
				}
			}

		}else if(right!=-1){ // one op..
			//printf("%s\tE[%i],E[%i]\n",op->token.c_str(),numreg,right);
			if(!insertOperatorInstruction(op->token, error_str,right)){
				print_error_cr("%s at line %i",error_str.c_str(),op->definedValueline);
				error|=true;
				return -1;
			}

		}else if(left!=-1){ // one op..
		//	printf("%s\tE[%i],E[%i]\n",op->token.c_str(),numreg,left);
			if(!insertOperatorInstruction(op->token, error_str,left)){
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
	numreg++;*/

	return r;
}

bool CCompiler::ast2asm_Recursive(PASTNode _node, CScriptFunction *sf){

	PASTNode currentNode;
	int j = 0;

	for(unsigned i = 0; i < _node->children.size(); i++){

		currentNode = _node->children[i];

		if(currentNode != NULL){
			if(!ast2asm_Recursive(_node->children[i],sf)){
				print_error_cr("Error 2!");
				return false;
			}else{ // perform its generation...
				switch(_node->children[i]->node_type){
				default:
					break;
				case UNKNOWN_NODE:print_info_cr("UNKNOWN_NODE");
				j++;
				break;
				break;
				case MAIN_NODE:print_info_cr("MAIN_NODE");
				break;
				case PUNCTUATOR_NODE:print_info_cr("PUNCTUATOR_NODE");break;
				case EXPRESSION_NODE:print_info_cr("EXPRESSION_NODE");

					break;
				case KEYWORD_NODE:print_info_cr("KEYWORD_NODE");break;
				case FUNCTION_ARGS_DECL_NODE:print_info_cr("FUNCTION_ARGS_DECL_NODE");break;
				case FUNCTION_OR_CLASS_ARGS_CALL_NODE:print_info_cr("FUNCTION_OR_CLASS_ARGS_CALL_NODE");break;
				case ARRAY_INDEX_NODE:print_info_cr("ARRAY_INDEX_NODE");break;
				case ARRAY_OBJECT_NODE:print_info_cr("ARRAY_OBJECT_NODE");break;
				case FUNCTION_OBJECT_NODE:print_info_cr("FUNCTION_OBJECT_NODE");break;
				case SYMBOL_NODE:print_info_cr("SYMBOL_NODE");break;
				case BODY_NODE:print_info_cr("BODY_NODE");break;
				case CONDITIONAL_NODE:print_info_cr("CONDITIONAL_NODE");break;
				case PRE_FOR_NODE:print_info_cr("PRE_FOR_NODE");break;
				case POST_FOR_NODE:print_info_cr("POST_FOR_NODE");break;
				case CLASS_VAR_COLLECTION_NODE:print_info_cr("CLASS_VAR_COLLECTION_NODE");break;
				case CLASS_FUNCTION_COLLECTION_NODE:print_info_cr("CLASS_FUNCTION_COLLECTION_NODE");break;
				case BASE_CLASS_NODE:print_info_cr("BASE_CLASS_NODE");break;
				case CALLING_OBJECT_NODE:print_info_cr("CALLING_OBJECT_NODE");break;
				}

			}
		}
	}

	return true;
}

bool CCompiler::ast2asm(PASTNode _node, CScriptFunction *sf){

	if(_node == NULL){
		print_error_cr("NULL node!");
		return false;
	}

	if(_node->node_type == NODE_TYPE::MAIN_NODE){

		for(unsigned i = 0; i < _node->children.size(); i++){
			if(!ast2asm_Recursive(_node->children[i],sf)){
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

bool CCompiler::compileExpression(const char *expression_str, int & m_line, CScriptFunction *sf, CScope *currentEvaluatingScope){

	/*int numreg=0;

	PASTNode ast_node;
	char *aux=(char *)expression_str;

	CVirtualMachine::tInfoStatementOp i_stat;

	this->m_currentScriptFunction = sf;
	this->m_currentListStatements = sf->getCompiledCode();

	//printf("%s:%i %s\n","file.zs",m_line,expression_str);
	//insertDebugInformation(expression_str);
	i_stat.m_line = m_line;
	i_stat.expression_str = expression_str;

	ast_node=generateBinaryAST(expression_str,m_line);

	if(ast_node==NULL){ // some error happend!
		return false;
	}

	// update n lines...
	while(*aux!=0){
		if(*aux=='\n') m_line++;
		aux++;
	}

	numreg=0;



	// new statment ...
	(*m_currentListStatements).push_back(i_stat);

	bool error_asm=false;
	generateAsmCode(ast_node,numreg,error_asm,currentEvaluatingScope);


	if(error_asm)
	{
		print_error_cr("Error generating code\n");
		return false;
	}*/


	return true;
}
/*
bool CCompiler::generateAsmCode_Recursive(root){

}


bool CCompiler::generateAsmCode(root){

}*/

bool CCompiler::compile(const string & s, CScriptFunction * pr){

	PASTNode root=NULL;
	// generate whole AST

	if(CAst::generateAST(s.c_str(),pr, &root)){

		ast2asm(root,pr);


		// then you have all information -> compile into asm!
		//generateAsmCode(root);

	}

	delete root;


	return false;
}


CCompiler::~CCompiler(){

}
