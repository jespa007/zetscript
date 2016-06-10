#include "zg_script.h"

CScriptFunction::CScriptFunction(CScriptFunction * _parentFunction){

	m_parentFunction = _parentFunction;
	m_scope = new CScope(this,NULL);

	returnVariable = CScope::m_undefinedSymbol;

	if(_parentFunction != NULL){
		_parentFunction->addFunction(this);
	}

}


void 	 CScriptFunction::addArg(const string & var_name){
	CScope::tInfoRegisteredVar *irv=getScope()->registerSymbol(var_name,-1);
	m_arg.push_back(irv->m_obj);
}

void 	 CScriptFunction::addFunction(CScriptFunction *sf){

	m_function.push_back(sf);
}

vector<CScriptFunction *> *	 CScriptFunction::getVectorFunction(){

	return &m_function;
}


CObject *	 CScriptFunction::getReturnValue(){

	return returnVariable;
}

void CScriptFunction::setReturnValue(CObject *obj){

	returnVariable = obj;
}



CObject *CScriptFunction::getArg(const string & var_name){
	for(unsigned i = 0; i < m_arg.size(); i++){
		if(m_arg[i]->getName() == var_name){
			return m_arg[i];
		}
	}
	return NULL;
}

CScope *CScriptFunction::getScope(){
	return m_scope;
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
}
