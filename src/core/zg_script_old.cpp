/*
bool CZetScript::existOperatorSignature(const string & op,const string & result, vector<string> * param){


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



bool CZetScript::registerOperatorInternal(const string & _op_name, const string &  result_type,vector<string> * param_type, void(*fun_ptr)()){

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



CZetScript::tInfoObjectOperator * CZetScript::getOperatorInfo(const string & op, string * type_op1,  string * type_op2){
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

			}
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
*/


#if 0
void CZetScript::execute(){


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
					CVariable *i1=NULL,*i2=NULL;
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
				: [fun] "r"(fun),[p1] "r"(i1), [p2] "r"(i2));
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


							if(dynamic_cast<CVariable *>((CVariable *)result) == NULL){
								print_error_cr("Error casting object");
								return;
							}

							statement_op[s].asm_op[i]->res=(CVariable *)result;

						}else{
							print_error_cr("Error result returning void");
							return;
						}
					}

					break;
				}
			}

			CVariable *obj = statement_op[s].asm_op[statement_op[s].asm_op.size()-1]->res;
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
#endif

bool CZetScript::insertLoadValueInstruction(const string & v, string & type_ptr){

	CNumber *num_obj;
	CString *str_obj;
	CBoolean *bool_obj;

	tInfoStatementOp *ptr_current_statement_op = &statement_op[statement_op.size()-1];
	void *obj;
	TYPE type=TYPE::UNKNOW;
	// try parse value...
	if((obj=CNumber::ParsePrimitive(v))!=NULL){
		type=TYPE::NUMBER;
		print_info_cr("%s detected as number\n",v.c_str());
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
		CVariable * var=getRegisteredVariable(v);
		type=TYPE::VAR;

		if(var==NULL){
			print_error_cr("ERROR: %s is not declared",v.c_str());
			return false;
		}

		obj = var;
		if((num_obj=dynamic_cast<CNumber *>(var))!=NULL){ // else it will store the value ...
			type = TYPE::NUMBER;
			obj=&num_obj->m_value;
		}else if((bool_obj=dynamic_cast<CBoolean *>(var))!=NULL){
			type = TYPE::BOOL;
			obj=&bool_obj->m_value;
		}else if((str_obj=dynamic_cast<CString *>(var))!=NULL){
			type = TYPE::STRING;
			obj=&str_obj->m_value;
		}



		/*bool undefined=dynamic_cast<CUndefined *>(obj);
		if(undefined){
			print_error_cr("ERROR: %s is undefined",v.c_str());
			return false;
		}*/
	}

	if(obj != NULL){

		tInfoAsmOp *asm_op = new tInfoAsmOp();

		asm_op->result_obj=obj;
		asm_op->result_type = type;

		/*if(type==TYPE::VAR){
		asm_op->type_res=((CVariable *)obj)->getPointerClassStr();
		//type_ptr = obj->getPointerClassStr();

			if(((CVariable *)obj)->getPointerClassStr()==""){
				print_error_cr("unknown type res operand");
				return false;
			}
		}*/


		asm_op->operator_type=ASM_OPERATOR::LOAD;


		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	return true;
}

bool CZetScript::insertMovVarInstruction(CVariable *var, int right){

	//string op="=";
	string left_type_ptr = var->getPointerClassStr();
	CNumber *num_obj;
	CString *str_obj;
	CBoolean *bool_obj;

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



		asm_op->result_type = TYPE::VAR; // this only stores pointer...
		asm_op->result_obj=var;

		if((num_obj=dynamic_cast<CNumber *>(var))!=NULL){ // else it will store the value ...
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

bool CZetScript::insertMovVarInstruction(CVariable *var, int right){

	//string op="=";
	string left_type_ptr = var->getPointerClassStr();
	CNumber *num_obj;
	CString *str_obj;
	CBoolean *bool_obj;

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



		asm_op->result_type = TYPE::VAR; // this only stores pointer...
		asm_op->result_obj=var;

		if((num_obj=dynamic_cast<CNumber *>(var))!=NULL){ // else it will store the value ...
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

		/*if((asm_op->funOp=getOperatorInfo(op,
				&left_type_ptr,
				&ptr_current_statement_op->asm_op[right]->type_res)) != NULL){

			asm_op->type_res = left_type_ptr;
			ptr_current_statement_op->asm_op.push_back(asm_op);
		}else{
			print_error_cr("cannot find = operator");
			return false;
		}*/

	}else{
		print_error_cr("ERROR: right operand is out of internal stack ");
		return false;
	}

	return true;
}


/*
string CZetScript::getTypeAsmResult(int index){

	tInfoStatementOp *ptr_current_statement_op = &statement_op[statement_op.size()-1];
	string result="unknow";

	if(index <0 || (unsigned)index >= ptr_current_statement_op->asm_op.size()){
		print_error_cr("ERROR: left operant is out of internal stack (%i,%i) ",index,ptr_current_statement_op->asm_op.size());
		return result;
	}

	return ptr_current_statement_op->asm_op[index]->result_type;
}
*/

bool CZetScript::insertOperatorInstruction(const string & op, int left, int right){


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

	 //tInfoObjectOperator *funOp;

	if(left >=0 && right >= 0){ // insert both op...

		if((t_left == CNumberFactory::getPointerTypeStr()) && t_right == CNumberFactory::getPointerTypeStr()){
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
		case TYPE::STRING:
			res_obj = new string;
			break;
		case TYPE::BOOL:
			res_obj = new bool;
			break;

		}

		/*if((funOp=getOperatorInfo(op,
				&ptr_current_statement_op->asm_op[left]->type_res,
				&ptr_current_statement_op->asm_op[right]->type_res)) != NULL){*/

			tInfoAsmOp *asm_op = new tInfoAsmOp();
			//asm_op->type_op=OPERATOR;
			asm_op->index_left = left;
			asm_op->index_right = right;
			//asm_op->funOp = funOp;
			//asm_op->type_res = asm_op->funOp->result_type;


			//-----------------------
			asm_op->result_type = result_type;
			asm_op->operator_type = id_op;
			asm_op->result_obj = res_obj;


			ptr_current_statement_op->asm_op.push_back(asm_op);



		/*}else{
			print_error_cr("cannot find dual operator \"%s\"",op.c_str());
			return false;
		}*/
	}else if(left >=0){ // insert one operand


		if((t_left == CNumberFactory::getPointerTypeStr()) ){
					if((id_op=getNumberOperatorId_OneOp(op))==ASM_OPERATOR::UNKNOW){
						print_error_cr("undefined operator %s",op.c_str());
						return false;
					}

					res_obj = new float;
					result_type = NUMBER;

				}else if(t_left== CStringFactory::getPointerTypeStr()){

					if((id_op=getStringOperatorId_OneOp(op))==ASM_OPERATOR::UNKNOW){
						print_error_cr("undefined operator %s",op.c_str());
						return false;
					}

					res_obj = new string;
					result_type = STRING;
				}else if((t_left == CBooleanFactory::getPointerTypeStr())){
					if((id_op=getBoleanOperatorId_OneOp(op))==ASM_OPERATOR::UNKNOW){
						print_error_cr("undefined operator %s",op.c_str());
						return false;
					}
					res_obj = new bool;
					result_type = BOOL;
				}else{
					print_error_cr("not compatible %s %s",t_left.c_str(),t_right.c_str());
					return false;
				}

		/*if((funOp=getOperatorInfo(op,
				&ptr_current_statement_op->asm_op[left]->type_res)) != NULL){*/

			tInfoAsmOp *asm_op = new tInfoAsmOp();
			//asm_op->type_op=OPERATOR;
			asm_op->operator_type = id_op;
			asm_op->result_type = result_type;
			asm_op->result_obj = res_obj;
			asm_op->index_left = left;
			asm_op->index_right = -1;
			//asm_op->funOp = funOp;
			//asm_op->type_res = asm_op->funOp->result_type;
			ptr_current_statement_op->asm_op.push_back(asm_op);
		/*}else{
			print_error_cr("cannot find one operator \"%s\"",op.c_str());
			return false;
		}*/

	}else{
		print_error_cr("ERROR: both operant is out of internal stack ");
		return false;
	}

	return true;

}


/*
void CZetScript::unregisterOperators(){

	for(map<string,vector<tInfoObjectOperator> *>::iterator it=m_mapContainerOperators.begin(); it != m_mapContainerOperators.end(); it++){
		vector< tInfoObjectOperator > * v= it->second;
		for(unsigned i = 0;i < v->size(); i++){ // for all signatures operator�
			delete it->second->at(i).param_type;
		}
		delete it->second;
	}

	m_mapContainerOperators.clear();

}
*/
