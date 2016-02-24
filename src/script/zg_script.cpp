
#include "ast.cpp"



bool CZG_Script::existOperatorSignature(const string & op,const string & result, vector<string> * param){


	if(m_mapContainerOperators.count(op)==1){
		vector< tInfoObjectOperator > * v= m_mapContainerOperators[op];

		for(unsigned i = 0;i < v->size(); i++){ // for all signatures operator�

			if(result == v->at(i).result_type && v->at(i).param_type->size() == param->size()){ // possibli matches...

				bool equal = true;

				for(unsigned j=0; j < v->at(i).param_type->size() && equal; j++){
						if(v->at(i).param_type->at(j)==param->at(j)){
							equal = false;
						}
				}

				if(equal)
					return true;
			}
		}

	}

	return false;
}



bool CZG_Script::registerOperatorInternal(const string & _op_name, const string &  result_type,vector<string> * param_type, void(*fun_ptr)()){

	if(!(param_type->size()==1 || param_type->size() ==2)){
		print_error_cr("fatal error. We expected 1 or 2 ops for operator");
		return false;
	}

	if(m_mapContainerOperators.count(_op_name) == 0){ // not exist
		m_mapContainerOperators[_op_name] = new vector<tInfoObjectOperator>();
	}

	if(existOperatorSignature(_op_name,result_type, param_type)){
		print_error_cr("operator signature already exist");
		return false;
	}
	tInfoObjectOperator info;
	info.result_type= result_type;
	info.param_type = param_type;
	info.fun_ptr = fun_ptr;
	// IMPPORTANT! for each operator we can have different signatures�
	m_mapContainerOperators[_op_name]->push_back(info);

	print_info_cr("registered operator \"%s\"",_op_name.c_str());
	return true;

}



CZG_Script::tInfoObjectOperator * CZG_Script::getOperatorInfo(const string & op, string * type_op1,  string * type_op2){
	string *ps1=type_op1;
	string *ps2=type_op2;
	int num_operands = 1;
	if(ps2!=NULL){
		num_operands = 2;
	}

	if(m_mapContainerOperators.count(op)==1){
		vector< tInfoObjectOperator > * v= m_mapContainerOperators[op];
		//print_info_cr("operator \"%s\" found");

		for(unsigned i = 0;i < v->size(); i++){ // for all signatures operator�

			if(v->at(i).param_type->size()==2 && num_operands==2){
				print_info_cr("try:%s %s",v->at(i).param_type->at(0).c_str(),v->at(i).param_type->at(1).c_str());
				if(v->at(i).param_type->at(0)==*ps1 && v->at(i).param_type->at(1)==*ps2){ // we found the signature

					return &v->at(i);
				}
			}else if(v->at(i).param_type->size()==1 && num_operands==1){ // insert operator for 1 op.
				print_info_cr("try:%s",v->at(i).param_type->at(0).c_str());
				if(v->at(i).param_type->at(0)==*ps1){ // we found the signature

					return &v->at(i);
				}

			}/*else{
				print_error_cr("fatal error. there's no number of op expected (internal error %i %i)",v->at(i).param_type->size(),num_operands);
			}*/
		}

		if(num_operands==2){
			print_error_cr("No any signature matches with operator \"%s\" (%s %s)",op.c_str(),type_op1->c_str(),type_op2->c_str());
		}else{
			print_error_cr("No any signature matches with operator \"%s\" (%s )",op.c_str(),type_op1->c_str());
		}

		print_error_cr("Possibilities:");
		for(unsigned i = 0;i < v->size(); i++){ // for all signatures operator�
			if(v->at(i).param_type->size()==2)
				print_error_cr("%s (%s,%s)",v->at(i).result_type.c_str(), v->at(i).param_type->at(0).c_str(),v->at(i).param_type->at(1).c_str());

			if(v->at(i).param_type->size()==1)
				print_error_cr("%s (%s)",v->at(i).result_type.c_str(), v->at(i).param_type->at(0).c_str());

		}




	}

	return NULL;



}



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

bool CZG_Script::eval(const string & s){

	char *current=(char *)s.c_str();
	string var_name;
	bool error;
	char statment[MAX_STATMENT_LENGTH];
	char *next;
	PASTNode ast_node;
	int length;
	int numreg=0;

	current=IGNORE_SPACES(current);

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

		if(isVarDeclarationStatment(statment,error)){

			if(error) {
				return false;
			}
		}else{ // let's try another op...
			tInfoStatementOp i_stat;

			ast_node=generateAST(statment);

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
		current=IGNORE_SPACES(current);

	}

	execute();

	return true;


	/*PASTNode op=generateAST((const char *)s.c_str());

	if(op==NULL){ // some error happend!
		return false;
	}
	else{
		int numreg=0;


		if(generateAsmCode(op,numreg)<0){
			printf("Error generating code\n");
		}

		execute();
	}


	return true;*/

}

string * CZG_Script::getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction){
	tInfoStatementOp *ptr_current_statement_op = &statement_op[statement_op.size()-1];

	if(instruction >=0 && instruction < ptr_current_statement_op->asm_op.size()){
		return &ptr_current_statement_op->asm_op[instruction]->type_res; // type result..
	}
	else{
		print_error_cr("index out of bounds");
	}

	return NULL;
}

bool CZG_Script::insertMovInstruction(const string & v, string & type_ptr){

	tInfoStatementOp *ptr_current_statement_op = &statement_op[statement_op.size()-1];
	CObject *obj;
	// try parse value...
	if((obj=CNumber::Parse(v))!=NULL){
		print_info_cr("%s detected as number\n",v.c_str());
	}
	else if(v[0]=='\"' && v[v.size()-1]=='\"'){
		string s=v.substr(1,v.size()-2);
		CString *so=NEW_STRING();
		so->m_value=s;
		obj = so;
		print_info_cr("%s detected as string\n",v.c_str());
	}
	else if((obj=CBoolean::Parse(v))!=NULL){
		print_info_cr("%s detected as boolean\n",v.c_str());
	}else{
		obj=getRegisteredVariable(v);
		if(obj==NULL){
			print_error_cr("ERROR: %s is not declared",v.c_str());
			return false;
		}

		/*bool undefined=dynamic_cast<CUndefined *>(obj);
		if(undefined){
			print_error_cr("ERROR: %s is undefined",v.c_str());
			return false;
		}*/
	}

	if(obj != NULL){

		tInfoAsmOp *asm_op = new tInfoAsmOp();

		asm_op->res=obj;
		asm_op->type_res=obj->getPointerClassStr();
		type_ptr = obj->getPointerClassStr();

		if(asm_op->res->getPointerClassStr()==""){
			print_error_cr("unknown type res operand");
			return false;
		}


		asm_op->type_op=LOAD_VALUE;


		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	return true;
}

bool CZG_Script::insertMovVarInstruction(CObject *left_var, int right){

	string op="=";
	string left_type_ptr = left_var->getPointerClassStr();

	tInfoStatementOp *ptr_current_statement_op = &statement_op[statement_op.size()-1];

	if(right <0 || (unsigned)right >= ptr_current_statement_op->asm_op.size()){
		print_error_cr("ERROR: right operant is out of internal stack (%i,%i) ",right,ptr_current_statement_op->asm_op.size());
		return false;
	}

	/*if(right <0 || (unsigned)right >= ptr_current_statement_op->asm_op.size()){
		print_error_cr("ERROR: right operant is out of internal stack (%i,%i)",right,ptr_current_statement_op->asm_op.size());
		return;
	}*/

	/*if(ptr_current_statement_op->asm_op[left]->res->getPointerClassStr()==""){
		print_error_cr("unknown type left operand");
		return;
	}

	if(ptr_current_statement_op->asm_op[right]->res->getPointerClassStr()==""){
		print_error_cr("unknown type right operand");
		return;
	}*/

	if(right >= 0){ // insert both op...


		tInfoAsmOp *asm_op = new tInfoAsmOp();
		asm_op->type_op=MOV_VAR;
		asm_op->left_var_obj=left_var;
		asm_op->index_left = -1;
		asm_op->index_right = right;

		if((asm_op->funOp=getOperatorInfo(op,
				&left_type_ptr,
				&ptr_current_statement_op->asm_op[right]->type_res)) != NULL){

			asm_op->type_res = left_type_ptr;
			ptr_current_statement_op->asm_op.push_back(asm_op);
		}else{
			print_error_cr("cannot find = operator");
			return false;
		}

	}else{
		print_error_cr("ERROR: right operand is out of internal stack ");
		return false;
	}

	return true;
}

ASM_OPERATOR getNumberOperatorId(const string & op){

	if(op=="+"){
		return ADD;
	}else if(op=="-"){
		return SUB;
	}else if(op=="/"){
		return DIV;
	}else if(op=="%"){
		return MOD;
	}

	return UNKNOW;
}

ASM_OPERATOR getBoleanOperatorId(const string & op){
	if(op=="&&"){
		return ADD;
	}else if(op=="||"){
		return SUB;
	}

	return UNKNOW;
}

ASM_OPERATOR getStringOperatorId(const string & op){
	if(op=="+"){
		return CAT;
	}

	return UNKNOW;
}

CZG_Script::TYPE getTypeAsmResult(int index){

	tInfoStatementOp *ptr_current_statement_op = &statement_op[statement_op.size()-1];

	if(index <0 || (unsigned)index >= ptr_current_statement_op->asm_op.size()){
		print_error_cr("ERROR: left operant is out of internal stack (%i,%i) ",index,ptr_current_statement_op->asm_op.size());
		return CZG_Script::TYPE::UNKNOW;;
	}

	return ptr_current_statement_op->asm_op[index].result_type;
}


bool CZG_Script::insertOperatorInstruction(const string & op, int left, int right){


	tInfoStatementOp *ptr_current_statement_op = &statement_op[statement_op.size()-1];

	if(left <0 || (unsigned)left >= ptr_current_statement_op->asm_op.size()){
		print_error_cr("ERROR: left operant is out of internal stack (%i,%i) ",left,ptr_current_statement_op->asm_op.size());
		return false;
	}

	TYPE t_left = getTypeAsmResult(left);
	TYPE t_right = getTypeAsmResult(right);
	ASM_OPERATOR id_op;
	TYPE result_type=UNKNOW;

	if((t_left == NUMBER) && t_right == NUMBER){
		if((id_op=getNumberOperatorId(op))==UNKNOW){
			print_error_cr("undefined operator %s",op.c_str());
			return false;
		}

		result_type = NUMBER;

	}else if(t_left== STRING){

		if((id_op=getStringOperatorId(op))==UNKNOW){
			print_error_cr("undefined operator %s",op.c_str());
			return false;
		}

		result_type = STRING;
	}else if((t_left == BOOL) && t_right == BOOL){
		if((id_op=getBoleanOperatorId(op))==UNKNOW){
			print_error_cr("undefined operator %s",op.c_str());
			return false;
		}
		result_type = BOOL;
	}else{
		print_error_cr("not compatible %i %i",t_left,t_right);
	}
	/*if(right <0 || (unsigned)right >= ptr_current_statement_op->asm_op.size()){
		print_error_cr("ERROR: right operant is out of internal stack (%i,%i)",right,ptr_current_statement_op->asm_op.size());
		return;
	}*/

	/*if(ptr_current_statement_op->asm_op[left]->res->getPointerClassStr()==""){
		print_error_cr("unknown type left operand");
		return;
	}

	if(ptr_current_statement_op->asm_op[right]->res->getPointerClassStr()==""){
		print_error_cr("unknown type right operand");
		return;
	}*/

	 tInfoObjectOperator *funOp;

	if(left >=0 && right >= 0){ // insert both op...




		if((funOp=getOperatorInfo(op,
				&ptr_current_statement_op->asm_op[left]->type_res,
				&ptr_current_statement_op->asm_op[right]->type_res)) != NULL){

			tInfoAsmOp *asm_op = new tInfoAsmOp();
			asm_op->type_op=OPERATOR;
			asm_op->index_left = left;
			asm_op->index_right = right;
			asm_op->funOp = funOp;
			asm_op->type_res = asm_op->funOp->result_type;


			//-----------------------
			asm_op->result_type = result_type;
			asm_op->operator_type = id_op;


			ptr_current_statement_op->asm_op.push_back(asm_op);



		}else{
			print_error_cr("cannot find dual operator \"%s\"",op.c_str());
			return false;
		}
	}else if(left >=0){ // insert one operand




		if((funOp=getOperatorInfo(op,
				&ptr_current_statement_op->asm_op[left]->type_res)) != NULL){

			tInfoAsmOp *asm_op = new tInfoAsmOp();
			asm_op->type_op=OPERATOR;
			asm_op->index_left = left;
			asm_op->index_right = -1;
			asm_op->funOp = funOp;
			asm_op->type_res = asm_op->funOp->result_type;
			ptr_current_statement_op->asm_op.push_back(asm_op);
		}else{
			print_error_cr("cannot find one operator \"%s\"",op.c_str());
			return false;
		}

	}else{
		print_error_cr("ERROR: both operant is out of internal stack ");
		return false;
	}

	return true;

}

void CZG_Script::execute(){


	for(unsigned s = 0; s < statement_op.size(); s++){

		if(statement_op[s].asm_op.size()>0){

			print_info_cr("executing code...");

			for(unsigned i = 0; i  < statement_op[s].asm_op.size(); i++){
				print_info_cr("executing instruction %i...",i);
				switch(statement_op[s].asm_op[i]->type_op){
				case LOAD_VALUE: // do nothing because the value is already stored in the list.
					print_info_cr("mov!");
					break;
				case MOV_VAR:
				case OPERATOR:{ // we will perform the operation (cross the fingers)
					print_info_cr("operator %p",statement_op[s].asm_op[i]->funOp->fun_ptr);
					CObject *i1=NULL,*i2=NULL;
					int result=0;//(int)statement_op.asm_op[statement_op.asm_op[i]->index_left]->res;
					int fun=(int)statement_op[s].asm_op[i]->funOp->fun_ptr;

					if(statement_op[s].asm_op[i]->type_op!=MOV_VAR){
						i1=statement_op[s].asm_op[statement_op[s].asm_op[i]->index_left]->res;
					}else{
						i1=statement_op[s].asm_op[i]->left_var_obj;
					}

					if(statement_op[s].asm_op[i]->index_right!=-1){
						i2=statement_op[s].asm_op[statement_op[s].asm_op[i]->index_right]->res;
					}

					if(i2==NULL){ // one ops

						print_info_cr("1-op index:%i ",statement_op[s].asm_op[i]->index_left);


		#ifdef _WIN32
		asm(
				"push %[p1]\n\t"
				//"push %%esp\n\t"
				"call *%P0\n\t" // call function
				//"add $4,%%esp"       // Clean up the stack.
				: "=a" (result) // The result code from puts.
				: "r"(fun),[p1] "r"(i1));
		#else // GNU!!!!
		asm(
				"push %[p1]\n\t"
				"push %%esp\n\t"
				"call *%P0\n\t" // call function
				"add $8,%%esp"       // Clean up the stack.
				: "=a" (result) // The result code from puts.
				: "r"(fun),[p1] "r"(i1));

		#endif
					}

					else{ // two ops


						if(statement_op[s].asm_op[i]->type_op!=MOV_VAR){
							print_info_cr("2 op:%i %i ",statement_op[s].asm_op[i]->index_left,statement_op[s].asm_op[i]->index_right);
						}


		#ifdef _WIN32
		asm(
				"push %[p2]\n\t"
				"push %[p1]\n\t"
				//"push %%esp\n\t"
				"call *%P0\n\t" // call function
				//"add $4,%%esp"       // Clean up the stack.
				: "=a" (result) // The result code from puts.
				: "r"(fun),[p1] "r"(i1), [p2] "r"(i2));
		#else // GNU!!!!
		asm(
				"push %[p2]\n\t"
				"push %[p1]\n\t"
				"push %%esp\n\t"
				"call *%P0\n\t" // call function
				"add $12,%%esp"       // Clean up the stack.
				: "=a" (result) // The result code from puts.
				: "r"(fun),[p1] "r"(i1), [p2] "r"(i2));

		#endif
					}



						if(result!=0){


							if(dynamic_cast<CObject *>((CObject *)result) == NULL){
								print_error_cr("Error casting object");
								return;
							}

							statement_op[s].asm_op[i]->res=(CObject *)result;

						}else{
							print_error_cr("Error result returning void");
							return;
						}
					}

					break;
				}
			}

			CObject *obj = statement_op[s].asm_op[statement_op[s].asm_op.size()-1]->res;
			CNumber *num;
			CString *str;
			CBoolean *bol;
			if((num = dynamic_cast<CNumber *>(obj))!=NULL){
				print_info_cr("Number with value=%f",num->m_value);
			}else if((str = dynamic_cast<CString *>(obj))!=NULL){
				print_info_cr("String with value=\"%s\"",str->m_value.c_str());
			}else if((bol = dynamic_cast<CBoolean *>(obj))!=NULL){
				print_info_cr("Boolean with value=%i",bol->m_value);
			}



		}
	}

}

void CZG_Script::unregisterOperators(){

	for(map<string,vector<tInfoObjectOperator> *>::iterator it=m_mapContainerOperators.begin(); it != m_mapContainerOperators.end(); it++){
		vector< tInfoObjectOperator > * v= it->second;
		for(unsigned i = 0;i < v->size(); i++){ // for all signatures operator�
			delete it->second->at(i).param_type;
		}
		delete it->second;
	}

	m_mapContainerOperators.clear();

}

//-------------------------------------------------------------------------------------
CZG_Script::~CZG_Script(){
	// unregister operators ...
	for(unsigned s = 0; s  <statement_op.size(); s++){
	for(unsigned i = 0; i  <statement_op[s].asm_op.size(); i++){

		delete statement_op[s].asm_op[i];
	}
	}

	unregisterOperators();

	delete m_defaultVar;
}
