#include "zg_script.h"

CScriptClass::CScriptClass(tInfoRegisteredFunctionSymbol *irv){

	this->irv = irv;
	//m_rootAst=NULL;
	//m_registeredVariable = NULL;
	//m_type = TYPE::SCRIPT_FUNCTION_TYPE;
	//pointer_function = NULL;
	//m_scope = scope;
	//m_parentFunction = _parentFunction;


	returnVariable = CScopeInfo::UndefinedSymbol;

	/*if(_parentFunction == NULL){ // this is the main function ...
		setName("Main");
		m_registeredVariable=new vector<tRegisteredSymbolInfo>();
	}
	else {
		_parentFunction->addFunction(this);
	}*/

	// reserve symbols...
	for ( unsigned i = 0; i < irv->object_info.local_symbols.m_registeredVariable.size(); i++){
		addSymbol(irv->object_info.local_symbols.m_registeredVariable[i].ast);
	}

	// create object functions ...
	for ( unsigned i = 0; i < irv->object_info.local_symbols.m_registeredFunction.size(); i++){
		print_info_cr("- Create function %s...",irv->object_info.local_symbols.m_registeredFunction[i].object_info.symbol_info.ast->value_symbol);
		//addSymbol(irv->object_info.local_symbols.m_registeredVariable[i].ast);
	}

}


void CScriptClass::addSymbol(tASTNode *ast){
	tSymbolInfo si;
	si.object = CScopeInfo::UndefinedSymbol;
	si.ast = ast;
	m_symbol.push_back(si);
}

void CScriptClass::addArgSymbol(const string & arg_name){
	tSymbolInfo si;
	si.ast = NULL;
	m_arg_symbol.push_back(si);
}

CScriptClass::tSymbolInfo * CScriptClass::getSymbol(unsigned idx){
	if(idx >= m_symbol.size()){
		print_error_cr("idx symbol index out of bounds");
		return NULL;
	}

	return &m_symbol[idx];
}

CScriptClass::tSymbolInfo * CScriptClass::getArgSymbol(unsigned idx){
	if(idx >= m_arg_symbol.size()){
		print_error_cr("idx ARGUMENT SYMBOL index out of bounds");
		return NULL;
	}

	return &m_arg_symbol[idx];
}
/*
bool CScriptClass::isMainFunction(){
	return m_parentFunction == NULL;
}

CScopeInfo::tInfoScopeVar * CScriptClass::registerArgument(const string & var_name){
	CScopeInfo::tInfoScopeVar * irv;

	if((irv=getScope()->registerSymbol(var_name,-1))!=NULL){

		m_arg.push_back(var_name);
	}

	return irv;
}

void 	 CScriptClass::addFunction(CScriptClass *sf){

	m_function.push_back(sf);
}

vector<CScriptClass *> *	 CScriptClass::getVectorFunction(){

	return &m_function;
}

vector<string> *	 CScriptClass::getArgVector(){

	return &m_arg;
}
*/
CObject *	 CScriptClass::getReturnObject(){

	return returnVariable;
}

CObject ** CScriptClass::getReturnObjectPtr(){
	return &returnVariable;
}


void CScriptClass::setReturnObject(CObject *obj){

	returnVariable = obj;
}

/*
void CScriptClass::add_C_function_argument(string arg_type){
	m_c_arg.push_back(arg_type);
}


void CScriptClass::setupAsFunctionPointer(void * _pointer_function){

	m_type= TYPE::C_FUNCTION_TYPE;

	pointer_function = _pointer_function;
}



bool CScriptClass::call_C_function(vector<CObject *> * argv){
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

CScriptClass::TYPE CScriptClass::getType(){
	return m_type;
}*/

tInfoRegisteredFunctionSymbol * CScriptClass::getFunctionInfo(unsigned idx){
	if(idx >= irv->object_info.local_symbols.m_registeredFunction.size()){
		print_error_cr("idx function index out of bounds");
		return NULL;
	}

	return &irv->object_info.local_symbols.m_registeredFunction[idx];

}

/*
CObject **CScriptClass::getArg(const string & var_name){


	CScopeInfo::tInfoScopeVar *irv = m_scope->getInfoRegisteredSymbol(var_name);

	if(irv != NULL){
		return &irv->m_obj;
	}

	return NULL;
}


CObject **CScriptClass::getArgSymbol(const string & index){

	if(index < 0 || index > m_arg.size()){ print_error_cr("out of bounds"); return NULL;}

	return getArg(m_arg[index]);
}

CObject **CScriptClass::getArgSymbol(unsigned index){

	if(index < 0 || index > m_arg.size()){ print_error_cr("out of bounds"); return NULL;}

	return getArg(m_arg[index]);
}


CScopeInfo *CScriptClass::getScope(){
	return m_scope;
}

PASTNode CScriptClass::getRootAst(){
	return m_rootAst;
}

PASTNode * CScriptClass::getRootAstPtr(){
	return &m_rootAst;
}


CScriptClass *CScriptClass::getParent(){
	return m_parentFunction;
}

*/




vector<tInfoStatementOp> * CScriptClass::getCompiledCode(){
	return &irv->object_info.statment_op;
}

CScriptClass::~CScriptClass(){





	/*if(m_registeredVariable != NULL){
		delete m_registeredVariable;
		m_registeredVariable=NULL;
	}*/

	//delete m_scope;
	//delete m_rootAst;
}
