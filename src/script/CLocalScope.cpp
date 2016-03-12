#include "zg_script.h"

#define MAX_STATMENT_LENGTH 8192
#define MAX_VAR_LENGTH 100


CUndefined *CLocalScope::m_defaultVar=NULL;

void CLocalScope::createSingletons(){
	m_defaultVar = new CUndefined();
}

void CLocalScope::destroySingletons(){
	delete m_defaultVar;
}


CLocalScope::CLocalScope(CLocalScope * _parent){
	m_parentScope = _parent;
}

void CLocalScope::addLocalScope(CLocalScope *_ls){
	m_scopeList.push_back(_ls);
}



bool CLocalScope::registerVariable(const string & var_name, int m_line){
	tInfoRegisteredVar * irv;
	if((irv = existRegisteredVariable(var_name))==NULL){ // check whether is local var registered scope ...
		irv = new tInfoRegisteredVar;
		irv->m_line = m_line;
		irv->m_obj = m_defaultVar;
		m_registeredVariable[var_name]=irv;
		return true;
	}else{
		print_error_cr("error var \"%s\" already registered at line %i!", var_name.c_str(), irv->m_line);
	}

	return false;
}

bool CLocalScope::defineVariable(const string & var_name, CObject *obj){

	tInfoRegisteredVar * irv;
	if((irv = existRegisteredVariable(var_name))!=NULL){ // check whether is local var registered scope ...
		irv->m_obj=obj;
		irv->m_obj->setName(var_name);
		return true;
	}else{
		print_error_cr("error var \"%s\" is not registered!", var_name.c_str());
	}
	return false;
}

CLocalScope * CLocalScope::getParent(){
	return m_parentScope;
}


CLocalScope::tInfoRegisteredVar * CLocalScope::existRegisteredVariableRecursive(const string & var_name, CLocalScope *lc){
	if(lc->m_registeredVariable.count(var_name)==0){ // not exit but we will deepth through parents ...
		CLocalScope * parent = lc->getParent();
		if(parent != NULL){
			return existRegisteredVariableRecursive(var_name, parent);
		}
		return NULL;
	}else{
		return lc->m_registeredVariable[var_name];
		//print_error_cr("variable %s already registered at line %i",var_name.c_str(),lc->m_registeredVariable[var_name]->m_line);
	}

	return NULL;
}

CLocalScope::tInfoRegisteredVar * CLocalScope::existRegisteredVariable(const string & var_name){
	return existRegisteredVariableRecursive(var_name, this);

}


CObject *CLocalScope::getRegisteredVariable(const string & var_name, bool print_msg){
	tInfoRegisteredVar * irv;
	if((irv = existRegisteredVariable(var_name))!=NULL){ // check whether is local var registered scope ...

		return irv->m_obj;
	}else{
		if(print_msg){
			print_error_cr("var \"%s\" doesn't exist!", var_name.c_str());
		}
	}

	return NULL;

}
//-----------------------------------------------------------------------------------------------------------

bool CLocalScope::isVarDeclarationStatment(const char *statment, bool & error, char ** eval_expression,int & m_line, CLocalScope * _localScope){
	// PRE: length(statment) < MAX_STATMENT_LENGTH
	char *aux = (char *)statment;

	//string var_name;
	//PASTNode expression;
	char *start_var,*end_var;
	*eval_expression=NULL;
	error=false;
	char stat[MAX_STATMENT_LENGTH]={0};
	string s_aux;
	int var_length;

	aux=CStringUtils::IGNORE_BLANKS(aux,m_line);

	if(strncmp(aux,"var",3)==0){ // possible variable...
		aux+=3;

		if(*aux!=' '){ // is not var word...
			return NULL;
		}

		*eval_expression=aux;

		aux=CStringUtils::IGNORE_BLANKS(aux,m_line);

		if(*aux!=0 && (
		   ('a' <= *aux && *aux <='z') ||
		   ('A' <= *aux && *aux <='Z'))
		){ // let's see it has right chars...
			start_var=aux;
			aux++;
			while((*aux!=0 || *aux!= '=') && (('a' <= *aux && *aux <='z') ||
				  ('0' <= *aux && *aux <='9') ||
				  (*aux=='_') ||
				  ('A' <= *aux && *aux <='Z'))){
				aux++;
			}
			end_var=aux;
			aux=CStringUtils::IGNORE_BLANKS(aux,m_line);

			if((*aux == 0 || *aux == '=')){

				var_length=end_var-start_var;

				strncpy(stat,start_var,var_length);
				print_info_cr("registered \"%s\" variable: ",stat);

				s_aux=stat;
				_localScope->registerVariable(s_aux,m_line);

				return true;
			}
			else{
				print_error_cr("variable cannot cannot contain char '%c'",*aux);
				error=true;
			}

		}else{
			print_error_cr("variable cannot start with char %c",*aux);
			error=true;
		}
	}
	return false;
}


//-----------------------------------------------------------------------------------------------------------





char * CLocalScope::evalRecursive(const char *str_to_eval, int & level_scope, int & m_line, CLocalScope * _scope){

	char *current=(char *) str_to_eval;
	string var_name;
	//int m_line=0;
	bool error;
	char statment[MAX_STATMENT_LENGTH];
	char *next;
	PASTNode ast_node;
	int length;
	int numreg=0;
	char *advanced_expression;
	CLocalScope * new_local_scope = NULL;

	current=CStringUtils::IGNORE_BLANKS(current,m_line);

	while(*current!=0){

		advanced_expression = NULL;

		if(*current == '{'){ // begin scope --> inc scope level ...
			// add into localscope


			_scope->m_scopeList.push_back(new_local_scope=new CLocalScope(_scope));
			_scope->insertPushScopeInstruction(new_local_scope);

			current = evalRecursive(current+1, ++level_scope, m_line, new_local_scope);

			int j=0;

		}else if(*current == '}'){ // end scope...
			if(level_scope > 0){ // download scope level
				_scope->insertPopScopeInstruction();//_scope->getIndexLastInsertedLocalScope());
				--level_scope;
				return (current+1);
			}else{
				print_error_cr("Unexpected } at line %i",m_line);
				return NULL;
			}
		}
		else{ // eval expressions...

			next=strstr(current,";");//getStatment(current);
			if(next ==0){
				print_error_cr("Expected ;");
				return NULL;
			}

			length=next-current;
			if(length>=MAX_STATMENT_LENGTH){
				print_error_cr("Max statment length!");
				return NULL;
			}

			memset(statment,0,sizeof(statment));
			strncpy(statment,current,(next-current));

			print_info_cr("eval:%s",statment);


			// advance current pointer...
			current=next+1; // ignore ;

			if(isVarDeclarationStatment(statment,error, &advanced_expression, m_line,_scope)){

				if(advanced_expression!=NULL){
					print_info_cr("eval expression %s",advanced_expression);
				}

				if(error) {
					return NULL;
				}
			}else{
				advanced_expression = statment;
			}
		}

		if(advanced_expression!=NULL){

			tInfoStatementOp i_stat;

			ast_node=generateAST(advanced_expression, m_line);

			if(ast_node==NULL){ // some error happend!
				return NULL;
			}

			numreg=0;

			// new statment ...
			_scope->m_listStatements.push_back(i_stat);

			bool error_asm=false;
			generateAsmCode(ast_node,_scope,numreg,error_asm);

			if(error_asm)
			{
				print_error_cr("Error generating code\n");
				return NULL;
			}
		}


		// next statment...
		current=CStringUtils::IGNORE_BLANKS(current,m_line);

	}

	// the generating code has been terminated... let's execute it...
	//
	return current;

}

int getLineBeginScope(const string  & s, int m_scope){
	// PRE s: expression to be evaluated.

	int m_line = 1;
	char *current = (char *)s.c_str();
	int current_scope=0;

	while (*current != 0){

		if(*current == '\n') m_line ++;

		if(*current == '{'){

			current_scope++;
			if(current_scope==m_scope){
				return m_line;
			}
		}
		else if(*current == '}'){
			current_scope--;
		}

		current++;
	}

	return 0;

}

bool CLocalScope::eval (const string & s){
	int m_scope=0;
	int m_line=1;
	char * current = evalRecursive((const char *)s.c_str(),m_scope, m_line,this);

	if(current != NULL){
		if(*current == 0){ // ok all processed...

			if(m_scope>0){
				print_error_cr("Cannot find close } at line %i", getLineBeginScope(s,m_scope));
			}else{
				return execute(this);
			}
		}else{
			print_error_cr("Unexpected ");
		}
	}

	return false;
}



string  CLocalScope::getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction){
	tInfoStatementOp *ptr_current_statement_op = &m_listStatements[m_listStatements.size()-1];
	string result="unknow";

	if(instruction >=0 && instruction < ptr_current_statement_op->asm_op.size()){
		switch(ptr_current_statement_op->asm_op[instruction]->result_type){
		default:
		case TYPE::UNKNOW:
			break;
		case TYPE::VAR:
			result= (((CObject *)ptr_current_statement_op->asm_op[instruction]->result_obj)->getPointerClassStr()); // type result..
			break;
		case TYPE::NUMBER:
			result= CNumberFactory::getPointerTypeStr();
			break;
		case TYPE::INTEGER:
			result= CIntegerFactory::getPointerTypeStr();
			break;
		case TYPE::STRING:
			result= CStringFactory::getPointerTypeStr();
			break;
		case TYPE::BOOL:
			result= CBooleanFactory::getPointerTypeStr();
			break;
		}
	}
	else{
		print_error_cr("index out of bounds");
	}
	return result;
}



bool CLocalScope::insertLoadValueInstruction(const string & v, string & type_ptr){

	CNumber *num_obj;
	CInteger *int_obj;
	CString *str_obj;
	CBoolean *bool_obj;

	tInfoStatementOp *ptr_current_statement_op = &m_listStatements[m_listStatements.size()-1];
	void *obj;
	TYPE type=TYPE::UNKNOW;
	// try parse value...
	if((obj=CInteger::ParsePrimitive(v))!=NULL){
			type=TYPE::INTEGER;
			print_info_cr("%s detected as int\n",v.c_str());
	}
	else if((obj=CNumber::ParsePrimitive(v))!=NULL){
		type=TYPE::NUMBER;
		print_info_cr("%s detected as float\n",v.c_str());
	}
	else if(v[0]=='\"' && v[v.size()-1]=='\"'){
		type=TYPE::STRING;
		string s=v.substr(1,v.size()-2);
		string *so=new string();
		(*so)=s;
		obj = so;
		print_info_cr("%s detected as string\n",v.c_str());
	}
	else if((obj=CBoolean::ParsePrimitive(v))!=NULL){
		type=TYPE::BOOL;
		print_info_cr("%s detected as boolean\n",v.c_str());
	}else{
		CObject * var=getRegisteredVariable(v);
		type=TYPE::VAR;

		if(var==NULL){
			print_error_cr("ERROR: %s is not declared",v.c_str());
			return false;
		}

		obj = var;
		if((int_obj=dynamic_cast<CInteger *>(var))!=NULL){ // else it will store the value ...
			type = TYPE::INTEGER;
			obj=&int_obj->m_value;
		}else if((num_obj=dynamic_cast<CNumber *>(var))!=NULL){
			type = TYPE::NUMBER;
			obj=&num_obj->m_value;
		}else if((bool_obj=dynamic_cast<CBoolean *>(var))!=NULL){
			type = TYPE::BOOL;
			obj=&bool_obj->m_value;
		}else if((str_obj=dynamic_cast<CString *>(var))!=NULL){
			type = TYPE::STRING;
			obj=&str_obj->m_value;
		}
	}

	if(obj != NULL){

		tInfoAsmOp *asm_op = new tInfoAsmOp();

		asm_op->result_obj=obj;
		asm_op->result_type = type;

		asm_op->operator_type=ASM_OPERATOR::LOAD;


		ptr_current_statement_op->asm_op.push_back(asm_op);
	}
	return true;
}

bool CLocalScope::insertMovVarInstruction(CObject *var, int right){

	//string op="=";
	string left_type_ptr = var->getPointerClassStr();
	CNumber *num_obj;
	CInteger * int_obj;
	CString *str_obj;
	CBoolean *bool_obj;

	tInfoStatementOp *ptr_current_statement_op = &m_listStatements[m_listStatements.size()-1];

	if(right <0 || (unsigned)right >= ptr_current_statement_op->asm_op.size()){
		print_error_cr("ERROR: right operant is out of internal stack (%i,%i) ",right,ptr_current_statement_op->asm_op.size());
		return false;
	}

	if(right >= 0){ // insert both op...

		tInfoAsmOp *asm_op = new tInfoAsmOp();
		asm_op->result_type = TYPE::VAR; // this only stores pointer...
		asm_op->result_obj=var;

		if((int_obj=dynamic_cast<CInteger *>(var))!=NULL){ // else it will store the value ...
				asm_op->result_type = TYPE::INTEGER;
				asm_op->result_obj=&int_obj->m_value;
		}else if((num_obj=dynamic_cast<CNumber *>(var))!=NULL){ // else it will store the value ...
			asm_op->result_type = TYPE::NUMBER;
			asm_op->result_obj=&num_obj->m_value;
		}else if((bool_obj=dynamic_cast<CBoolean *>(var))!=NULL){
			asm_op->result_type = TYPE::BOOL;
			asm_op->result_obj=&bool_obj->m_value;
		}else if((str_obj=dynamic_cast<CString *>(var))!=NULL){
			asm_op->result_type = TYPE::STRING;
			asm_op->result_obj=&str_obj->m_value;
		}

		asm_op->index_left = -1;
		asm_op->index_right = right;
		asm_op->operator_type = ASM_OPERATOR::MOV;
		ptr_current_statement_op->asm_op.push_back(asm_op);


	}else{
		print_error_cr("ERROR: right operand is out of internal stack ");
		return false;
	}

	return true;
}

void CLocalScope::insertPushScopeInstruction(CLocalScope * _goto_scope){
	tInfoStatementOp op_push_scope;
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->result_obj = _goto_scope;
	asm_op->operator_type=ASM_OPERATOR::PUSH_SCOPE;
	op_push_scope.asm_op.push_back(asm_op);

	m_listStatements.push_back(op_push_scope);
}


void CLocalScope::insertPopScopeInstruction(){

	tInfoStatementOp op_pop_scope;
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->result_obj = this->m_parentScope;
	asm_op->operator_type=ASM_OPERATOR::POP_SCOPE;
	op_pop_scope.asm_op.push_back(asm_op);

	m_listStatements.push_back(op_pop_scope);

}


ASM_OPERATOR CLocalScope::getIntegerOperatorId_TwoOps(const string & op, CLocalScope::TYPE & result_type){

	result_type = TYPE::INTEGER;

	if(op=="+"){
		return ADD;
	}else if(op=="/"){
		return DIV;
	}else if(op=="%"){
		return MOD;
	}else if(op=="*"){
		return MUL;
	}else if(op=="&"){
		return AND;
	}else if(op=="|"){
		return OR;
	}else if(op=="^"){
		return XOR;
	}else if(op=="<<"){
		return SHL;
	}else if(op==">>"){
		return SHR;
	}else if(op==">="){
		result_type = TYPE::BOOL;
		return GTE;
	}else if(op==">"){
		result_type = TYPE::BOOL;
		return GT;
	}else if(op=="<"){
		result_type = TYPE::BOOL;
		return LT;
	}else if(op=="<="){
		result_type = TYPE::BOOL;
		return LTE;
	}else if(op=="=="){
		result_type = TYPE::BOOL;
		return EQU;
	}
	return ASM_OPERATOR::UNKNOW;
}

ASM_OPERATOR CLocalScope::getNumberOperatorId_TwoOps(const string & op, CLocalScope::TYPE & result_type){

	result_type = TYPE::NUMBER;

	if(op=="+"){
		return ADD;
	}else if(op=="/"){
		return DIV;
	}else if(op=="%"){
		return MOD;
	}else if(op=="*"){
		return MUL;
	}else if(op==">="){
		result_type = TYPE::BOOL;
		return GTE;
	}else if(op==">"){
		result_type = TYPE::BOOL;
		return GT;
	}else if(op=="<"){
		result_type = TYPE::BOOL;
		return LT;
	}else if(op=="<="){
		result_type = TYPE::BOOL;
		return LTE;
	}else if(op=="=="){
		result_type = TYPE::BOOL;
		return EQU;
	}
	return ASM_OPERATOR::UNKNOW;
}

ASM_OPERATOR CLocalScope::getNumberOperatorId_OneOp(const string & op){

	if(op=="-"){
		return NEG;
	}else if(op=="++"){
		return PRE_INC;
	}else if(op=="post_++"){
		return POST_INC;
	}else if(op=="--"){
		return PRE_DEC;
	}else if(op=="post_--"){
		return POST_DEC;
	}

	return ASM_OPERATOR::UNKNOW;
}

ASM_OPERATOR CLocalScope::getBoleanOperatorId_TwoOps(const string & op, CLocalScope::TYPE & result_type){

	result_type = TYPE::BOOL;

	if(op=="&&"){
		return LOGIC_AND;
	}else if(op=="||"){
		return LOGIC_OR;
	}

	return ASM_OPERATOR::UNKNOW;
}

ASM_OPERATOR CLocalScope::getBoleanOperatorId_OneOp(const string & op){
	if(op=="!"){
		return NOT;
	}
	return ASM_OPERATOR::UNKNOW;
}

ASM_OPERATOR CLocalScope::getStringOperatorId_TwoOps(const string & op, CLocalScope::TYPE & result_type){

	result_type = TYPE::STRING;

	if(op=="+"){
		return CAT;
	}else if(op=="=="){
		result_type = TYPE::BOOL;
		return EQU;
	}
	return ASM_OPERATOR::UNKNOW;
}

ASM_OPERATOR CLocalScope::getStringOperatorId_OneOp(const string & op){

	return ASM_OPERATOR::UNKNOW;
}


bool CLocalScope::insertOperatorInstruction(const string & op, int left, int right){

	tInfoStatementOp *ptr_current_statement_op = &m_listStatements[m_listStatements.size()-1];

	if(left <0 || (unsigned)left >= ptr_current_statement_op->asm_op.size()){
		print_error_cr("ERROR: left operant is out of internal stack (%i,%i) ",left,ptr_current_statement_op->asm_op.size());
		return false;
	}

	string t_left = getUserTypeResultCurrentStatmentAtInstruction(left);
	string t_right = "unknow";
	if(right>=0){
		t_right = getUserTypeResultCurrentStatmentAtInstruction(right);
	}

	ASM_OPERATOR id_op;
	TYPE result_type=UNKNOW;
	void *res_obj=NULL;


	if(left >=0 && right >= 0){ // insert both op...

		if(((t_left == CIntegerFactory::getPointerTypeStr())) &&
						((t_right == CIntegerFactory::getPointerTypeStr()))){
					if((id_op=getIntegerOperatorId_TwoOps(op,result_type))==ASM_OPERATOR::UNKNOW){
						print_error_cr("undefined operator %s",op.c_str());
						return false;
					}

		}else if(((t_left == CNumberFactory::getPointerTypeStr()) ) &&
				((t_right == CNumberFactory::getPointerTypeStr()) )){
			if((id_op=getNumberOperatorId_TwoOps(op,result_type))==ASM_OPERATOR::UNKNOW){
				print_error_cr("undefined operator %s",op.c_str());
				return false;
			}

		}else if(t_left== CStringFactory::getPointerTypeStr()){

			if((id_op=getStringOperatorId_TwoOps(op,result_type))==ASM_OPERATOR::UNKNOW){
				print_error_cr("undefined operator %s",op.c_str());
				return false;
			}

		}else if((t_left == CBooleanFactory::getPointerTypeStr()) && t_right == CBooleanFactory::getPointerTypeStr()){
			if((id_op=getBoleanOperatorId_TwoOps(op, result_type))==ASM_OPERATOR::UNKNOW){
				print_error_cr("undefined operator %s",op.c_str());
				return false;
			}
		}else{
			print_error_cr("not compatible %s %s",t_left.c_str(),t_right.c_str());
			return false;
		}

		switch(result_type){
		default:
		case TYPE::UNKNOW:
			print_error_cr("unknow result type");
			return false;
			break;
		case TYPE::NUMBER:
			res_obj = new float;
			break;
		case TYPE::INTEGER:
			res_obj = new int;
			break;
		case TYPE::STRING:
			res_obj = new string;
			break;
		case TYPE::BOOL:
			res_obj = new bool;
			break;

		}

		tInfoAsmOp *asm_op = new tInfoAsmOp();
		//asm_op->type_op=OPERATOR;
		asm_op->index_left = left;
		asm_op->index_right = right;


		//-----------------------
		asm_op->result_type = result_type;
		asm_op->operator_type = id_op;
		asm_op->result_obj = res_obj;

		ptr_current_statement_op->asm_op.push_back(asm_op);


	}else if(left >=0){ // insert one operand

			if((t_left == CIntegerFactory::getPointerTypeStr()) ){
						if((id_op=getNumberOperatorId_OneOp(op))==ASM_OPERATOR::UNKNOW){
							print_error_cr("undefined operator %s for number type",op.c_str());
							return false;
						}
						res_obj = new int;
						result_type = INTEGER;

			}else if((t_left == CNumberFactory::getPointerTypeStr()) ){
				if((id_op=getNumberOperatorId_OneOp(op))==ASM_OPERATOR::UNKNOW){
					print_error_cr("undefined operator %s for number type",op.c_str());
					return false;
				}
				res_obj = new float;
				result_type = NUMBER;

			}else if(t_left== CStringFactory::getPointerTypeStr()){

				if((id_op=getStringOperatorId_OneOp(op))==ASM_OPERATOR::UNKNOW){
					print_error_cr("undefined operator %s for string type",op.c_str());
					return false;
				}
				res_obj = new string;
				result_type = STRING;
			}else if((t_left == CBooleanFactory::getPointerTypeStr())){
				if((id_op=getBoleanOperatorId_OneOp(op))==ASM_OPERATOR::UNKNOW){
					print_error_cr("undefined operator %s for boolean type",op.c_str());
					return false;
				}
				res_obj = new bool;
				result_type = BOOL;
			}else{
				print_error_cr("not compatible %s %s",t_left.c_str(),t_right.c_str());
				return false;
			}

			tInfoAsmOp *asm_op = new tInfoAsmOp();
			//asm_op->type_op=OPERATOR;
			asm_op->operator_type = id_op;
			asm_op->result_type = result_type;
			asm_op->result_obj = res_obj;
			asm_op->index_left = left;
			asm_op->index_right = -1;
			ptr_current_statement_op->asm_op.push_back(asm_op);

	}else{
		print_error_cr("ERROR: both operant is out of internal stack ");
		return false;
	}
	return true;
}

float default_value=0;


#define CAST_RESULT_AS_NUMBER(asm_op)\
 (asm_op->result_type == TYPE::NUMBER)?(*((float *)asm_op->result_obj)):\
 (asm_op->result_type == TYPE::INTEGER)?(*((int *)asm_op->result_obj)):\
 default_value


#define PERFORM_NUMBER_TWO_OPS(v1,op,v2)\
(v1->result_type == TYPE::NUMBER && v2->result_type == TYPE::NUMBER)?(*((float *)v1->result_obj)op(*((float *)v2->result_obj))):\
(v1->result_type == TYPE::NUMBER && v2->result_type == TYPE::INTEGER)?(*((float *)v1->result_obj)op(*((int *)v2->result_obj))):\
(v1->result_type == TYPE::INTEGER && v2->result_type == TYPE::NUMBER)?(*((int *)v1->result_obj)op(*((float *)v2->result_obj))):\
(v1->result_type == TYPE::INTEGER && v2->result_type == TYPE::INTEGER)?(*((int *)v1->result_obj)op(*((int *)v2->result_obj))):\
0


#define ASSIGN_NUMBER(asm_op,v)\
(asm_op->result_type == TYPE::NUMBER)?(*((float *)asm_op->result_obj)=(v)):\
(asm_op->result_type == TYPE::INTEGER)?(*((int *)asm_op->result_obj)=(v)):\
 default_value = (v)

bool CLocalScope::execute(CLocalScope *lc){

	int index_right=-1,index_left=-1;


	for(unsigned s = 0; s < lc->m_listStatements.size(); s++){

		if(lc->m_listStatements[s].asm_op.size()>0){

			print_info_cr("executing code...");

			tInfoAsmOp * instruction=NULL;
			tInfoStatementOp * current_statment = &lc->m_listStatements[s];
			for(unsigned i = 0; i  < lc->m_listStatements[s].asm_op.size(); i++){
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
						case TYPE::UNKNOW:
							print_error_cr("result type must be var (result type:%i)");
							break;
						case TYPE::INTEGER:
						case TYPE::NUMBER:
							if(right_instruction->result_type == TYPE::INTEGER || right_instruction->result_type == TYPE::NUMBER){

								cout << "assign:" << (CAST_RESULT_AS_NUMBER(right_instruction))<< "" << endl;
								ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(right_instruction));
							}else{
								print_error_cr("right operant is not number");
							}
							break;

						case TYPE::STRING:
							if(right_instruction->result_type == TYPE::STRING){
								*((string *)instruction->result_obj) = *((string *)right_instruction->result_obj);
							}else{
								print_error_cr("right operant is not string");
							}
							break;
						case TYPE::BOOL:
							if(right_instruction->result_type == TYPE::BOOL){
								*((bool *)instruction->result_obj) = *((bool *)right_instruction->result_obj);
							}else{
								print_error_cr("right operant is not bool");
							}
							break;
						case TYPE::VAR:
							if(right_instruction->result_type == TYPE::VAR){ // assign pointer ?

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

						if((left_instruction->result_type == TYPE::NUMBER || left_instruction->result_type == TYPE::INTEGER)&&
						    (right_instruction->result_type == TYPE::NUMBER || right_instruction->result_type == TYPE::INTEGER)){
							//print_info_cr("%f + %f",*((float *)left_instruction->result_obj),*((float *)right_instruction->result_obj));
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " + " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
							ASSIGN_NUMBER(instruction, PERFORM_NUMBER_TWO_OPS(left_instruction, +,right_instruction));

							{
								int jjj;
								jjj=PERFORM_NUMBER_TWO_OPS(left_instruction, +,right_instruction);
								cout << "assign2:" << jjj << "" << endl;
							}
							cout << "assign:" << (CAST_RESULT_AS_NUMBER(instruction))<< "" << endl;

						}else if(left_instruction->result_type == TYPE::STRING && right_instruction->result_type == TYPE::STRING){
							*((string *)instruction->result_obj) = *((string *)left_instruction->result_obj) + *((string *)right_instruction->result_obj);
						}else if(left_instruction->result_type == TYPE::STRING &&  right_instruction->result_type == TYPE::NUMBER){
							*((string *)instruction->result_obj) = *((string *)left_instruction->result_obj) + CStringUtils::doubleToString(CAST_RESULT_AS_NUMBER(right_instruction));
						}else{
							print_error_cr("cannot operate + with (type(%i) + type(%i))",left_instruction->result_type, right_instruction->result_type);
						}
						break;
					case DIV: // for numbers...
						print_info_cr("div");
						right_instruction = current_statment->asm_op[index_right];
						left_instruction = current_statment->asm_op[index_left];

						if((left_instruction->result_type == TYPE::NUMBER || left_instruction->result_type == TYPE::INTEGER)&&
							    (right_instruction->result_type == TYPE::NUMBER || right_instruction->result_type == TYPE::INTEGER)){
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

						if((left_instruction->result_type == TYPE::NUMBER || left_instruction->result_type == TYPE::INTEGER)&&
							    (right_instruction->result_type == TYPE::NUMBER || right_instruction->result_type == TYPE::INTEGER)){
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

						if(left_instruction->result_type == TYPE::BOOL && right_instruction->result_type == TYPE::BOOL){
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

						if(left_instruction->result_type == TYPE::BOOL && right_instruction->result_type == TYPE::BOOL){
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

						if((left_instruction->result_type == TYPE::NUMBER || left_instruction->result_type == TYPE::INTEGER)&&
							    (right_instruction->result_type == TYPE::NUMBER || right_instruction->result_type == TYPE::INTEGER)){
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

						if((left_instruction->result_type == TYPE::NUMBER || left_instruction->result_type == TYPE::INTEGER)&&
							    (right_instruction->result_type == TYPE::NUMBER || right_instruction->result_type == TYPE::INTEGER)){
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

						if((left_instruction->result_type == TYPE::NUMBER || left_instruction->result_type == TYPE::INTEGER)&&
							    (right_instruction->result_type == TYPE::NUMBER || right_instruction->result_type == TYPE::INTEGER)){
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

						if((left_instruction->result_type == TYPE::NUMBER || left_instruction->result_type == TYPE::INTEGER)&&
							    (right_instruction->result_type == TYPE::NUMBER || right_instruction->result_type == TYPE::INTEGER)){
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

						if((left_instruction->result_type == TYPE::NUMBER || left_instruction->result_type == TYPE::INTEGER)&&
							    (right_instruction->result_type == TYPE::NUMBER || right_instruction->result_type == TYPE::INTEGER)){
							cout << (CAST_RESULT_AS_NUMBER(left_instruction)) << " == " << (CAST_RESULT_AS_NUMBER(right_instruction)) << endl;
							*((bool *)instruction->result_obj) = PERFORM_NUMBER_TWO_OPS(left_instruction, ==,right_instruction);
						}
						else if(left_instruction->result_type == TYPE::STRING && right_instruction->result_type == TYPE::STRING){
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

						if(left_instruction->result_type == TYPE::INTEGER && right_instruction->result_type == TYPE::INTEGER){
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

						if(left_instruction->result_type == TYPE::INTEGER && right_instruction->result_type == TYPE::INTEGER){
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

						if(left_instruction->result_type == TYPE::INTEGER && right_instruction->result_type == TYPE::INTEGER){
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

						if(left_instruction->result_type == TYPE::INTEGER && right_instruction->result_type == TYPE::INTEGER){
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

						if(left_instruction->result_type == TYPE::INTEGER && right_instruction->result_type == TYPE::INTEGER){
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

						if(left_instruction->result_type == TYPE::INTEGER && right_instruction->result_type == TYPE::INTEGER){
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
						print_info_cr("pop scope");
						return true;
						break;
					case PUSH_SCOPE: // because the variables are already defined, the instruction push/pop are useless here.
						print_info_cr("push scope");
						if(!execute((CLocalScope *)(instruction->result_obj))){
							return false;
						}
						break;

				}
			}

			// try to get the result...
			instruction=current_statment->asm_op[current_statment->asm_op.size()-1];
			if(instruction!=NULL){
				switch(instruction->result_type){
				case TYPE::UNKNOW:
					print_info_cr("unknow type result");
					break;
				case TYPE::INTEGER:
					print_info_cr("Integer with value=%i",*((int*)instruction->result_obj));
					break;
				case TYPE::NUMBER:
					print_info_cr("Number with value=%f",*((float*)instruction->result_obj));
					break;
				case TYPE::BOOL:
					print_info_cr("Boolean with value=%i",*((bool*)instruction->result_obj));
					break;
				case TYPE::STRING:
					print_info_cr("String with value=%s",((string*)instruction->result_obj)->c_str());
					break;
				case TYPE::VAR: // print type var?
					print_info_cr("print type var not implemented!");
					break;

				}
			}
		}
	}

	return true;

}

CLocalScope::~CLocalScope(){
	for(unsigned s = 0; s  <m_listStatements.size(); s++){
		for(unsigned i = 0; i  <m_listStatements[s].asm_op.size(); i++){

			delete m_listStatements[s].asm_op[i];
		}
	}
}
