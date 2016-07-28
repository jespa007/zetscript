#include "zg_script.h"

CScriptFunction::CScriptFunction(CScriptFunction * _parentFunction, CScope *scope){

	m_rootAst=NULL;
	m_registeredSymbol = NULL;
	m_type = TYPE::SCRIPT_FUNCTION_TYPE;
	pointer_function = NULL;
	m_scope = scope;
	m_parentFunction = _parentFunction;


	returnVariable = CScope::UndefinedSymbol;

	if(_parentFunction == NULL){ // this is the main function ...
		setName("Main");
		m_registeredSymbol=new vector<tRegisteredSymbolInfo>();
	}
	else {
		_parentFunction->addFunction(this);
	}

}


void CScriptFunction::addSymbol(PASTNode *ast){
	tSymbolInfo si;
	si.ast = ast;
	m_symbol.push_back(si);
}

bool CScriptFunction::isMainFunction(){
	return m_parentFunction == NULL;
}

CScope::tInfoScopeVar * CScriptFunction::registerArgument(const string & var_name){
	CScope::tInfoScopeVar * irv;

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
	CScope::tInfoScopeVar *irv = m_scope->getInfoRegisteredSymbol(var_name);

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






vector<CCompiler::tInfoStatementOp> * CScriptFunction::getCompiledCode(){
	return &m_listStatements;
}

CScriptFunction::~CScriptFunction(){

	for(unsigned s = 0; s  <m_listStatements.size(); s++){
		for(unsigned i = 0; i  <m_listStatements[s].asm_op.size(); i++){

			delete m_listStatements[s].asm_op[i];
		}
	}



	if(m_registeredSymbol != NULL){
		delete m_registeredSymbol;
		m_registeredSymbol=NULL;
	}

	delete m_scope;
	delete m_rootAst;
}
