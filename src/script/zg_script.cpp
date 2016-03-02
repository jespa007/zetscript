
#include "ast.cpp"

#define MAX_STATMENT_LENGTH 8192
#define MAX_VAR_LENGTH 100


CZG_Script * CZG_Script::m_instance = NULL;


CZG_Script * CZG_Script::getInstance(){
	if(m_instance==NULL){
		m_instance = new CZG_Script();
	}

	return m_instance;
}

void CZG_Script::destroy(){

	if(m_instance!=NULL){
		delete m_instance;
	}

	CFactoryContainer::destroySingletons();
}

CZG_Script::CZG_Script(){

	m_defaultVar = new CUndefined();

	iniFactory<CNumberFactory>("CNumber");
	iniFactory<CIntegerFactory>("CInteger");
	iniFactory<CBooleanFactory>("CBoolean");
	iniFactory<CStringFactory>("CString");

}


void CZG_Script::init(){
	CFactoryContainer::getInstance()->registerScriptFunctions();
}

bool CZG_Script::registerVariable(const string & var_name){
	if(!existRegisteredVariable(var_name)){
		m_registeredVariable[var_name]=m_defaultVar;
		return true;
	}else{
		print_error_cr("error var \"%s\" already registered!", var_name.c_str());
	}

	return false;
}

bool CZG_Script::defineVariable(const string & var_name, CObject *obj){
	if(m_registeredVariable.count(var_name)==1){
		m_registeredVariable[var_name]=obj;
		obj->setName(var_name);
		return true;
	}else{
		print_error_cr("error var \"%s\" is not registered!", var_name.c_str());
	}
	return false;
}

bool CZG_Script::existRegisteredVariable(const string & var_name){
	return m_registeredVariable.count(var_name)==1;
}


CObject *CZG_Script::getRegisteredVariable(const string & var_name, bool print_msg){
	if(m_registeredVariable.count(var_name)==1){
		return m_registeredVariable[var_name];
	}else{
		if(print_msg){
			print_error_cr("var \"%s\" doesn't exist!", var_name.c_str());
		}
	}

	return NULL;

}

bool CZG_Script::isVarDeclarationStatment(const char *statment, bool & error, char ** eval_expression,int & m_line){
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
				CZG_Script::getInstance()->registerVariable(s_aux);

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

bool CZG_Script::eval(const string & s){

	char *current=(char *)s.c_str();
	string var_name;
	int m_line=0;
	bool error;
	char statment[MAX_STATMENT_LENGTH];
	char *next;
	PASTNode ast_node;
	int length;
	int numreg=0;
	char *eval_expression;

	current=CStringUtils::IGNORE_BLANKS(current,m_line);

	while(*current!=0){

		next=strstr(current,";");//getStatment(current);
		if(next ==0){
			print_error_cr("Expected ;");
			return false;
		}

		length=next-current;
		if(length>=MAX_STATMENT_LENGTH){
			print_error_cr("Max statment length!");
			return false;
		}

		memset(statment,0,sizeof(statment));
		strncpy(statment,current,(next-current));

		print_info_cr("eval:%s",statment);
		eval_expression = NULL;

		if(isVarDeclarationStatment(statment,error, &eval_expression, m_line)){

			if(eval_expression!=NULL){
				print_info_cr("eval expression %s",eval_expression);
			}

			if(error) {
				return false;
			}
		}else{
			eval_expression = statment;
		}

		if(eval_expression!=NULL){

			tInfoStatementOp i_stat;

			ast_node=generateAST(eval_expression, m_line);

			if(ast_node==NULL){ // some error happend!
				return false;
			}

			numreg=0;

			// new statment ...
			statement_op.push_back(i_stat);

			bool error_asm=false;
			generateAsmCode(ast_node,numreg,error_asm);

			if(error_asm)
			{
				print_error_cr("Error generating code\n");
				return false;
			}
		}

		// next statment...
		current=next+1; // ignore ;
		current=CStringUtils::IGNORE_BLANKS(current,m_line);

	}

	execute();

	return true;

}


string  CZG_Script::getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction){
	tInfoStatementOp *ptr_current_statement_op = &statement_op[statement_op.size()-1];
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

bool CZG_Script::insertLoadValueInstruction(const string & v, string & type_ptr){

	CNumber *num_obj;
	CInteger *int_obj;
	CString *str_obj;
	CBoolean *bool_obj;

	tInfoStatementOp *ptr_current_statement_op = &statement_op[statement_op.size()-1];
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

bool CZG_Script::insertMovVarInstruction(CObject *var, int right){

	//string op="=";
	string left_type_ptr = var->getPointerClassStr();
	CNumber *num_obj;
	CInteger * int_obj;
	CString *str_obj;
	CBoolean *bool_obj;

	tInfoStatementOp *ptr_current_statement_op = &statement_op[statement_op.size()-1];

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

ASM_OPERATOR CZG_Script::getIntegerOperatorId_TwoOps(const string & op, CZG_Script::TYPE & result_type){

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

ASM_OPERATOR CZG_Script::getNumberOperatorId_TwoOps(const string & op, CZG_Script::TYPE & result_type){

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

ASM_OPERATOR CZG_Script::getNumberOperatorId_OneOp(const string & op){

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

ASM_OPERATOR CZG_Script::getBoleanOperatorId_TwoOps(const string & op, CZG_Script::TYPE & result_type){

	result_type = TYPE::BOOL;

	if(op=="&&"){
		return LOGIC_AND;
	}else if(op=="||"){
		return LOGIC_OR;
	}

	return ASM_OPERATOR::UNKNOW;
}

ASM_OPERATOR CZG_Script::getBoleanOperatorId_OneOp(const string & op){
	if(op=="!"){
		return NOT;
	}
	return ASM_OPERATOR::UNKNOW;
}

ASM_OPERATOR CZG_Script::getStringOperatorId_TwoOps(const string & op, CZG_Script::TYPE & result_type){

	result_type = TYPE::STRING;

	if(op=="+"){
		return CAT;
	}else if(op=="=="){
		result_type = TYPE::BOOL;
		return EQU;
	}
	return ASM_OPERATOR::UNKNOW;
}

ASM_OPERATOR CZG_Script::getStringOperatorId_OneOp(const string & op){

	return ASM_OPERATOR::UNKNOW;
}


bool CZG_Script::insertOperatorInstruction(const string & op, int left, int right){

	tInfoStatementOp *ptr_current_statement_op = &statement_op[statement_op.size()-1];

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


void CZG_Script::execute(){

	int index_right=-1,index_left=-1;

	for(unsigned s = 0; s < statement_op.size(); s++){

		if(statement_op[s].asm_op.size()>0){

			print_info_cr("executing code...");

			tInfoAsmOp * instruction=NULL;
			for(unsigned i = 0; i  < statement_op[s].asm_op.size(); i++){
				print_info_cr("executing instruction %i...",i);
				index_right = statement_op[s].asm_op[i]->index_right;
				index_left = statement_op[s].asm_op[i]->index_left;
				tInfoAsmOp * instruction=statement_op[s].asm_op[i];
				tInfoAsmOp * right_instruction, *left_instruction;//=statement_op[s].asm_op[i];
				instruction=statement_op[s].asm_op[i];
				switch(instruction->operator_type){
					default:
						print_error_cr("operator type(%i) not implemented",instruction->operator_type);
						break;
					case LOAD:
						print_info_cr("load value...");
					break;
					case MOV:{ // only can mov vars...

						print_info_cr("mov var");
						right_instruction = statement_op[s].asm_op[index_right];

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
						//left_instruction = statement_op[s].asm_op[index_left];
						print_info_cr("pre inc");
						left_instruction = statement_op[s].asm_op[index_left];
						// increment variable...
						ASSIGN_NUMBER(left_instruction,CAST_RESULT_AS_NUMBER(left_instruction)+1);
						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));

						break;
					case POST_INC: // for numbers...
						//left_instruction = statement_op[s].asm_op[index_left];
						print_info_cr("post inc");
						left_instruction = statement_op[s].asm_op[index_left];

						// set it to expression result...
						ASSIGN_NUMBER(instruction,CAST_RESULT_AS_NUMBER(left_instruction));
						// then increment variable...
						ASSIGN_NUMBER(left_instruction,CAST_RESULT_AS_NUMBER(left_instruction)+1);

						break;

					case ADD: // for numbers...
						print_info_cr("add");
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						right_instruction = statement_op[s].asm_op[index_right];
						left_instruction = statement_op[s].asm_op[index_left];

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
						left_instruction = statement_op[s].asm_op[index_left];
						(*((bool *)instruction->result_obj))=!(*((bool *)left_instruction->result_obj));

						break;
					case NEG:
						print_info_cr("===============================================");
						print_info_cr("negative");
						left_instruction = statement_op[s].asm_op[index_left];
						ASSIGN_NUMBER(instruction,-CAST_RESULT_AS_NUMBER(left_instruction));

						break;

				}
			}

			// try to get the result...
			instruction=statement_op[s].asm_op[statement_op[s].asm_op.size()-1];
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

}
//-------------------------------------------------------------------------------------
CZG_Script::~CZG_Script(){
	// unregister operators ...
	for(unsigned s = 0; s  <statement_op.size(); s++){
		for(unsigned i = 0; i  <statement_op[s].asm_op.size(); i++){

			delete statement_op[s].asm_op[i];
		}
	}

	//unregisterOperators();

	delete m_defaultVar;
}
