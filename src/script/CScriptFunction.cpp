#include "zg_script.h"

CScriptFunction::CScriptFunction(CScriptFunction * _base){
	m_scope = new CScope(this);
	m_base = _base;
	if(m_base == NULL){
		m_base = this;
	}
}

CScriptFunction * CScriptFunction::getBase(){
	return m_base;
}

bool CScriptFunction::eval(const string & _str){

	bool ok = m_scope->eval(_str);


	return ok;
}

vector<CVirtualMachine::tInfoStatementOp> * CScriptFunction::getCompiledCode(){
	return &m_listStatements;
}

CScriptFunction::~CScriptFunction(){

	for(unsigned s = 0; s  <m_listStatements.size(); s++){
		for(unsigned i = 0; i  <m_listStatements[s].asm_op.size(); i++){

			delete m_listStatements[s].asm_op[i];
		}
	}

	//destroy registered functions ..
	for(unsigned i = 0; i < m_function.size(); i++){
		delete m_function[i];
	}
	delete m_scope;
}
