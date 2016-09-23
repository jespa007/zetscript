#include "core/zg_core.h"

void CScriptVariable::createSymbols(tInfoRegisteredClass *irv){
	for ( unsigned i = 0; i < irv->object_info.local_symbols.m_registeredVariable.size(); i++){
		addVariableSymbol(irv->object_info.local_symbols.m_registeredVariable[i].ast);
	}

	for ( unsigned i = 0; i < irv->object_info.local_symbols.m_registeredFunction.size(); i++){
		print_info_cr("=========================================");
		print_info_cr("- Create function %s...",irv->object_info.local_symbols.m_registeredFunction[i].object_info.symbol_info.info_var_scope->name.c_str());
		addFunctionSymbol(
				irv->object_info.local_symbols.m_registeredFunction[i].object_info.symbol_info.ast,
				&irv->object_info.local_symbols.m_registeredFunction[i]

				);
		//addSymbol(m_infoRegisteredClass->object_info.local_symbols.m_registeredVariable[i].ast);
	}

	if(irv->baseClass != NULL){
		CScriptVariable::createSymbols(irv->baseClass);
	}

}

CScriptVariable::CScriptVariable(tInfoRegisteredClass *irv){


	this->m_infoRegisteredClass = irv;
	//m_rootAst=NULL;
	//m_registeredVariable = NULL;
	//m_type = TYPE::SCRIPT_FUNCTION_TYPE;
	//pointer_function = NULL;
	//m_scope = scope;
	//m_parentFunction = _parentFunction;



	createSymbols(irv);
	/*if(_parentFunction == NULL){ // this is the main function ...
		setName("Main");
		m_registeredVariable=new vector<tRegisteredSymbolInfo>();
	}
	else {
		_parentFunction->addFunction(this);
	}*/

	// reserve symbolsand its extended classes...

	// create object functions ...
	/*for ( unsigned i = 0; i < m_infoRegisteredClass->object_info.local_symbols.m_registeredFunction.size(); i++){
		print_info_cr("=========================================");
		print_info_cr("- Create function %s...",m_infoRegisteredClass->object_info.local_symbols.m_registeredFunction[i].object_info.symbol_info.info_var_scope->name.c_str());
		addFunctionSymbol(
				m_infoRegisteredClass->object_info.local_symbols.m_registeredFunction[i].object_info.symbol_info.ast,
				&m_infoRegisteredClass->object_info.local_symbols.m_registeredFunction[i]

				);
		//addSymbol(m_infoRegisteredClass->object_info.local_symbols.m_registeredVariable[i].ast);
	}*/

}


void CScriptVariable::addVariableSymbol(tASTNode *ast){
	tSymbolInfo si;
	si.object = CScopeInfo::UndefinedSymbol;
	si.ast = ast;
	m_variableSymbol.push_back(si);
}

void CScriptVariable::addFunctionSymbol(tASTNode *ast,tInfoRegisteredFunctionSymbol *irv){
	tSymbolInfo si;
	si.object = irv;
	si.ast = ast;
	m_functionSymbol.push_back(si);
}


/*
void CScriptVariable::addArgSymbol(const string & arg_name){
	tSymbolInfo si;
	si.ast = NULL;
	m_arg_symbol.push_back(si);
}
*/
CScriptVariable::tSymbolInfo * CScriptVariable::getVariableSymbol(const string & varname){
	for(unsigned int i = 0; i < this->m_variableSymbol.size(); i++){
		if(varname == this->m_variableSymbol[i].ast->value_symbol){
			return &m_variableSymbol[i];
		}
	}

	return NULL;
}


CScriptVariable::tSymbolInfo * CScriptVariable::getVariableSymbolByIndex(unsigned idx){
	if(idx >= m_variableSymbol.size()){
		print_error_cr("idx symbol index out of bounds");
		return NULL;
	}

	return &m_variableSymbol[idx];
}

CScriptVariable::tSymbolInfo * CScriptVariable::getFunctionSymbol(const string & varname){
	for(unsigned int i = 0; i < this->m_functionSymbol.size(); i++){
		if(varname == this->m_functionSymbol[i].ast->value_symbol){
			return &m_functionSymbol[i];
		}
	}

	return NULL;
}

CScriptVariable::tSymbolInfo *CScriptVariable::getFunctionSymbolByIndex(unsigned idx){
	if(idx >= m_functionSymbol.size()){
		print_error_cr("idx symbol index out of bounds");
		return NULL;
	}
	return &m_functionSymbol[idx];

}

/*
CScriptVariable::tSymbolInfo * CScriptVariable::getArgSymbol(unsigned idx){
	if(idx >= m_arg_symbol.size()){
		print_error_cr("idx ARGUMENT SYMBOL index out of bounds");
		return NULL;
	}

	return &m_arg_symbol[idx];
}*/
/*
bool CScriptVariable::isMainFunction(){
	return m_parentFunction == NULL;
}

CScopeInfo::tInfoScopeVar * CScriptVariable::registerArgument(const string & var_name){
	CScopeInfo::tInfoScopeVar * irv;

	if((irv=getScope()->registerSymbol(var_name,-1))!=NULL){

		m_arg.push_back(var_name);
	}

	return irv;
}

void 	 CScriptVariable::addFunction(CScriptVariable *sf){

	m_function.push_back(sf);
}

vector<CScriptVariable *> *	 CScriptVariable::getVectorFunction(){

	return &m_function;
}

vector<string> *	 CScriptVariable::getArgVector(){

	return &m_arg;
}
*/


/*
void CScriptVariable::add_C_function_argument(string arg_type){
	m_c_arg.push_back(arg_type);
}


void CScriptVariable::setupAsFunctionPointer(void * _pointer_function){

	m_type= TYPE::C_FUNCTION_TYPE;

	pointer_function = _pointer_function;
}



bool CScriptVariable::call_C_function(vector<CVariable *> * argv){
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

CScriptVariable::TYPE CScriptVariable::getType(){
	return m_type;
}*/
/*
tInfoRegisteredFunctionSymbol * CScriptVariable::getFunctionInfo(unsigned idx){
	if(idx >= m_infoRegisteredClass->object_info.local_symbols.m_registeredFunction.size()){
		print_error_cr("idx function index out of bounds");
		return NULL;
	}

	return &m_infoRegisteredClass->object_info.local_symbols.m_registeredFunction[idx];

}
*/
/*
CVariable **CScriptVariable::getArg(const string & var_name){


	CScopeInfo::tInfoScopeVar *irv = m_scope->getInfoRegisteredSymbol(var_name);

	if(irv != NULL){
		return &m_infoRegisteredClass->m_obj;
	}

	return NULL;
}


CVariable **CScriptVariable::getArgSymbol(const string & index){

	if(index < 0 || index > m_arg.size()){ print_error_cr("out of bounds"); return NULL;}

	return getArg(m_arg[index]);
}

CVariable **CScriptVariable::getArgSymbol(unsigned index){

	if(index < 0 || index > m_arg.size()){ print_error_cr("out of bounds"); return NULL;}

	return getArg(m_arg[index]);
}


CScopeInfo *CScriptVariable::getScope(){
	return m_scope;
}

PASTNode CScriptVariable::getRootAst(){
	return m_rootAst;
}

PASTNode * CScriptVariable::getRootAstPtr(){
	return &m_rootAst;
}


CScriptVariable *CScriptVariable::getParent(){
	return m_parentFunction;
}

*/




vector<tInfoStatementOp> * CScriptVariable::getCompiledCode(int idx_function){
	tSymbolInfo *si=CScriptVariable::getFunctionSymbolByIndex(idx_function);
	if(si){
		return &((tInfoRegisteredFunctionSymbol *)si->object)->object_info.statment_op;
	}

	return NULL;
//	return &m_infoRegisteredClass->object_info.statment_op;
}

CScriptVariable::~CScriptVariable(){





	/*if(m_registeredVariable != NULL){
		delete m_registeredVariable;
		m_registeredVariable=NULL;
	}*/

	//delete m_scope;
	//delete m_rootAst;
}
