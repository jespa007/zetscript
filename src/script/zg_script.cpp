
#include "ast.cpp"



bool CZG_Script::existOperatorSignature(const string & op,const string & result, vector<string> * param){


	if(m_mapContainerOperators.count(op)==1){
		vector< tInfoObjectOperator > * v= m_mapContainerOperators[op];

		for(unsigned i = 0;i < v->size(); i++){ // for all signatures operator…

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
	// IMPPORTANT! for each operator we can have different signatures…
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

		for(unsigned i = 0;i < v->size(); i++){ // for all signatures operator…

			if(v->at(i).param_type->size()==2 && num_operands==2){
				print_info_cr("try:%s %s",v->at(i).param_type->at(0).c_str(),v->at(i).param_type->at(1).c_str());
				if(v->at(i).param_type->at(0)==*ps1 && v->at(i).param_type->at(1)==*ps2){ // we found the signature

					return &v->at(i);
				}
			}else if(v->at(i).param_type->size()==1 && num_operands==1){ // insert operator for 1 op.
				print_info_cr("try:%s %s",v->at(i).param_type->at(0).c_str());
				if(v->at(i).param_type->at(0)==*ps1){ // we found the signature

					return &v->at(i);
				}

			}else{
				print_error_cr("fatal error. there's no number of op expected (internal error %i %i)",v->at(i).param_type->size(),num_operands);
			}
		}

		if(num_operands==2){
			print_error_cr("No any signature matches with operator \"%s\" (%s %s)",op.c_str(),type_op1->c_str(),type_op2->c_str());
		}else{
			print_error_cr("No any signature matches with operator \"%s\" (%s )",op.c_str(),type_op1->c_str());
		}

		print_error_cr("Possibilities:");
		for(unsigned i = 0;i < v->size(); i++){ // for all signatures operator…
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
	iniFactory<CNumberFactory>("CNumber");
	iniFactory<CBooleanFactory>("CBoolean");
	iniFactory<CStringFactory>("CString");

}

void CZG_Script::init(){
	CFactoryContainer::getInstance()->registerScriptFunctions();
}

bool CZG_Script::eval(const string & s){

	PASTOperator op=generateAST((const char *)s.c_str());

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


	return true;

}

bool CZG_Script::insertMovInstruction(const string & v){

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
		print_error_cr("ERROR: %s is unkown variable\n",v.c_str());
		return false;
	}

	if(obj != NULL){

		tInfoAsmOp *asm_op = new tInfoAsmOp();



		asm_op->res=obj;
		asm_op->type_res=obj->getPointerClassStr();

		if(asm_op->res->getPointerClassStr()==""){
			print_error_cr("unknown type res operand");
			return false;
		}


		asm_op->type_op=MOV_VALUE;


		statement_op.asm_op.push_back(asm_op);
	}

	return true;
}

bool CZG_Script::insertOperatorInstruction(const string & op, int left, int right){
	if(left <0 || (unsigned)left >= statement_op.asm_op.size()){
		print_error_cr("ERROR: left operant is out of internal stack (%i,%i) ",left,statement_op.asm_op.size());
		return false;
	}

	/*if(right <0 || (unsigned)right >= statement_op.asm_op.size()){
		print_error_cr("ERROR: right operant is out of internal stack (%i,%i)",right,statement_op.asm_op.size());
		return;
	}*/

	/*if(statement_op.asm_op[left]->res->getPointerClassStr()==""){
		print_error_cr("unknown type left operand");
		return;
	}

	if(statement_op.asm_op[right]->res->getPointerClassStr()==""){
		print_error_cr("unknown type right operand");
		return;
	}*/

	if(left >=0 && right >= 0){ // insert both op...


		tInfoAsmOp *asm_op = new tInfoAsmOp();
		asm_op->type_op=OPERATOR;
		asm_op->index_left = left;
		asm_op->index_right = right;

		if((asm_op->funOp=getOperatorInfo(op,
				&statement_op.asm_op[left]->type_res,
				&statement_op.asm_op[right]->type_res)) != NULL){

			asm_op->type_res = asm_op->funOp->result_type;
			statement_op.asm_op.push_back(asm_op);
		}else{
			print_error_cr("cannot find dual operator \"%s\"",op.c_str());
			return false;
		}
	}else if(left >=0){ // insert one operand
		tInfoAsmOp *asm_op = new tInfoAsmOp();
		asm_op->type_op=OPERATOR;
		asm_op->index_left = left;
		asm_op->index_right = -1;

		if((asm_op->funOp=getOperatorInfo(op,
				&statement_op.asm_op[left]->type_res)) != NULL){

			asm_op->type_res = asm_op->funOp->result_type;
			statement_op.asm_op.push_back(asm_op);
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

	if(statement_op.asm_op.size()>0){

		print_info_cr("executing code...");

		for(unsigned i = 0; i  <statement_op.asm_op.size(); i++){
			print_info_cr("executing instruction %i...",i);
			switch(statement_op.asm_op[i]->type_op){
			case MOV_VALUE: // do nothing because the value is already stored in the list.
				print_info_cr("mov!");
				break;
			case OPERATOR:{ // we will perform the operation (cross the fingers)
				print_info_cr("operator %p",statement_op.asm_op[i]->funOp->fun_ptr);
				CObject *i1,*i2;
				int result=0;//(int)statement_op.asm_op[statement_op.asm_op[i]->index_left]->res;
				int fun=(int)statement_op.asm_op[i]->funOp->fun_ptr;

				i1=statement_op.asm_op[statement_op.asm_op[i]->index_left]->res;

				if(statement_op.asm_op[i]->index_right==-1){ // one ops

					print_info_cr("1-op index:%i ",statement_op.asm_op[i]->index_left);


	#ifdef _WIN32
	asm(
			"push %[p1]\n\t"
			//"push %%esp\n\t"
			"call %P0\n\t" // call function
			//"add $4,%%esp"       // Clean up the stack.
			: "=a" (result) // The result code from puts.
			: "r"(fun),[p1] "r"(i1));
	#else // GNU!!!!
	asm(
			"push %[p2]\n\t"
			"push %[p1]\n\t"
			"push %%esp\n\t"
			"call %P0\n\t" // call function
			"add $12,%%esp"       // Clean up the stack.
			: "=a" (result) // The result code from puts.
			: "r"(fun),[p1] "r"(&i1));

	#endif
				}

				else{ // two ops

					i2=statement_op.asm_op[statement_op.asm_op[i]->index_right]->res;

					print_info_cr("2 op:%i %i ",statement_op.asm_op[i]->index_left,statement_op.asm_op[i]->index_right);


	#ifdef _WIN32
	asm(
			"push %[p2]\n\t"
			"push %[p1]\n\t"
			//"push %%esp\n\t"
			"call %P0\n\t" // call function
			//"add $4,%%esp"       // Clean up the stack.
			: "=a" (result) // The result code from puts.
			: "r"(fun),[p1] "r"(i1), [p2] "r"(i2));
	#else // GNU!!!!
	asm(
			"push %[p2]\n\t"
			"push %[p1]\n\t"
			"push %%esp\n\t"
			"call %P0\n\t" // call function
			"add $12,%%esp"       // Clean up the stack.
			: "=a" (result) // The result code from puts.
			: "r"(fun),[p1] "r"(&i1), [p2] "r"(&i2));

	#endif
				}



					if(result!=0){


						if(dynamic_cast<CObject *>((CObject *)result) == NULL){
							print_error_cr("Error casting object");
							return;
						}

						statement_op.asm_op[i]->res=(CObject *)result;

					}else{
						print_error_cr("Error result returning void");
						return;
					}
				}

				break;
			}
		}

		CObject *obj = statement_op.asm_op[statement_op.asm_op.size()-1]->res;
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

void CZG_Script::unregisterOperators(){

	for(map<string,vector<tInfoObjectOperator> *>::iterator it=m_mapContainerOperators.begin(); it != m_mapContainerOperators.end(); it++){
		vector< tInfoObjectOperator > * v= it->second;
		for(unsigned i = 0;i < v->size(); i++){ // for all signatures operator…
			delete it->second->at(i).param_type;
		}
		delete it->second;
	}

	m_mapContainerOperators.clear();

}

//-------------------------------------------------------------------------------------
CZG_Script::~CZG_Script(){
	// unregister operators ...
	for(unsigned i = 0; i  <statement_op.asm_op.size(); i++){

		delete statement_op.asm_op[i];
	}

	unregisterOperators();
}
