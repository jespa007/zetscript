#include "core/zg_core.h"


CUndefined *CScriptVariable::UndefinedSymbol=NULL;
CVoid *CScriptVariable::VoidSymbol=NULL;



void CScriptVariable::createSingletons(){
	UndefinedSymbol = NEW_UNDEFINED_VAR;
	VoidSymbol = NEW_VOID_VAR;

}


void CScriptVariable::destroySingletons(){
	delete UndefinedSymbol;
	delete VoidSymbol;
}





void CScriptVariable::createSymbols(tInfoRegisteredClass *ir_class){
	tSymbolInfo *si;

	//if(ir_class == this->m_infoRegisteredClass)
	{ // only register vars from higher class because the heredited symbols were created at CCompiler::gacClass

		for ( unsigned i = 0; i < ir_class->metadata_info.object_info.local_symbols.m_registeredVariable.size(); i++){


			tInfoRegisteredVariableSymbol * ir_var = &ir_class->metadata_info.object_info.local_symbols.m_registeredVariable[i];

			si = addVariableSymbol(ir_var->symbol_name,ir_var->ast);

			// Warning if you put any var for primitives (i.e CInteger, CNumber, etc will crash in recursive manner)
			if(IS_CLASS_C){ // we know the type object so we allocate new var symbol ...
				// check if primitive type (only 4 no more no else)...
				void *ptr_variable = (void*) ((unsigned long long) c_object + ir_var->ref_ptr);

				if(CScriptClassFactory::valid_C_PrimitiveType[CScriptClassFactory::INT_PTR_TYPE].type_str==ir_var->c_type.c_str()){//={typeid(int *).name(),"int *",INT_PTR_TYPE};
					si->object = new CInteger(CScriptClassFactory::getInstance()->getRegisteredClassInteger(),(int *)ptr_variable);
				}else if(CScriptClassFactory::valid_C_PrimitiveType[CScriptClassFactory::FLOAT_PTR_TYPE].type_str==ir_var->c_type.c_str()){//={typeid(float *).name(),"float *",FLOAT_PTR_TYPE};
					si->object = new CNumber(CScriptClassFactory::getInstance()->getRegisteredClassNumber(),(float *)ptr_variable);
				}else if(CScriptClassFactory::valid_C_PrimitiveType[CScriptClassFactory::STRING_PTR_TYPE].type_str==ir_var->c_type.c_str()){//={typeid(string *).name(),"string *",STRING_PTR_TYPE};
					si->object = new CString(CScriptClassFactory::getInstance()->getRegisteredClassString(),(string *)ptr_variable);
				}else if(CScriptClassFactory::valid_C_PrimitiveType[CScriptClassFactory::BOOL_PTR_TYPE].type_str==ir_var->c_type.c_str()){//={typeid(bool *).name(),"bool *",BOOL_PTR_TYPE};
					si->object = new CBoolean(CScriptClassFactory::getInstance()->getRegisteredClassString(),(bool *)ptr_variable);
				}else{
					tInfoRegisteredClass *info_registered_class = CScriptClassFactory::getInstance()->getRegisteredClassBy_C_ClassPtr(ir_var->c_type);

					if(info_registered_class){
						si->object = new CScriptVariable(info_registered_class,ptr_variable);
					}

					/*if(idx_registered_class != -1){

					}*/
				}
				//si->
				//si->object = CScriptClassFactory::getInstance()->get(ir_var->c_type);
			}

		}

	}


	// Register even for primitives (if appropiate)
	for ( unsigned i = 0; i < ir_class->metadata_info.object_info.local_symbols.m_registeredFunction.size(); i++){
		tInfoRegisteredFunctionSymbol * ir_fun  = &ir_class->metadata_info.object_info.local_symbols.m_registeredFunction[i];
		//print_info_cr("=========================================");
		//print_info_cr("- Create function %s...",irv->object_info.local_symbols.m_registeredFunction[i].object_info.symbol_info.symbol_name.c_str());
		 si =addFunctionSymbol(
				ir_class->metadata_info.object_info.local_symbols.m_registeredFunction[i].object_info.symbol_info.symbol_name,
				ir_class->metadata_info.object_info.local_symbols.m_registeredFunction[i].object_info.symbol_info.ast,
				ir_fun

				);
		 if((ir_fun->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // create proxy function ...
			si->proxy_ptr = (*((std::function<void *(void *)> *)ir_fun->object_info.symbol_info.ref_ptr))(c_object);
		}
		//addSymbol(m_infoRegisteredClass->object_info.local_symbols.m_registeredVariable[i].ast);
	}


	/*if(ir_class->baseClass != NULL){
		CScriptVariable::createSymbols(ir_class->baseClass);
	}*/

}

CScriptVariable::CScriptVariable(){

	setup();

	this->m_infoRegisteredClass = NULL;
	c_object = NULL;
	created_object = NULL;
	m_value = NULL;

}

CScriptVariable::CScriptVariable(tInfoRegisteredClass *irv, void *_object_by_user){

	setup();

	created_object = NULL;
	c_object = NULL;
	m_value = NULL;

	this->m_infoRegisteredClass = irv;
	//m_rootAst=NULL;
	//m_registeredVariable = NULL;
	//m_type = TYPE::SCRIPT_FUNCTION_TYPE;
	//pointer_function = NULL;
	//m_scope = scope;
	//m_parentFunction = _parentFunction;
	//print_info_cr("creating var type %s",irv->object_info.symbol_info.symbol_name.c_str());

	if(IS_CLASS_C){
		if(_object_by_user == NULL){
			created_object = (*m_infoRegisteredClass->c_constructor)();
			c_object = created_object;
		}else{
			c_object = _object_by_user;
		}
	}else{
		c_object = this;
	}


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


tInfoRegisteredFunctionSymbol *CScriptVariable::getConstructorFunction(){

	if(m_infoRegisteredClass->idx_function_script_constructor != -1){
		return &m_infoRegisteredClass->metadata_info.object_info.local_symbols.m_registeredFunction[m_infoRegisteredClass->idx_function_script_constructor];
	}

	return NULL;
}


CScriptVariable::tSymbolInfo * CScriptVariable::addVariableSymbol(const string & value_symbol, tASTNode *ast){
	tSymbolInfo si;
	si.proxy_ptr=0;
	si.object = CScriptVariable::UndefinedSymbol;
	si.ast = ast;
	si.value_symbol = value_symbol;
	m_variableSymbol.push_back(si);

	return &m_variableSymbol[m_variableSymbol.size()-1];
}


CScriptVariable::tSymbolInfo *CScriptVariable::addFunctionSymbol(const string & value_symbol,tASTNode *ast,tInfoRegisteredFunctionSymbol *irv){
	tSymbolInfo si;
	si.proxy_ptr=0;
	si.object = irv;

	// get super function ...
	si.super_function = getFunctionSymbol(value_symbol);

	si.value_symbol = value_symbol;
	si.ast = ast;
	m_functionSymbol.push_back(si);


	return &m_functionSymbol[m_functionSymbol.size()-1];
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
		if(varname == this->m_variableSymbol[i].value_symbol){
			return &m_variableSymbol[i];
		}
	}

	return NULL;
}


CScriptVariable::tSymbolInfo * CScriptVariable::getVariableSymbolByIndex(unsigned idx){
	if(idx >= m_variableSymbol.size()){
		print_error_cr("idx symbol index out of bounds (%i)",idx);
		return NULL;
	}

	return &m_variableSymbol[idx];
}

/*
CScriptVariable::tSymbolInfo *getFunctionSymbolRecursive(const string & varname){

}
*/
CScriptVariable::tSymbolInfo * CScriptVariable::getFunctionSymbol(const string & varname){

	// from lat value to first to get last override function...
	for(int i = this->m_functionSymbol.size()-1; i >= 0; i--){
		if(varname == this->m_functionSymbol[i].value_symbol){
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


	if(created_object != NULL){
		 (*m_infoRegisteredClass->c_destructor)(created_object);
	}
	/*if(m_registeredVariable != NULL){
		delete m_registeredVariable;
		m_registeredVariable=NULL;
	}*/

	//delete m_scope;
	//delete m_rootAst;
}
