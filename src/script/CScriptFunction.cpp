#include "zg_script.h"

CScriptFunction::CScriptFunction(CScriptFunction * _parentFunction){



	m_type = TYPE::SCRIPT_FUNCTION_TYPE;
	pointer_function = NULL;

	m_parentFunction = _parentFunction;
	m_scope = new CScope(this,m_parentFunction!=NULL?m_parentFunction->getScope():NULL);

	returnVariable = CScope::UndefinedSymbol;

	if(_parentFunction == NULL){ // this is the main function ...
		setName("Main");
	}
	else {
		_parentFunction->addFunction(this);
	}

}


CScope::tInfoRegisteredVar * CScriptFunction::registerArgument(const string & var_name){
	CScope::tInfoRegisteredVar * irv;

	if((irv=getScope()->registerSymbol(var_name,-1))!=NULL){

		m_arg.push_back(var_name);
	}

	return irv;
}

void 	 CScriptFunction::addFunction(CScriptFunction *sf){

	m_function.push_back(sf);
}

vector<CScriptFunction *> *	 CScriptFunction::getVectorFunction(){

	return &m_function;
}

vector<string> *	 CScriptFunction::getArgVector(){

	return &m_arg;
}

CObject *	 CScriptFunction::getReturnObject(){

	return returnVariable;
}

CObject ** CScriptFunction::getReturnObjectPtr(){
	return &returnVariable;
}


void CScriptFunction::setReturnObject(CObject *obj){

	returnVariable = obj;
}

void CScriptFunction::add_C_function_argument(string arg_type){
	m_c_arg.push_back(arg_type);
}


void CScriptFunction::setupAsFunctionPointer(void * _pointer_function){

	m_type= TYPE::C_FUNCTION_TYPE;

	pointer_function = _pointer_function;
}

CObject * CScriptFunction::call_print_1p(CObject *obj_arg){


	int fp = (int)this->pointer_function;

	if(fp==0){
		print_error_cr("Null function");
		return NULL;
	}

	if(this->m_c_arg.size() != 1){
		print_error_cr("Argument doestn't match");
		return NULL;
	}

	//
	fntConversionType fun1=CZG_Script::getConversionType(obj_arg->getPointerClassStr(),m_c_arg[0]);
	int result=0;

	if(fun1 != NULL){
		// Normalize argument ...
		int ptr_arg = fun1(obj_arg);

		((int (*)(int))fp)(ptr_arg);

		/*asm(
				"push %[p1]\n\t"
				"call *%P0\n\t" // call function
				"add $4,%%esp"  // Clean up the stack (you must multiply 4*n_arguments here).
				: "=a" (result) // The result code from puts.
				: "r"(fun1),[p1] "r"(ptr_arg)
		);*/

		//float h = (float)result;

		print_info_cr("hh:%i",result);


	}
	return NULL;

}

CObject * CScriptFunction::call_print_0p(){

	int fp = (int)this->pointer_function;
	if(fp==0){
		print_error_cr("Null function");
		return NULL;
	}
	// Normalize argument ...

	int result=0;

	((int (*)())fp)();

	// convert result to object ...


	return (CObject *)result;


}

bool CScriptFunction::call_C_function(vector<CObject *> * argv){
		switch(argv->size()){
		case 0:
			call_print_0p();
			print_info_cr("0 call!");
			break;
		case 1:
			call_print_1p(argv->at(0));
			print_info_cr("1 call!");
			break;

		}
		print_error_cr("cannot call !");
		return false;
}

CScriptFunction::TYPE CScriptFunction::getType(){
	return m_type;
}

CObject **CScriptFunction::getArg(const string & var_name){
	CScope::tInfoRegisteredVar *irv = m_scope->getInfoRegisteredSymbol(var_name);

	if(irv != NULL){
		return &irv->m_obj;
	}
	/*for(unsigned i = 0; i < m_arg.size(); i++){
		if(m_arg[i]->getName() == var_name){
			return m_arg[i];
		}
	}*/
	return NULL;
}

CObject **CScriptFunction::getArg(unsigned index){

	if(index < 0 || index > m_arg.size()){ print_error_cr("out of bounds"); return NULL;}

	return getArg(m_arg[index]);
}


CScope *CScriptFunction::getScope(){
	return m_scope;
}

PASTNode CScriptFunction::getRootAst(){
	return m_rootAst;
}

PASTNode * CScriptFunction::getRootAstPtr(){
	return &m_rootAst;
}


CScriptFunction *CScriptFunction::getParent(){
	return m_parentFunction;
}


CScriptFunction * CScriptFunction::registerClass(const string & class_name, int m_line){
	tInfoRegisteredClass * irv;
	if((irv = existRegisteredClass(class_name))==NULL){ // check whether is local var registered scope ...

		CScriptFunction *sf = new CScriptFunction(this);

		irv = new tInfoRegisteredClass;
		irv->m_line = m_line;
		irv->m_scriptFunction = sf;
		m_registeredClass[class_name]=irv;
		return sf;
	}else{
		print_error_cr("error class \"%s\" already registered at line %i!", class_name.c_str(), irv->m_line);
	}

	return NULL;
}

CScriptFunction::tInfoRegisteredClass * CScriptFunction::existRegisteredClass(const string & class_name){
	if(m_registeredClass.count(class_name)==0){ // not exit but we will deepth through parents ...
		CScriptFunction * parent =  getParent();
		if(parent != NULL){
			return parent->existRegisteredClass(class_name);
		}
		return NULL;
	}else{
		return m_registeredClass[class_name];
		//print_error_cr("variable %s already registered at line %i",var_name.c_str(),lc->m_registeredSymbol[var_name]->m_line);
	}

	return NULL;
}

CScriptFunction::tInfoRegisteredClass *CScriptFunction::getRegisteredClass(const string & class_name, bool print_msg){
	tInfoRegisteredClass * irv;
	if((irv = existRegisteredClass(class_name))!=NULL){ // check whether is local var registered scope ...

		return irv;
	}else{
		if(print_msg){
			print_error_cr("class \"%s\" doesn't exist!", class_name.c_str());
		}
	}

	return NULL;

}



vector<CCompiler::tInfoStatementOp> * CScriptFunction::getCompiledCode(){
	return &m_listStatements;
}

CScriptFunction::~CScriptFunction(){

	for(unsigned s = 0; s  <m_listStatements.size(); s++){
		for(unsigned i = 0; i  <m_listStatements[s].asm_op.size(); i++){

			delete m_listStatements[s].asm_op[i];
		}
	}

	for(map<string,tInfoRegisteredClass *>::iterator it = m_registeredClass.begin();it!= m_registeredClass.end();it++){
			delete it->second->m_scriptFunction;
			delete it->second;
	}


	delete m_scope;
	delete m_rootAst;
}
